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

//  "ShowColorTool"
//
void App::ShowColorTool([[maybe_unused]] const char * uuid, [[maybe_unused]] bool * p_open, [[maybe_unused]] ImGuiWindowFlags flags)
{
    //  1.  CREATE THE WINDOW...
    if (!ImGui::Begin(uuid, p_open, flags)) {
        ImGui::End();
        return;
    }

    
    //  2.  COLOR-SHADE CALCULATOR TOOL...
    //ImGui::NewLine();
    ImGui::SetNextItemOpen(false, ImGuiCond_Once);
    if ( ImGui::CollapsingHeader("Color Tint/Shade Calculator") ) {
        this->ColorShaderTool();
    }

    
    //  3.  COLORMAP CREATOR TOOL...
    ImGui::SetNextItemOpen(true, ImGuiCond_Once);
    if ( ImGui::CollapsingHeader("Color-Map Creator") ) {
        this->ColorMapCreatorTool();
    }
    
    
    ImGui::End();
    return;
}


//  "ColorShaderTool"
//
void App::ColorShaderTool(void)
{
    constexpr size_t                BUFFER_SIZE             = 256;
    [[maybe_unused]] ImGuiIO &      io                      = ImGui::GetIO(); (void)io;
    [[maybe_unused]] ImGuiStyle &   style                   = ImGui::GetStyle();
    
    //  DIMENSIONS...
    static ImVec2                   base_img_size           = ImVec2(180 * S.m_dpi_scale,   180 * S.m_dpi_scale);
    static ImVec2                   palette_img_size        = ImVec2(30 * S.m_dpi_scale,    30 * S.m_dpi_scale);
    static float                    LABEL_COLUMN_WIDTH      = 150.0f * S.m_dpi_scale;
    static float                    WIDGET_COLUMN_WIDTH     = 300.0f * S.m_dpi_scale;

    static ImVec4                   base_color              = ImVec4(114.0f/255.0f, 144.0f/255.0f, 154.0f/255.0f, 1.0f);    //  Base color in RGB (normalized to [0,1]).
    static char                     hex_input[BUFFER_SIZE]  = "#728C9A";    //  Hex input for base color.
    static int                      steps                   = 3;            //  Steps and lightness delta for variants.
    static float                    delta_l                 = 0.1f;

    //  Table layout constants
    static bool                     freeze_header           = false;
    static bool                     freeze_column           = false;
    static bool                     stretch_column_1        = true;

    static ImGuiTableColumnFlags    col0_flags              = ImGuiTableColumnFlags_WidthFixed;
    static ImGuiTableColumnFlags    col1_flags              = stretch_column_1 ? ImGuiTableColumnFlags_WidthStretch : ImGuiTableColumnFlags_WidthFixed;
    static ImGuiTableFlags          table_flags             = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable | ImGuiTableFlags_NoKeepColumnsVisible; //ImGuiTableFlags_ScrollX;
    static ImGuiSliderFlags         SLIDER_FLAGS            = ImGuiSliderFlags_AlwaysClamp;

    //              COLOR-INPUT FLAGS...
    static ImGuiColorEditFlags      COLOR_INPUT_FLAGS       = ImGuiColorEditFlags_None | ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_NoSmallPreview; //ImGuiColorEditFlags_NoAlpha
    //              COLOR-IMAGE FLAGS...    ImGuiColorEditFlags_AlphaPreviewHalf
    static ImGuiColorEditFlags      BASE_IMG_FLAGS          = ImGuiColorEditFlags_PickerHueBar | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoInputs;    // ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoInputs; //ImGuiColorEditFlags_NoAlpha
    static ImGuiColorEditFlags      PALETTE_IMG_FLAGS       = ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_NoOptions; //ImGuiColorEditFlags_NoAlpha
    



    
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




        //  ImGui::Text("Color widget HSV with Alpha:");
        //  ImGui::ColorEdit4("MyColor##2", (float*)&color, ImGuiColorEditFlags_DisplayHSV | base_flags);

        //  IMGUI_DEMO_MARKER("Widgets/Color/ColorEdit (float display)");
        //  ImGui::Text("Color widget with Float Display:");
        //  ImGui::ColorEdit4("MyColor##2f", (float*)&color, ImGuiColorEditFlags_Float | base_flags);
        
        



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
        ImGui::SliderFloat("##delta_l", &delta_l, 0.01f, 0.5f, "%.2f%%");

        ImGui::EndTable();
    }


    ImGui::NewLine();
    ImGui::SeparatorText("Color Palette");

    //  GENERATE THE COLOR PALETTE...
    //  3.  Helper: color button with tooltip & copy-on-click
    auto color_button_with_tooltip = [&](const char * id, const ImVec4 & col)
    {
        ImGui::ColorButton(id, col, PALETTE_IMG_FLAGS, palette_img_size);
        if (ImGui::IsItemHovered())
        {
            char hex_buf[8];
            snprintf(hex_buf, sizeof(hex_buf), "#%02X%02X%02X",
                     int(col.x * 255), int(col.y * 255), int(col.z * 255));
            ImGui::SetTooltip("%s\n(click to copy)", hex_buf);
        }
        if (ImGui::IsItemClicked())
        {
            char hex_buf[8];
            snprintf(hex_buf, sizeof(hex_buf), "#%02X%02X%02X",
                     int(col.x * 255), int(col.y * 255), int(col.z * 255));
            ImGui::SetClipboardText(hex_buf);
        }
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
        ImGui::TextUnformatted("Comp Shades");
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
        ImGui::TextUnformatted("Comp Tints");
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
    constexpr float                 IDX_COL_WIDTH       = 20.0f;
    constexpr float                 COLOR_COL_WIDTH     = -1.0f;
    constexpr float                 ORDER_COL_WIDTH     = 80.0f;
    constexpr ImGuiTableFlags       TABLE_FLAGS         = ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable;
    //
    // SLIDER WIDGETS...
    constexpr ImGuiSliderFlags      POS_FLAGS           = ImGuiSliderFlags_AlwaysClamp;
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
    bool                    changed             = false;
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
        ImGui::SeparatorText("Preview");
        
        float avail_w = ImGui::GetContentRegionAvail().x;
        ImPlot::ColormapButton("##preview", ImVec2(avail_w, PREVIEW_WIDTH), cmap_custom);
        ImGui::Text("Index: %d", cmap_custom);
    }
    //
    //
    ImGui::NewLine();
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
        static float    t_custom        = 0.5f;
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
    ImGui::SeparatorText("Export Sampling");
    
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
    const ImVec2    SPACING     = ImVec2( 0.0f, 0.5*ImGui::GetTextLineHeightWithSpacing() );
    
    
    
    //  0.  CMAP WIDGET DEMO...
    CMAPDemoWidget();



    


    //  1.  COLORMAP "LOADING" SECTION...
    ImGui::Dummy(SPACING);
    ImGui::SeparatorText("Load Existing Colormap");
    bool apply = CMAPLoader();


    
    //  2.  COLORMAP PREVIEW...
    ImGui::Dummy(SPACING);
    ImGui::SeparatorText("Preview Colormap");
    CMAPPreview(apply);



    //  3.  TABLE OF COLORS...
    ImGui::Dummy(SPACING);
    ImGui::SeparatorText("Colormap Table");
    CMAPTable();



    //  4.  EXPORT COLORMAP...
    ImGui::Dummy(SPACING);
    ImGui::SeparatorText("Save and Export");
    CMAPExport();


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
