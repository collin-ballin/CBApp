/***********************************************************************************
*
*       *********************************************************************
*       ****              T O O L S . C P P  ____  F I L E               ****
*       *********************************************************************
*
*              AUTHOR:      Collin A. Bond
*               DATED:      May 20, 2025.
*
**************************************************************************************
**************************************************************************************/
#include "app/app.h"
#include "implot.h"
#include "implot_internal.h"
#include <random>
#include <algorithm>



namespace cb { //     BEGINNING NAMESPACE "cb"...
// *************************************************************************** //
// *************************************************************************** //



//  3.3     SECONDARY APPLICATIONS / TOOLS...
// *************************************************************************** //
// *************************************************************************** //

//  ...



// *************************************************************************** //
//
//
//      1.  IMGUI DEMOS...
// *************************************************************************** //
// *************************************************************************** //

//  "ShowImGuiStyleEditor"
//
void App::ShowImGuiStyleEditor([[maybe_unused]] const char * uuid, [[maybe_unused]] bool * p_open, [[maybe_unused]] ImGuiWindowFlags flags)
{
    ImGui::SetNextWindowDockID( this->S.m_main_dock_id, ImGuiCond_FirstUseEver );
    ImGui::Begin(uuid, p_open, flags);
        ImGui::ShowStyleEditor();
    ImGui::End();
    return;
}


//  "ShowImGuiMetricsWindow"
//
void App::ShowImGuiMetricsWindow([[maybe_unused]] const char * uuid, [[maybe_unused]] bool * p_open, [[maybe_unused]] ImGuiWindowFlags flags)
{
    ImGui::SetNextWindowDockID( this->S.m_main_dock_id, ImGuiCond_FirstUseEver );
    ImGui::ShowMetricsWindow(p_open);
    return;
}


//  "ShowImGuiIDStackTool"
//
void App::ShowImGuiIDStackTool([[maybe_unused]] const char * uuid, [[maybe_unused]] bool * p_open, [[maybe_unused]] ImGuiWindowFlags flags)
{
    //  ImGui::SetNextWindowDockID( this->S.m_main_dock_id, ImGuiCond_FirstUseEver );
    ImGui::ShowMetricsWindow(p_open);
    return;
}


//  "ShowImGuiItemPickerTool"
//
void App::ShowImGuiItemPickerTool([[maybe_unused]] const char * uuid, [[maybe_unused]] bool * p_open, [[maybe_unused]] ImGuiWindowFlags flags)
{
    //  ImGui::SetNextWindowDockID( this->S.m_main_dock_id, ImGuiCond_FirstUseEver );
    ImGui::ShowMetricsWindow(p_open);
    return;
}


    
//  "ShowImGuiDemoWindow"
//
void App::ShowImGuiDemoWindow([[maybe_unused]] const char * uuid, [[maybe_unused]] bool * p_open, [[maybe_unused]] ImGuiWindowFlags flags)
{
    ImGui::SetNextWindowDockID( this->S.m_main_dock_id, ImGuiCond_FirstUseEver );
    ImGui::ShowDemoWindow(p_open);
    return;
}




// *************************************************************************** //
//
//
//      2.  IMPLOT DEMOS...
// *************************************************************************** //
// *************************************************************************** //

//  "ShowImPlotStyleEditor"
//
void App::ShowImPlotStyleEditor([[maybe_unused]] const char * uuid, [[maybe_unused]] bool * p_open, [[maybe_unused]] ImGuiWindowFlags flags)
{
    ImGui::SetNextWindowDockID( this->S.m_main_dock_id, ImGuiCond_FirstUseEver );
    ImGui::Begin(uuid, p_open, flags);
        ImPlot::ShowStyleEditor();
    ImGui::End();
    return;
}

//  "ShowImPlotMetricsWindow"
//
void App::ShowImPlotMetricsWindow([[maybe_unused]] const char * uuid, [[maybe_unused]] bool * p_open, [[maybe_unused]] ImGuiWindowFlags flags)
{
    ImGui::SetNextWindowDockID( this->S.m_main_dock_id, ImGuiCond_FirstUseEver );
    ImPlot::ShowMetricsWindow(p_open);  //"ImPlot Metrics"
    return;
}

//  "ShowImPlotDemoWindow"
//
void App::ShowImPlotDemoWindow([[maybe_unused]] const char * uuid, [[maybe_unused]] bool * p_open, [[maybe_unused]] ImGuiWindowFlags flags)
{
    ImGui::SetNextWindowDockID( this->S.m_main_dock_id, ImGuiCond_FirstUseEver );
    ImPlot::ShowDemoWindow(p_open);
    return;
}



    


// *************************************************************************** //
//
//
//  3.4     COLOR TOOLS...
// *************************************************************************** //
// *************************************************************************** //

//  "ShowColorTool"
//
void App::ShowColorTool([[maybe_unused]] const char * uuid, [[maybe_unused]] bool * p_open, [[maybe_unused]] ImGuiWindowFlags flags)
{
    ImGui::SetNextWindowDockID( this->S.m_main_dock_id, ImGuiCond_FirstUseEver );
    
    //  1.  CREATE THE WINDOW...
    if (!ImGui::Begin(uuid, p_open, flags)) {
        ImGui::End();
        return;
    }

    
    //  2.  COLOR-SHADE CALCULATOR TOOL...
    ImGui::SetNextItemOpen(false, ImGuiCond_Once);
    if ( ImGui::CollapsingHeader("Color Tint/Shade Calculator") ) {
        this->ColorShaderTool();
    }

    
    //  3.  COLORMAP CREATOR TOOL...
    ImGui::SetNextItemOpen(false, ImGuiCond_Once);
    if ( ImGui::CollapsingHeader("Color-Map Creator") ) {
        this->ColorMapCreatorTool();
    }
    
    
    ImGui::End();
    return;
}












// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //

enum CopyFormat : int { FMT_HEX, FMT_HEXA, FMT_IM_COL32, FMT_IMVEC4, FMT_COUNT };


//  "shader"
//
namespace shader {
    //  constexpr size_t                BUFFER_SIZE             = 256;
    static bool                     first_frame             = true;
    
    //  DIMENSIONS...
    static ImVec2                   base_img_size           = ImVec2(180,   180);
    static ImVec2                   palette_img_size        = ImVec2(30,    30);
    static float                    LABEL_COLUMN_WIDTH      = 150.0f;
    static float                    WIDGET_COLUMN_WIDTH     = 300.0f;

    static ImVec4                   base_color              = ImVec4(0.224f, 0.467f, 0.847, 1.0f);    //  Base color in RGB (normalized to [0,1]).
    //  static char                     hex_input[BUFFER_SIZE]  = "#728C9A";    //  Hex input for base color.
    static int                      steps                   = 3;            //  Steps and lightness delta for variants.
    static float                    delta_l                 = 0.1f;

    //  Table layout constants
    static bool                     freeze_header           = false;
    static bool                     freeze_column           = false;
    static bool                     stretch_column_1        = true;

    static ImGuiTableColumnFlags    col0_flags              = ImGuiTableColumnFlags_WidthFixed;
    static ImGuiTableColumnFlags    col1_flags              = stretch_column_1 ? ImGuiTableColumnFlags_WidthStretch : ImGuiTableColumnFlags_WidthFixed;
    static ImGuiTableFlags          table_flags             = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable | ImGuiTableFlags_NoKeepColumnsVisible; //ImGuiTableFlags_ScrollX;
    //  static ImGuiSliderFlags         SLIDER_FLAGS            = ImGuiSliderFlags_AlwaysClamp;

