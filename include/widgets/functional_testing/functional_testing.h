/***********************************************************************************
*
*       **************************************************************************
*       ****      F U N C T I O N A L _ T E S T I N G . H  ____  F I L E      ****
*       **************************************************************************
*
*              AUTHOR:      Collin A. Bond
*               DATED:      July 23, 2025.
*
*       ********************************************************************
*                FILE:      [./PyStream.h]
*
*
*
**************************************************************************************
**************************************************************************************/
#ifndef _CBWIDGETS_FUNCTIONAL_TESTING_H
#define _CBWIDGETS_FUNCTIONAL_TESTING_H  1



//  1.  INCLUDES    | Headers, Modules, etc...
// *************************************************************************** //
// *************************************************************************** //

//  0.1.        ** MY **  HEADERS...
#include CBAPP_USER_CONFIG
#include "cblib.h"
#include "utility/utility.h"
#include "widgets/functional_testing/_types.h"
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



namespace cb { namespace ui { //     BEGINNING NAMESPACE "cb::ui"...
// *************************************************************************** //
// *************************************************************************** //



namespace ft {  //     BEGINNING NAMESPACE "ft"...
// *************************************************************************** //
// *************************************************************************** //




// *************************************************************************** //
// *************************************************************************** //
//                PRIMARY CLASS INTERFACE:
// 		Class-Interface for the "ActionExecutor" Abstraction.
// *************************************************************************** //
// *************************************************************************** //

struct ActionExecutor
{
//  0.              CONSTANTS AND ALIASES...
// *************************************************************************** //
// *************************************************************************** //

    // *************************************************************************** //
    //      NESTED TYPENAME ALIASES.
    // *************************************************************************** //
    using                               State                           = ExecutionState;
    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      STATIC CONSTEXPR CONSTANTS.
    // *************************************************************************** //
    static constexpr float              ms_MIN_DURATION_S               = 0.001f;
    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      REFERENCES TO GLOBAL ARRAYS.
    // *************************************************************************** //
    //static constexpr auto &             ms_STATE_NAMES                  = DEF_MYCLASS_STATE_NAMES;
    
//
//
// *************************************************************************** //
// *************************************************************************** //   END "CONSTANTS AND ALIASES".



// *************************************************************************** //
//
//  1.              CLASS DATA-MEMBERS...
// *************************************************************************** //
// *************************************************************************** //

    // *************************************************************************** //
    //      IMPORTANT DATA-MEMBERS.
    // *************************************************************************** //
    GLFWwindow*                         m_window                        { nullptr };
    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      OTHER DATA MEMBERS.
    // *************************************************************************** //
    ImVec2                              m_first_pos                     {  };        // starting mouse-cursor pos
    ImVec2                              m_last_pos                      {  };        // destination mouse-cursor pos
    float                               m_duration_s                    { 0.0f };  // total move time
    float                               m_elapsed_s                     { 0.0f };  // accumulated time
    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      STATE VARIABLES.
    // *************************************************************************** //
    State                               m_state                         { State::None };
    
//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "CLASS DATA-MEMBERS".



// *************************************************************************** //
//
//
//  2.A.            PUBLIC MEMBER FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //
    
    // *************************************************************************** //
    //      INITIALIZATION METHODS.         |   "init.cpp" ...
    // *************************************************************************** //
                                        ActionExecutor          (void) noexcept     = default;
    explicit                            ActionExecutor          (GLFWwindow * window) noexcept;
                                        ~ActionExecutor         (void);
                                        
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      MAIN API.                       |   "interface.cpp" ...
    // *************************************************************************** //
    void                                Begin                               (const char * id = "##EditorCanvas");
    void                                start_cursor_move                   (GLFWwindow * window, ImVec2 first, ImVec2 last, float duration_s);
    void                                start_button_action                 (GLFWwindow * window, ImGuiKey key, bool ctrl, bool shift, bool alt);
    //
    //
    void                                abort                               (void);
    void                                busy                                (void);
    void                                update                              (void);
    
//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "PUBLIC MEMBER FUNCS".



// *************************************************************************** //
//
//
//  2.C                 INLINE FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

    // *************************************************************************** //
    //      CENTRALIZED STATE MANAGEMENT FUNCTIONS.
    // *************************************************************************** //
    
    //  "reset_all"
    inline void                         reset                               (void) noexcept
    { this->reset_state; this->reset_data; }
    
    //  "reset_state"
    inline void                         reset_state                         (void) noexcept
    { this->m_state = State::None; }
    
    //  "reset_data"
    inline void                         reset_data                          (void) noexcept
    { this->m_data.clear(); }
    
    
    
    // *************************************************************************** //
    
//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "INLINE" FUNCTIONS.






// *************************************************************************** //
// *************************************************************************** //
};//	END "ActionExecutor" STRUCT DEFINITION.









// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
} //   END OF "ft" NAMESPACE.


















// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
} } //   END OF "cb::ui" NAMESPACE.






#endif      //  _CBWIDGETS_FUNCTIONAL_TESTING_H  //
// *************************************************************************** //
// *************************************************************************** //
//
//  END.
