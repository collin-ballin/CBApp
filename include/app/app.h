/***********************************************************************************
*
*       ********************************************************************
*       ****                  A P P . H  ____  F I L E                  ****
*       ********************************************************************
*
*              AUTHOR:      Collin A. Bond
*               DATED:      April 16, 2025.
*
*       ********************************************************************
*                FILE:      [./PyStream.h]
*
*
*
**************************************************************************************
**************************************************************************************/
#ifndef _CBAPP_APP_H
#define _CBAPP_APP_H  1



//  1.  INCLUDES    | Headers, Modules, etc...
// *************************************************************************** //
// *************************************************************************** //

//  0.1.        ** MY **  HEADERS...
#include CBAPP_USER_CONFIG
#include "cblib.h"
#include "utility/utility.h"
#include "utility/pystream/pystream.h"
#include "widgets/widgets.h"
#include "app/state/state.h"
//  #include "widgets/functional_testing/functional_testing.h"
//
#include "app/c_counter/c_counter.h"        //  Seperate Application Classes.
#include "app/editor_app/editor_app.h"
#include "app/graph_app/graph_app.h"
//
#include "app/delegators/_menubar.h"        //  Delegator Classes.
#include "app/delegators/_browser.h"
#include "app/delegators/_controlbar.h"
#include "app/delegators/_detail_view.h"
//#include "app/titlebar/_titlebar.h"



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
#include "imgui_stdlib.h"
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

//  0.      UTILITY FUNCTIONS [NON-MEMBER FUNCTIONS]        [handler.cpp]...
int                     run_application             ([[maybe_unused]] int argc, [[maybe_unused]] char ** argv);


[[maybe_unused]]
void                    KeepProgramAwake            (GLFWwindow * );


//  DEAR IMGUI DEMO APPLICATIONS...
//
void                    ShowCustomRendering         ([[maybe_unused]] const char *,     [[maybe_unused]] bool *,    [[maybe_unused]] ImGuiWindowFlags);
void                    ShowExampleAppLog           ([[maybe_unused]] const char *,     [[maybe_unused]] bool *,    [[maybe_unused]] ImGuiWindowFlags);
void                    ShowExampleAppConsole       ([[maybe_unused]] const char *,     [[maybe_unused]] bool *,    [[maybe_unused]] ImGuiWindowFlags);
void                    ShowExampleAppDocuments     ([[maybe_unused]] const char *,     [[maybe_unused]] bool *,    [[maybe_unused]] ImGuiWindowFlags);




// *************************************************************************** //
// *************************************************************************** //
//                PRIMARY CLASS INTERFACE:
// 		Class-Interface for the Primary Application.
// *************************************************************************** //
// *************************************************************************** //

class App
{
//  0.              CONSTANTS AND ALIASES...
// *************************************************************************** //
// *************************************************************************** //
public:

    // *************************************************************************** //
    //      NESTED TYPENAME ALIASES.
    // *************************************************************************** //
    
    CBAPP_APPSTATE_ALIAS_API        //  CLASS-DEFINED, NESTED TYPENAME ALIASES.
    using                       WinRenderFn                     = std::function<void(const char *, bool *, ImGuiWindowFlags)>;
    
//
//
// *************************************************************************** //
// *************************************************************************** //   END "CONSTANTS AND ALIASES".



// *************************************************************************** //
//
//
//      1.              CLASS DATA-MEMBERS...
// *************************************************************************** //
// *************************************************************************** //
protected:
    // *************************************************************************** //
    //      DELAGATOR CLASSES.              |
    // *************************************************************************** //
    app::AppState &             CBAPP_STATE_NAME                = app::AppState::instance(); //app::AppState();
    MenuBar                     m_menubar;
    ControlBar                  m_controlbar;
    Browser                     m_browser;
    DetailView                  m_detview;
    // *************************************************************************** //
    //
    //
    //
    // *************************************************************************** //
    //      APPLICATION SUB-CLASSES.        |
    // *************************************************************************** //
    CCounterApp                 m_counter_app;
    EditorApp                   m_editor_app;
    GraphApp                    m_graph_app;
#ifdef CBAPP_ENABLE_FUNCTIONAL_TESTING
    ui::ActionComposer          m_composer;
#endif  //  CBAPP_ENABLE_FUNCTIONAL_TESTING  //
#ifdef CBAPP_ENABLE_CB_DEMO
    CBDemo                      m_cb_demo                       = CBDemo();
#endif  //  CBAPP_ENABLE_CB_DEMO  //
//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "DATA-MEMBERS".



// *************************************************************************** //
//
//
//  2.A.            PUBLIC API...
// *************************************************************************** //
// *************************************************************************** //
public:
    