    //              COLOR-INPUT FLAGS...
    static ImGuiColorEditFlags      COLOR_INPUT_FLAGS       = ImGuiColorEditFlags_None | ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_NoSmallPreview; //ImGuiColorEditFlags_NoAlpha
    //              COLOR-IMAGE FLAGS...    ImGuiColorEditFlags_AlphaPreviewHalf
    static ImGuiColorEditFlags      BASE_IMG_FLAGS          = ImGuiColorEditFlags_PickerHueBar | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoInputs;    // ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoInputs; //ImGuiColorEditFlags_NoAlpha
    static ImGuiColorEditFlags      PALETTE_IMG_FLAGS       = ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_NoOptions; //ImGuiColorEditFlags_NoAlpha
    
    
    static int                      copy_format             = FMT_HEX;  // default
    static const char *             copy_format_labels[FMT_COUNT] = {
        "#RRGGBB",        //   0  e.g.  #1E90FF
        "#RRGGBBAA",      //   1  with alpha
        "IM_COL32",       //   2  IM_COL32(30,144,255,255)
        "ImVec4"          //   3  ImVec4(0.118f,0.565f,1.000f,1.0f)
    };
}



// *************************************************************************** //
// *************************************************************************** //

//  "_color_shader_widgets"
//
void _color_shader_widgets(void)
{
    using namespace shader;
    
    
    //  1.  PREVIEW OF BASE COLOR...
    //ImGui::ColorButton("##BaseColorImage",  base_color,     BASE_IMG_FLAGS,    base_img_size);
    ImGui::SetNextItemWidth(base_img_size.x);
    ImGui::ColorPicker4("##BaseColorImage",  (float*)&base_color,     BASE_IMG_FLAGS); //,    base_img_size);


    //  1.  TABLE OF WIDGETS...
    if (ImGui::BeginTable("ColorToolControls", 2, table_flags))
    {
        if (freeze_column || freeze_header)
            ImGui::TableSetupScrollFreeze(freeze_column ? 1 : 0, freeze_header ? 1 : 0);

        ImGui::TableSetupColumn("Label",  col0_flags, LABEL_COLUMN_WIDTH);
        ImGui::TableSetupColumn("Widget", col1_flags, stretch_column_1 ? 1.0f : WIDGET_COLUMN_WIDTH);
        // ImGui::TableHeadersRow();

        
        //  0.      COPY SELECTION TYPE...
        ImGui::TableNextRow();  ImGui::TableSetColumnIndex(0);  ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted("Copy format");
        ImGui::TableSetColumnIndex(1);      ImGui::SetNextItemWidth( ImGui::GetColumnWidth() );
        ImGui::SetNextItemWidth(ImGui::GetColumnWidth());
        ImGui::Combo("##copy_fmt", &copy_format, copy_format_labels, FMT_COUNT);
        

        //  1A.     BASE-COLOR [Hex INPUT]...
        ImGui::TableNextRow();      ImGui::TableSetColumnIndex(0);      ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted("Base Color");
        ImGui::TableSetColumnIndex(1);      ImGui::SetNextItemWidth( ImGui::GetColumnWidth() );
        ImGui::ColorEdit4("##HexInput",     (float*)&base_color,            COLOR_INPUT_FLAGS | ImGuiColorEditFlags_DisplayHex);


        //  1B.     BASE-COLOR [RGB INPUT]...
        ImGui::TableNextRow();      ImGui::TableSetColumnIndex(0);      ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted("(RGB)");
        ImGui::TableSetColumnIndex(1);      ImGui::SetNextItemWidth( ImGui::GetColumnWidth() );
        ImGui::ColorEdit4("##BaseColorRGB",     (float*)&base_color,        COLOR_INPUT_FLAGS | ImGuiColorEditFlags_DisplayRGB);


        //  1C.     BASE-COLOR [FLOAT INPUT]...
        ImGui::TableNextRow();      ImGui::TableSetColumnIndex(0);      ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted("(Float)");
        ImGui::TableSetColumnIndex(1);      ImGui::SetNextItemWidth( ImGui::GetColumnWidth() );
        ImGui::ColorEdit4("##BaseColorFloat",     (float*)&base_color,        COLOR_INPUT_FLAGS | ImGuiColorEditFlags_Float);


        //  1D.     BASE-COLOR [HSV INPUT]...
        ImGui::TableNextRow();      ImGui::TableSetColumnIndex(0);      ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted("(HSV)");
        ImGui::TableSetColumnIndex(1);      ImGui::SetNextItemWidth( ImGui::GetColumnWidth() );
        ImGui::ColorEdit4("##BaseColorHSV",     (float*)&base_color,        COLOR_INPUT_FLAGS | ImGuiColorEditFlags_DisplayHSV);


        //  3.      NUMBER-OF-STEPS...
        ImGui::TableNextRow();  ImGui::TableSetColumnIndex(0);  ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted("Steps");
        ImGui::TableSetColumnIndex(1);      ImGui::SetNextItemWidth( ImGui::GetColumnWidth() );
        ImGui::SliderInt("##steps", &steps, 2, 10);


        //  4.      LIGHT-NESS / SHADE ITERATION...
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted("Shading Amount");
        ImGui::TableSetColumnIndex(1);      ImGui::SetNextItemWidth( ImGui::GetColumnWidth() );
        //ImGui::SliderFloat("##delta_l", &delta_l, 0.001f, 0.500f, "%.3f%%");
        
        float delta_pct = delta_l * 100.0f;                      // 0.1 → 50.0 %
        if (ImGui::SliderFloat("##delta_l",        // label
                               &delta_pct,         // displayed %
                               0.1f, 50.0f,        // min / max %
                               "%.1f%%"))          // one decimal place
        {
            delta_l = delta_pct / 100.0f;          // convert back to 0 – 0.5
        }

        ImGui::EndTable();
    }


    return;
}



//  "_color_shader_table"
//
static void _color_shader_table(void)
{
    using namespace shader;


    //  GENERATE THE COLOR PALETTE...
    //  3.  Helper: color button with tooltip & copy-on-click
    //
    auto to_string_color = [&](const ImVec4& c)->std::string
    {
        int r = int(c.x * 255.0f + 0.5f);
        int g = int(c.y * 255.0f + 0.5f);
        int b = int(c.z * 255.0f + 0.5f);
        int a = int(c.w * 255.0f + 0.5f);
        char buf[64]{};

        switch (copy_format)
        {
            case FMT_HEX:      snprintf(buf, sizeof(buf), "#%02X%02X%02X",       r, g, b);                break;
            case FMT_HEXA:     snprintf(buf, sizeof(buf), "#%02X%02X%02X%02X",   r, g, b, a);             break;
            case FMT_IM_COL32: snprintf(buf, sizeof(buf), "IM_COL32(%d,%d,%d,%d)", r, g, b, a);          break;
            case FMT_IMVEC4:   snprintf(buf, sizeof(buf), "ImVec4(%.3ff,%.3ff,%.3ff,%.3ff)",
                                        c.x, c.y, c.z, c.w);                                                     break;
            default:           snprintf(buf, sizeof(buf), "#%02X%02X%02X",       r, g, b);                break;
        }
        return {buf};
    };

    auto color_button_with_tooltip = [&](const char* id, const ImVec4& col)
    {
        ImGui::ColorButton(id, col, PALETTE_IMG_FLAGS, palette_img_size);
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("%s\n(click to copy)", to_string_color(col).c_str());
        if (ImGui::IsItemClicked())
            ImGui::SetClipboardText(to_string_color(col).c_str());
    };
    


    //  4.  Generate palette table with fixed first column and stretch others
    float           h, s, l;
    float           cr, cg, cb;
    ImGui::ColorConvertRGBtoHSV(base_color.x, base_color.y, base_color.z, h, s, l);
    float           h_comp          = fmodf(h + 0.5f, 1.0f);
    ImGui::ColorConvertHSVtoRGB(h_comp, s, l, cr, cg, cb);
    ImVec4          comp_color      = ImVec4(cr, cg, cb, base_color.w);
    int             cols            = steps + 1; // extra for row labels
    static float    C_WIDTH         = -1.0f;
        
    ImGui::ColorConvertRGBtoHSV(base_color.x, base_color.y, base_color.z, h, s, l);
    if (ImGui::BeginTable("ColorPalette", cols, table_flags))
    {
        // Setup columns: fixed label col, then stretch columns for swatches
        //  ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed, LABEL_COLUMN_WIDTH);
        ImGui::TableSetupColumn("Type",     col0_flags,     LABEL_COLUMN_WIDTH);
        
        C_WIDTH    = (1/steps) * ImGui::GetContentRegionAvail().x;
        
        
        for (int i = 0; i < steps; ++i)
            ImGui::TableSetupColumn( ("##col" + std::to_string(i)).c_str(), ImGuiTableColumnFlags_None, C_WIDTH);     //ImGuiTableColumnFlags_WidthStretch ImGuiTableColumnFlags_WidthFixed


        // Header row: lightness change
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::TextUnformatted("");
        for (int i = 0; i < steps; ++i)
        {
            ImGui::TableSetColumnIndex(i + 1);
            char hdr[16];
            if (i == 0) strcpy(hdr, "0%"); else snprintf(hdr, sizeof(hdr), "%.0f%%", i * delta_l * 100.0f);
            ImGui::TextUnformatted(hdr);
        }

        // Shades row
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::TextUnformatted("Shades");
        for (int i = 0; i < steps; ++i)
        {
            ImGui::TableSetColumnIndex(i + 1);
            float new_l = ImClamp(l - i * delta_l, 0.0f, 1.0f);
            float r, g, b;
            ImGui::ColorConvertHSVtoRGB(h, s, new_l, r, g, b);
            color_button_with_tooltip(("##shade" + std::to_string(i)).c_str(), ImVec4(r, g, b, base_color.w));
            ImGui::SameLine();
            char hex[8]; snprintf(hex, sizeof(hex), "#%02X%02X%02X", int(r*255), int(g*255), int(b*255));
            ImGui::TextUnformatted(hex);
        }

        // Tints row
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::TextUnformatted("Tints");
        for (int i = 0; i < steps; ++i)
        {
            ImGui::TableSetColumnIndex(i + 1);
            float new_l = ImClamp(l + i * delta_l, 0.0f, 1.0f);
            float r, g, b;
            ImGui::ColorConvertHSVtoRGB(h, s, new_l, r, g, b);
            color_button_with_tooltip(("##tint" + std::to_string(i)).c_str(), ImVec4(r, g, b, base_color.w));
            ImGui::SameLine();
            char hex[8]; snprintf(hex, sizeof(hex), "#%02X%02X%02X", int(r*255), int(g*255), int(b*255));
            ImGui::TextUnformatted(hex);
        }

        // Complementary Shades row
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::TextUnformatted("Complimentary Shades");
        for (int i = 0; i < steps; ++i)
        {
            ImGui::TableSetColumnIndex(i + 1);
            float new_l = ImClamp(l - i * delta_l, 0.0f, 1.0f);
            float r, g, b;
            ImGui::ColorConvertHSVtoRGB(h_comp, s, new_l, r, g, b);
            color_button_with_tooltip(("##cshade" + std::to_string(i)).c_str(), ImVec4(cr * (1 - float(i) / steps) + r * (float(i) / steps),
                                                                                   cg * (1 - float(i) / steps) + g * (float(i) / steps),
                                                                                   cb * (1 - float(i) / steps) + b * (float(i) / steps), base_color.w));
            ImGui::SameLine();
            char hex[8]; snprintf(hex, sizeof(hex), "#%02X%02X%02X", int(r*255), int(g*255), int(b*255));
            ImGui::TextUnformatted(hex);
        }

        // Complementary Tints row
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::TextUnformatted("Compimentary Tints");
        for (int i = 0; i < steps; ++i)
        {
            ImGui::TableSetColumnIndex(i + 1);
            float new_l = ImClamp(l + i * delta_l, 0.0f, 1.0f);
            float r, g, b;
            ImGui::ColorConvertHSVtoRGB(h_comp, s, new_l, r, g, b);
            color_button_with_tooltip(("##ctint" + std::to_string(i)).c_str(), ImVec4(cr * (1 - float(i) / steps) + r * (float(i) / steps),
                                                                                   cg * (1 - float(i) / steps) + g * (float(i) / steps),
                                                                                   cb * (1 - float(i) / steps) + b * (float(i) / steps), base_color.w));
            ImGui::SameLine();
            char hex[8]; snprintf(hex, sizeof(hex), "#%02X%02X%02X", int(r*255), int(g*255), int(b*255));
            ImGui::TextUnformatted(hex);
        }

        ImGui::EndTable();
    }


    return;
}



//  "ColorShaderTool"
//
void App::ColorShaderTool(void)
{
    using namespace shader;
    [[maybe_unused]] ImGuiIO &      io                      = ImGui::GetIO(); (void)io;
    [[maybe_unused]] ImGuiStyle &   style                   = ImGui::GetStyle();
    
    if (first_frame) {
        first_frame                 = false;
        base_img_size.x            *= S.m_dpi_scale;        base_img_size.y            *= S.m_dpi_scale;
        palette_img_size.x         *= S.m_dpi_scale;        palette_img_size.y         *= S.m_dpi_scale;
        LABEL_COLUMN_WIDTH          = 150.0f * S.m_dpi_scale;
        WIDGET_COLUMN_WIDTH         = 300.0f * S.m_dpi_scale;
    }
    
    
    
    _color_shader_widgets();
    
    
    ImGui::NewLine();
    ImGui::SeparatorText("Color Palette");
    
    
    _color_shader_table();
    
    
    
    return;
}












// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //

namespace cmap {
    //  CONSTANTS...
    constexpr size_t                BUFF_SIZE           = 256;
    constexpr float                 MARGIN              = 0.80f;
    constexpr float                 LABEL_WIDTH         = 0.15f;
    constexpr float                 PREVIEW_WIDTH       = 40.0f;
    static constexpr const char *   EXPORT_MODES[]      = {"Float", "Hex"};
    //
    //  TABLE...
    //      constexpr float                 IDX_COL_WIDTH       = 20.0f;
    //      constexpr float                 COLOR_COL_WIDTH     = -1.0f;
    //      constexpr float                 ORDER_COL_WIDTH     = 80.0f;
    constexpr ImGuiTableFlags       TABLE_FLAGS         = ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable;
    //
    // SLIDER WIDGETS...
    //      constexpr ImGuiSliderFlags      POS_FLAGS           = ImGuiSliderFlags_AlwaysClamp;
    //
    //  LOOK-UP TABLE (LUT) RESOLUTION...
    constexpr int                   LUT_SIZE            = 256;
//
//
//
    enum class SampleMode { Auto = 0, Custom = 1 };

