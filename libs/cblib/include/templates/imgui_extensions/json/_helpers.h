/***********************************************************************************
*
*       ********************************************************************
*       ****             _ H E L P E R S . H  ____  F I L E             ****
*       ********************************************************************
*
*              AUTHOR:      Collin A. Bond.
*               DATED:      August 24, 2025.
*
**************************************************************************************
**************************************************************************************/
#ifndef _CBLIB_IMGUI_JSON_HELPERS_H
#define _CBLIB_IMGUI_JSON_HELPERS_H 1

//      C++ Standard Libraries.
#include <iostream>
#include <stdexcept>    // std::runtime_error
#include <vector>
//
#include <type_traits>
#include <functional>  // std::hash
#include <concepts>    // std::integral
#include <algorithm>
//
#include <compare>     // <=>


//      C-Libraries.
#include <stdio.h>
#include <cmath>
#include <math.h>
#include <cstddef>     // size_t
#include <cstdint>     // uint32_t, uint16_t, …


//      **My** Headers.
#include "json.hpp"
#include "imgui.h"                      //  0.3     "DEAR IMGUI" HEADERS...
#include "implot.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#if defined(IMGUI_IMPL_OPENGL_ES2)
# include <GLES2/gl2.h>
#endif      //  IMGUI_IMPL_OPENGL_ES2  //
#include <GLFW/glfw3.h>     //  <======| Will drag system OpenGL headers






namespace cblib { json { //     BEGINNING NAMESPACE "cblib::json"...
// *************************************************************************** //
// *************************************************************************** //



//      1.      STATIC INLINE HELPER FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //






// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //   END OF "HELPER FUNCTIONS".






// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
}//   END OF "cblib::json" NAMESPACE.












// *************************************************************************** //
// *************************************************************************** //
#endif  //  _CBLIB_IMGUI_JSON_HELPERS_H  //
