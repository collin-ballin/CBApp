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
#include "utility/pystream/pystream.h"
#include "widgets/widgets.h"
#include "app/_init.h"
#include "app/state/_state.h"
//
#include "app/_graphing_app.h"              //  Seperate Application Classes.
#include "app/c_counter/c_counter.h"            
//
#include "app/menubar/_menubar.h"           //  Delegator Classes.
#include "app/sidebar/_sidebar.h"
#include "app/titlebar/_titlebar.h"



//  0.2     STANDARD LIBRARY HEADERS...
#include <iostream>         //  <======| std::cout, std::cerr, std::endl, ...
#include <cstdlib>          // C-Headers...
#include <stdio.h>
#include <unistd.h>


#include <string>           //  <======| std::string, ...
#include <string_view>
#include <format>
#include <vector>           //  <======| std::vector, ...
#include <stdexcept>        //  <======| ...
#include <limits.h>
#include <math.h>


#ifdef __CBAPP_DEBUG__      //  <======| Fix for issue wherein multiple instances of application
# include <thread>          //           are launched when DEBUG build is run inside Xcode IDE...
# include <chrono>
#endif     //  __CBAPP_DEBUG__  //



//  0.3     "DEAR IMGUI" HEADERS...
#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "implot.h"
#if defined(IMGUI_IMPL_OPENGL_ES2)
# include <GLES2/gl2.h>
#endif      //  IMGUI_IMPL_OPENGL_ES2  //

#include <GLFW/glfw3.h>     //  <======| Will drag system OpenGL headers

//  [Win32]     Our example includes a copy of glfw3.lib pre-compiled with VS2010 to maximize ease of testing and compatibility with old VS compilers.
//                  - To link with VS2010-era libraries, VS2015+ requires linking with legacy_stdio_definitions.lib, which we do using this pragma.
//                  - Your own project should not be affected, as you are likely to link with a newer binary of GLFW that is adequate for your version of Visual Studio.
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
[[maybe_unused]]
void                KeepProgramAwake            (GLFWwindow * );


//  DEAR IMGUI DEMO APPLICATIONS...
void                ShowStyleEditor             ([[maybe_unused]] const char *,     [[maybe_unused]] bool *,    [[maybe_unused]] ImGuiWindowFlags);
void                ShowExampleAppLog           ([[maybe_unused]] const char *,     [[maybe_unused]] bool *,    [[maybe_unused]] ImGuiWindowFlags);
void                ShowExampleAppConsole       ([[maybe_unused]] const char *,     [[maybe_unused]] bool *,    [[maybe_unused]] ImGuiWindowFlags);
void                ShowMetricsWindow           ([[maybe_unused]] const char *,     [[maybe_unused]] bool *,    [[maybe_unused]] ImGuiWindowFlags);
void                ShowExampleAppDockSpace     ([[maybe_unused]] const char *,     [[maybe_unused]] bool *,    [[maybe_unused]] ImGuiWindowFlags);

void                ShowImGuiDemoWindow         ([[maybe_unused]] const char *,     [[maybe_unused]] bool *,    [[maybe_unused]] ImGuiWindowFlags);
void                ShowImPlotDemoWindow        ([[maybe_unused]] const char *,     [[maybe_unused]] bool *,    [[maybe_unused]] ImGuiWindowFlags);
void                ShowAboutWindow             ([[maybe_unused]] const char *,     [[maybe_unused]] bool *,    [[maybe_unused]] ImGuiWindowFlags);




// *************************************************************************** //
// *************************************************************************** //
//                PRIMARY CLASS INTERFACE:
// 		Class-Interface for the Primary Application.
// *************************************************************************** //
// *************************************************************************** //

class App
{
    CBAPP_APPSTATE_ALIAS_API        //  CLASS-DEFINED, NESTED TYPENAME ALIASES.
// *************************************************************************** //
// *************************************************************************** //
public:
    //  1               PUBLIC MEMBER FUNCTIONS...
    // *************************************************************************** //
    //  1.1             Default Constructor, Destructor, etc...
                        App                         (void);                                     //  Def. Constructor.
                        ~App                        (void);                                     //  Def. Destructor.
    