    static SampleMode load_mode   = SampleMode::Auto;
    static int        load_N      = 32;      // desired table points when custom

    static SampleMode export_load_mode = SampleMode::Auto;
    static int        export_N    = 256;     // desired LUT size when custom
    // (export_N slider will be clamped to ≥ current control-point count)
//
//
//
    // 2) Control‑point storage ------------------------------------------
    static std::vector<std::pair<float,ImVec4>> pts = {
        {0.0f, ImVec4(0.231f, 0.298f, 0.753f, 1.0f)},
        {1.0f, ImVec4(0.865f, 0.000f, 0.000f, 1.0f)}
    };
//
//
//
    static char                 csv_buf[4096]           = "(#FF0000FF, 0.000), (#FF7F00FF, 0.167), (#FFFF00FF, 0.333), (#00FF00FF, 0.500), (#0000FFFF, 0.667), (#4B0082FF, 0.833), (#8F00FFFF, 1.000)";
    static char                 name_buf[BUFF_SIZE]     = "MyColormap";
    static int                  load_idx                = ImPlotColormap_Viridis;
//
//
//
    static ImPlotColormap       cmap_custom             = -1;
    static bool                 applied                 = false;
    static int                  export_mode             = 1;
//
//
//
}


// ------------------------- helpers -----------------------------------------
static ImVec4 lerp(const ImVec4 &a, const ImVec4 & b, float t) {
    return ImVec4(a.x + (b.x - a.x) * t,
                  a.y + (b.y - a.y) * t,
                  a.z + (b.z - a.z) * t,
                  a.w + (b.w - a.w) * t);
}

static ImVec4 sample_cmap(float t, const std::vector<std::pair<float,ImVec4>> & points)
{
    if (points.empty())                   return ImVec4(0,0,0,1);
    if (t <= points.front().first)        return points.front().second;
    if (t >= points.back().first)         return points.back().second;
    for (size_t i = 0; i + 1 < points.size(); ++i) {
        const auto &l = points[i];
        const auto &r = points[i + 1];
        if (t >= l.first && t <= r.first) {
            float u = (t - l.first)/(r.first - l.first);
            return lerp(l.second, r.second, u);
        }
    }
    return points.back().second; // fallback
}


static bool parse_hex_rgba(const char* str, ImVec4 &out) {
    // expects 8 hex digits (RGBA)
    if(str[0]=='#') ++str;
    if(std::strlen(str)!=8) return false;
    unsigned int v; if(sscanf(str, "%8x", &v)!=1) return false;
    unsigned r=(v>>24)&0xFF, g=(v>>16)&0xFF, b=(v>>8)&0xFF, a=v&0xFF;
    out.x=r/255.0f; out.y=g/255.0f; out.z=b/255.0f; out.w=a/255.0f; return true;
}

static bool load_from_csv(const char * input, std::vector<std::pair<float,ImVec4>> & points) {
    points.clear();
    const char *p=input;
    while(*p) {
        // skip whitespace and optional opening parenthesis
        while(*p && std::isspace(*p)) ++p;
        if(*p=='(') ++p;
        while(*p && std::isspace(*p)) ++p;
        // parse #RRGGBBAA
        char hex[10]={0}; int hidx=0;
        if(*p!='#') return false;
        hex[hidx++]=*p++;
        for(int i=0;i<8 && std::isxdigit(*p);++i) hex[hidx++]=*p++;
        hex[hidx]='\0';
        ImVec4 col; if(!parse_hex_rgba(hex,col)) return false;
        // skip comma
        while(*p && *p!=',') ++p; if(*p!=',') return false; ++p;
        // parse position float
        float pos; int n=0; if(sscanf(p,"%f%n", &pos,&n)!=1) return false; p+=n;
        // advance to ')' or ','
        while(*p && *p!=')' && *p!=',') ++p;
        if(*p==')') ++p; // consume ')'
        // push
        points.emplace_back(std::clamp(pos,0.0f,1.0f), col);
        // skip any trailing commas
        while(*p && (*p==','||std::isspace(*p))) ++p;
    }
    // sort by position
    std::sort(points.begin(),points.end(),[](auto&a,auto&b){return a.first<b.first;});
    return !points.empty();
}


// -------------------- More UI Helpers -------------------------------------
auto RadioPair = [](const char * label1, const char * label2, cmap::SampleMode& m)
{
    int v = static_cast<int>(m);
    ImGui::RadioButton(label1, &v, 0);
    ImGui::SameLine();
    ImGui::RadioButton(label2, &v, 1);
    m = static_cast<cmap::SampleMode>(v);
};









// ──────────────────────────────────────────────────────────────────────
//  FUNCTIONS...
// ──────────────────────────────────────────────────────────────────────

//  "CMAPDemoWidget"
//
void CMAPDemoWidget(void)
{
    static int      cmap_demo       = ImPlotColormap_Viridis;
    static float    t_demo          = 0.5f;
    static ImVec4   col_demo;

    ImGui::SetNextItemOpen(false, ImGuiCond_Once);
    //
    if (ImGui::TreeNode("Demo Widgets"))
    {
        if (ImPlot::ColormapButton("Button", ImVec2(0,0), cmap_demo))
            cmap_demo = (cmap_demo + 1) % ImPlot::GetColormapCount();
    
        ImGui::ColorButton("##Display", col_demo, ImGuiColorEditFlags_NoInputs);
        ImGui::SameLine();
        ImPlot::ColormapSlider("Slider", &t_demo, &col_demo, "%.3f", cmap_demo);

        ImPlot::ColormapIcon(cmap_demo); ImGui::SameLine(); ImGui::Text("Icon");

        static ImPlotColormapScaleFlags flags_demo = 0;
        static float scale_demo[2] = {0, 100};
        ImPlot::ColormapScale("Scale", scale_demo[0], scale_demo[1], ImVec2(0,0), "%g dB", flags_demo, cmap_demo);
        ImGui::InputFloat2("Scale", scale_demo);
        #define CHECKBOX(F) ImGui::CheckboxFlags("#" #F, &flags_demo, F)
        CHECKBOX(ImPlotColormapScaleFlags_NoLabel);
        CHECKBOX(ImPlotColormapScaleFlags_Opposite);
        CHECKBOX(ImPlotColormapScaleFlags_Invert);
        #undef CHECKBOX
        
        // ------------------------------------------------------------------
        ImGui::TreePop();
    }

    return;
}



// ──────────────────────────────────────────────────────────────────────
// ──────────────────────────────────────────────────────────────────────

// ──────────────────────────────────────────────────────────────────────
// 1.  LOADING FUNCTION...
// ──────────────────────────────────────────────────────────────────────
bool CMAPLoader(void)
{
    using namespace cmap;

    // 5) LOAD COLOR MAP... -------------------------------------------------
    //
    // ----------- LOAD BUILT‑IN ----------------------------------------
    const int                   cmap_count              = ImPlot::GetColormapCount();
    const char *                current_name            = ImPlot::GetColormapName(load_idx);
    
    
    ImGui::TextUnformatted("Load colormap from CSV values.  Ex: \"(#FF0000FF, 0.00), (#00FF0088, 0.50), (#0000FFFF, 0.99)\"");
    
    ImGui::InputTextMultiline("##CSV", csv_buf, IM_ARRAYSIZE(csv_buf), ImVec2(-FLT_MIN, 60));
    //ImGui::SameLine();
    if(ImGui::Button("Parse CSV")) {
        if(!load_from_csv(csv_buf, pts)) ImGui::OpenPopup("csv_err");
    }
    if(ImGui::BeginPopupModal("csv_err", nullptr, ImGuiWindowFlags_AlwaysAutoResize)){
        ImGui::Text("Failed to parse CSV string.\nExpected format: (#RRGGBBAA, pos), …");
        if(ImGui::Button("OK")) ImGui::CloseCurrentPopup();
        ImGui::EndPopup();
    }
    
    
    
    
    
    // ──────────────────────────────────────────────────────────────────────
    // 3.  LOAD section  (insert inside your existing “Load Colormap” UI)
    // ──────────────────────────────────────────────────────────────────────
    ImGui::SeparatorText("Load Sampling");
    ImGui::BeginGroup();
    //
        RadioPair("Auto##load", "Custom##load", load_mode);
        if (load_mode == SampleMode::Custom) {
            ImGui::SameLine();
            ImGui::SliderInt("##loadN", &load_N, 2, 256, "Create %d Colors");
        }
    //
    ImGui::EndGroup();
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal))
        ImGui::SetTooltip( "Compute N evenly-distributed color-samples from the colormap." );



    utl::LeftLabel2("Load Colormap", LABEL_WIDTH,   MARGIN);
    ImGui::PushID("load_section");
    if(ImGui::BeginCombo("##LoadColormap", current_name))
    {
        for(int n=0;n<cmap_count;++n) {
            bool sel = (load_idx==n);
            if(ImGui::Selectable(ImPlot::GetColormapName(n), sel)) load_idx=n;
            if(sel) ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }
    ImGui::SameLine();
    if( ImGui::Button("Load", ImVec2(-1, 0)) )
    {
        pts.clear();
        int src_sz = ImPlot::GetColormapSize(load_idx);

        // number of samples we’ll actually pull
        int N = (load_mode == SampleMode::Auto)
                  ? src_sz
                  : std::clamp(load_N, 2, src_sz);

        for (int i = 0; i < N; ++i) {
            // evenly spaced indices through original map
            int src_idx = static_cast<int>(std::round(i * (src_sz - 1) / float(N - 1)));
            float t     = (N > 1) ? i / float(N - 1) : 0.0f;
            pts.emplace_back(t, ImPlot::GetColormapColor(src_idx, load_idx));
        }
        // ensure unique name suggestion
        std::snprintf(name_buf,sizeof(name_buf),"%s_copy", current_name);
    }
    ImGui::PopID();



    // ------------------------------------------------------------------
    // 6) Name input ------------------------------------------------------
    
    ImGui::Separator();
    utl::LeftLabel2("Colormap Name", LABEL_WIDTH);
    ImGui::InputText("##ColormapName",  name_buf,   BUFF_SIZE);
    if ( ImGui::Button("Add Control Point") ) {
        pts.emplace_back(0.5f, ImGui::GetStyle().Colors[ImGuiCol_PlotLines]);
        //  order_changed = true;   // will renormalize below
    }
    ImGui::SameLine();
    bool    apply               = ImGui::Button("Apply");
    
    
    return apply;
}



