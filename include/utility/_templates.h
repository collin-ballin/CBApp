/***********************************************************************************
*
*       ********************************************************************
*       ****           _ T E M P L A T E S . H  ____  F I L E           ****
*       ********************************************************************
*
*              AUTHOR:      Collin A. Bond
*               DATED:      July 29, 2025.
*
**************************************************************************************
**************************************************************************************/
#ifndef _CBAPP_UTILITY_TEMPLATES_H
#define _CBAPP_UTILITY_TEMPLATES_H      1

#include <stdio.h>
#include <vector>
#include <math.h>
#include <cmath>

#include "json.hpp"
#include "imgui.h"                      //  0.3     "DEAR IMGUI" HEADERS...
#include "implot.h"
#include "imgui_internal.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#if defined(IMGUI_IMPL_OPENGL_ES2)
# include <GLES2/gl2.h>
#endif      //  IMGUI_IMPL_OPENGL_ES2  //
#include <GLFW/glfw3.h>     //  <======| Will drag system OpenGL headers






namespace cb { namespace utl { //     BEGINNING NAMESPACE "cb" :: "utl"...
// *************************************************************************** //
// *************************************************************************** //



//      1.      UTILITY TYPES...
// *************************************************************************** //
// *************************************************************************** //

//  "HoverItem"
//
struct HoverItem
{
    explicit operator bool() const noexcept { return id != 0; }
//
//
    ImGuiID                 id                      = 0;                    //  0 => no hovered item
    ImGuiWindow *           window                  = nullptr;              //  owning window (if any)
    ImGuiViewport *         viewport                = nullptr;              //  owning viewport (fallback to main if unknown)ss

    // Available only when the hovered item is also the most recently submitted item.
    // (Safe to use immediately after the widget was submitted; otherwise false.)
    bool                    has_rect                = false;
    ImRect                  rect;                                           //  screen-space AABB of the item
    int                     frame_stamp                   = -1;   // frame stamp for rect validity
};




#

namespace highlight
{
    // --------------------------------------------------------------------- API
    void begin_frame();                 // call once per frame before any Begin()
    void add(ImGuiID id);               // start highlighting this widget
    void remove(ImGuiID id);            // stop
    void clear();                       // remove all

    // ---------------------------------------------------------------- INTERNAL
    struct Watched
    {
        ImGuiID         id          = 0;
        ImGuiViewport*  viewport    = nullptr;
        ImRect          rect{};
        int             last_seen   = -1;      // frame index
    };

    struct PerContext
    {
        ImVector<Watched> watched;
        static void IMGUI_CDECL hook(const ImGuiID* id, ImGuiDataType, const void*);
    };

    // one PerContext per ImGuiContext
    inline PerContext& ctx_data()
    {
        // Static map: one PerContext per ImGuiContext*
        static std::unordered_map<ImGuiContext*, PerContext> table;

        ImGuiContext* g = ImGui::GetCurrentContext();
        auto [it, inserted] = table.emplace(g, PerContext{});

        if (inserted)                                          // first time for this context
        {
    #if !defined(IMGUI_DISABLE_DEBUG_TOOLS)
            // Install our hook only if the field is a function pointer.
            if constexpr (std::is_pointer_v<decltype(g->DebugHookIdInfo)>) {
                //g->DebugHookIdInfo = PerContext::hook;
            }
    #endif
        }
        return it->second;
    }

    // ---------------------------------------------------------------- API impl
    inline void add(ImGuiID id)
    {
        if (!id) return;
        auto& list = ctx_data().watched;
        for (auto& w : list) if (w.id == id) return;    // already present
        list.push_back(Watched{ id });
    }
    inline void remove(ImGuiID id)
    {
        auto& list = ctx_data().watched;
        for (int i = 0; i < list.Size; ++i)
            if (list[i].id == id) { list.erase(list.Data + i); break; }
    }
    inline void clear() { ctx_data().watched.clear(); }

    // --------------------------------------------------------------- Debug hook
    // Called by ImGui every time GetID/PushID hashes a new ID.
    inline void IMGUI_CDECL PerContext::hook(const ImGuiID* id,
                                             ImGuiDataType, const void*)
    {
        if (!id || *id == 0) return;
        PerContext& pc = ctx_data();
        for (auto& w : pc.watched)
            if (w.id == *id)
                w.viewport = GImGui->CurrentWindow
                           ? GImGui->CurrentWindow->Viewport
                           : ImGui::GetMainViewport();
    }

