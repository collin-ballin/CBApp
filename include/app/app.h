//
//  app.h
//  CBApp
//
//  Created by Collin Bond on 4/16/25.
//
// *************************************************************************** //
// *************************************************************************** //
#ifndef _CBAPP_APP_H
#define _CBAPP_APP_H  1



//  1.  INCLUDES    | Headers, Modules, etc...
// *************************************************************************** //
// *************************************************************************** //

//  0.1.        ** MY **  HEADERS...
#include "_config.h"
#include "cblib.h"
#include "utility/utility.h"
#include "widgets/widgets.h"
#include "app/_init.h"
#include "app/state/_state.h"
#include "app/_graphing_app.h"
#include "app/menubar/_menubar.h"



//  0.2     STANDARD LIBRARY HEADERS...
#include <iostream>         //  <======| std::cout, std::cerr, std::endl, ...
#include <cstdlib>          // C-Headers...
#include <stdio.h>
#include <unistd.h>


#include <string>           //  <======| std::string, ...
#include <string_view>
#include <vector>           //  <======| std::vector, ...
#include <stdexcept>        //  <======| ...
#include <limits.h>
#include <math.h>


#ifdef DEBUG                //  <======| Fix for issue wherein multiple instances of application
# include <thread>          //           are launched when DEBUG build is run inside Xcode IDE...
# include <chrono>
#endif     //  DEBUG  //



//  0.3     "DEAR IMGUI" HEADERS...
#include "imgui.h"
#include "implot.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#if defined(IMGUI_IMPL_OPENGL_ES2)
# include <GLES2/gl2.h>
#endif      //  IMGUI_IMPL_OPENGL_ES2  //

#include <GLFW/glfw3.h>     //  <======| Will drag system OpenGL headers

//  [Win32]     Our example includes a copy of glfw3.lib pre-compiled with VS2010 to maximize ease of testing and compatibility with old VS compilers.
//                  To link with VS2010-era libraries, VS2015+ requires linking with legacy_stdio_definitions.lib, which we do using this pragma.
//                  Your own project should not be affected, as you are likely to link with a newer binary of GLFW that is adequate for your version of Visual Studio.
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
# pragma comment(lib, "legacy_stdio_definitions")
#endif

//              This example can also compile and run with Emscripten! See 'Makefile.emscripten' for details.
#ifdef __EMSCRIPTEN__
# include "../../libs/emscripten/emscripten_mainloop_stub.h"
#endif






namespace cb { //     BEGINNING NAMESPACE "cb"...
// *************************************************************************** //
// *************************************************************************** //

//  0.      UTILITY FUNCTIONS [NON-MEMBER FUNCTIONS]...
int                 run_application             ([[maybe_unused]] int argc, [[maybe_unused]] char ** argv);

//  DEAR IMGUI DEMO APPLICATIONS...
void                ShowStyleEditor             ([[maybe_unused]] const char *,     [[maybe_unused]] bool *,    [[maybe_unused]] ImGuiWindowFlags);
void                ShowExampleAppLog           ([[maybe_unused]] const char *,     [[maybe_unused]] bool *,    [[maybe_unused]] ImGuiWindowFlags);
void                ShowExampleAppConsole       ([[maybe_unused]] const char *,     [[maybe_unused]] bool *,    [[maybe_unused]] ImGuiWindowFlags);
void                ShowMetricsWindow           ([[maybe_unused]] const char *,     [[maybe_unused]] bool *,    [[maybe_unused]] ImGuiWindowFlags);

void                ShowImGuiDemoWindow         ([[maybe_unused]] const char *,     [[maybe_unused]] bool *,    [[maybe_unused]] ImGuiWindowFlags);
void                ShowImPlotDemoWindow        ([[maybe_unused]] const char *,     [[maybe_unused]] bool *,    [[maybe_unused]] ImGuiWindowFlags);




// *************************************************************************** //
// *************************************************************************** //
//                PRIMARY CLASS INTERFACE:
// 		Class-Interface for the Primary Application.
// *************************************************************************** //
// *************************************************************************** //

class App
{
// *************************************************************************** //
// *************************************************************************** //
public:
    using               AppState                        = app::AppState;
    using               Window                          = AppState::Window;
    using               ImWindows                       = AppState::ImWindows;
    using               Font                            = AppState::Font;
    using               ImFonts                         = AppState::ImFonts;  
    
    //  1               PUBLIC MEMBER FUNCTIONS...
    // *************************************************************************** //
    //  1.1             Default Constructor, Destructor, etc...
                        App                     (void);                                     //  Def. Constructor.
                        ~App                    (void);                                     //  Def. Destructor.
    
    //  1.2             Public Member Functions...
    void                run                     (void);

    //  1.3             Deleted Operators, Functions, etc...
                        App                     (const App & src)               = delete;   //  Copy. Constructor.
                        App                     (App && src)                    = delete;   //  Move Constructor.
    App &               operator =              (const App & src)               = delete;   //  Assgn. Operator.
    App &               operator =              (App && src)                    = delete;   //  Move-Assgn. Operator.
    
    
// *************************************************************************** //
// *************************************************************************** //
protected:
    //  2.A             PROTECTED DATA-MEMBERS...
    // *************************************************************************** //
    
