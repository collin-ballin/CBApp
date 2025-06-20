/***********************************************************************************
*
*       ********************************************************************
*       ****          C O N T R O L B A R . C P P  ____  F I L E        ****
*       ********************************************************************
*              AUTHOR:      Collin A. Bond
*               DATED:      June 05, 2025.
*
**************************************************************************************
**************************************************************************************/
#include "app/app.h"
#include "app/delegators/_controlbar.h"
#include <random>
#include <algorithm>



namespace cb { //     BEGINNING NAMESPACE "cb"...
// *************************************************************************** //
// *************************************************************************** //



//  1.      INITIALIZATION  | DEFAULT CONSTRUCTOR, DESTRUCTOR, ETC...
// *************************************************************************** //
// *************************************************************************** //

//  Default Constructor.
//
ControlBar::ControlBar(app::AppState & src)
    : S(src)                        { }


//  "initialize"
//
void ControlBar::initialize(void)
{
    if (this->m_initialized)
        return;
        
    this->init();
    return;
}


//  "init"          | protected
//
void ControlBar::init(void) {
    this->ms_PLOT_SIZE.y                           *= S.m_dpi_scale;
    
    this->m_window_class.DockNodeFlagsOverrideSet   = ImGuiDockNodeFlags_NoTabBar;
    return;
}


//  Destructor.
//
ControlBar::~ControlBar(void)             { this->destroy(); }


//  "destroy"       | protected
//
void ControlBar::destroy(void)         { }




// *************************************************************************** //
//
//
//  1B.     PUBLIC API MEMBER FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

//  "toggle_sidebar"
//
void ControlBar::toggle_sidebar(void) {
    this->S.m_show_browser_window           = !this->S.m_show_browser_window;
    S.m_windows[ Window::Browser ].open     = !S.m_windows[ Window::Browser ].open;
    //this->S.m_browser_ratio                 = this->S.m_show_browser_window ? app::DEF_SB_OPEN_WIDTH : 0.0f;
}


void ControlBar::open_sidebar(void) {
    if (this->S.m_show_browser_window)  return;
        
    this->S.m_show_browser_window           = true;
    S.m_windows[ Window::Browser ].open     = true;
    this->S.m_browser_ratio                 = this->S.m_show_browser_window;
}

void ControlBar::close_sidebar(void) {
    if (!this->S.m_show_browser_window) return;

    this->S.m_show_browser_window           = false;
    S.m_windows[ Window::Browser ].open     = false;
    this->S.m_browser_ratio                 = 0.0f;
}




//  "toggle_detview"
//
void ControlBar::toggle_detview(void) {
    this->S.m_show_detview_window = !this->S.m_show_detview_window;
}


void ControlBar::open_detview(void) {
    //  ...
}


void ControlBar::close_detview(void) {
    //  ...
}







// *************************************************************************** //
//
//
//  1C.     PUBLIC MEMBER FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

//  "Begin"
//
void ControlBar::Begin([[maybe_unused]] const char *        uuid,
                    [[maybe_unused]] bool *                 p_open,
                    [[maybe_unused]] ImGuiWindowFlags       flags)
{
    static constexpr ImVec2                 ms_WINDOW_PADDING   = ImVec2(4.0f,      4.0f);  //  Default here should be      ImVec2(8.0f, 8.0f);
    static constexpr ImVec2                 ms_FRAME_PADDING    = ImVec2(10.0f,     5.0f);
    static constexpr ImVec2                 ms_ITEM_PADDING     = ImVec2(10.0f,     5.0f);
    static constexpr float                  ms_SMALL_ITEM_PAD   = 4.0f;
    static constexpr float                  ms_BIG_ITEM_PAD     = 8.0f;
    
    [[maybe_unused]] ImGuiIO &              io                  = ImGui::GetIO(); (void)io;
    [[maybe_unused]] ImGuiStyle &           style               = ImGui::GetStyle();
    [[maybe_unused]] ImGuiContext &         g                   = *GImGui;
    [[maybe_unused]] ImGuiMetricsConfig *   cfg                 = &g.DebugMetricsConfig;
    
    
    
    
    //  0.  SETUP NEW STYLE OPTIONS FOR CONTROL BAR WIDGETS...
    ImGui::PushStyleVar(    ImGuiStyleVar_WindowPadding,    ms_WINDOW_PADDING       );
    ImGui::PushStyleVar(    ImGuiStyleVar_FramePadding,     ms_FRAME_PADDING        );
    ImGui::PushStyleVar(    ImGuiStyleVar_ItemSpacing,      ms_ITEM_PADDING         );
    ImGui::PushStyleColor(  ImGuiCol_WindowBg,              S.m_controlbar_bg       );      // Push before ImGui::Begin()
    
    constexpr ImGuiButtonFlags      BUTTON_FLAGS    = ImGuiButtonFlags_None;
    const float                     ROW_H           = ImGui::GetFrameHeightWithSpacing();   //  font_size + 2*FramePadding.y
    const float                     WIDGET_HEIGHT   = ROW_H - style.WindowPadding.y;
    const ImVec2                    BUTTON_SIZE     = ImVec2(ROW_H, WIDGET_HEIGHT);         //  exactly one‑row square buttons
    
    
    //  1.  CREATE THE WINDOW AND BEGIN APPENDING WIDGETS INTO IT...
    ImGui::SetNextWindowClass(&this->m_window_class);
    ImGui::SetNextWindowSizeConstraints( ImVec2(-1, ROW_H), ImVec2(FLT_MAX, ROW_H) );
    ImGui::Begin(uuid, p_open, flags);
        ImGui::PopStyleColor();

        S.PushFont(Font::Small);
        

        
        //      1.      OPEN / CLOSE SIDEBAR WINDOW...
        if ( ImGui::ArrowButtonEx("##ControlBar_ToggleSidebar",     (this->S.m_show_browser_window) ? ImGuiDir_Left : ImGuiDir_Right,
                                  BUTTON_SIZE,                      BUTTON_FLAGS) )
        {
                this->S.m_show_browser_window           = !this->S.m_show_browser_window;
                S.m_windows[ Window::Browser ].open     = !S.m_windows[ Window::Browser ].open;
        }
        

        
        //      2.      OPEN / CLOSE DETAIL VIEW WINDOW...
        ImGui::SameLine(0, ms_SMALL_ITEM_PAD);
        if ( ImGui::ArrowButtonEx("##ControlBar_ToggleDetailView",  (this->S.m_show_detview_window) ? ImGuiDir_Down : ImGuiDir_Up,
                                  BUTTON_SIZE,                      BUTTON_FLAGS) )
        {
            this->S.m_show_detview_window = !this->S.m_show_detview_window;
        }
        

        
        //      3.      TOGGLE SYSTEM PREFERENCES...
        ImGui::SameLine(0, ms_BIG_ITEM_PAD);
        if ( ImGui::Button( (this->S.m_show_system_preferences)
                            ? "Browser##ControlBar" : "Preferences##ControlBar", ImVec2(120, BUTTON_SIZE.y)) ) {
            this->S.m_show_system_preferences = !this->S.m_show_system_preferences;
        }
    
        
        
        //      99.     PERFORMANCE...
        {
            constexpr const char *      io_time_fmt     = "%.2f ms  ";      //  FORMAT STRINGS.
            constexpr const char *      fps_fmt         = "%.1f FPS  ";     //  FORMAT STRINGS.
            //
            const float                 delta_t         = 1000 * io.DeltaTime;
            const float                 fps_ct          = io.Framerate;
            const ImVec2                ts              = ImVec2( ImGui::CalcTextSize(io_time_fmt).x + ImGui::CalcTextSize(fps_fmt).x, 0.0f);
            const ImVec2                text_size       = ImVec2( 1.2f * ts.x, ts.y );
            
            
            //  S.PushFont(Font::FootNote);
                ImGui::SameLine();                      utl::RightHandJustify(text_size);
                //
                //
                ImGui::AlignTextToFramePadding();
                ImGui::Text(io_time_fmt,    delta_t);
                
                ImGui::SameLine(0, ms_SMALL_ITEM_PAD);  ImGui::AlignTextToFramePadding();
                
                ImGui::Text(fps_fmt,        fps_ct);
            //  S.PopFont();
        }
        
        
        
        
        
    
        S.PopFont();
    ImGui::End();
    
    
    
    ImGui::PopStyleVar(3); // ItemSpacing, FramePadding, WindowPadding
    
    return;
}


    //  this->S.m_show_browser_window           = !this->S.m_show_browser_window;
    //  S.m_windows[ Window::Browser ].open     = !S.m_windows[ Window::Browser ].open;
    //  this->S.m_browser_ratio                 = this->S.m_show_browser_window ? app::DEF_SB_OPEN_WIDTH : 0.0f;



// *************************************************************************** //
//
//
//  2.      PROTECTED MEMBER FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //











// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
}//   END OF "cb" NAMESPACE.






// *************************************************************************** //
// *************************************************************************** //
//
//  END.
