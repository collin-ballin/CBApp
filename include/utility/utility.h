//
//  utility.h
//  CBApp
//
//  Created by Collin Bond on 4/16/25.
//
//***************************************************************************//
//***************************************************************************//
#ifndef _CBAPP_UTILITY_H
#define _CBAPP_UTILITY_H  1


#include <stdio.h>
#include <math.h>

#include "imgui.h"                      //  0.3     "DEAR IMGUI" HEADERS...
#include "implot.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#if defined(IMGUI_IMPL_OPENGL_ES2)
# include <GLES2/gl2.h>
#endif      //  IMGUI_IMPL_OPENGL_ES2  //
#include <GLFW/glfw3.h>     //  <======| Will drag system OpenGL headers



namespace cb { namespace utl { //     BEGINNING NAMESPACE "cb" :: "utl"...
//***************************************************************************//
//***************************************************************************//

//  1.  MISC / UTILITY FUNCTIONS...
void                                glfw_error_callback         (int error, const char * description);
[[nodiscard]] GLFWmonitor *         get_current_monitor         (GLFWwindow * window);
void                                set_window_scale            (GLFWwindow * window, const float scale);

void                                set_next_window_geometry    (GLFWwindow * glfw_window, float pos_x_frac, float pos_y_frac,
                                                                                           float width_frac, float height_frac);
void                                Sparkline                   (const char* id, const float* values, int count, float min_v, float max_v, int offset, const ImVec4& col, const ImVec2& size);



//***************************************************************************//
//
//
//  2.  CONTEXT CREATION / INITIALIZATION FUNCTIONS...
//***************************************************************************//
//***************************************************************************//

const char *                        get_glsl_version            (void);



//***************************************************************************//
//
//
//  3.  INLINE TEMPLATES...
//***************************************************************************//
//***************************************************************************//

//  "RandomRange"
template <typename T>
inline T RandomRange(T min, T max) {
    T scale = rand() / (T) RAND_MAX;
    return min + scale * ( max - min );
}


//  "ScrollingBuffer"
//  Utility structure for realtime plot
struct ScrollingBuffer {
    int MaxSize;
    int Offset;
    ImVector<ImVec2> Data;
    ScrollingBuffer(int max_size = 2000) {
        MaxSize = max_size;
        Offset  = 0;
        Data.reserve(MaxSize);
    }
    void AddPoint(float x, float y) {
        if (Data.size() < MaxSize)
            Data.push_back(ImVec2(x,y));
        else {
            Data[Offset] = ImVec2(x,y);
            Offset =  (Offset + 1) % MaxSize;
        }
    }
    void Erase() {
        if (Data.size() > 0) {
            Data.shrink(0);
            Offset  = 0;
        }
    }
};


//  "RollingBuffer"
//  Utility structure for realtime plot
struct RollingBuffer {
    float Span;
    ImVector<ImVec2> Data;
    RollingBuffer() {
        Span = 10.0f;
        Data.reserve(2000);
    }
    void AddPoint(float x, float y) {
        float xmod = fmodf(x, Span);
        if (!Data.empty() && xmod < Data.back().x)
            Data.shrink(0);
        Data.push_back(ImVec2(xmod, y));
    }
};






//***************************************************************************//
//
//
//
//***************************************************************************//
//***************************************************************************//
} }//   END OF "cb" NAMESPACE.






#endif      //  _CBAPP_UTILITY_H  //
//***************************************************************************//
//***************************************************************************//
//
//  END.
