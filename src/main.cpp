/******************************************************************************
*
*       ************************************************************
*       ****          M A I N . C P P  ____  F I L E            ****
*       ************************************************************
*
*              AUTHOR:      Collin A. Bond.
*           PROFESSOR:      Dr. Erik J. Sánchez.
*              COURSE:      N/A-XXX.
*               DATED:      April 15, 2025.
*
*       ************************************************************
*
*       Dear ImGui: standalone example application for GLFW + OpenGL 3, using programmable pipeline
*       (GLFW is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan/Metal graphics context creation, etc.)
*
*       Learn about Dear ImGui:
*           - FAQ                  https://dearimgui.com/faq
*           - Getting Started      https://dearimgui.com/getting-started
*           - Documentation        https://dearimgui.com/docs (same as your local docs/ folder).
*           - Introduction, links and more at the top of imgui.cpp
*
*
******************************************************************************
******************************************************************************/

//  0.1     "DEAR IMGUI" HEADERS...
#include "main.h"           //  <======| MY HEADERS...



//  "main"
//
int main(int argc, char ** argv)
{
    int     status      = 0;
    status              = cb::run_application(argc, argv);
    
    return status;
}





// *************************************************************************** //
// *************************************************************************** //
//
//  END.



























/*

#include "main.h"           //  <======| MY HEADERS...
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>
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
# include "../libs/emscripten/emscripten_mainloop_stub.h"
#endif


#ifdef CBAPP_DEBUG                //  <======| Fix for issue wherein multiple instances of application
# include <thread>                //           are launched when DEBUG build is run inside Xcode IDE...
# include <chrono>
#endif     //  CBAPP_DEBUG  //


// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //

//  "main"
//
int main(int argc, char ** argv)
{
    constexpr const char *  xcp_header              = "MAIN | ";
    constexpr const char *  xcp_type_runtime        = "Caught std::runtime_error exception";
    constexpr const char *  xcp_type_unknown        = "Caught std::runtime_error exception";
    constexpr const char *  xcp_at_start            = "while initializing the application.\n";
    constexpr const char *  xcp_at_runtime          = "during program runtime.\n";
    int                     status                  = EXIT_SUCCESS;
    
    
#ifdef CBAPP_DEBUG    //  WORK-AROUND / PATCH FOR XCODE ISSUE...
    std::this_thread::sleep_for(std::chrono::seconds(1));
#endif

    try {   //  1.     CREATE APPLICATION INSTANCE...
        cb::App my_app;
        
            try {   //  2.     ENTER APPLICATION MAIN-LOOP...
                my_app.run();
            }
            catch (const std::runtime_error & e) {      //  2.1     CATCH RUNTIME ERROR (PROGRAM WAS INITIALIZED CORRECTLY)...
        std::cerr << xcp_header << xcp_type_runtime << xcp_at_runtime << "Traceback: " << e.what() << "\n";
                status = EXIT_FAILURE;
            }
            catch (...) {                               //  2.2     CATCH OTHER EXCEPTIONS AND EXIT...
        std::cerr << xcp_header << xcp_type_unknown << xcp_at_runtime;
                status = EXIT_FAILURE;
            }
    }
    catch (const std::runtime_error & e) {      //  1.1     CATCH INITIALIZATION-TIME ERROR (DURING "DEAR IMGUI" INITIALIZATION)...
        std::cerr << xcp_header << xcp_type_runtime << xcp_at_start << "Traceback: " << e.what() << "\n";
        status = EXIT_FAILURE;
    }
    catch (...) {                               //  1.2.    CATCH OTHER EXCEPTIONS AND EXIT...
        std::cerr << xcp_header << xcp_type_unknown << xcp_at_start;
        status = EXIT_FAILURE;
    }
    
    return status;
}





// *************************************************************************** //
// *************************************************************************** //
//
//  END.



*/