    // --------------------------------------------------------------- Frame step
    inline void begin_frame()
    {
        ImGuiContext& g = *GImGui;
        PerContext&   pc = ctx_data();
        const int     frame = g.FrameCount;

        // Capture rect of *last* submitted item if it is watched.
        if (g.LastItemData.ID != 0)
            for (auto& w : pc.watched)
                if (w.id == g.LastItemData.ID)
                {
                    w.rect      = g.LastItemData.Rect;
                    w.last_seen = frame;
                }

        // Draw highlights
        const ImU32 col = IM_COL32(255,255,0,255);
        const float pad = 1.5f, thick = 2.0f;

        for (const auto& w : pc.watched)
        {
            if (w.last_seen != frame || !w.viewport) continue;

            ImRect r = w.rect; r.Min -= ImVec2(pad,pad); r.Max += ImVec2(pad,pad);
            ImDrawList* dl = ImGui::GetForegroundDrawList(w.viewport);
            dl->AddRect(r.Min, r.Max, col, 0.0f, 0, thick);
            dl->AddLine(r.Min, r.Max, col, thick*0.5f);
            dl->AddLine({r.Min.x,r.Max.y}, {r.Max.x,r.Min.y}, col, thick*0.5f);
        }
    }
} // namespace highlight






// *************************************************************************** //
// *************************************************************************** //   END "UTILITY TYPES".












//  1.4A    INLINE TEMPLATES FOR HELPER WIDGET FUNCTIONS / ABSTRACTIONS     [1 OF 2]...
// *************************************************************************** //
// *************************************************************************** //

//  "centered_offset"
//      Utility: horizontal offset to center an item of width w inside a container W.
//
inline float centered_offset(float container_width, float item_width)
{
    return (container_width - item_width) * 0.5f;
}


//  "collimated_row"
//
//      Draws a single ImGui::Columns() section with `count` equally sized columns.
//      For each column it invokes `draw_widget_fn`, then automatically renders a
//      centered label underneath, shifting both widget+label just enough (plus a
//      configurable safety factor) to avoid clipping.
//
//      draw_widget_fn signature:
//          void(int col_index, float widget_x, const ImVec2 &item_size, const char *label)
//      ➜  The function should set the cursor X to `widget_x` before drawing the
//         widget (button, checkbox, etc.). This gives the caller full control
//         over the widget type while letting the helper handle alignment math.
//
template<typename DrawFn>
inline void collimated_row(const char* uuid,
                           int         count,
                           ImVec2      item_size,
                           const char* const* labels,
                           int         label_count,
                           DrawFn      draw_widget_fn,
                           float       safety_factor = 1.2f,
                           ImGuiOldColumnFlags flags = 0)
{
    if (count <= 0) return;

    ImGui::Columns(count, uuid, flags);

    for (int col = 0; col < count; ++col)
    {
        if (col > 0) ImGui::NextColumn();

        const char* label = (labels && col < label_count) ? labels[col] : "";
        ImVec2 text_sz = ImGui::CalcTextSize(label);
        float  col_w   = ImGui::GetColumnWidth();
        float  x0      = ImGui::GetCursorPosX();

        // Baseline centered positions.
        float widget_x = x0 + centered_offset(col_w, item_size.x);
        float label_x  = x0 + centered_offset(col_w, text_sz.x);

        // Minimal shift to prevent clipping.
        float shift = 0.0f;
        if (label_x < x0)
            shift = (x0 - label_x) * safety_factor; // shift right
        else if (label_x + text_sz.x > x0 + col_w)
            shift = -((label_x + text_sz.x) - (x0 + col_w)) * safety_factor; // shift left

        widget_x += shift;
        label_x  += shift;

        // Draw widget via user callback.
        draw_widget_fn(col, widget_x, item_size, label);

        // Draw label (or keep spacing for empty cell).
        ImGui::SetCursorPosX(label_x);
        if (label && *label)
            ImGui::TextUnformatted(label);
        else
            ImGui::Dummy(ImVec2(0, ImGui::GetFontSize()));
    }

    ImGui::Columns(1); // restore
}






// *************************************************************************** //
//
//
//  1.4A    INLINE TEMPLATES FOR HELPER WIDGET FUNCTIONS / ABSTRACTIONS     [2 OF 2]...
// *************************************************************************** //
// *************************************************************************** //

//  "MakeCtrlTable"
//
/// @brief      DESC
/// @tparam             T           [Template parameter, if any]
/// @param      arg1        DESC
/// @return                 DESC
/// @see        MakeCtrlTable()
///
/// @todo       TODO
template<typename T, auto NR, auto NC>
inline void MakeCtrlTable(const T (&rows)[NR], TableCFG<NC> & cfg)
{
    //  1.  PRIMARY TABLE ENTRY...
    ImGui::PushID(cfg.uuid);
    if ( ImGui::BeginTable(cfg.uuid, NC, cfg.table_flags) )
    {
        //  if (freeze_column || freeze_header)
        //      ImGui::TableSetupScrollFreeze(freeze_column ? 1 : 0, freeze_header ? 1 : 0);

        
        for (size_t i = 0; i < NC; ++i)
        for (auto & col : cfg.columns)
            ImGui::TableSetupColumn(col.uuid,    col.flags,     col.width);
            
            
        //ImGui::TableSetupColumn("Widget",   col1_flags,     WIDGET_COLUMN_WIDTH);
        if (cfg.header_row)
            ImGui::TableHeadersRow();


        for (const auto & row : rows) {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::AlignTextToFramePadding();
            ImGui::TextUnformatted(row.label);
            ImGui::TableSetColumnIndex(1);
            row.render();
        }


        ImGui::EndTable();
    }// END "Table".
    
    ImGui::PopID();     //  Pop Table UUID.
    return;
}


//  "MakeCtrlTable"
//
/// @brief Explicit template specialization of "MakeCtrlTable"
/// @description Specialized for the case of 2-columns within a widget control table.
/// @see        MakeCtrlTable()
template<typename T, auto NR>
inline void MakeCtrlTable(const T (&rows)[NR], TableCFG<2> & cfg)
{
    //  1.  PRIMARY TABLE ENTRY...
    ImGui::PushID(cfg.uuid);                //  1.  PRIMARY TABLE ENTRY...
    if ( ImGui::BeginTable(cfg.uuid, 2, cfg.table_flags) )
    {
        for (auto & col : cfg.columns)      //  2.  SETUP EACH COLUMN...
            ImGui::TableSetupColumn(col.uuid,    col.flags,     col.width);
            
        if (cfg.header_row)                 //  3.  CONDITIONALLY DRAW THE HEADER ROW...
            ImGui::TableHeadersRow();

        for (const auto & row : rows) {     //  4.  SETUP EACH COLUMN...
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::AlignTextToFramePadding();
            ImGui::TextUnformatted(row.label);
            ImGui::TableSetColumnIndex(1);
            row.render();
        }

        ImGui::EndTable();
    }// END "Table".
    
    ImGui::PopID();     //  Pop Table UUID.
    return;
}

//      OVERLOADS FOR STD::VECTOR...
//
//
//  "MakeCtrlTable"
//
/// @brief      DESC
/// @tparam             T           [Template parameter, if any]
/// @param      arg1        DESC
/// @return                 DESC
/// @see        MakeCtrlTable()
///
/// @todo       TODO
template<typename T, auto NC>
inline void MakeCtrlTable(const std::vector<T> & rows, TableCFG<NC> & cfg)
{
    //  1.  PRIMARY TABLE ENTRY...
    ImGui::PushID(cfg.uuid);
    if ( ImGui::BeginTable(cfg.uuid, NC, cfg.table_flags) )
    {
        //  if (freeze_column || freeze_header)
        //      ImGui::TableSetupScrollFreeze(freeze_column ? 1 : 0, freeze_header ? 1 : 0);

        
        for (size_t i = 0; i < NC; ++i)
        for (auto & col : cfg.columns)
            ImGui::TableSetupColumn(col.uuid,    col.flags,     col.width);
            
            
        //ImGui::TableSetupColumn("Widget",   col1_flags,     WIDGET_COLUMN_WIDTH);
        if (cfg.header_row)
            ImGui::TableHeadersRow();


        for (const auto & row : rows) {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::AlignTextToFramePadding();
            ImGui::TextUnformatted(row.label);
            ImGui::TableSetColumnIndex(1);
            row.render();
        }


        ImGui::EndTable();
    }// END "Table".
    
    ImGui::PopID();     //  Pop Table UUID.
    return;
}


//  "MakeCtrlTable"
//
/// @brief Explicit template specialization of "MakeCtrlTable"
/// @description Specialized for the case of 2-columns within a widget control table.
/// @see        MakeCtrlTable()
template<typename T>
inline void MakeCtrlTable(const std::vector<T> & rows, TableCFG<2> & cfg)
{
    //  1.  PRIMARY TABLE ENTRY...
    ImGui::PushID(cfg.uuid);                //  1.  PRIMARY TABLE ENTRY...
    if ( ImGui::BeginTable(cfg.uuid, 2, cfg.table_flags) )
    {
        for (auto & col : cfg.columns)      //  2.  SETUP EACH COLUMN...
            ImGui::TableSetupColumn(col.uuid,    col.flags,     col.width);
            
        if (cfg.header_row)                 //  3.  CONDITIONALLY DRAW THE HEADER ROW...
            ImGui::TableHeadersRow();

        for (const auto & row : rows) {     //  4.  SETUP EACH COLUMN...
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::AlignTextToFramePadding();
            ImGui::TextUnformatted(row.label);
            ImGui::TableSetColumnIndex(1);
            row.render();
        }

        ImGui::EndTable();
    }// END "Table".
    
    ImGui::PopID();     //  Pop Table UUID.
    return;
}


// *************************************************************************** //
//
//
//  1.4B    INLINE TEMPLATES FOR PLOTS...
// *************************************************************************** //
// *************************************************************************** //

//  "MakePlotCFG"
//
inline bool MakePlotCFG(PlotCFG & cfg)
{
    //  CASE 1 :    FAILED TO CREATE PLOT...
    ImGui::PushID(cfg.graph.uuid);
    //if ( !ImPlot::BeginPlot(cfg.graph.uuid, cfg.graph.size, cfg.graph.flags) )
    if ( !ImPlot::BeginPlot(cfg.graph.uuid, cfg.graph.size, cfg.graph.flags) )
    {
        ImGui::PopID();
        return false;
    }
    
    //  CASE 2 :    SUCCESSFULLY CREATED PLOT...
    ImPlot::SetupAxes(cfg.axes[0].uuid,         cfg.axes[1].uuid,       //  3.  CONFIGURE THE PLOT APPEARANCE...
                      cfg.axes[0].flags,        cfg.axes[1].flags);
    ImPlot::SetupLegend(cfg.legend.location,    cfg.legend.flags);
    
    //  ImPlot::SetupAxisLimits(ImAxis_X1, 0, NX - 1,           ImGuiCond_Always);
    //  ImPlot::SetupAxisLimits(ImAxis_Y1, YLIMS[0], YLIMS[1],  ImGuiCond_Always);
    

    

    ImGui::PopID();
    return true;
}










// *************************************************************************** //
//
//
//  3.  INLINE TEMPLATES FOR GENERAL STUFF... 
// *************************************************************************** //
// *************************************************************************** //

//  "EnumArray"
//      Simple Struct/Class to use Indices based off Enum Class-Members Specifically.
//  template<typename E, typename T, std::size_t N = static_cast<std::size_t>(E::Count)>
//  struct EnumArray {
//      std::array<T, N> data;
//      T &                 operator []         (E e) noexcept              { return data[static_cast<std::size_t>(e)]; }
//      const T &           operator []         (E e) const noexcept        { return data[static_cast<std::size_t>(e)]; }
//  };
//
//  OLD IMPLEMENTATION BELOW...
//      template<typename E, typename T, std::size_t N>
//      struct EnumArray {
//          std::array<T, N> data;
//          T & operator[](E e) noexcept                { return data[static_cast<std::size_t>(e)]; }
//          const T & operator [](E e) const noexcept   { return data[static_cast<std::size_t>(e)]; }
//      };


//  "enum_index_t"
//
//  template <typename E>
//  using enum_index_t = std::underlying_type_t<E>;





// *************************************************************************** //
//
//
//  4.  INLINE TEMPLATES FOR IMGUI STUFF...
// *************************************************************************** //
// *************************************************************************** //

//  "RandomRange"
//
template <typename T>
inline T RandomRange(T min, T max) {
    T scale = rand() / (T) RAND_MAX;
    return min + scale * ( max - min );
}


//  **OLD IMPLEMENTATION**      "ScrollingBuffer"
//      Utility structure for realtime plot.
//
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
//      Utility structure for realtime plot.
//
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



//  "sinusoid_wave_IMPL"
//
template<typename T, size_t NX, size_t NY>
void sinusoid_wave_IMPL(T (&data)[NX][NY], const T time, const T amp, const T freq)
{
    constexpr T     pi      = T(3.14159265358979323846);
    T               omega   = T(2) * pi * freq;             //  ω = 2πf
    constexpr T     k       = T(2) * pi / T(NY);            //  spatial wavenumber: one cycle over NY rows


    auto wave = [&](size_t row, size_t) -> T {
        T phase = k * (T(NY - 1 - row)) - omega * time;
        return amp * std::sin(phase);
    };

    for (size_t row = 0; row < NX; ++row) {
        for (size_t col = 0; col < NY; ++col) {
            data[row][col] = wave(row, col);
        }
    }
    
    return;
}


//  "sinusoid_wave"
//
template<typename T, typename size_type=std::size_t>
inline void sinusoid_wave( T * data,        const size_type X,  const size_type Y,
                           const T time,    const T amp,        const T freq )
{
    if (!data || X==0 || Y==0)  return;
    
    constexpr T             pi          = T(3.14159265358979323846);
    const T                 omega       = T(2) * pi * freq;       // temporal frequency ω = 2πf
    const T                 k           = T(2) * pi / T(Y);       // spatial wavenumber k = 2π / λ, with λ = Y rows


    for (size_type i = 0; i < X; ++i)
    {
        T       phase       = k * T(Y - 1 - i) - omega * time;
        T *     row_ptr     = data + i * Y;      // pointer to start of row i
        
        for (size_type j = 0; j < Y; ++j) {
            row_ptr[j] = amp * std::sin(phase);
        }
    }
    
    return;
}


//  "sinusoid_wave_IMPL_1D"
//
template<typename T, size_t N>
void sinusoid_wave_IMPL_1D(T (&data)[N], T time, T amp, T freq) noexcept
{
    //  π and angular frequency
    constexpr T pi    = T(3.14159265358979323846);
    T           omega = T(2) * pi * freq;      // ω = 2πf

    // one spatial cycle over N points
    constexpr T k     = T(2) * pi / T(N);

    for (size_t i = 0; i < N; ++i) {
        // invert index so “row 0” is one end of the wave
        T phase      = k * (T(N - 1 - i)) - omega * time;
        data[i]      = amp * std::sin(phase);
    }
}




//  "ScrollingSparkline"
//
/// @brief      DESC
/// @tparam     arg1        DESC
/// @return                 DESC
///
template<typename T, auto N>
// Adapted ScrollingSparkline for RingBuffer<ImVec2, BUFFER_SIZE>
void ScrollingSparkline(const float time,               const float window,         const cblib::RingBuffer<T, N> & data,
                        const ImPlotAxisFlags flags,    const ImVec4 &   color,     const ImVec2 size,
                        const float center)
{
    ImPlot::PushStyleVar(ImPlotStyleVar_PlotPadding, ImVec2(0,0));
    if (ImPlot::BeginPlot("##Scrolling", size, ImPlotFlags_CanvasOnly))
    {
        ImPlot::SetupAxes(nullptr, nullptr,
                          flags | ImPlotAxisFlags_AutoFit,
                          flags | ImPlotAxisFlags_AutoFit);

        const float xmin = time - center * window;
        const float xmax = xmin + window;
        ImPlot::SetupAxisLimits(ImAxis_X1, xmin, xmax, ImGuiCond_Always);

        if (!data.empty())
        {
            ImPlot::SetNextLineStyle(color);
            ImPlot::SetNextFillStyle(color, 0.25f);
            ImPlot::PlotLine("##Scrolling",
                             &data.front().x,
                             &data.front().y,
                              static_cast<int>( data.size() ),
                              ImPlotLineFlags_Shaded,
                              static_cast<int>( data.offset() ),
                              sizeof(ImVec2));
        }

        ImPlot::EndPlot();
    }
    ImPlot::PopStyleVar();
}



// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
} }//   END OF "cb" NAMESPACE.