//  "CMAPTable"
//
void CMAPTable(void)
{
    using namespace cmap;
    //  bool                    changed             = false;
    bool                    pos_changed         = false;   // slider modified
    bool                    order_changed       = false;    // up/down swap



    // ------------------------------------------------------------------
    // 3) Table -----------------------------------------------------------
    if(ImGui::BeginTable("ControlPoints", 3, TABLE_FLAGS))
    {
        // Weight‑based sizing so nothing gets squeezed out of view
        ImGui::TableSetupColumn("Index",            ImGuiTableColumnFlags_WidthStretch, 0.15f);
        ImGui::TableSetupColumn("Color/Position",   ImGuiTableColumnFlags_WidthStretch, 0.55f);
        ImGui::TableSetupColumn("Controls",         ImGuiTableColumnFlags_WidthStretch, 0.30f);
        ImGui::TableHeadersRow();

        for (int i = 0; i < static_cast<int>(pts.size()); /* manual increment at end */)
        {
            ImGui::TableNextRow();
            ImGui::PushID(i);

            // ----------------------------------------------------------------------
            // column 0 : index label
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("%d", i);

            // ----------------------------------------------------------------------
            // column 1 : position slider + color edit
            ImGui::TableSetColumnIndex(1);
            float &t  = pts[i].first;
            ImVec4 &c = pts[i].second;
            float tmp = t;

            const float avail = ImGui::GetContentRegionAvail().x;
            ImGui::SetNextItemWidth(avail * 0.60f);
            if (ImGui::SliderFloat("##pos", &tmp, 0.0f, 1.0f, "%.4f") && tmp != t) {
                t           = tmp;
                pos_changed = true;
            }
            ImGui::SameLine();
            ImGui::SetNextItemWidth(avail * 0.35f);
            ImGui::ColorEdit4("##clr", &c.x, ImGuiColorEditFlags_NoInputs);

            // ----------------------------------------------------------------------
            // column 2 : control buttons
            ImGui::TableSetColumnIndex(2);
            bool at_top    = (i == 0);
            bool at_bottom = (i == static_cast<int>(pts.size()) - 1);
            bool remove_row = false;

            ImGui::BeginGroup();
            {
                ImGui::BeginDisabled(at_top);
                if (ImGui::ArrowButton("##up", ImGuiDir_Up)) {
                    std::swap(pts[i], pts[i - 1]);
                    order_changed = true;
                }
                ImGui::EndDisabled();
                ImGui::SameLine(0.0f, 3.0f);

                ImGui::BeginDisabled(at_bottom);
                if (ImGui::ArrowButton("##dn", ImGuiDir_Down)) {
                    std::swap(pts[i], pts[i + 1]);
                    order_changed = true;
                }
                ImGui::EndDisabled();
                ImGui::SameLine(0.0f, 3.0f);

                if (ImGui::Button("+##add")) {
                    pts.insert(pts.begin() + i + 1, {t, c});
                    order_changed = true;
                }
                if (ImGui::IsItemHovered())
                    ImGui::SetTooltip("Copy and paste color to row below");
                ImGui::SameLine(0.0f, 3.0f);

                if (ImGui::Button("X##del")) {
                    remove_row = true; // defer erase until after EndGroup/PopID
                }
                if (ImGui::IsItemHovered())
                    ImGui::SetTooltip("Delete control point");
            }
            ImGui::EndGroup();

            ImGui::PopID();

            if (remove_row) {
                pts.erase(pts.begin() + i);
                order_changed = true;
                // do not increment i -> next row now occupies current index
            }
            else {
                ++i; // normal increment
            }
        }
        
        ImGui::EndTable();
    }// END "if BeginTable".
    
    if(order_changed){size_t n=pts.size();if(n>1){for(size_t k=0;k<n;++k)pts[k].first=(float)k/(float)(n-1);} }
    else if(pos_changed){std::sort(pts.begin(),pts.end(),[](auto&a,auto&b){return a.first<b.first;});}


    // ------------------------------------------------------------------
    // 4) Ensure positions monotonic -------------------------------------
    if (order_changed) {
        const size_t n = pts.size();
        if (n > 1) {
            for (size_t k = 0; k < n; ++k)
                pts[k].first = (float)k / (float)(n - 1);
        }
    } else if (pos_changed) {
        std::sort(pts.begin(), pts.end(), [](auto &a, auto &b){ return a.first < b.first; });
    }
    
    return;
}


    



//  "CMAPPreview"
//
void CMAPPreview(bool apply)
{
    using namespace cmap;

    // 5) Apply + preview -------------------------------------------------
    //
    if (cmap_custom >= 0) {
        
        float avail_w = ImGui::GetContentRegionAvail().x;
        ImPlot::ColormapButton("##preview", ImVec2(avail_w, PREVIEW_WIDTH), cmap_custom);
        ImGui::Text("Index: %d", cmap_custom);
    }
    //
    //
    if (apply || applied) {
        applied = true;
        
        static std::vector<ImVec4> lut(LUT_SIZE);
        for (int i = 0; i < LUT_SIZE; ++i)
            lut[i] = sample_cmap((float)i / (LUT_SIZE - 1), pts);
        std::string base   = name_buf[0] ? name_buf : "Colormap";
        std::string unique = base;
        for (int id = 1; ImPlot::GetColormapIndex(unique.c_str()) != -1; ++id)
            unique = base + "_" + std::to_string(id);
            
        if (apply)
            cmap_custom = ImPlot::AddColormap(unique.c_str(), lut.data(), LUT_SIZE, false);
            
            
        //  6.  ADDITIONAL PREVIEWS FROM DEMO...
        //
        //  if (ImPlot::ColormapButton("Button", ImVec2(0,0), cmap_demo))
        //      cmap_demo = (cmap_demo + 1) % ImPlot::GetColormapCount();
        //  static float    t_custom        = 0.5f;
        static ImVec4   col_custom;
    
        if (ImGui::TreeNode("Additional Previews"))
        {
            ImGui::ColorButton("##Display", col_custom, ImGuiColorEditFlags_NoInputs);
            //ImGui::SameLine();
            //ImPlot::ColormapSlider("Slider", &t_custom, &col_custom, "%.3f", col_custom);

            ImPlot::ColormapIcon(cmap_custom); ImGui::SameLine(); ImGui::Text("Icon");

            static ImPlotColormapScaleFlags flags_preview = 0;
            static float scale_preview[2] = {0, 100};
            
            ImPlot::ColormapScale("Scale", scale_preview[0], scale_preview[1], ImVec2(0,0), "%g dB", flags_preview, cmap_custom);
            ImGui::InputFloat2("Scale", scale_preview);
            #define CHECKBOX(F) ImGui::CheckboxFlags("#" #F, &flags_preview, F)
            CHECKBOX(ImPlotColormapScaleFlags_NoLabel);
            CHECKBOX(ImPlotColormapScaleFlags_Opposite);
            CHECKBOX(ImPlotColormapScaleFlags_Invert);
            #undef CHECKBOX
            ImGui::TreePop();
        }
    }

    return;
}


    



//  "CMAPExport"
//
void CMAPExport(void)
{
    using namespace cmap;

    // 6) Export ----------------------------------------------------------
    //
    /*
    if (applied) {
        ImGui::NewLine();
        ImGui::SeparatorText("Export");
        //
        if (cmap_custom >= 0 && ImGui::Button("Copy as C++ Array")) {
            ImGui::LogToClipboard();
            ImGui::LogText("static const ImVec4 %s[%d] = {\n", name_buf[0] ? name_buf : "custom_cmap", LUT_SIZE);
            const char *indent = "    ";
            for (int i = 0; i < LUT_SIZE; ++i) {
                ImVec4 c = sample_cmap((float)i/(LUT_SIZE-1), pts);
                ImGui::LogText("%sImVec4(%.3ff, %.3ff, %.3ff, %.3ff)%s\n", indent, c.x, c.y, c.z, c.w, i == LUT_SIZE-1 ? "" : ",");
            }
            ImGui::LogText("};\n");
            ImGui::LogFinish();
        }
    }*/
    
    
    
    // ──────────────────────────────────────────────────────────────────────
    // 4.  EXPORT section  (inside “Apply / preview / export” block)
    // ──────────────────────────────────────────────────────────────────────
    ImGui::BeginGroup();
    //
        RadioPair("Auto##export", "Custom##export", export_load_mode);
        if (export_load_mode == SampleMode::Custom)
        {
            // cannot export < current control-points (M)
            int minN = static_cast<int>(pts.size());
            ImGui::SameLine();
            ImGui::SliderInt("##exportN", &export_N, minN, 512, "%d Interpolated Shades");
        }
    //
    ImGui::EndGroup();
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal))
        ImGui::SetTooltip( "Smooths the colormap by computing a total of N linearly-interpolated shades equally spaced between each color defined in the table." );
        


    //  when you build the LUT (before AddColormap / copy-to-clipboard):
    int lut_sz = (export_load_mode == SampleMode::Auto)
                   ? static_cast<int>(pts.size())
                   : std::max(export_N, static_cast<int>(pts.size()));


    std::vector<ImVec4> lut(lut_sz);
    for (int i = 0; i < lut_sz; ++i) {
        float t = (lut_sz > 1) ? i / float(lut_sz - 1) : 0.0f;
        lut[i]  = sample_cmap(t, pts);       // uses your helper
    }
    
    
    
    ImGui::BeginGroup();
    //
        utl::LeftLabel2("Export Type", LABEL_WIDTH, MARGIN);
        ImGui::Combo("##ExportType", &export_mode, EXPORT_MODES, IM_ARRAYSIZE(EXPORT_MODES));
        ImGui::SameLine();
        //
        // Copy as C++ Array ------------------------------------------------
        if( cmap_custom>=0 && ImGui::Button("Copy as C-Style Array", ImVec2(-1,0)) )
        {
            ImGui::LogToClipboard();
            if(export_mode==0)          //  FLOATING-POINT EXPORT...
            {
                ImGui::LogText("static const ImVec4 %s[%d] = {\n", name_buf[0]?name_buf:"custom_cmap", LUT_SIZE);
                const char *    indent = "    ";
                for(int i=0;i<LUT_SIZE;++i){ ImVec4 c = sample_cmap((float)i/(LUT_SIZE-1), pts);
                    ImGui::LogText("%sImVec4(%.3ff, %.3ff, %.3ff, %.3ff)%s, ", indent, c.x,c.y,c.z,c.w, i==LUT_SIZE-1?"":" ,");
                    //ImGui::LogText("%sImVec4(%.3ff, %.3ff, %.3ff, %.3ff)%s\n", indent, c.x,c.y,c.z,c.w, i==LUT_SIZE-1?"":" ,");
                }
                ImGui::LogText("};\n");
            }
            else                        //  RGB EXPORT...
            {
                ImGui::LogText("static const ImU32 %s[%d] = {\n", name_buf[0]?name_buf:"custom_cmap", LUT_SIZE);
                for(int i=0;i<LUT_SIZE;++i)
                {
                    ImVec4      c       = sample_cmap((float)i/(LUT_SIZE-1), pts);
                    ImU32       ui      = ImGui::ColorConvertFloat4ToU32(c);
                    ImGui::LogText("    0x%08X%s", ui, i==LUT_SIZE-1?"":" ,");
                    //ImGui::LogText("    0x%08X%s\n", ui, i==LUT_SIZE-1?"":" ,");
                }
                ImGui::LogText("};\n");
            }
            ImGui::LogFinish();
        }
    //
    ImGui::EndGroup();
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal))
        ImGui::SetTooltip( "Copy this colormap to clipboard (formatted as a C++ array)." );



    return;
}






