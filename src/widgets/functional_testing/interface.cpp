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
    using                           Font                    = app::AppState::Font;
    static constexpr const char *   uuid                    = "##Editor_Controls_Columns";
    static constexpr const char *   SETTINGS_MENU_UUID      = "ActionComposer_SettingsMenu";
    static constexpr int            NC                      = 8;
    static constexpr int            NUM_ENTRIES_AT_END      = 3;
    //
    static ImGuiOldColumnFlags      COLUMN_FLAGS            = ImGuiOldColumnFlags_None;
    static ImGuiSliderFlags         SLIDER_FLAGS            = ImGuiSliderFlags_AlwaysClamp;
    //  constexpr ImGuiButtonFlags      BUTTON_FLAGS            = ImGuiOldColumnFlags_NoPreserveWidths;
    //
    static ImVec2                   WIDGET_SIZE             = ImVec2( -1,               32 );
    static ImVec2                   BUTTON_SIZE             = ImVec2( WIDGET_SIZE.y,    WIDGET_SIZE.y );
    //
    //  "column_label"
    auto                            column_label            = [&](const char * label) -> void
    { S.PushFont(Font::FootNote);     ImGui::TextDisabled("%s", label);     S.PopFont(); };
    
    
    ImGuiIO &                       io                      = ImGui::GetIO();
    const ImVec2                    mpos                    = ImVec2(io.MousePos.x, io.MousePos.y);     // this->get_cursor_pos();    // glfwGetCursorPos(S.m_glfw_window, &cx, &cy);
    S.PushFont(Font::Small);
        
    
    
    //  BEGIN COLUMNS...
    ImGui::Columns(NC, uuid, COLUMN_FLAGS);
    //
    //
    //
        //      1.      OPEN / CLOSE SIDEBAR WINDOW...
        column_label("Browser:");
        //
        if ( ImGui::ArrowButtonEx("##ActionComposer_ToggleCompositionBrowser",     (this->m_show_composition_browser) ? ImGuiDir_Left : ImGuiDir_Right,
                                  BUTTON_SIZE,                      ImGuiButtonFlags_None) )
        { this->m_show_composition_browser = !this->m_show_composition_browser; }
        
        
    
        //  2.  PLAY / PAUSE...
        ImGui::NextColumn();        column_label("Controls:");
        //
        ImGui::SetNextItemWidth( WIDGET_SIZE.x );
        ImGui::BeginDisabled( m_actions->empty() );
        {
            if ( !this->is_running() )
            {
                if ( ImGui::Button("Run All", WIDGET_SIZE) ) {
                    this->_run_all();
                }
            }
            else
            {
                if ( utl::CButton("Stop", 0xFF453AFF, WIDGET_SIZE) ) {
                    this->reset_all();
                }
            }
        }
        ImGui::EndDisabled();
    
    
    
        //  3.  RUN ONCE...
        ImGui::NextColumn();        column_label("");
        ImGui::SetNextItemWidth( WIDGET_SIZE.x );
        ImGui::BeginDisabled( !this->is_running() && m_actions->empty() && (m_sel < 0) );
            if ( ImGui::Button("Run Once", WIDGET_SIZE) ) {
                m_play_index = m_sel;
                m_step_req   = true;                       // flag for single action
                m_state      = State::Run;
            }
        ImGui::EndDisabled();
        


    
        //      //  4.  STEP BUTTON...
        //      ImGui::NextColumn();        ImGui::NewLine();
        //      ImGui::BeginDisabled(m_sel < 0);               // usable while *not* running
        //      if ( ImGui::Button("Step", WIDGET_SIZE) ) {
        //          m_play_index = m_sel;
        //          m_step_req   = true;                       // identical to Run Once
        //          m_state      = State::Run;
        //      }
        //      ImGui::EndDisabled();



        //      5.      PLAYBACK SPEED...
        ImGui::NextColumn();        column_label("Playback Speed:");
        //
        ImGui::SetNextItemWidth(WIDGET_SIZE.x);
        if ( ImGui::SliderScalar( "##ActionComposer_PlaybackSpeed",         ImGuiDataType_Double,
                                  &m_executor.m_playback_speed.value,       &m_executor.m_playback_speed.limits.min,       &m_executor.m_playback_speed.limits.max,
                                  "%.1f", SLIDER_FLAGS ) )
        { }



        //      ?.      EMPTY SPACES FOR LATER...
        for (int i = ImGui::GetColumnIndex(); i < NC - NUM_ENTRIES_AT_END; ++i) {
            ImGui::Dummy( ImVec2(0,0) );    ImGui::NextColumn();
        }



        //      X.      INFO...
        column_label("Info:");
        //
        if ( this->is_running() )                   { ImGui::Text("running: %d / %zu", m_play_index + 1, m_actions->size()); }
        else                                        { ImGui::Text("idle"); }
        
        
        
        //      XX.     MOUSE COORDINATES...
        ImGui::NextColumn();        column_label("Global Position:");
        //
        ImGui::Text("(%.1f , %.1f)",    mpos.x, mpos.y);     //  Live cursor read-out in the same units we feed to glfwSetCursorPos...
                


        //      XXX.    SETTINGS MENU...
        ImGui::NextColumn();        column_label("Settings:");
        //
        if ( ImGui::Button("+", BUTTON_SIZE) )      { ImGui::OpenPopup(SETTINGS_MENU_UUID); }
        if ( ImGui::BeginPopup(SETTINGS_MENU_UUID) )              // draw the popup
        {
            this->_draw_settings_menu();
            ImGui::EndPopup();
        }
    //
    //
    //
    ImGui::Columns(1);      //  END COLUMNS...
    
    
    
    S.PopFont();
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


    //  3.  INPUT-BLOCKER WINDOW...
    if ( (this->m_allow_input_blocker) && (m_state == State::MouseCapture || m_state == State::KeyCapture) )
    { _draw_input_blocker(); }
    

    //  4.  OVERLAY WINDOW...
    ImGui::SetNextWindowBgAlpha(ms_OVERLAY_ALPHA);
    ImGui::SetNextWindowPos(pos, ImGuiCond_Always);
        if ( ImGui::BeginTooltip() )
        {
            this->_dispatch_overlay_content();
            m_overlay_size  = ImGui::GetWindowSize();
        }
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
    static constexpr const char *   FMT_STRING              = "\t Local: (%.0f, %.0f)";
    //
    static const char *             state_str               = nullptr;
    static State                    state_cache             = static_cast<State>( static_cast<int>(this->m_state) - 1 );
    //
    static ImVec4                   color                   = S.SystemColor.Gray;
    const ImVec2                    mpos                    = this->get_cursor_pos();
            
    if ( state_cache != this->m_state )
    {
        state_cache     = static_cast<State>( this->m_state );
        state_str       = ms_COMPOSER_STATE_NAMES[ static_cast<size_t>(this->m_state) ];
        
        switch ( this->m_state ) {
            case State::Run             : { color = S.SystemColor.Green;    break;  }    //  1.  "Run" STATE...
            case State::MouseCapture    : { color = S.SystemColor.Red;      break;  }    //  2.  "Mouse Capture" STATE...
            case State::KeyCapture      : { color = S.SystemColor.Orange;   break;  }    //  3.  "Keyboard Capture" STATE...
            default                     : { color = S.SystemColor.Gray;     break;  }    //  4.  DEFAULT...
        }
    }
    ImGui::TextColored( color,   "%s",   state_str);
    ImGui::SameLine();
    ImGui::Text(FMT_STRING, mpos.x, mpos.y);
    ImGui::Separator();
    
    return;
}


