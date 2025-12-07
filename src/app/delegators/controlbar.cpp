/***********************************************************************************
*
*       ********************************************************************
*       ****          C O N T R O L B A R . C P P  ____  F I L E        ****
*       ********************************************************************
*              AUTHOR:      Collin A. Bond.
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
ControlBar::~ControlBar(void)               { this->destroy(); }


//  "destroy"       | protected
//
void ControlBar::destroy(void)              { }




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
void ControlBar::BeginOLD( [[maybe_unused]] const char *            uuid,
                           [[maybe_unused]] bool *                  p_open,
                           [[maybe_unused]] ImGuiWindowFlags        flags )
{
    static constexpr ImVec2                 ms_WINDOW_PADDING   = ImVec2(4.0f,      4.0f);  //  Default here should be      ImVec2(8.0f, 8.0f);
    static constexpr ImVec2                 ms_FRAME_PADDING    = ImVec2(10.0f,     5.0f);
    static constexpr ImVec2                 ms_ITEM_PADDING     = ImVec2(10.0f,     5.0f);
    
    [[maybe_unused]] ImGuiIO &              io                  = ImGui::GetIO(); (void)io;
    [[maybe_unused]] ImGuiStyle &           style               = ImGui::GetStyle();
    [[maybe_unused]] ImGuiContext &         g                   = *GImGui;
    [[maybe_unused]] ImGuiMetricsConfig *   cfg                 = &g.DebugMetricsConfig;
    
    
    
    
    //  0.  SETUP NEW STYLE OPTIONS FOR CONTROL BAR WIDGETS...
    ImGui::PushStyleVar     ( ImGuiStyleVar_WindowPadding   , ms_WINDOW_PADDING                             );
    ImGui::PushStyleVar     ( ImGuiStyleVar_FramePadding    , ms_FRAME_PADDING                              );
    ImGui::PushStyleVar     ( ImGuiStyleVar_ItemSpacing     , ms_ITEM_PADDING                               );
    ImGui::PushStyleColor   ( ImGuiCol_WindowBg             , S.GetUIColor(app::UIColor::ControlBar_BG)     );      // Push before ImGui::Begin()
    
    constexpr ImGuiButtonFlags      BUTTON_FLAGS    = ImGuiButtonFlags_None;
    const float                     ROW_H           = ImGui::GetFrameHeightWithSpacing();   //  font_size + 2*FramePadding.y
    //  const float                     WIDGET_HEIGHT   = ROW_H - style.WindowPadding.y;
    
    
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
            constexpr const char *      task_fmt        = "00000000000000000000";       //  FORMAT STRINGS.
            constexpr const char *      io_time_fmt     = "%.2f ms  ";                  //  FORMAT STRINGS.
            constexpr const char *      fps_fmt         = "%.1f FPS  ";                 //  FORMAT STRINGS.
            //
            const char *                task            = S.GetCurrentAppletName();
            const float                 delta_t         = 1000 * io.DeltaTime;
            const float                 fps_ct          = io.Framerate;
            const ImVec2                task_size       = ImGui::CalcTextSize(task);
            const ImVec2                max_task_size   = ImGui::CalcTextSize(task_fmt);
            
            const ImVec2                ts              = ImVec2( max_task_size.x + ImGui::CalcTextSize(io_time_fmt).x + ImGui::CalcTextSize(fps_fmt).x, 0.0f);
            const ImVec2                TOTAL_SIZE      = ImVec2( 1.2f * ts.x, ts.y );
            
            
            ImGui::SameLine();                          utl::RightHandJustify(TOTAL_SIZE);
            //
            //
            //  TASK.
            ImGui::AlignTextToFramePadding();
            //
            switch ( this->S.GetCurrentApplet() ) {
                //  case Applet::Undefined      : { ImGui::TextDisabled( "%s", task );                              break;      }
                default                     : { ImGui::TextColored( app::DEF_APPLE_BLUE,    "%s",     task );   break;      }
            }
            //
            ImGui::SameLine(0, ms_SMALL_ITEM_PAD);      ImGui::Dummy(ImVec2( 0.65f * (max_task_size.x - task_size.x), 0));
            ImGui::SameLine(0, ms_SMALL_ITEM_PAD);
            //
            //
            //  UPDATE TIME.
            ImGui::AlignTextToFramePadding();           ImGui::SameLine();
            ImGui::Text(io_time_fmt,    delta_t);
            ImGui::SameLine(0, ms_SMALL_ITEM_PAD);      ImGui::AlignTextToFramePadding();
            //
            //
            //  FPS.
            ImGui::Text(fps_fmt,        fps_ct);
            ImGui::SameLine(0, ms_SMALL_ITEM_PAD);      ImGui::AlignTextToFramePadding();
                
        }
        
        
        
        
        
    
        S.PopFont();
    ImGui::End();
    
    
    
    ImGui::PopStyleVar(3); // ItemSpacing, FramePadding, WindowPadding
    
    return;
}


//  "Begin"
//
void ControlBar::Begin( [[maybe_unused]] const char *           uuid,
                        [[maybe_unused]] bool *                 p_open,
                        [[maybe_unused]] ImGuiWindowFlags       flags )
{
    static constexpr ImVec2                 ms_WINDOW_PADDING   = ImVec2(4.0f,      4.0f);  //  Default here should be      ImVec2(8.0f, 8.0f);
    static constexpr ImVec2                 ms_FRAME_PADDING    = ImVec2(10.0f,     5.0f);
    static constexpr ImVec2                 ms_ITEM_PADDING     = ImVec2(10.0f,     5.0f);
    
    [[maybe_unused]] ImGuiIO &              io                  = ImGui::GetIO(); (void)io;
    [[maybe_unused]] ImGuiStyle &           style               = ImGui::GetStyle();
    [[maybe_unused]] ImGuiContext &         g                   = *GImGui;
    [[maybe_unused]] ImGuiMetricsConfig *   cfg                 = &g.DebugMetricsConfig;
    
    
    
    
    //  0.  SETUP NEW STYLE OPTIONS FOR CONTROL BAR WIDGETS...
    ImGui::PushStyleVar     ( ImGuiStyleVar_WindowPadding   , ms_WINDOW_PADDING                             );
    ImGui::PushStyleVar     ( ImGuiStyleVar_FramePadding    , ms_FRAME_PADDING                              );
    ImGui::PushStyleVar     ( ImGuiStyleVar_ItemSpacing     , ms_ITEM_PADDING                               );
    ImGui::PushStyleColor   ( ImGuiCol_WindowBg             , S.GetUIColor(app::UIColor::ControlBar_BG)     );      // Push before ImGui::Begin()
    
    const float                             ROW_H               = ImGui::GetFrameHeightWithSpacing();   //  font_size + 2*FramePadding.y
    
    
    //  1.  CREATE THE WINDOW AND BEGIN APPENDING WIDGETS INTO IT...
    ImGui::SetNextWindowClass(&this->m_window_class);
    ImGui::SetNextWindowSizeConstraints( ImVec2(-1, ROW_H), ImVec2(FLT_MAX, ROW_H) );
    ImGui::Begin(uuid, p_open, flags);
    //
    //
        ImGui::PopStyleColor();
        this->draw_all();
    //
    //
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



//  "draw_all"
//
void ControlBar::draw_all(void)
{
    using                                   IconAnchor                  = utl::icon_widgets::Anchor;
    //
    static constexpr float                  ms_CONTROLBAR_ICON_SCALE    = 1.50f;
    static constexpr ImGuiHoveredFlags      HOVER_FLAGS                 = ImGuiHoveredFlags_DelayNormal;
    ImGuiStyle &                            style                       = ImGui::GetStyle();
    //
    this->SPACING                                                       = ImVec2( 0.0f,                     style.ItemSpacing.y + style.FramePadding.y      );
    this->WIDGET_SIZE                                                   = ImVec2( -1,                       ImGui::GetFrameHeight()                         );
    this->BUTTON_SIZE                                                   = ImVec2( this->WIDGET_SIZE.y,      this->WIDGET_SIZE.y                             );
    S.PushFont(Font::Small);
    


    //  0.      DRAW COLUMNS-WIDGET...
    ImGui::Columns(this->ms_NC, this->ms_UUID, ms_COLUMN_FLAGS);
    //
    //
    //
        this->S.PushFont( Font::Main );
        {
        //
        //
        //
        //  //      1.        OPEN/CLOSE SIDEBAR BROWSER...
            if ( utl::IconButton( "##ControlBar_ToggleBrowser"
                                  , (this->S.m_show_browser_window)
                                        ? this->S.SystemColor.Blue              : this->S.SystemColor.Gray
                                  , ICON_FA_LIST
                                  , ms_CONTROLBAR_ICON_SCALE
                                  , IconAnchor::Center
                                  , BUTTON_SIZE )
            )
            {
                this->S.m_show_browser_window           = !this->S.m_show_browser_window;
                S.m_windows[ Window::Browser ].open     = !S.m_windows[ Window::Browser ].open;
            }
            if ( ImGui::IsItemHovered(HOVER_FLAGS) ) {
                ImGui::BeginTooltip();
                    ImGui::TextUnformatted("Open/Close the \"Browser\" panel");
                ImGui::EndTooltip();
            }
            //
            //
            //
            //      2.        SWITCH BETWEEN BROWSER AND SYSTEM PREFERENCES...
            ImGui::SameLine(0, ms_SMALL_ITEM_PAD);
            ImGui::BeginDisabled( !this->S.m_show_browser_window );
                if ( utl::IconButton(   "##ControlBar_BrowserToggle"
                                      , this->S.SystemColor.White
                                      , (this->S.m_show_system_preferences)
                                            ? ICON_FA_FOLDER_TREE               : ICON_FA_GEARS
                                      , ms_CONTROLBAR_ICON_SCALE
                                      , IconAnchor::Center
                                      , BUTTON_SIZE )
                )
                {
                    this->S.m_show_system_preferences       = !this->S.m_show_system_preferences;
                }
            ImGui::EndDisabled();
            //
            if ( ImGui::IsItemHovered(HOVER_FLAGS) ) {
                ImGui::BeginTooltip();
                    ImGui::TextUnformatted("Toggle between \"Browser\" and \"System Preferences\" inside the Browser panel");
                ImGui::EndTooltip();
            }
            //
            //
            //
            //      3.        OPEN/CLOSE DETAIL VIEW...
            ImGui::SameLine(0, 2.0f * ms_BIG_ITEM_PAD);
            if ( utl::IconButton( "##ControlBar_ToggleDetailView"
                                  , (this->S.m_show_detview_window)
                                        ? this->S.SystemColor.Blue              : this->S.SystemColor.Gray
                                  , (this->S.m_show_detview_window)
                                        ? ICON_FA_BOOK_OPEN                     : ICON_FA_BOOK
                                        //  ? ICON_FA_MAGNIFYING_GLASS_MINUS        : ICON_FA_MAGNIFYING_GLASS_PLUS
                                  , ms_CONTROLBAR_ICON_SCALE
                                  , IconAnchor::Center
                                  , BUTTON_SIZE )
            )
            {
                this->S.m_show_detview_window           = !this->S.m_show_detview_window;
            }
            if ( ImGui::IsItemHovered(HOVER_FLAGS) ) {
                ImGui::BeginTooltip();
                    ImGui::TextUnformatted("Open/Close the \"Detail View\" panel");
                ImGui::EndTooltip();
            }
        //
        //
        //
        }
        this->S.PopFont();
        //  ImGui::SameLine(0, ms_BIG_ITEM_PAD);



        //      ?.      EMPTY SPACES FOR LATER...
        for (int i = ImGui::GetColumnIndex(); i < ms_NC - ms_NE; ++i) {
            ImGui::Dummy( ImVec2(0,0) );    ImGui::NextColumn();
        }



        //      X.      PERFORMANCE...
        {
            this->_draw_info_metrics();
        }
 
        
    //
    //
    //
    ImGui::Columns(1);      //  END COLUMNS...
    
    
    
    //      ImGui::Dummy( SPACING );
    S.PopFont();
    return;
}



//  "_draw_info_metrics"
//
void ControlBar::_draw_info_metrics(void)
{
    constexpr const char *          io_time_fmt         = "%.2f ms  ";                  //  FORMAT STRINGS.
    constexpr const char *          fps_fmt             = "%.1f FPS  ";                 //  FORMAT STRINGS.
    //
    ImGuiIO &                       io                  = ImGui::GetIO();
    //
    const char *                    task                = S.GetCurrentAppletName();
    const std::string &             nav_window_str      = S.GetNavWindowStr();
    const char *                    nav_window_name     = (nav_window_str.empty()) ? "---" : nav_window_str.c_str();
    //
    const float                     delta_t             = 1000 * io.DeltaTime;
    const float                     fps_ct              = io.Framerate;
    
    
    
    // ImGui::SameLine();                          utl::RightHandJustify(TOTAL_SIZE);
    //
    //
    //      1.      TASK.
    ImGui::AlignTextToFramePadding();
    ImGui::TextColored( app::DEF_APPLE_BLUE,    "%s",     task      );
    //
    //
    //      2.      CURRENT NAV WINDOW.
    ImGui::NextColumn();        ImGui::AlignTextToFramePadding();
    this->column_label(     "%s",               nav_window_name     );
    //
    //
    //      3.      UPDATE TIME.
    ImGui::NextColumn();        ImGui::AlignTextToFramePadding();
    ImGui::Text(            io_time_fmt,        delta_t             );
    //
    //
    //      4.      FPS.
    ImGui::NextColumn();        ImGui::AlignTextToFramePadding();
    ImGui::Text(            fps_fmt,            fps_ct              );
        
        
        
    return;
}








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