// ──────────────────────────────────────────────────────────────────────
//      MAIN FUNCTION...
// ──────────────────────────────────────────────────────────────────────
void App::ColorMapCreatorTool()
{
    using namespace cmap;
    const ImVec2    SPACING     = ImVec2( 0.0f, 2.0*ImGui::GetTextLineHeightWithSpacing() );
    bool apply = false;
    
    
    
    //  0.  CMAP WIDGET DEMO...
    ImGui::SetNextItemOpen(true, ImGuiCond_Once);
    if ( ImGui::TreeNode("Colormap Widget Demos") ) {
        CMAPDemoWidget();
        ImGui::TreePop();
    }



    //  1.  COLORMAP "LOADING" SECTION...
    ImGui::Dummy(SPACING);
    ImGui::SetNextItemOpen(true, ImGuiCond_Once);
    if ( ImGui::TreeNode("Load Existing Colormap") ) {
        apply = CMAPLoader();
        ImGui::TreePop();
    }
    //bool apply = CMAPLoader();


    
    //  2.  COLORMAP PREVIEW...
    ImGui::Dummy(SPACING);
    ImGui::SetNextItemOpen(true, ImGuiCond_Once);
    if ( ImGui::TreeNode("Preview Colormap") ) {
        CMAPPreview(apply);
        ImGui::TreePop();
    }



    //  3.  TABLE OF COLORS...
    ImGui::Dummy(SPACING);
    ImGui::SetNextItemOpen(true, ImGuiCond_Once);
    if ( ImGui::TreeNode("Colormap Table") ) {
        CMAPTable();
        ImGui::TreePop();
    }



    //  4.  EXPORT COLORMAP...
    ImGui::Dummy(SPACING);
    ImGui::SetNextItemOpen(true, ImGuiCond_Once);
    if ( ImGui::TreeNode("Save and Export") ) {
        CMAPExport();
        ImGui::TreePop();
    }

    return;
}










// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
}//   END OF "cb" NAMESPACE.
