    //                  1.  BOOLEANS...
    bool                m_running                       = true;
    //
    //                          Sidebar / System Preferences.
    bool                m_show_sidebar_window           = true;
    bool                m_show_perf_metrics             = app::DEF_PERF_METRICS_STATE;
    bool                m_show_perf_plots               = app::DEF_PERF_PLOTS_STATE;
    //
    //                          Main Applications.
    //
    //                          Dear ImGui Applications.
    
    
    //                  2.  APPEARANCE...
    //                          Dimensions.
    uint32_t            m_sys_width                     = 0U;       //  Sys. Display Dims.
    uint32_t            m_sys_height                    = 0U;
    int                 m_window_w                      = -1;       //  Main Window Dims.
    int                 m_window_h                      = -1;
    ImVec2              m_sidebar_width                 = ImVec2(40.0f, 400.0f);
    float               m_sidebar_ratio                 = app::DEF_SB_OPEN_WIDTH;
    bool                m_rebuild_dockspace             = true;
    ImGuiID             m_dockspace_id                  = 0;
    std::vector<std::string> m_primary_windows          = { };
    
    //
    //                          Colors.
    ImVec4              m_dock_bg                       = cb::app::DEF_INVISIBLE_COLOR;
    ImVec4              m_glfw_bg                       = cb::app::DEF_ROOT_WIN_BG;
    ImVec4              m_sidebar_bg                    = cb::app::DEF_SIDEBAR_WIN_BG;
    ImVec4              m_main_bg                       = cb::app::DEF_MAIN_WIN_BG;
    
    
    //                  3.  WINDOW / GUI ITEMS...           //  ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize
    ImGuiConfigFlags    m_io_flags                      = ImGuiConfigFlags_None | ImGuiConfigFlags_NavEnableKeyboard | ImGuiConfigFlags_NavEnableGamepad | ImGuiConfigFlags_DockingEnable | ImGuiConfigFlags_ViewportsEnable;
#ifdef CBAPP_ENABLE_MOVE_AND_RESIZE
    ImGuiWindowFlags    m_host_win_flags                = ImGuiWindowFlags_None | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
                                                          ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoBackground;
# else
    ImGuiWindowFlags    m_host_win_flags                = ImGuiWindowFlags_None | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
                                                          ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoBackground;
#endif  //  CBAPP_ENABLE_MOVE_AND_RESIZE  //
    
    
    //                  4.  MISC INFORMATION...
    const char *        m_dock_name                     = "RootDockspace";
    const char *        m_glsl_version                  = nullptr;
    
    
    //                  5.  IMPORTANT VARIABLES...
    ImGuiViewport *     m_main_viewport                 = nullptr;
    GLFWwindow *        m_window                        = nullptr;
    
    //                  6.  DELAGATOR CLASSES...
    app::AppState       m_state                         = app::AppState();
    MenuBar             m_menubar;
    GraphingApp         m_graphing_app;
    
    
    //                  9.  APPLICATION SUB-CLASSES...
#ifdef CBAPP_ENABLE_CB_DEMO
    CBDemo              m_cb_demo                       = CBDemo();
#endif  //  CBAPP_ENABLE_CB_DEMO  //
    
    
    //  2.B             PROTECTED MEMBER FUNCTIONS...
    // *************************************************************************** //
    
    //  2B.1            Class Initializations.      [app.cpp]...
    void                init                        (void);
    void                load                        (void);
    void                dispatch_window_function    (const Window & uuid);
    void                destroy                     (void);
    
    
    
// *************************************************************************** //
// *************************************************************************** //
private:
    //  3.              PRIVATE MEMBER FUNCTIONS...
    // *************************************************************************** //
    
    //  3.2             Main GUI Functions.         [app.cpp]...
    void                run_IMPL                    (void);                                                         //  [app.cpp].
    void                Display_Main_Window         (const char *,      bool *,     ImGuiWindowFlags);              //  [app.cpp].
    void                CoincidenceCounter          (void);                                                         //  [main_application.cpp].
    void                Display_Sidebar_Menu        (const char *,      bool *,     ImGuiWindowFlags);              //  [main_application.cpp].
    
    
    
    //  3.2             Tools / Debugging Apps.     [app.cpp]...
    
    
    
    
    //                  SIDEBAR MENU...             [sidebar.cpp].
    void                Display_Preferences_Menu    (void);
    //
    void                disp_appearance_mode        (void);     //  Other...
    void                disp_font_selector          (void);
    void                disp_color_palette          (void);
    void                disp_performance_metrics    (void);
    
    
    
    
    
    
    //  3.3             Primary Menu Functions.     [interface.cpp]...
    void                disp_graphing_app           (void);
    
    
    
    //  3.4             Secondary Menu Functions.   [interface.cpp]...
    
    
    //  3.5             Primary GUI Functions.      [main_application.cpp]...
    void                ImPlot_Testing1             (void);
    void                ImPlot_Testing2             (void);
    void                ImPlot_Testing3             (void);
    
    
    //  3.6             Misc. GUI Functions.        [temp.cpp]...
    void                RebuildDockLayout           (void);
    void                PushFont                    ( [[maybe_unused]] const Font & );
    void                PopFont                     (void);
    void                Test_Basic_Widgets          (void);
    void                ImGui_Demo_Code             (void);



// *************************************************************************** //
// *************************************************************************** //
};//	END MATRIX INLINE CLASS DEFINITION.






// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
}//   END OF "cb" NAMESPACE.






#endif      //  _CBAPP_APP_H  //
// *************************************************************************** //
// *************************************************************************** //
//
//  END.