    //  1.2             Public Member Functions...
    void                run                         (void);

    //  1.3             Deleted Operators, Functions, etc...
                        App                         (const App & src)               = delete;   //  Copy. Constructor.
                        App                         (App && src)                    = delete;   //  Move Constructor.
    App &               operator =                  (const App & src)               = delete;   //  Assgn. Operator.
    App &               operator =                  (App && src)                    = delete;   //  Move-Assgn. Operator.
    
    
// *************************************************************************** //
// *************************************************************************** //
protected:
    //  2.A             PROTECTED DATA-MEMBERS...
    // *************************************************************************** //
    
    //                  1.  BOOLEANS...
    //                          ...
    
    
    //                  2.  APPEARANCE...
    //                          ...
    
    

    
    
    //                  4.  MISC INFORMATION...
    //                          ...
    
    
    //                  5.  IMPORTANT VARIABLES...
    //                          ...
    
    
    //                  6.  DELAGATOR CLASSES...
    app::AppState       CBAPP_STATE_NAME                = app::AppState();
    SideBar             m_sidebar;
    TitleBar            m_titlebar;
    MenuBar             m_menubar;
    //utl::PyStream       m_pystream                      = utl::PyStream(app::PYTHON_FILEPATH, );
    
    //                  9.  APPLICATION SUB-CLASSES...
    GraphingApp         m_graphing_app;
    CCounterApp         m_counter_app;
#ifdef CBAPP_ENABLE_CB_DEMO
    CBDemo              m_cb_demo                       = CBDemo();
#endif  //  CBAPP_ENABLE_CB_DEMO  //
    
    
    //  2.B             PROTECTED MEMBER FUNCTIONS...
    // *************************************************************************** //
    
    //  2B.1            Class Initializations.      [init.cpp]...
    //              1A.
    void                init                        (void);                     //  [init.cpp].
    void                CreateContext               (void);                     //  [init.cpp].
    void                destroy                     (void);                     //  [init.cpp].
    //              1B.
    void                init_appstate               (void);                     //  [init.cpp].
    void                dispatch_window_function    (const Window & uuid);      //  [init.cpp].
    //              1C.
    void                load                        (void);                     //  [init.cpp].
    void                init_asserts                (void);                     //  [init.cpp].
    
    
    //  2C.1            Main GUI Functions.         [app.cpp]...
    void                run_IMPL                    (void);                                                         //  [app.cpp].
    void                ShowMainWindow              ([[maybe_unused]] const char *,     [[maybe_unused]] bool *,    [[maybe_unused]] ImGuiWindowFlags);
    void                ShowDockspace               ([[maybe_unused]] const char *,     [[maybe_unused]] bool *,    [[maybe_unused]] ImGuiWindowFlags);
    void                TestTabBar                  (void);
    void                ShowAboutWindow             ([[maybe_unused]] const char *,     [[maybe_unused]] bool *,    [[maybe_unused]] ImGuiWindowFlags);
    
    
    //  2D.1            Primary Menu Functions.     [interface.cpp]...
    void                stream_test                 (void);                                                         //  [main_application.cpp].
    void                ShowColorTool               ([[maybe_unused]] const char * ,    [[maybe_unused]] bool * ,   [[maybe_unused]] ImGuiWindowFlags);
    
    
    //  2E.1            Set-Up GUI Functions.       [app.cpp]...
    void                RebuildDockLayout           (void);
    
    
    
// *************************************************************************** //
// *************************************************************************** //
private:
    //  3.              PRIVATE MEMBER FUNCTIONS...
    // *************************************************************************** //
    
    //  3.1             Primary GUI Functions.      [main_application.cpp]...
    void                ImPlot_Testing1             (void);
    void                ImPlot_Testing2             (void);
    void                ImPlot_Testing3             (void);
    
    //  3.2             Misc. GUI Functions.        [temp.cpp]...
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