/*



//-----------------------------------------------------------------------------
// [SECTION] Example App: Simple overlay / ShowExampleAppSimpleOverlay()
//-----------------------------------------------------------------------------

// Demonstrate creating a simple static window with no decoration
// + a context-menu to choose which corner of the screen to use.
static void ShowExampleAppSimpleOverlay(bool* p_open)
{
    static int location = 0;
    ImGuiIO& io = ImGui::GetIO();
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
    if (location >= 0)
    {
        const float PAD = 10.0f;
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImVec2 work_pos = viewport->WorkPos; // Use work area to avoid menu-bar/task-bar, if any!
        ImVec2 work_size = viewport->WorkSize;
        ImVec2 window_pos, window_pos_pivot;
        window_pos.x = (location & 1) ? (work_pos.x + work_size.x - PAD) : (work_pos.x + PAD);
        window_pos.y = (location & 2) ? (work_pos.y + work_size.y - PAD) : (work_pos.y + PAD);
        window_pos_pivot.x = (location & 1) ? 1.0f : 0.0f;
        window_pos_pivot.y = (location & 2) ? 1.0f : 0.0f;
        ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
        ImGui::SetNextWindowViewport(viewport->ID);
        window_flags |= ImGuiWindowFlags_NoMove;
    }
    else if (location == -2)
    {
        // Center window
        ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
        window_flags |= ImGuiWindowFlags_NoMove;
    }
    ImGui::SetNextWindowBgAlpha(0.35f); // Transparent background
    if (ImGui::Begin("Example: Simple overlay", p_open, window_flags))
    {
        IMGUI_DEMO_MARKER("Examples/Simple Overlay");
        ImGui::Text("Simple overlay\n" "(right-click to change position)");
        ImGui::Separator();
        if (ImGui::IsMousePosValid())
            ImGui::Text("Mouse Position: (%.1f,%.1f)", io.MousePos.x, io.MousePos.y);
        else
            ImGui::Text("Mouse Position: <invalid>");
        if (ImGui::BeginPopupContextWindow())
        {
            if (ImGui::MenuItem("Custom",       NULL, location == -1)) location = -1;
            if (ImGui::MenuItem("Center",       NULL, location == -2)) location = -2;
            if (ImGui::MenuItem("Top-left",     NULL, location == 0)) location = 0;
            if (ImGui::MenuItem("Top-right",    NULL, location == 1)) location = 1;
            if (ImGui::MenuItem("Bottom-left",  NULL, location == 2)) location = 2;
            if (ImGui::MenuItem("Bottom-right", NULL, location == 3)) location = 3;
            if (p_open && ImGui::MenuItem("Close")) *p_open = false;
            ImGui::EndPopup();
        }
    }
    ImGui::End();
}









//-----------------------------------------------------------------------------
// [SECTION] Example App: Custom Rendering using ImDrawList API / ShowExampleAppCustomRendering()
//-----------------------------------------------------------------------------

// Add a |_| looking shape
static void PathConcaveShape(ImDrawList* draw_list, float x, float y, float sz)
{
    const ImVec2 pos_norms[] = { { 0.0f, 0.0f }, { 0.3f, 0.0f }, { 0.3f, 0.7f }, { 0.7f, 0.7f }, { 0.7f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };
    for (const ImVec2& p : pos_norms)
        draw_list->PathLineTo(ImVec2(x + 0.5f + (int)(sz * p.x), y + 0.5f + (int)(sz * p.y)));
}

// Demonstrate using the low-level ImDrawList to draw custom shapes.
static void ShowExampleAppCustomRendering(bool* p_open)
{
    if (!ImGui::Begin("Example: Custom rendering", p_open))
    {
        ImGui::End();
        return;
    }
    IMGUI_DEMO_MARKER("Examples/Custom Rendering");

    // Tip: If you do a lot of custom rendering, you probably want to use your own geometrical types and benefit of
    // overloaded operators, etc. Define IM_VEC2_CLASS_EXTRA in imconfig.h to create implicit conversions between your
    // types and ImVec2/ImVec4. Dear ImGui defines overloaded operators but they are internal to imgui.cpp and not
    // exposed outside (to avoid messing with your types) In this example we are not using the maths operators!

    if (ImGui::BeginTabBar("##TabBar"))
    {
        if (ImGui::BeginTabItem("Primitives"))
        {
            ImGui::PushItemWidth(-ImGui::GetFontSize() * 15);
            ImDrawList* draw_list = ImGui::GetWindowDrawList();

            // Draw gradients
            // (note that those are currently exacerbating our sRGB/Linear issues)
            // Calling ImGui::GetColorU32() multiplies the given colors by the current Style Alpha, but you may pass the IM_COL32() directly as well..
            ImGui::Text("Gradients");
            ImVec2 gradient_size = ImVec2(ImGui::CalcItemWidth(), ImGui::GetFrameHeight());
            {
                ImVec2 p0 = ImGui::GetCursorScreenPos();
                ImVec2 p1 = ImVec2(p0.x + gradient_size.x, p0.y + gradient_size.y);
                ImU32 col_a = ImGui::GetColorU32(IM_COL32(0, 0, 0, 255));
                ImU32 col_b = ImGui::GetColorU32(IM_COL32(255, 255, 255, 255));
                draw_list->AddRectFilledMultiColor(p0, p1, col_a, col_b, col_b, col_a);
                ImGui::InvisibleButton("##gradient1", gradient_size);
            }
            {
                ImVec2 p0 = ImGui::GetCursorScreenPos();
                ImVec2 p1 = ImVec2(p0.x + gradient_size.x, p0.y + gradient_size.y);
                ImU32 col_a = ImGui::GetColorU32(IM_COL32(0, 255, 0, 255));
                ImU32 col_b = ImGui::GetColorU32(IM_COL32(255, 0, 0, 255));
                draw_list->AddRectFilledMultiColor(p0, p1, col_a, col_b, col_b, col_a);
                ImGui::InvisibleButton("##gradient2", gradient_size);
            }

            // Draw a bunch of primitives
            ImGui::Text("All primitives");
            static float sz = 36.0f;
            static float thickness = 3.0f;
            static int ngon_sides = 6;
            static bool circle_segments_override = false;
            static int circle_segments_override_v = 12;
            static bool curve_segments_override = false;
            static int curve_segments_override_v = 8;
            static ImVec4 colf = ImVec4(1.0f, 1.0f, 0.4f, 1.0f);
            ImGui::DragFloat("Size", &sz, 0.2f, 2.0f, 100.0f, "%.0f");
            ImGui::DragFloat("Thickness", &thickness, 0.05f, 1.0f, 8.0f, "%.02f");
            ImGui::SliderInt("N-gon sides", &ngon_sides, 3, 12);
            ImGui::Checkbox("##circlesegmentoverride", &circle_segments_override);
            ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
            circle_segments_override |= ImGui::SliderInt("Circle segments override", &circle_segments_override_v, 3, 40);
            ImGui::Checkbox("##curvessegmentoverride", &curve_segments_override);
            ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
            curve_segments_override |= ImGui::SliderInt("Curves segments override", &curve_segments_override_v, 3, 40);
            ImGui::ColorEdit4("Color", &colf.x);

            const ImVec2 p = ImGui::GetCursorScreenPos();
            const ImU32 col = ImColor(colf);
            const float spacing = 10.0f;
            const ImDrawFlags corners_tl_br = ImDrawFlags_RoundCornersTopLeft | ImDrawFlags_RoundCornersBottomRight;
            const float rounding = sz / 5.0f;
            const int circle_segments = circle_segments_override ? circle_segments_override_v : 0;
            const int curve_segments = curve_segments_override ? curve_segments_override_v : 0;
            const ImVec2 cp3[3] = { ImVec2(0.0f, sz * 0.6f), ImVec2(sz * 0.5f, -sz * 0.4f), ImVec2(sz, sz) }; // Control points for curves
            const ImVec2 cp4[4] = { ImVec2(0.0f, 0.0f), ImVec2(sz * 1.3f, sz * 0.3f), ImVec2(sz - sz * 1.3f, sz - sz * 0.3f), ImVec2(sz, sz) };

            float x = p.x + 4.0f;
            float y = p.y + 4.0f;
            for (int n = 0; n < 2; n++)
            {
                // First line uses a thickness of 1.0f, second line uses the configurable thickness
                float th = (n == 0) ? 1.0f : thickness;
                draw_list->AddNgon(ImVec2(x + sz*0.5f, y + sz*0.5f), sz*0.5f, col, ngon_sides, th);                 x += sz + spacing;  // N-gon
                draw_list->AddCircle(ImVec2(x + sz*0.5f, y + sz*0.5f), sz*0.5f, col, circle_segments, th);          x += sz + spacing;  // Circle
                draw_list->AddEllipse(ImVec2(x + sz*0.5f, y + sz*0.5f), ImVec2(sz*0.5f, sz*0.3f), col, -0.3f, circle_segments, th); x += sz + spacing;  // Ellipse
                draw_list->AddRect(ImVec2(x, y), ImVec2(x + sz, y + sz), col, 0.0f, ImDrawFlags_None, th);          x += sz + spacing;  // Square
                draw_list->AddRect(ImVec2(x, y), ImVec2(x + sz, y + sz), col, rounding, ImDrawFlags_None, th);      x += sz + spacing;  // Square with all rounded corners
                draw_list->AddRect(ImVec2(x, y), ImVec2(x + sz, y + sz), col, rounding, corners_tl_br, th);         x += sz + spacing;  // Square with two rounded corners
                draw_list->AddTriangle(ImVec2(x+sz*0.5f,y), ImVec2(x+sz, y+sz-0.5f), ImVec2(x, y+sz-0.5f), col, th);x += sz + spacing;  // Triangle
                //draw_list->AddTriangle(ImVec2(x+sz*0.2f,y), ImVec2(x, y+sz-0.5f), ImVec2(x+sz*0.4f, y+sz-0.5f), col, th);x+= sz*0.4f + spacing; // Thin triangle
                PathConcaveShape(draw_list, x, y, sz); draw_list->PathStroke(col, ImDrawFlags_Closed, th);          x += sz + spacing;  // Concave Shape
                //draw_list->AddPolyline(concave_shape, IM_ARRAYSIZE(concave_shape), col, ImDrawFlags_Closed, th);
                draw_list->AddLine(ImVec2(x, y), ImVec2(x + sz, y), col, th);                                       x += sz + spacing;  // Horizontal line (note: drawing a filled rectangle will be faster!)
                draw_list->AddLine(ImVec2(x, y), ImVec2(x, y + sz), col, th);                                       x += spacing;       // Vertical line (note: drawing a filled rectangle will be faster!)
                draw_list->AddLine(ImVec2(x, y), ImVec2(x + sz, y + sz), col, th);                                  x += sz + spacing;  // Diagonal line

                // Path
                draw_list->PathArcTo(ImVec2(x + sz*0.5f, y + sz*0.5f), sz*0.5f, 3.141592f, 3.141592f * -0.5f);
                draw_list->PathStroke(col, ImDrawFlags_None, th);
                x += sz + spacing;

                // Quadratic Bezier Curve (3 control points)
                draw_list->AddBezierQuadratic(ImVec2(x + cp3[0].x, y + cp3[0].y), ImVec2(x + cp3[1].x, y + cp3[1].y), ImVec2(x + cp3[2].x, y + cp3[2].y), col, th, curve_segments);
                x += sz + spacing;

                // Cubic Bezier Curve (4 control points)
                draw_list->AddBezierCubic(ImVec2(x + cp4[0].x, y + cp4[0].y), ImVec2(x + cp4[1].x, y + cp4[1].y), ImVec2(x + cp4[2].x, y + cp4[2].y), ImVec2(x + cp4[3].x, y + cp4[3].y), col, th, curve_segments);

                x = p.x + 4;
                y += sz + spacing;
            }

            // Filled shapes
            draw_list->AddNgonFilled(ImVec2(x + sz * 0.5f, y + sz * 0.5f), sz * 0.5f, col, ngon_sides);             x += sz + spacing;  // N-gon
            draw_list->AddCircleFilled(ImVec2(x + sz * 0.5f, y + sz * 0.5f), sz * 0.5f, col, circle_segments);      x += sz + spacing;  // Circle
            draw_list->AddEllipseFilled(ImVec2(x + sz * 0.5f, y + sz * 0.5f), ImVec2(sz * 0.5f, sz * 0.3f), col, -0.3f, circle_segments); x += sz + spacing;// Ellipse
            draw_list->AddRectFilled(ImVec2(x, y), ImVec2(x + sz, y + sz), col);                                    x += sz + spacing;  // Square
            draw_list->AddRectFilled(ImVec2(x, y), ImVec2(x + sz, y + sz), col, 10.0f);                             x += sz + spacing;  // Square with all rounded corners
            draw_list->AddRectFilled(ImVec2(x, y), ImVec2(x + sz, y + sz), col, 10.0f, corners_tl_br);              x += sz + spacing;  // Square with two rounded corners
            draw_list->AddTriangleFilled(ImVec2(x+sz*0.5f,y), ImVec2(x+sz, y+sz-0.5f), ImVec2(x, y+sz-0.5f), col);  x += sz + spacing;  // Triangle
            //draw_list->AddTriangleFilled(ImVec2(x+sz*0.2f,y), ImVec2(x, y+sz-0.5f), ImVec2(x+sz*0.4f, y+sz-0.5f), col); x += sz*0.4f + spacing; // Thin triangle
            PathConcaveShape(draw_list, x, y, sz); draw_list->PathFillConcave(col);                                 x += sz + spacing;  // Concave shape
            draw_list->AddRectFilled(ImVec2(x, y), ImVec2(x + sz, y + thickness), col);                             x += sz + spacing;  // Horizontal line (faster than AddLine, but only handle integer thickness)
            draw_list->AddRectFilled(ImVec2(x, y), ImVec2(x + thickness, y + sz), col);                             x += spacing * 2.0f;// Vertical line (faster than AddLine, but only handle integer thickness)
            draw_list->AddRectFilled(ImVec2(x, y), ImVec2(x + 1, y + 1), col);                                      x += sz;            // Pixel (faster than AddLine)

            // Path
            draw_list->PathArcTo(ImVec2(x + sz * 0.5f, y + sz * 0.5f), sz * 0.5f, 3.141592f * -0.5f, 3.141592f);
            draw_list->PathFillConvex(col);
            x += sz + spacing;

            // Quadratic Bezier Curve (3 control points)
            draw_list->PathLineTo(ImVec2(x + cp3[0].x, y + cp3[0].y));
            draw_list->PathBezierQuadraticCurveTo(ImVec2(x + cp3[1].x, y + cp3[1].y), ImVec2(x + cp3[2].x, y + cp3[2].y), curve_segments);
            draw_list->PathFillConvex(col);
            x += sz + spacing;

            draw_list->AddRectFilledMultiColor(ImVec2(x, y), ImVec2(x + sz, y + sz), IM_COL32(0, 0, 0, 255), IM_COL32(255, 0, 0, 255), IM_COL32(255, 255, 0, 255), IM_COL32(0, 255, 0, 255));
            x += sz + spacing;

            ImGui::Dummy(ImVec2((sz + spacing) * 13.2f, (sz + spacing) * 3.0f));
            ImGui::PopItemWidth();
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Canvas"))
        {
            static ImVector<ImVec2> points;
            static ImVec2 scrolling(0.0f, 0.0f);
            static bool opt_enable_grid = true;
            static bool opt_enable_context_menu = true;
            static bool adding_line = false;

            ImGui::Checkbox("Enable grid", &opt_enable_grid);
            ImGui::Checkbox("Enable context menu", &opt_enable_context_menu);
            ImGui::Text("Mouse Left: drag to add lines,\nMouse Right: drag to scroll, click for context menu.");

            // Typically you would use a BeginChild()/EndChild() pair to benefit from a clipping region + own scrolling.
            // Here we demonstrate that this can be replaced by simple offsetting + custom drawing + PushClipRect/PopClipRect() calls.
            // To use a child window instead we could use, e.g:
            //      ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));      // Disable padding
            //      ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(50, 50, 50, 255));  // Set a background color
            //      ImGui::BeginChild("canvas", ImVec2(0.0f, 0.0f), ImGuiChildFlags_Borders, ImGuiWindowFlags_NoMove);
            //      ImGui::PopStyleColor();
            //      ImGui::PopStyleVar();
            //      [...]
            //      ImGui::EndChild();

            // Using InvisibleButton() as a convenience 1) it will advance the layout cursor and 2) allows us to use IsItemHovered()/IsItemActive()
            ImVec2 canvas_p0 = ImGui::GetCursorScreenPos();      // ImDrawList API uses screen coordinates!
            ImVec2 canvas_sz = ImGui::GetContentRegionAvail();   // Resize canvas to what's available
            if (canvas_sz.x < 50.0f) canvas_sz.x = 50.0f;
            if (canvas_sz.y < 50.0f) canvas_sz.y = 50.0f;
            ImVec2 canvas_p1 = ImVec2(canvas_p0.x + canvas_sz.x, canvas_p0.y + canvas_sz.y);

            // Draw border and background color
            ImGuiIO& io = ImGui::GetIO();
            ImDrawList* draw_list = ImGui::GetWindowDrawList();
            draw_list->AddRectFilled(canvas_p0, canvas_p1, IM_COL32(50, 50, 50, 255));
            draw_list->AddRect(canvas_p0, canvas_p1, IM_COL32(255, 255, 255, 255));

            // This will catch our interactions
            ImGui::InvisibleButton("canvas", canvas_sz, ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight);
            const bool is_hovered = ImGui::IsItemHovered(); // Hovered
            const bool is_active = ImGui::IsItemActive();   // Held
            const ImVec2 origin(canvas_p0.x + scrolling.x, canvas_p0.y + scrolling.y); // Lock scrolled origin
            const ImVec2 mouse_pos_in_canvas(io.MousePos.x - origin.x, io.MousePos.y - origin.y);

            // Add first and second point
            if (is_hovered && !adding_line && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
            {
                points.push_back(mouse_pos_in_canvas);
                points.push_back(mouse_pos_in_canvas);
                adding_line = true;
            }
            if (adding_line)
            {
                points.back() = mouse_pos_in_canvas;
                if (!ImGui::IsMouseDown(ImGuiMouseButton_Left))
                    adding_line = false;
            }

            // Pan (we use a zero mouse threshold when there's no context menu)
            // You may decide to make that threshold dynamic based on whether the mouse is hovering something etc.
            const float mouse_threshold_for_pan = opt_enable_context_menu ? -1.0f : 0.0f;
            if (is_active && ImGui::IsMouseDragging(ImGuiMouseButton_Right, mouse_threshold_for_pan))
            {
                scrolling.x += io.MouseDelta.x;
                scrolling.y += io.MouseDelta.y;
            }

            // Context menu (under default mouse threshold)
            ImVec2 drag_delta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Right);
            if (opt_enable_context_menu && drag_delta.x == 0.0f && drag_delta.y == 0.0f)
                ImGui::OpenPopupOnItemClick("context", ImGuiPopupFlags_MouseButtonRight);
            if (ImGui::BeginPopup("context"))
            {
                if (adding_line)
                    points.resize(points.size() - 2);
                adding_line = false;
                if (ImGui::MenuItem("Remove one", NULL, false, points.Size > 0)) { points.resize(points.size() - 2); }
                if (ImGui::MenuItem("Remove all", NULL, false, points.Size > 0)) { points.clear(); }
                ImGui::EndPopup();
            }

            // Draw grid + all lines in the canvas
            draw_list->PushClipRect(canvas_p0, canvas_p1, true);
            if (opt_enable_grid)
            {
                const float GRID_STEP = 64.0f;
                for (float x = fmodf(scrolling.x, GRID_STEP); x < canvas_sz.x; x += GRID_STEP)
                    draw_list->AddLine(ImVec2(canvas_p0.x + x, canvas_p0.y), ImVec2(canvas_p0.x + x, canvas_p1.y), IM_COL32(200, 200, 200, 40));
                for (float y = fmodf(scrolling.y, GRID_STEP); y < canvas_sz.y; y += GRID_STEP)
                    draw_list->AddLine(ImVec2(canvas_p0.x, canvas_p0.y + y), ImVec2(canvas_p1.x, canvas_p0.y + y), IM_COL32(200, 200, 200, 40));
            }
            for (int n = 0; n < points.Size; n += 2)
                draw_list->AddLine(ImVec2(origin.x + points[n].x, origin.y + points[n].y), ImVec2(origin.x + points[n + 1].x, origin.y + points[n + 1].y), IM_COL32(255, 255, 0, 255), 2.0f);
            draw_list->PopClipRect();

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("BG/FG draw lists"))
        {
            static bool draw_bg = true;
            static bool draw_fg = true;
            ImGui::Checkbox("Draw in Background draw list", &draw_bg);
            ImGui::SameLine(); HelpMarker("The Background draw list will be rendered below every Dear ImGui windows.");
            ImGui::Checkbox("Draw in Foreground draw list", &draw_fg);
            ImGui::SameLine(); HelpMarker("The Foreground draw list will be rendered over every Dear ImGui windows.");
            ImVec2 window_pos = ImGui::GetWindowPos();
            ImVec2 window_size = ImGui::GetWindowSize();
            ImVec2 window_center = ImVec2(window_pos.x + window_size.x * 0.5f, window_pos.y + window_size.y * 0.5f);
            if (draw_bg)
                ImGui::GetBackgroundDrawList()->AddCircle(window_center, window_size.x * 0.6f, IM_COL32(255, 0, 0, 200), 0, 10 + 4);
            if (draw_fg)
                ImGui::GetForegroundDrawList()->AddCircle(window_center, window_size.y * 0.6f, IM_COL32(0, 255, 0, 200), 0, 10);
            ImGui::EndTabItem();
        }

        // Demonstrate out-of-order rendering via channels splitting
        // We use functions in ImDrawList as each draw list contains a convenience splitter,
        // but you can also instantiate your own ImDrawListSplitter if you need to nest them.
        if (ImGui::BeginTabItem("Draw Channels"))
        {
            ImDrawList* draw_list = ImGui::GetWindowDrawList();
            {
                ImGui::Text("Blue shape is drawn first: appears in back");
                ImGui::Text("Red shape is drawn after: appears in front");
                ImVec2 p0 = ImGui::GetCursorScreenPos();
                draw_list->AddRectFilled(ImVec2(p0.x, p0.y), ImVec2(p0.x + 50, p0.y + 50), IM_COL32(0, 0, 255, 255)); // Blue
                draw_list->AddRectFilled(ImVec2(p0.x + 25, p0.y + 25), ImVec2(p0.x + 75, p0.y + 75), IM_COL32(255, 0, 0, 255)); // Red
                ImGui::Dummy(ImVec2(75, 75));
            }
            ImGui::Separator();
            {
                ImGui::Text("Blue shape is drawn first, into channel 1: appears in front");
                ImGui::Text("Red shape is drawn after, into channel 0: appears in back");
                ImVec2 p1 = ImGui::GetCursorScreenPos();

                // Create 2 channels and draw a Blue shape THEN a Red shape.
                // You can create any number of channels. Tables API use 1 channel per column in order to better batch draw calls.
                draw_list->ChannelsSplit(2);
                draw_list->ChannelsSetCurrent(1);
                draw_list->AddRectFilled(ImVec2(p1.x, p1.y), ImVec2(p1.x + 50, p1.y + 50), IM_COL32(0, 0, 255, 255)); // Blue
                draw_list->ChannelsSetCurrent(0);
                draw_list->AddRectFilled(ImVec2(p1.x + 25, p1.y + 25), ImVec2(p1.x + 75, p1.y + 75), IM_COL32(255, 0, 0, 255)); // Red

                // Flatten/reorder channels. Red shape is in channel 0 and it appears below the Blue shape in channel 1.
                // This works by copying draw indices only (vertices are not copied).
                draw_list->ChannelsMerge();
                ImGui::Dummy(ImVec2(75, 75));
                ImGui::Text("After reordering, contents of channel 0 appears below channel 1.");
            }
            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }

    ImGui::End();
}

*/