#endif      //  _CBAPP_UTILITY_TEMPLATES_H  //
// *************************************************************************** //
// *************************************************************************** //
//
//  END.














/*
void ImGui::ShowStyleEditor(ImGuiStyle* ref)
{
    IMGUI_DEMO_MARKER("Tools/Style Editor");
    // You can pass in a reference ImGuiStyle structure to compare to, revert to and save to
    // (without a reference style pointer, we will use one compared locally as a reference)
    ImGuiStyle& style = ImGui::GetStyle();
    static ImGuiStyle ref_saved_style;

    // Default to using internal storage as reference
    static bool init = true;
    if (init && ref == NULL)
        ref_saved_style = style;
    init = false;
    if (ref == NULL)
        ref = &ref_saved_style;

    ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.50f);

    if (ImGui::ShowStyleSelector("Colors##Selector"))
        ref_saved_style = style;
    ImGui::ShowFontSelector("Fonts##Selector");

    // Simplified Settings (expose floating-pointer border sizes as boolean representing 0.0f or 1.0f)
    if (ImGui::SliderFloat("FrameRounding", &style.FrameRounding, 0.0f, 12.0f, "%.0f"))
        style.GrabRounding = style.FrameRounding; // Make GrabRounding always the same value as FrameRounding
    { bool border = (style.WindowBorderSize > 0.0f); if (ImGui::Checkbox("WindowBorder", &border)) { style.WindowBorderSize = border ? 1.0f : 0.0f; } }
    ImGui::SameLine();
    { bool border = (style.FrameBorderSize > 0.0f);  if (ImGui::Checkbox("FrameBorder",  &border)) { style.FrameBorderSize  = border ? 1.0f : 0.0f; } }
    ImGui::SameLine();
    { bool border = (style.PopupBorderSize > 0.0f);  if (ImGui::Checkbox("PopupBorder",  &border)) { style.PopupBorderSize  = border ? 1.0f : 0.0f; } }

    // Save/Revert button
    if (ImGui::Button("Save Ref"))
        *ref = ref_saved_style = style;
    ImGui::SameLine();
    if (ImGui::Button("Revert Ref"))
        style = *ref;
    ImGui::SameLine();
    HelpMarker(
        "Save/Revert in local non-persistent storage. Default Colors definition are not affected. "
        "Use \"Export\" below to save them somewhere.");

    ImGui::Separator();

    if (ImGui::BeginTabBar("##tabs", ImGuiTabBarFlags_None))
    {
        if (ImGui::BeginTabItem("Sizes"))
        {
            ImGui::SeparatorText("Main");
            ImGui::SliderFloat2("WindowPadding", (float*)&style.WindowPadding, 0.0f, 20.0f, "%.0f");
            ImGui::SliderFloat2("FramePadding", (float*)&style.FramePadding, 0.0f, 20.0f, "%.0f");
            ImGui::SliderFloat2("ItemSpacing", (float*)&style.ItemSpacing, 0.0f, 20.0f, "%.0f");
            ImGui::SliderFloat2("ItemInnerSpacing", (float*)&style.ItemInnerSpacing, 0.0f, 20.0f, "%.0f");
            ImGui::SliderFloat2("TouchExtraPadding", (float*)&style.TouchExtraPadding, 0.0f, 10.0f, "%.0f");
            ImGui::SliderFloat("IndentSpacing", &style.IndentSpacing, 0.0f, 30.0f, "%.0f");
            ImGui::SliderFloat("ScrollbarSize", &style.ScrollbarSize, 1.0f, 20.0f, "%.0f");
            ImGui::SliderFloat("GrabMinSize", &style.GrabMinSize, 1.0f, 20.0f, "%.0f");

            ImGui::SeparatorText("Borders");
            ImGui::SliderFloat("WindowBorderSize", &style.WindowBorderSize, 0.0f, 1.0f, "%.0f");
            ImGui::SliderFloat("ChildBorderSize", &style.ChildBorderSize, 0.0f, 1.0f, "%.0f");
            ImGui::SliderFloat("PopupBorderSize", &style.PopupBorderSize, 0.0f, 1.0f, "%.0f");
            ImGui::SliderFloat("FrameBorderSize", &style.FrameBorderSize, 0.0f, 1.0f, "%.0f");

            ImGui::SeparatorText("Rounding");
            ImGui::SliderFloat("WindowRounding", &style.WindowRounding, 0.0f, 12.0f, "%.0f");
            ImGui::SliderFloat("ChildRounding", &style.ChildRounding, 0.0f, 12.0f, "%.0f");
            ImGui::SliderFloat("FrameRounding", &style.FrameRounding, 0.0f, 12.0f, "%.0f");
            ImGui::SliderFloat("PopupRounding", &style.PopupRounding, 0.0f, 12.0f, "%.0f");
            ImGui::SliderFloat("ScrollbarRounding", &style.ScrollbarRounding, 0.0f, 12.0f, "%.0f");
            ImGui::SliderFloat("GrabRounding", &style.GrabRounding, 0.0f, 12.0f, "%.0f");

            ImGui::SeparatorText("Tabs");
            ImGui::SliderFloat("TabBorderSize", &style.TabBorderSize, 0.0f, 1.0f, "%.0f");
            ImGui::SliderFloat("TabBarBorderSize", &style.TabBarBorderSize, 0.0f, 2.0f, "%.0f");
            ImGui::SliderFloat("TabBarOverlineSize", &style.TabBarOverlineSize, 0.0f, 3.0f, "%.0f");
            ImGui::SameLine(); HelpMarker("Overline is only drawn over the selected tab when ImGuiTabBarFlags_DrawSelectedOverline is set.");
            ImGui::DragFloat("TabCloseButtonMinWidthSelected", &style.TabCloseButtonMinWidthSelected, 0.1f, -1.0f, 100.0f, (style.TabCloseButtonMinWidthSelected < 0.0f) ? "%.0f (Always)" : "%.0f");
            ImGui::DragFloat("TabCloseButtonMinWidthUnselected", &style.TabCloseButtonMinWidthUnselected, 0.1f, -1.0f, 100.0f, (style.TabCloseButtonMinWidthUnselected < 0.0f) ? "%.0f (Always)" : "%.0f");
            ImGui::SliderFloat("TabRounding", &style.TabRounding, 0.0f, 12.0f, "%.0f");

            ImGui::SeparatorText("Tables");
            ImGui::SliderFloat2("CellPadding", (float*)&style.CellPadding, 0.0f, 20.0f, "%.0f");
            ImGui::SliderAngle("TableAngledHeadersAngle", &style.TableAngledHeadersAngle, -50.0f, +50.0f);
            ImGui::SliderFloat2("TableAngledHeadersTextAlign", (float*)&style.TableAngledHeadersTextAlign, 0.0f, 1.0f, "%.2f");

            ImGui::SeparatorText("Windows");
            ImGui::SliderFloat2("WindowTitleAlign", (float*)&style.WindowTitleAlign, 0.0f, 1.0f, "%.2f");
            ImGui::SliderFloat("WindowBorderHoverPadding", &style.WindowBorderHoverPadding, 1.0f, 20.0f, "%.0f");
            int window_menu_button_position = style.WindowMenuButtonPosition + 1;
            if (ImGui::Combo("WindowMenuButtonPosition", (int*)&window_menu_button_position, "None\0Left\0Right\0"))
                style.WindowMenuButtonPosition = (ImGuiDir)(window_menu_button_position - 1);

            ImGui::SeparatorText("Widgets");
            if (ImGui::BeginCombo("TreeLinesFlags", GetTreeLinesFlagsName(style.TreeLinesFlags)))
            {
                const ImGuiTreeNodeFlags options[] = { ImGuiTreeNodeFlags_DrawLinesNone, ImGuiTreeNodeFlags_DrawLinesFull, ImGuiTreeNodeFlags_DrawLinesToNodes };
                for (ImGuiTreeNodeFlags option : options)
                    if (ImGui::Selectable(GetTreeLinesFlagsName(option), style.TreeLinesFlags == option))
                        style.TreeLinesFlags = option;
                ImGui::EndCombo();
            }
            ImGui::SliderFloat("TreeLinesSize", &style.TreeLinesSize, 0.0f, 2.0f, "%.0f");
            ImGui::Combo("ColorButtonPosition", (int*)&style.ColorButtonPosition, "Left\0Right\0");
            ImGui::SliderFloat2("ButtonTextAlign", (float*)&style.ButtonTextAlign, 0.0f, 1.0f, "%.2f");
            ImGui::SameLine(); HelpMarker("Alignment applies when a button is larger than its text content.");
            ImGui::SliderFloat2("SelectableTextAlign", (float*)&style.SelectableTextAlign, 0.0f, 1.0f, "%.2f");
            ImGui::SameLine(); HelpMarker("Alignment applies when a selectable is larger than its text content.");
            ImGui::SliderFloat("SeparatorTextBorderSize", &style.SeparatorTextBorderSize, 0.0f, 10.0f, "%.0f");
            ImGui::SliderFloat2("SeparatorTextAlign", (float*)&style.SeparatorTextAlign, 0.0f, 1.0f, "%.2f");
            ImGui::SliderFloat2("SeparatorTextPadding", (float*)&style.SeparatorTextPadding, 0.0f, 40.0f, "%.0f");
            ImGui::SliderFloat("LogSliderDeadzone", &style.LogSliderDeadzone, 0.0f, 12.0f, "%.0f");
            ImGui::SliderFloat("ImageBorderSize", &style.ImageBorderSize, 0.0f, 1.0f, "%.0f");

            ImGui::SeparatorText("Docking");
            ImGui::SliderFloat("DockingSplitterSize", &style.DockingSeparatorSize, 0.0f, 12.0f, "%.0f");

            ImGui::SeparatorText("Tooltips");
            for (int n = 0; n < 2; n++)
                if (ImGui::TreeNodeEx(n == 0 ? "HoverFlagsForTooltipMouse" : "HoverFlagsForTooltipNav"))
                {
                    ImGuiHoveredFlags* p = (n == 0) ? &style.HoverFlagsForTooltipMouse : &style.HoverFlagsForTooltipNav;
                    ImGui::CheckboxFlags("ImGuiHoveredFlags_DelayNone", p, ImGuiHoveredFlags_DelayNone);
                    ImGui::CheckboxFlags("ImGuiHoveredFlags_DelayShort", p, ImGuiHoveredFlags_DelayShort);
                    ImGui::CheckboxFlags("ImGuiHoveredFlags_DelayNormal", p, ImGuiHoveredFlags_DelayNormal);
                    ImGui::CheckboxFlags("ImGuiHoveredFlags_Stationary", p, ImGuiHoveredFlags_Stationary);
                    ImGui::CheckboxFlags("ImGuiHoveredFlags_NoSharedDelay", p, ImGuiHoveredFlags_NoSharedDelay);
                    ImGui::TreePop();
                }

            ImGui::SeparatorText("Misc");
            ImGui::SliderFloat2("DisplayWindowPadding", (float*)&style.DisplayWindowPadding, 0.0f, 30.0f, "%.0f"); ImGui::SameLine(); HelpMarker("Apply to regular windows: amount which we enforce to keep visible when moving near edges of your screen.");
            ImGui::SliderFloat2("DisplaySafeAreaPadding", (float*)&style.DisplaySafeAreaPadding, 0.0f, 30.0f, "%.0f"); ImGui::SameLine(); HelpMarker("Apply to every windows, menus, popups, tooltips: amount where we avoid displaying contents. Adjust if you cannot see the edges of your screen (e.g. on a TV where scaling has not been configured).");

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Colors"))
        {
            static int output_dest = 0;
            static bool output_only_modified = true;
            if (ImGui::Button("Export"))
            {
                if (output_dest == 0)
                    ImGui::LogToClipboard();
                else
                    ImGui::LogToTTY();
                ImGui::LogText("ImVec4* colors = ImGui::GetStyle().Colors;" IM_NEWLINE);
                for (int i = 0; i < ImGuiCol_COUNT; i++)
                {
                    const ImVec4& col = style.Colors[i];
                    const char* name = ImGui::GetStyleColorName(i);
                    if (!output_only_modified || memcmp(&col, &ref->Colors[i], sizeof(ImVec4)) != 0)
                        ImGui::LogText("colors[ImGuiCol_%s]%*s= ImVec4(%.2ff, %.2ff, %.2ff, %.2ff);" IM_NEWLINE,
                            name, 23 - (int)strlen(name), "", col.x, col.y, col.z, col.w);
                }
                ImGui::LogFinish();
            }
            ImGui::SameLine(); ImGui::SetNextItemWidth(120); ImGui::Combo("##output_type", &output_dest, "To Clipboard\0To TTY\0");
            ImGui::SameLine(); ImGui::Checkbox("Only Modified Colors", &output_only_modified);

            static ImGuiTextFilter filter;
            filter.Draw("Filter colors", ImGui::GetFontSize() * 16);

            static ImGuiColorEditFlags alpha_flags = 0;
            if (ImGui::RadioButton("Opaque", alpha_flags == ImGuiColorEditFlags_AlphaOpaque))       { alpha_flags = ImGuiColorEditFlags_AlphaOpaque; } ImGui::SameLine();
            if (ImGui::RadioButton("Alpha",  alpha_flags == ImGuiColorEditFlags_None))              { alpha_flags = ImGuiColorEditFlags_None; } ImGui::SameLine();
            if (ImGui::RadioButton("Both",   alpha_flags == ImGuiColorEditFlags_AlphaPreviewHalf))  { alpha_flags = ImGuiColorEditFlags_AlphaPreviewHalf; } ImGui::SameLine();
            HelpMarker(
                "In the color list:\n"
                "Left-click on color square to open color picker,\n"
                "Right-click to open edit options menu.");

            ImGui::SetNextWindowSizeConstraints(ImVec2(0.0f, ImGui::GetTextLineHeightWithSpacing() * 10), ImVec2(FLT_MAX, FLT_MAX));
            ImGui::BeginChild("##colors", ImVec2(0, 0), ImGuiChildFlags_Borders | ImGuiChildFlags_NavFlattened, ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_AlwaysHorizontalScrollbar);
            ImGui::PushItemWidth(ImGui::GetFontSize() * -12);
            for (int i = 0; i < ImGuiCol_COUNT; i++)
            {
                const char* name = ImGui::GetStyleColorName(i);
                if (!filter.PassFilter(name))
                    continue;
                ImGui::PushID(i);
#ifndef IMGUI_DISABLE_DEBUG_TOOLS
                if (ImGui::Button("?"))
                    ImGui::DebugFlashStyleColor((ImGuiCol)i);
                ImGui::SetItemTooltip("Flash given color to identify places where it is used.");
                ImGui::SameLine();
#endif
                ImGui::ColorEdit4("##color", (float*)&style.Colors[i], ImGuiColorEditFlags_AlphaBar | alpha_flags);
                if (memcmp(&style.Colors[i], &ref->Colors[i], sizeof(ImVec4)) != 0)
                {
                    // Tips: in a real user application, you may want to merge and use an icon font into the main font,
                    // so instead of "Save"/"Revert" you'd use icons!
                    // Read the FAQ and docs/FONTS.md about using icon fonts. It's really easy and super convenient!
                    ImGui::SameLine(0.0f, style.ItemInnerSpacing.x); if (ImGui::Button("Save")) { ref->Colors[i] = style.Colors[i]; }
                    ImGui::SameLine(0.0f, style.ItemInnerSpacing.x); if (ImGui::Button("Revert")) { style.Colors[i] = ref->Colors[i]; }
                }
                ImGui::SameLine(0.0f, style.ItemInnerSpacing.x);
                ImGui::TextUnformatted(name);
                ImGui::PopID();
            }
            ImGui::PopItemWidth();
            ImGui::EndChild();

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Fonts"))
        {
            ImGuiIO& io = ImGui::GetIO();
            ImFontAtlas* atlas = io.Fonts;
            HelpMarker("Read FAQ and docs/FONTS.md for details on font loading.");
            ImGui::ShowFontAtlas(atlas);

            // Post-baking font scaling. Note that this is NOT the nice way of scaling fonts, read below.
            // (we enforce hard clamping manually as by default DragFloat/SliderFloat allows CTRL+Click text to get out of bounds).
            const float MIN_SCALE = 0.3f;
            const float MAX_SCALE = 2.0f;
            HelpMarker(
                "Those are old settings provided for convenience.\n"
                "However, the _correct_ way of scaling your UI is currently to reload your font at the designed size, "
                "rebuild the font atlas, and call style.ScaleAllSizes() on a reference ImGuiStyle structure.\n"
                "Using those settings here will give you poor quality results.");
            static float window_scale = 1.0f;
            ImGui::PushItemWidth(ImGui::GetFontSize() * 8);
            if (ImGui::DragFloat("window scale", &window_scale, 0.005f, MIN_SCALE, MAX_SCALE, "%.2f", ImGuiSliderFlags_AlwaysClamp)) // Scale only this window
                ImGui::SetWindowFontScale(window_scale);
            ImGui::DragFloat("global scale", &io.FontGlobalScale, 0.005f, MIN_SCALE, MAX_SCALE, "%.2f", ImGuiSliderFlags_AlwaysClamp); // Scale everything
            ImGui::PopItemWidth();

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Rendering"))
        {
            ImGui::Checkbox("Anti-aliased lines", &style.AntiAliasedLines);
            ImGui::SameLine();
            HelpMarker("When disabling anti-aliasing lines, you'll probably want to disable borders in your style as well.");

            ImGui::Checkbox("Anti-aliased lines use texture", &style.AntiAliasedLinesUseTex);
            ImGui::SameLine();
            HelpMarker("Faster lines using texture data. Require backend to render with bilinear filtering (not point/nearest filtering).");

            ImGui::Checkbox("Anti-aliased fill", &style.AntiAliasedFill);
            ImGui::PushItemWidth(ImGui::GetFontSize() * 8);
            ImGui::DragFloat("Curve Tessellation Tolerance", &style.CurveTessellationTol, 0.02f, 0.10f, 10.0f, "%.2f");
            if (style.CurveTessellationTol < 0.10f) style.CurveTessellationTol = 0.10f;

            // When editing the "Circle Segment Max Error" value, draw a preview of its effect on auto-tessellated circles.
            ImGui::DragFloat("Circle Tessellation Max Error", &style.CircleTessellationMaxError , 0.005f, 0.10f, 5.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
            const bool show_samples = ImGui::IsItemActive();
            if (show_samples)
                ImGui::SetNextWindowPos(ImGui::GetCursorScreenPos());
            if (show_samples && ImGui::BeginTooltip())
            {
                ImGui::TextUnformatted("(R = radius, N = approx number of segments)");
                ImGui::Spacing();
                ImDrawList* draw_list = ImGui::GetWindowDrawList();
                const float min_widget_width = ImGui::CalcTextSize("R: MMM\nN: MMM").x;
                for (int n = 0; n < 8; n++)
                {
                    const float RAD_MIN = 5.0f;
                    const float RAD_MAX = 70.0f;
                    const float rad = RAD_MIN + (RAD_MAX - RAD_MIN) * (float)n / (8.0f - 1.0f);

                    ImGui::BeginGroup();

                    // N is not always exact here due to how PathArcTo() function work internally
                    ImGui::Text("R: %.f\nN: %d", rad, draw_list->_CalcCircleAutoSegmentCount(rad));

                    const float canvas_width = IM_MAX(min_widget_width, rad * 2.0f);
                    const float offset_x     = floorf(canvas_width * 0.5f);
                    const float offset_y     = floorf(RAD_MAX);

                    const ImVec2 p1 = ImGui::GetCursorScreenPos();
                    draw_list->AddCircle(ImVec2(p1.x + offset_x, p1.y + offset_y), rad, ImGui::GetColorU32(ImGuiCol_Text));
                    ImGui::Dummy(ImVec2(canvas_width, RAD_MAX * 2));

                    
                    // const ImVec2 p2 = ImGui::GetCursorScreenPos();
                    // draw_list->AddCircleFilled(ImVec2(p2.x + offset_x, p2.y + offset_y), rad, ImGui::GetColorU32(ImGuiCol_Text));
                    // ImGui::Dummy(ImVec2(canvas_width, RAD_MAX * 2));
                    

                    ImGui::EndGroup();
                    ImGui::SameLine();
                }
                ImGui::EndTooltip();
            }
            ImGui::SameLine();
            HelpMarker("When drawing circle primitives with \"num_segments == 0\" tessellation will be calculated automatically.");

            ImGui::DragFloat("Global Alpha", &style.Alpha, 0.005f, 0.20f, 1.0f, "%.2f"); // Not exposing zero here so user doesn't "lose" the UI (zero alpha clips all widgets). But application code could have a toggle to switch between zero and non-zero.
            ImGui::DragFloat("Disabled Alpha", &style.DisabledAlpha, 0.005f, 0.0f, 1.0f, "%.2f"); ImGui::SameLine(); HelpMarker("Additional alpha multiplier for disabled items (multiply over current value of Alpha).");
            ImGui::PopItemWidth();

            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }

    ImGui::PopItemWidth();
}
*/
