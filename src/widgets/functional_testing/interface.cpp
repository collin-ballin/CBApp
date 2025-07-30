/***********************************************************************************
*
*       **************************************************************************
*       ****             I N T E R F A C E . C P P  ____  F I L E             ****
*       **************************************************************************
*
*              AUTHOR:      Collin A. Bond
*               DATED:      July 24, 2025.
*
**************************************************************************************
**************************************************************************************/
#include "widgets/functional_testing/functional_testing.h"
#include "app/state/state.h"
#include "app/state/_types.h"


namespace cb { namespace ui { //     BEGINNING NAMESPACE "cb::ui"...
// *************************************************************************** //
// *************************************************************************** //



// *************************************************************************** //
//
//
//
// *************************************************************************** //
//          SECONDARY UI FUNCTIONS...
// *************************************************************************** //

//  "_draw_controlbar"
//
void ActionComposer::_draw_controlbar(void)
{
    static constexpr const char *   uuid                    = "##Editor_Controls_Columns";
    static constexpr int            NC                      = 9;
    static constexpr const char *   SETTINGS_MENU_UUID      = "ActionComposer_SettingsMenu";
    //
    static ImGuiOldColumnFlags      COLUMN_FLAGS            = ImGuiOldColumnFlags_None;
    static ImVec2                   WIDGET_SIZE             = ImVec2( -1,  32 );
    static ImVec2                   BUTTON_SIZE             = ImVec2( 22,   WIDGET_SIZE.y );
    //
    constexpr ImGuiButtonFlags      BUTTON_FLAGS            = ImGuiOldColumnFlags_NoPreserveWidths;
    
    //this->S.PushFont( Font::Small );
   
    
    
   
   
    //  BEGIN COLUMNS...
    //
    ImGui::Columns(NC, uuid, COLUMN_FLAGS);
    //
    //
    //
        //  1.  PLAY / PAUSE...
        ImGui::TextDisabled("Controls:");
        //
        ImGui::SetNextItemWidth( WIDGET_SIZE.x );
        ImGui::BeginDisabled( m_actions->empty() );
            if ( !this->is_running() )
            {
                if ( ImGui::Button("Run All", WIDGET_SIZE) ) {
                    m_play_index        = (m_sel >= 0           ? m_sel             : 0);
                    m_is_running        = !m_actions->empty();
                    m_state             = (m_actions->empty())   ? State::Idle       : State::Run;
                }
            }
            else
            {
                if ( utl::CButton("Stop", 0xFF453AFF, WIDGET_SIZE) ) {
                    this->reset_all();
                }
            }
        ImGui::EndDisabled();
    
    
    
        //  2.  RUN ONCE...
        ImGui::NextColumn();        ImGui::NewLine();
        ImGui::SetNextItemWidth( WIDGET_SIZE.x );
        ImGui::BeginDisabled( !this->is_running() && m_actions->empty() && (m_sel < 0) );
            if ( ImGui::Button("Run Once", WIDGET_SIZE) ) {
                m_play_index = m_sel;
                m_step_req   = true;                       // flag for single action
                m_state      = State::Run;
            }
        ImGui::EndDisabled();
        


    
        //  3.  STEP BUTTON...
        ImGui::NextColumn();        ImGui::NewLine();
        ImGui::BeginDisabled(m_sel < 0);               // usable while *not* running
        if ( ImGui::Button("Step", WIDGET_SIZE) ) {
            m_play_index = m_sel;
            m_step_req   = true;                       // identical to Run Once
            m_state      = State::Run;
        }
        ImGui::EndDisabled();



        //  4.  [TOGGLE]    OVERVIEW...
        ImGui::NextColumn();
        ImGui::TextDisabled("Overlay:");
        //
        ImGui::SetNextItemWidth( BUTTON_SIZE.x );
        ImGui::Checkbox("##ActionComposer_OverlayToggle",           &m_show_overlay);



        //  5.  [TOGGLE]    RENDER VISUALS...
        ImGui::NextColumn();
        ImGui::TextDisabled("Render Visuals:");
        //
        ImGui::SetNextItemWidth( BUTTON_SIZE.x );
        ImGui::Checkbox("##ActionComposer_RenderVisualsToggle",     &m_render_visuals);



        //  6.  INFO...
        ImGui::NextColumn();
        ImGui::TextDisabled("Info:");
        //
        if ( this->is_running() )       { ImGui::Text("running: %d / %zu", m_play_index + 1, m_actions->size()); }
        else                            { ImGui::Text("idle"); }



        //  7.  SETTINGS MENU...
        ImGui::NextColumn();
        ImGui::TextDisabled("Settings:");
        
        if ( ImGui::Button("+") )       { ImGui::OpenPopup("ActionsPopup"); }

        if ( ImGui::BeginPopup("ActionsPopup") )              // draw the popup
        {
            ImGui::SeparatorText("Action Composer Settings...");         // header

            this->_draw_settings_menu();

            ImGui::EndPopup();
        }





        
        //  8.  EMPTY SPACES FOR LATER...
        for (int i = ImGui::GetColumnIndex(); i < NC - 1; ++i) {
            ImGui::Dummy( ImVec2(0,0) );    ImGui::NextColumn();
        }


        //  X.  MOUSE COORDINATES...
        ImGui::TextDisabled("Global Position:");
        //ImVec2 mpos = ImGui::GetMousePos();
        ImVec2 mpos = this->get_cursor_pos();
        
        ImGui::Text("(%.1f , %.1f)",    mpos.x, mpos.y);     //  Live cursor read-out in the same units we feed to glfwSetCursorPos...
    //
    //
    //
    ImGui::Columns(1);      //  END COLUMNS...
    
    
    //this->S.PopFont();
   
    return;
}


//  "_draw_toolbar"
//
void ActionComposer::_draw_toolbar(void)
{
    return;
}












// *************************************************************************** //
//
//
//
// *************************************************************************** //
//          OVERLAY UI CONTENT...
// *************************************************************************** //

//  "_draw_overlay"
//
void ActionComposer::_draw_overlay(void)
{
    static constexpr ImGuiWindowFlags       flags           = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
    ImVec2                                  mpos            = ImGui::GetMousePos();
    ImVec2                                  pos             = mpos + ms_OVERLAY_OFFSET;

    if ( !m_show_overlay )                  { return; }
    if ( ImGui::GetIO().AppFocusLost )
    {
        return;
    }
    //if ( glfwGetWindowAttrib(S.m_glfw_window, GLFW_FOCUSED) == 0 )    { return; }

    //  1.  CACHE CURRENT MONITOR DATA...
    _refresh_monitor_cache(mpos);


    //  2.  CLAMP INSIDE CACHED MONITOR WORKSPACE...
    pos.x       = std::clamp( pos.x,        m_monitor_bounds.Min.x,       m_monitor_bounds.Max.x - m_overlay_size.x );
    pos.y       = std::clamp( pos.y,        m_monitor_bounds.Min.y,       m_monitor_bounds.Max.y - m_overlay_size.y );



    ImGui::SetNextWindowBgAlpha(ms_OVERLAY_ALPHA);
    ImGui::SetNextWindowPos(pos, ImGuiCond_Always);
    //  ImGuiViewport * vp = ImGui::GetMainViewport();
    //  ImGui::SetNextWindowViewport(vp->ID);               //  pin to main viewport
    
        //if ( ImGui::Begin("##ac_overlay", nullptr, flags) )
        if ( ImGui::BeginTooltip() )
        {
            this->_dispatch_overlay_content();
            m_overlay_size  = ImGui::GetWindowSize();
        }
        
    //ImGui::End();
    ImGui::EndTooltip();
    return;
}


//  "_dispatch_overlay_content"
//
inline void ActionComposer::_dispatch_overlay_content(void)
{
    this->_overlay_ui_none();

    switch (m_state)
    {
        //  1.  "Run" STATE...
        case State::Run :  {
            this->_overlay_ui_run();
            break;
        }
        
        //  2.  "Mouse Capture" STATE...
        case State::MouseCapture :  {
            this->_overlay_ui_mouse_capture();
            break;
        }
        
        //  3.  "Keyboard Capture" STATE...
        case State::KeyCapture :  {
            this->_overlay_ui_key_capture();
            break;
        }
        
        //  DEFAULT...
        default :   { break; }
    }
    
    return;
}


//  "_overlay_ui_none"
//
inline void ActionComposer::_overlay_ui_none(void)
{
    static constexpr const char *   FMT_STRING              = "%s. \t Local: (%.0f, %.0f)";
    //
    static const char *             state_str               = nullptr;
    static State                    state_cache             = static_cast<State>( static_cast<int>(this->m_state) - 1 );
    //const ImVec2                    mpos                    = ImGui::GetMousePos();
    const ImVec2                    mpos                    = this->get_cursor_pos();
            
            
        //ImGui::TextColored(ImVec4(1,1,0,1), "Press key, Enter=accept, Esc=cancel");
    if ( state_cache != this->m_state ) {
        state_cache     = static_cast<State>( this->m_state );
        state_str       = ms_COMPOSER_STATE_NAMES[ static_cast<size_t>(this->m_state) ];
    }
    
    //  void ImGui::TextColored(const ImVec4& col, const char* fmt, ...)
    //  ImGui::TextColored(FMT_STRING, state_str, mpos.x, mpos.y);
    ImGui::Text(FMT_STRING, state_str, mpos.x, mpos.y);
    ImGui::Separator();
    
    return;
}


//  "_overlay_ui_run"
//
inline void ActionComposer::_overlay_ui_run(void)
{
    ImGui::TextUnformatted("Running test... (press ESC to CANCEL)");
    return;
}


//  "_overlay_ui_mouse_capture"
//
inline void ActionComposer::_overlay_ui_mouse_capture(void)
{
    ImGui::TextUnformatted("Capturing... (press ESC to ENTER)");
    return;
}


//  "_overlay_ui_key_capture"
//
inline void ActionComposer::_overlay_ui_key_capture(void)
{
    ImGui::TextUnformatted("Capturing... (press ESC to ENTER)");
    return;
}
























// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
} }//   END OF "cb" "ui" NAMESPACE.











// *************************************************************************** //
// *************************************************************************** //
//
//  END.