    // *************************************************************************** //
    //  1.1             INLINE PUBLIC MEMBER FUNCTIONS...
    // *************************************************************************** //
    
    //  "instance"              | Meyers-Style Singleton.       Created on first call, once.
    static inline App &         instance                    (void)
    { static App   single = App();     return single; }
    
    
    //  "enqueue_signal"        | Called only by async handlers (thread / signal safe)
    inline void                 enqueue_signal              (CBSignalFlags f) noexcept
    { S.m_pending.fetch_or(f, std::memory_order_relaxed); }
    
    

    // *************************************************************************** //
    //  1.2             PUBLIC API...
    // *************************************************************************** //
    void                        run                         (void);
    //
    //                      DELETED OPERATORS AND FUNCTIONS:
                                App                         (const App &    )               = delete;   //  Copy. Constructor.
                                App                         (App &&         )               = delete;   //  Move Constructor.
    App &                       operator =                  (const App &    )               = delete;   //  Assgn. Operator.
    App &                       operator =                  (App &&         )               = delete;   //  Move-Assgn. Operator.
    
    // *************************************************************************** //
    //      PRIVATE CLASS INITIALIZATIONS.  |   "init.cpp" ...
    // *************************************************************************** //
private:
                                App                         (void);                     //  Def. Constructor.
                                ~App                        (void);                     //  Def. Destructor.
    // *************************************************************************** //
    
//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "PUBLIC API".
    

    
// *************************************************************************** //
//
//
//      2.A             PROTECTED MEMBER FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //
protected:
//
    // *************************************************************************** //
    //      CLASS INITIALIZATIONS.          |   "init.cpp" ...
    // *************************************************************************** //
    //                      ADDITIONAL INIT. FUNCTIONS:
    void                        init                        (void);                     //  [init.cpp].
    void                        CreateContext               (void);                     //  [init.cpp].
    void                        destroy                     (void);                     //  [init.cpp].
    //
    //                      SUB-CLASS INIT. FUNCTIONS:
    void                        init_appstate               (void);                     //  [init.cpp].
    [[nodiscard]] WinRenderFn   dispatch_window_function    (const Window & uuid);      //  [init.cpp].
    //
    //                      RUNTIME INIT. FUNCTIONS:
    void                        load                        (void);                     //  [init.cpp].
    bool                        init_asserts                (void);                     //  [init.cpp].
    static void                 install_signal_handlers     (void);                     //  [handler.cpp].
    // *************************************************************************** //
    //
    //
    //
    // *************************************************************************** //
    //      MAIN GUI FUNCTIONS.             |   "app.cpp" ...
    // *************************************************************************** //
    //                      MAIN GUI FUNCTIONS:
    void                        ShowMainWindow              ([[maybe_unused]] const char *,     [[maybe_unused]] bool *,    [[maybe_unused]] ImGuiWindowFlags);
    void                        ShowDockspace               ([[maybe_unused]] const char *,     [[maybe_unused]] bool *,    [[maybe_unused]] ImGuiWindowFlags);
    void                        ShowAboutWindow             ([[maybe_unused]] const char *,     [[maybe_unused]] bool *,    [[maybe_unused]] ImGuiWindowFlags);
    //
    //                      UTILITIES FOR MAIN GUI FUNCTIONS:
    void                        run_IMPL                    (void);
    void                        show_about_info             (void) const;
    //
    void                        get_cbapp_info              (void) const;
    void                        get_build_info              (void) const;
    void                        get_config_info             (void) const;
    void                        get_imgui_info              (void) const;
    // *************************************************************************** //
    //
    //
    //
    // *************************************************************************** //
    //      TOOLS / EXTRA APPLICATIONS.     |   "tools.cpp" ...
    // *************************************************************************** //                           //  [main_application.cpp].
    void                        ShowColorTool               ([[maybe_unused]] const char *,     [[maybe_unused]] bool *,    [[maybe_unused]] ImGuiWindowFlags);
    void                        ColorShaderTool             (void);
    void                        ColorMapCreatorTool         (void);
    // *************************************************************************** //
    //
    //
    //
    // *************************************************************************** //
    //      UTILITY FUNCTIONS.              |   "main_application.cpp" ...
    // *************************************************************************** //
    //                      APPLICATION SIGNAL & HOTKEY HANDLERS:
    void                        KeyboardShortcutHandler     (void);
    inline void                 DialogHandler               (void);
    void                        SaveHandler                 (void);
    void                        OpenHandler                 (void);
    void                        CopyHandler                 (void);
    void                        PasteHandler                (void);
    void                        UndoHandler                 (void);
    void                        RedoHandler                 (void);
    void                        QuerySignalStates           (void);
    //
    //                      PLACEHOLDERS:
    void                        SaveHandler_Default         (void);
    void                        CopyHandler_Default         (void);
    void                        PasteHandler_Default        (void);
    void                        UndoHandler_Default         (void);
    void                        RedoHandler_Default         (void);
    //
    //                      MISC. GUI FUNCTIONS:
    void                        InitDockspace               (void);
    void                        RebuildDockLayout           (void);
    void                        OnDpiScaleChanged           ([[maybe_unused]] float xs, [[maybe_unused]] float ys);
    // *************************************************************************** //
    //
    //
    //
    // *************************************************************************** //
    //      TEMPORARY / TESTING FUNCTIONS.  |   "main_application.cpp" ...
    // *************************************************************************** //
    void                        PyStreamTest                (void);
    void                        TestTabBar                  (void);
    //
    void                        ShowImGuiMetricsWindow      ([[maybe_unused]] const char *,     [[maybe_unused]] bool *,    [[maybe_unused]] ImGuiWindowFlags);
    void                        ShowImGuiDemoWindow         ([[maybe_unused]] const char *,     [[maybe_unused]] bool *,    [[maybe_unused]] ImGuiWindowFlags);
    void                        ShowImGuiStyleEditor        ([[maybe_unused]] const char *,     [[maybe_unused]] bool *,    [[maybe_unused]] ImGuiWindowFlags);
    //
    void                        ShowImPlotStyleEditor       ([[maybe_unused]] const char *,     [[maybe_unused]] bool *,    [[maybe_unused]] ImGuiWindowFlags);
    void                        ShowImPlotMetricsWindow     ([[maybe_unused]] const char *,     [[maybe_unused]] bool *,    [[maybe_unused]] ImGuiWindowFlags);
    void                        ShowImPlotDemoWindow        ([[maybe_unused]] const char *,     [[maybe_unused]] bool *,    [[maybe_unused]] ImGuiWindowFlags);
    // *************************************************************************** //
    //
    //
    //
    // *************************************************************************** //
    //      PRIVATE MEMBER FUNCTIONS.       |   "main_application.cpp" ...
    // *************************************************************************** //
private:
    void                        ImPlot_Testing0             (void);
    void                        ImPlot_Testing0ALT          (void);
    void                        Test_Browser                (void);
    void                        Test_Editor                 (void);
    // *************************************************************************** //
    //
    //
    //
    // *************************************************************************** //
    //      UNIT TESTING STUFF.             |   "tests.cpp" ...
    // *************************************************************************** //
    void                        BeginFunctionalTesting      ([[maybe_unused]] const char *,     [[maybe_unused]] bool *,    [[maybe_unused]] ImGuiWindowFlags);
    
//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "PROTECTED" FUNCTIONS.

    
   
// *************************************************************************** //
//
//
//  2.C                 INLINE FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //
protected:

    //  "PerFrameCache"
    //
    //      TO-DO:
    //          - This needs to be a centralized function that will be called  ONE-TIME  at the beginning of  EACH FRAME.
    //          - It will ensure that each data-member of the App State is Up-To-Date and prepared for usage across any opteration
    //          that make take place across the coming frame.
    //
    inline void                         PerFrameCache               (void)
    {
    
    
        return;
    }

    
    
    
    // *************************************************************************** //
    
//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "INLINE" FUNCTIONS.






// *************************************************************************** //
// *************************************************************************** //
};//	END "App" CLASS DEFINITION.












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