//  "_overlay_ui_run"
//
inline void ActionComposer::_overlay_ui_run(void)
{
    static constexpr const char *   FMT         = "Description: %s";
    Action *                        act         = &(*m_actions)[m_play_index];
    
    const bool                      updated     = this->m_overlay_cache.update_cache( m_play_index, m_comp_sel, act );
    Action *                        proxy       = this->m_overlay_cache.m_action;
    
    
    if ( proxy != nullptr && !proxy->descr.empty() ) {
        ImGui::TextColored( S.SystemColor.Gray,     FMT,    proxy->descr.c_str() );
    }
    

    ImGui::TextColored( S.SystemColor.Yellow,   "%s",   "([ESC] to exit)");
    return;
}


//  "_overlay_ui_mouse_capture"
//
inline void ActionComposer::_overlay_ui_mouse_capture(void)
{
    ImGui::TextColored( S.SystemColor.Yellow,   "%s",   "([ENTER] or [LMB] to acccept,  [ESC] to cancel,  [TAB] to switch end-points)");
    return;
}


//  "_overlay_ui_key_capture"
//
inline void ActionComposer::_overlay_ui_key_capture(void)
{
    ImGui::TextColored( S.SystemColor.Yellow,   "%s",   "([ENTER] to acccept,  [ESC] to cancel)");
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