/*

//-----------------------------------------------------------------------------
// [SECTION] Example App: Documents Handling / ShowExampleAppDocuments()
//-----------------------------------------------------------------------------

// Simplified structure to mimic a Document model
struct MyDocument
{
    char        Name[32];   // Document title
    int         UID;        // Unique ID (necessary as we can change title)
    bool        Open;       // Set when open (we keep an array of all available documents to simplify demo code!)
    bool        OpenPrev;   // Copy of Open from last update.
    bool        Dirty;      // Set when the document has been modified
    ImVec4      Color;      // An arbitrary variable associated to the document

    MyDocument(int uid, const char* name, bool open = true, const ImVec4& color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f))
    {
        UID = uid;
        snprintf(Name, sizeof(Name), "%s", name);
        Open = OpenPrev = open;
        Dirty = false;
        Color = color;
    }
    void DoOpen()       { Open = true; }
    void DoForceClose() { Open = false; Dirty = false; }
    void DoSave()       { Dirty = false; }
};

struct ExampleAppDocuments
{
    ImVector<MyDocument>    Documents;
    ImVector<MyDocument*>   CloseQueue;
    MyDocument*             RenamingDoc = NULL;
    bool                    RenamingStarted = false;

    ExampleAppDocuments()
    {
        Documents.push_back(MyDocument(0, "Lettuce",             true,  ImVec4(0.4f, 0.8f, 0.4f, 1.0f)));
        Documents.push_back(MyDocument(1, "Eggplant",            true,  ImVec4(0.8f, 0.5f, 1.0f, 1.0f)));
        Documents.push_back(MyDocument(2, "Carrot",              true,  ImVec4(1.0f, 0.8f, 0.5f, 1.0f)));
        Documents.push_back(MyDocument(3, "Tomato",              false, ImVec4(1.0f, 0.3f, 0.4f, 1.0f)));
        Documents.push_back(MyDocument(4, "A Rather Long Title", false, ImVec4(0.4f, 0.8f, 0.8f, 1.0f)));
        Documents.push_back(MyDocument(5, "Some Document",       false, ImVec4(0.8f, 0.8f, 1.0f, 1.0f)));
    }

    // As we allow to change document name, we append a never-changing document ID so tabs are stable
    void GetTabName(MyDocument* doc, char* out_buf, size_t out_buf_size)
    {
        snprintf(out_buf, out_buf_size, "%s###doc%d", doc->Name, doc->UID);
    }

    // Display placeholder contents for the Document
    void DisplayDocContents(MyDocument* doc)
    {
        ImGui::PushID(doc);
        ImGui::Text("Document \"%s\"", doc->Name);
        ImGui::PushStyleColor(ImGuiCol_Text, doc->Color);
        ImGui::TextWrapped("Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.");
        ImGui::PopStyleColor();

        ImGui::SetNextItemShortcut(ImGuiMod_Ctrl | ImGuiKey_R, ImGuiInputFlags_Tooltip);
        if (ImGui::Button("Rename.."))
        {
            RenamingDoc = doc;
            RenamingStarted = true;
        }
        ImGui::SameLine();

        ImGui::SetNextItemShortcut(ImGuiMod_Ctrl | ImGuiKey_M, ImGuiInputFlags_Tooltip);
        if (ImGui::Button("Modify"))
            doc->Dirty = true;

        ImGui::SameLine();
        ImGui::SetNextItemShortcut(ImGuiMod_Ctrl | ImGuiKey_S, ImGuiInputFlags_Tooltip);
        if (ImGui::Button("Save"))
            doc->DoSave();

        ImGui::SameLine();
        ImGui::SetNextItemShortcut(ImGuiMod_Ctrl | ImGuiKey_W, ImGuiInputFlags_Tooltip);
        if (ImGui::Button("Close"))
            CloseQueue.push_back(doc);
        ImGui::ColorEdit3("color", &doc->Color.x);  // Useful to test drag and drop and hold-dragged-to-open-tab behavior.
        ImGui::PopID();
    }

    // Display context menu for the Document
    void DisplayDocContextMenu(MyDocument* doc)
    {
        if (!ImGui::BeginPopupContextItem())
            return;

        char buf[256];
        sprintf(buf, "Save %s", doc->Name);
        if (ImGui::MenuItem(buf, "Ctrl+S", false, doc->Open))
            doc->DoSave();
        if (ImGui::MenuItem("Rename...", "Ctrl+R", false, doc->Open))
            RenamingDoc = doc;
        if (ImGui::MenuItem("Close", "Ctrl+W", false, doc->Open))
            CloseQueue.push_back(doc);
        ImGui::EndPopup();
    }

    // [Optional] Notify the system of Tabs/Windows closure that happened outside the regular tab interface.
    // If a tab has been closed programmatically (aka closed from another source such as the Checkbox() in the demo,
    // as opposed to clicking on the regular tab closing button) and stops being submitted, it will take a frame for
    // the tab bar to notice its absence. During this frame there will be a gap in the tab bar, and if the tab that has
    // disappeared was the selected one, the tab bar will report no selected tab during the frame. This will effectively
    // give the impression of a flicker for one frame.
    // We call SetTabItemClosed() to manually notify the Tab Bar or Docking system of removed tabs to avoid this glitch.
    // Note that this completely optional, and only affect tab bars with the ImGuiTabBarFlags_Reorderable flag.
    void NotifyOfDocumentsClosedElsewhere()
    {
        for (MyDocument& doc : Documents)
        {
            if (!doc.Open && doc.OpenPrev)
                ImGui::SetTabItemClosed(doc.Name);
            doc.OpenPrev = doc.Open;
        }
    }
};

void ShowExampleAppDocuments(bool* p_open)
{
    static ExampleAppDocuments app;

    // Options
    enum Target
    {
        Target_None,
        Target_Tab,                 // Create documents as local tab into a local tab bar
        Target_DockSpaceAndWindow   // Create documents as regular windows, and create an embedded dockspace
    };
    static Target opt_target = Target_Tab;
    static bool opt_reorderable = true;
    static ImGuiTabBarFlags opt_fitting_flags = ImGuiTabBarFlags_FittingPolicyDefault_;

    // When (opt_target == Target_DockSpaceAndWindow) there is the possibily that one of our child Document window (e.g. "Eggplant")
    // that we emit gets docked into the same spot as the parent window ("Example: Documents").
    // This would create a problematic feedback loop because selecting the "Eggplant" tab would make the "Example: Documents" tab
    // not visible, which in turn would stop submitting the "Eggplant" window.
    // We avoid this problem by submitting our documents window even if our parent window is not currently visible.
    // Another solution may be to make the "Example: Documents" window use the ImGuiWindowFlags_NoDocking.

    bool window_contents_visible = ImGui::Begin("Example: Documents", p_open, ImGuiWindowFlags_MenuBar);
    if (!window_contents_visible && opt_target != Target_DockSpaceAndWindow)
    {
        ImGui::End();
        return;
    }

    // Menu
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            int open_count = 0;
            for (MyDocument& doc : app.Documents)
                open_count += doc.Open ? 1 : 0;

            if (ImGui::BeginMenu("Open", open_count < app.Documents.Size))
            {
                for (MyDocument& doc : app.Documents)
                    if (!doc.Open && ImGui::MenuItem(doc.Name))
                        doc.DoOpen();
                ImGui::EndMenu();
            }
            if (ImGui::MenuItem("Close All Documents", NULL, false, open_count > 0))
                for (MyDocument& doc : app.Documents)
                    app.CloseQueue.push_back(&doc);
            if (ImGui::MenuItem("Exit") && p_open)
                *p_open = false;
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    // [Debug] List documents with one checkbox for each
    for (int doc_n = 0; doc_n < app.Documents.Size; doc_n++)
    {
        MyDocument& doc = app.Documents[doc_n];
        if (doc_n > 0)
            ImGui::SameLine();
        ImGui::PushID(&doc);
        if (ImGui::Checkbox(doc.Name, &doc.Open))
            if (!doc.Open)
                doc.DoForceClose();
        ImGui::PopID();
    }
    ImGui::PushItemWidth(ImGui::GetFontSize() * 12);
    ImGui::Combo("Output", (int*)&opt_target, "None\0TabBar+Tabs\0DockSpace+Window\0");
    ImGui::PopItemWidth();
    bool redock_all = false;
    if (opt_target == Target_Tab)                { ImGui::SameLine(); ImGui::Checkbox("Reorderable Tabs", &opt_reorderable); }
    if (opt_target == Target_DockSpaceAndWindow) { ImGui::SameLine(); redock_all = ImGui::Button("Redock all"); }

    ImGui::Separator();

    // About the ImGuiWindowFlags_UnsavedDocument / ImGuiTabItemFlags_UnsavedDocument flags.
    // They have multiple effects:
    // - Display a dot next to the title.
    // - Tab is selected when clicking the X close button.
    // - Closure is not assumed (will wait for user to stop submitting the tab).
    //   Otherwise closure is assumed when pressing the X, so if you keep submitting the tab may reappear at end of tab bar.
    //   We need to assume closure by default otherwise waiting for "lack of submission" on the next frame would leave an empty
    //   hole for one-frame, both in the tab-bar and in tab-contents when closing a tab/window.
    //   The rarely used SetTabItemClosed() function is a way to notify of programmatic closure to avoid the one-frame hole.

    // Tabs
    if (opt_target == Target_Tab)
    {
        ImGuiTabBarFlags tab_bar_flags = (opt_fitting_flags) | (opt_reorderable ? ImGuiTabBarFlags_Reorderable : 0);
        tab_bar_flags |= ImGuiTabBarFlags_DrawSelectedOverline;
        if (ImGui::BeginTabBar("##tabs", tab_bar_flags))
        {
            if (opt_reorderable)
                app.NotifyOfDocumentsClosedElsewhere();

            // [DEBUG] Stress tests
            //if ((ImGui::GetFrameCount() % 30) == 0) docs[1].Open ^= 1;            // [DEBUG] Automatically show/hide a tab. Test various interactions e.g. dragging with this on.
            //if (ImGui::GetIO().KeyCtrl) ImGui::SetTabItemSelected(docs[1].Name);  // [DEBUG] Test SetTabItemSelected(), probably not very useful as-is anyway..

            // Submit Tabs
            for (MyDocument& doc : app.Documents)
            {
                if (!doc.Open)
                    continue;

                // As we allow to change document name, we append a never-changing document id so tabs are stable
                char doc_name_buf[64];
                app.GetTabName(&doc, doc_name_buf, sizeof(doc_name_buf));
                ImGuiTabItemFlags tab_flags = (doc.Dirty ? ImGuiTabItemFlags_UnsavedDocument : 0);
                bool visible = ImGui::BeginTabItem(doc_name_buf, &doc.Open, tab_flags);

                // Cancel attempt to close when unsaved add to save queue so we can display a popup.
                if (!doc.Open && doc.Dirty)
                {
                    doc.Open = true;
                    app.CloseQueue.push_back(&doc);
                }

                app.DisplayDocContextMenu(&doc);
                if (visible)
                {
                    app.DisplayDocContents(&doc);
                    ImGui::EndTabItem();
                }
            }

            ImGui::EndTabBar();
        }
    }
    else if (opt_target == Target_DockSpaceAndWindow)
    {
        if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_DockingEnable)
        {
            app.NotifyOfDocumentsClosedElsewhere();

            // Create a DockSpace node where any window can be docked
            ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
            ImGui::DockSpace(dockspace_id);

            // Create Windows
            for (int doc_n = 0; doc_n < app.Documents.Size; doc_n++)
            {
                MyDocument* doc = &app.Documents[doc_n];
                if (!doc->Open)
                    continue;

                ImGui::SetNextWindowDockID(dockspace_id, redock_all ? ImGuiCond_Always : ImGuiCond_FirstUseEver);
                ImGuiWindowFlags window_flags = (doc->Dirty ? ImGuiWindowFlags_UnsavedDocument : 0);
                bool visible = ImGui::Begin(doc->Name, &doc->Open, window_flags);

                // Cancel attempt to close when unsaved add to save queue so we can display a popup.
                if (!doc->Open && doc->Dirty)
                {
                    doc->Open = true;
                    app.CloseQueue.push_back(doc);
                }

                app.DisplayDocContextMenu(doc);
                if (visible)
                    app.DisplayDocContents(doc);

                ImGui::End();
            }
        }
        else
        {
            ShowDockingDisabledMessage();
        }
    }

    // Early out other contents
    if (!window_contents_visible)
    {
        ImGui::End();
        return;
    }

    // Display renaming UI
    if (app.RenamingDoc != NULL)
    {
        if (app.RenamingStarted)
            ImGui::OpenPopup("Rename");
        if (ImGui::BeginPopup("Rename"))
        {
            ImGui::SetNextItemWidth(ImGui::GetFontSize() * 30);
            if (ImGui::InputText("###Name", app.RenamingDoc->Name, IM_ARRAYSIZE(app.RenamingDoc->Name), ImGuiInputTextFlags_EnterReturnsTrue))
            {
                ImGui::CloseCurrentPopup();
                app.RenamingDoc = NULL;
            }
            if (app.RenamingStarted)
                ImGui::SetKeyboardFocusHere(-1);
            ImGui::EndPopup();
        }
        else
        {
            app.RenamingDoc = NULL;
        }
        app.RenamingStarted = false;
    }

    // Display closing confirmation UI
    if (!app.CloseQueue.empty())
    {
        int close_queue_unsaved_documents = 0;
        for (int n = 0; n < app.CloseQueue.Size; n++)
            if (app.CloseQueue[n]->Dirty)
                close_queue_unsaved_documents++;

        if (close_queue_unsaved_documents == 0)
        {
            // Close documents when all are unsaved
            for (int n = 0; n < app.CloseQueue.Size; n++)
                app.CloseQueue[n]->DoForceClose();
            app.CloseQueue.clear();
        }
        else
        {
            if (!ImGui::IsPopupOpen("Save?"))
                ImGui::OpenPopup("Save?");
            if (ImGui::BeginPopupModal("Save?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
            {
                ImGui::Text("Save change to the following items?");
                float item_height = ImGui::GetTextLineHeightWithSpacing();
                if (ImGui::BeginChild(ImGui::GetID("frame"), ImVec2(-FLT_MIN, 6.25f * item_height), ImGuiChildFlags_FrameStyle))
                    for (MyDocument* doc : app.CloseQueue)
                        if (doc->Dirty)
                            ImGui::Text("%s", doc->Name);
                ImGui::EndChild();

                ImVec2 button_size(ImGui::GetFontSize() * 7.0f, 0.0f);
                if (ImGui::Button("Yes", button_size))
                {
                    for (MyDocument* doc : app.CloseQueue)
                    {
                        if (doc->Dirty)
                            doc->DoSave();
                        doc->DoForceClose();
                    }
                    app.CloseQueue.clear();
                    ImGui::CloseCurrentPopup();
                }
                ImGui::SameLine();
                if (ImGui::Button("No", button_size))
                {
                    for (MyDocument* doc : app.CloseQueue)
                        doc->DoForceClose();
                    app.CloseQueue.clear();
                    ImGui::CloseCurrentPopup();
                }
                ImGui::SameLine();
                if (ImGui::Button("Cancel", button_size))
                {
                    app.CloseQueue.clear();
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }
        }
    }

    ImGui::End();
}


*/
















// *************************************************************************** //
// *************************************************************************** //
//
//  END.
