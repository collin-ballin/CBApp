/***********************************************************************************
*
*       *********************************************************************
*       ****    M A I N _ A P P L I C A T I O N . C P P  ____  F I L E   ****
*       *********************************************************************
*
*              AUTHOR:      Collin A. Bond
*               DATED:      May 03, 2025.
*
**************************************************************************************
**************************************************************************************/
#include "app/app.h"
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
    constexpr size_t                BUFFER_SIZE             = 256;
    [[maybe_unused]] ImGuiIO &      io                      = ImGui::GetIO(); (void)io;
    [[maybe_unused]] ImGuiStyle &   style                   = ImGui::GetStyle();
    
    //  CREATE THE WINDOW...
    if (!ImGui::Begin(uuid, p_open, flags)) {
        ImGui::End();
        return;
    }
    
    
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
    
    ImGui::End();
    
    return;
}






//  "ShowAboutWindow"
//
void App::ShowAboutWindow([[maybe_unused]]   const char *        uuid,
                          [[maybe_unused]]   bool *              p_open,
                          [[maybe_unused]]   ImGuiWindowFlags    flags)
{
    static bool         show_config_info        = false;


    if (!ImGui::Begin(uuid, p_open, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::End();
        return;
    }
    
    
    ImGui::Text("Dear ImGui %s (%d)", IMGUI_VERSION, IMGUI_VERSION_NUM);

    ImGui::TextLinkOpenURL("Homepage", "https://github.com/ocornut/imgui");
    ImGui::SameLine();
    ImGui::TextLinkOpenURL("FAQ", "https://github.com/ocornut/imgui/blob/master/docs/FAQ.md");
    ImGui::SameLine();
    ImGui::TextLinkOpenURL("Wiki", "https://github.com/ocornut/imgui/wiki");
    ImGui::SameLine();
    ImGui::TextLinkOpenURL("Releases", "https://github.com/ocornut/imgui/releases");
    ImGui::SameLine();
    ImGui::TextLinkOpenURL("Funding", "https://github.com/ocornut/imgui/wiki/Funding");

    ImGui::Separator();
    ImGui::Text("(c) 2014-2025 Omar Cornut");
    ImGui::Text("Developed by Omar Cornut and all Dear ImGui contributors.");
    ImGui::Text("Dear ImGui is licensed under the MIT License, see LICENSE for more information.");
    ImGui::Text("If your company uses this, please consider funding the project.");





    ImGui::Checkbox("Config/Build Information", &show_config_info);
    if (show_config_info)
    {
        ImGuiIO& io = ImGui::GetIO();
        ImGuiStyle& style = ImGui::GetStyle();

        bool copy_to_clipboard = ImGui::Button("Copy to clipboard");
        ImVec2 child_size = ImVec2(0, ImGui::GetTextLineHeightWithSpacing() * 18);
        ImGui::BeginChild(ImGui::GetID("cfg_infos"), child_size, ImGuiChildFlags_FrameStyle);
        if (copy_to_clipboard)
        {
            ImGui::LogToClipboard();
            ImGui::LogText("```\n"); // Back quotes will make text appears without formatting when pasting on GitHub
        }

        ImGui::Text("Dear ImGui %s (%d)", IMGUI_VERSION, IMGUI_VERSION_NUM);
        ImGui::Separator();
        ImGui::Text("sizeof(size_t): %d, sizeof(ImDrawIdx): %d, sizeof(ImDrawVert): %d", (int)sizeof(size_t), (int)sizeof(ImDrawIdx), (int)sizeof(ImDrawVert));
        ImGui::Text("define: __cplusplus=%d", (int)__cplusplus);
#ifdef IMGUI_DISABLE_OBSOLETE_FUNCTIONS
        ImGui::Text("define: IMGUI_DISABLE_OBSOLETE_FUNCTIONS");
#endif
#ifdef IMGUI_DISABLE_WIN32_DEFAULT_CLIPBOARD_FUNCTIONS
        ImGui::Text("define: IMGUI_DISABLE_WIN32_DEFAULT_CLIPBOARD_FUNCTIONS");
#endif
#ifdef IMGUI_DISABLE_WIN32_DEFAULT_IME_FUNCTIONS
        ImGui::Text("define: IMGUI_DISABLE_WIN32_DEFAULT_IME_FUNCTIONS");
#endif
#ifdef IMGUI_DISABLE_WIN32_FUNCTIONS
        ImGui::Text("define: IMGUI_DISABLE_WIN32_FUNCTIONS");
#endif
#ifdef IMGUI_DISABLE_DEFAULT_SHELL_FUNCTIONS
        ImGui::Text("define: IMGUI_DISABLE_DEFAULT_SHELL_FUNCTIONS");
#endif
#ifdef IMGUI_DISABLE_DEFAULT_FORMAT_FUNCTIONS
        ImGui::Text("define: IMGUI_DISABLE_DEFAULT_FORMAT_FUNCTIONS");
#endif
#ifdef IMGUI_DISABLE_DEFAULT_MATH_FUNCTIONS
        ImGui::Text("define: IMGUI_DISABLE_DEFAULT_MATH_FUNCTIONS");
#endif
#ifdef IMGUI_DISABLE_DEFAULT_FILE_FUNCTIONS
        ImGui::Text("define: IMGUI_DISABLE_DEFAULT_FILE_FUNCTIONS");
#endif
#ifdef IMGUI_DISABLE_FILE_FUNCTIONS
        ImGui::Text("define: IMGUI_DISABLE_FILE_FUNCTIONS");
#endif
#ifdef IMGUI_DISABLE_DEFAULT_ALLOCATORS
        ImGui::Text("define: IMGUI_DISABLE_DEFAULT_ALLOCATORS");
#endif
#ifdef IMGUI_USE_BGRA_PACKED_COLOR
        ImGui::Text("define: IMGUI_USE_BGRA_PACKED_COLOR");
#endif
#ifdef _WIN32
        ImGui::Text("define: _WIN32");
#endif
#ifdef _WIN64
        ImGui::Text("define: _WIN64");
#endif
#ifdef __linux__
        ImGui::Text("define: __linux__");
#endif
#ifdef __APPLE__
        ImGui::Text("define: __APPLE__");
#endif
#ifdef _MSC_VER
        ImGui::Text("define: _MSC_VER=%d", _MSC_VER);
#endif
#ifdef _MSVC_LANG
        ImGui::Text("define: _MSVC_LANG=%d", (int)_MSVC_LANG);
#endif
#ifdef __MINGW32__
        ImGui::Text("define: __MINGW32__");
#endif
#ifdef __MINGW64__
        ImGui::Text("define: __MINGW64__");
#endif
#ifdef __GNUC__
        ImGui::Text("define: __GNUC__=%d", (int)__GNUC__);
#endif
#ifdef __clang_version__
        ImGui::Text("define: __clang_version__=%s", __clang_version__);
#endif
#ifdef __EMSCRIPTEN__
        ImGui::Text("define: __EMSCRIPTEN__");
        ImGui::Text("Emscripten: %d.%d.%d", __EMSCRIPTEN_major__, __EMSCRIPTEN_minor__, __EMSCRIPTEN_tiny__);
#endif
#ifdef IMGUI_HAS_VIEWPORT
        ImGui::Text("define: IMGUI_HAS_VIEWPORT");
#endif
#ifdef IMGUI_HAS_DOCK
        ImGui::Text("define: IMGUI_HAS_DOCK");
#endif
        ImGui::Separator();
        ImGui::Text("io.BackendPlatformName: %s", io.BackendPlatformName ? io.BackendPlatformName : "NULL");
        ImGui::Text("io.BackendRendererName: %s", io.BackendRendererName ? io.BackendRendererName : "NULL");
        ImGui::Text("io.ConfigFlags: 0x%08X", io.ConfigFlags);
        if (io.ConfigFlags & ImGuiConfigFlags_NavEnableKeyboard)        ImGui::Text(" NavEnableKeyboard");
        if (io.ConfigFlags & ImGuiConfigFlags_NavEnableGamepad)         ImGui::Text(" NavEnableGamepad");
        if (io.ConfigFlags & ImGuiConfigFlags_NoMouse)                  ImGui::Text(" NoMouse");
        if (io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange)      ImGui::Text(" NoMouseCursorChange");
        if (io.ConfigFlags & ImGuiConfigFlags_NoKeyboard)               ImGui::Text(" NoKeyboard");
        if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)            ImGui::Text(" DockingEnable");
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)          ImGui::Text(" ViewportsEnable");
        if (io.ConfigFlags & ImGuiConfigFlags_DpiEnableScaleViewports)  ImGui::Text(" DpiEnableScaleViewports");
        if (io.ConfigFlags & ImGuiConfigFlags_DpiEnableScaleFonts)      ImGui::Text(" DpiEnableScaleFonts");
        if (io.MouseDrawCursor)                                         ImGui::Text("io.MouseDrawCursor");
        if (io.ConfigViewportsNoAutoMerge)                              ImGui::Text("io.ConfigViewportsNoAutoMerge");
        if (io.ConfigViewportsNoTaskBarIcon)                            ImGui::Text("io.ConfigViewportsNoTaskBarIcon");
        if (io.ConfigViewportsNoDecoration)                             ImGui::Text("io.ConfigViewportsNoDecoration");
        if (io.ConfigViewportsNoDefaultParent)                          ImGui::Text("io.ConfigViewportsNoDefaultParent");
        if (io.ConfigDockingNoSplit)                                    ImGui::Text("io.ConfigDockingNoSplit");
        if (io.ConfigDockingWithShift)                                  ImGui::Text("io.ConfigDockingWithShift");
        if (io.ConfigDockingAlwaysTabBar)                               ImGui::Text("io.ConfigDockingAlwaysTabBar");
        if (io.ConfigDockingTransparentPayload)                         ImGui::Text("io.ConfigDockingTransparentPayload");
        if (io.ConfigMacOSXBehaviors)                                   ImGui::Text("io.ConfigMacOSXBehaviors");
        if (io.ConfigNavMoveSetMousePos)                                ImGui::Text("io.ConfigNavMoveSetMousePos");
        if (io.ConfigNavCaptureKeyboard)                                ImGui::Text("io.ConfigNavCaptureKeyboard");
        if (io.ConfigInputTextCursorBlink)                              ImGui::Text("io.ConfigInputTextCursorBlink");
        if (io.ConfigWindowsResizeFromEdges)                            ImGui::Text("io.ConfigWindowsResizeFromEdges");
        if (io.ConfigWindowsMoveFromTitleBarOnly)                       ImGui::Text("io.ConfigWindowsMoveFromTitleBarOnly");
        if (io.ConfigMemoryCompactTimer >= 0.0f)                        ImGui::Text("io.ConfigMemoryCompactTimer = %.1f", io.ConfigMemoryCompactTimer);
        ImGui::Text("io.BackendFlags: 0x%08X", io.BackendFlags);
        if (io.BackendFlags & ImGuiBackendFlags_HasGamepad)             ImGui::Text(" HasGamepad");
        if (io.BackendFlags & ImGuiBackendFlags_HasMouseCursors)        ImGui::Text(" HasMouseCursors");
        if (io.BackendFlags & ImGuiBackendFlags_HasSetMousePos)         ImGui::Text(" HasSetMousePos");
        if (io.BackendFlags & ImGuiBackendFlags_PlatformHasViewports)   ImGui::Text(" PlatformHasViewports");
        if (io.BackendFlags & ImGuiBackendFlags_HasMouseHoveredViewport)ImGui::Text(" HasMouseHoveredViewport");
        if (io.BackendFlags & ImGuiBackendFlags_RendererHasVtxOffset)   ImGui::Text(" RendererHasVtxOffset");
        if (io.BackendFlags & ImGuiBackendFlags_RendererHasViewports)   ImGui::Text(" RendererHasViewports");
        ImGui::Separator();
        ImGui::Text("io.Fonts: %d fonts, Flags: 0x%08X, TexSize: %d,%d", io.Fonts->Fonts.Size, io.Fonts->Flags, io.Fonts->TexWidth, io.Fonts->TexHeight);
        ImGui::Text("io.DisplaySize: %.2f,%.2f", io.DisplaySize.x, io.DisplaySize.y);
        ImGui::Text("io.DisplayFramebufferScale: %.2f,%.2f", io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
        ImGui::Separator();
        ImGui::Text("style.WindowPadding: %.2f,%.2f", style.WindowPadding.x, style.WindowPadding.y);
        ImGui::Text("style.WindowBorderSize: %.2f", style.WindowBorderSize);
        ImGui::Text("style.FramePadding: %.2f,%.2f", style.FramePadding.x, style.FramePadding.y);
        ImGui::Text("style.FrameRounding: %.2f", style.FrameRounding);
        ImGui::Text("style.FrameBorderSize: %.2f", style.FrameBorderSize);
        ImGui::Text("style.ItemSpacing: %.2f,%.2f", style.ItemSpacing.x, style.ItemSpacing.y);
        ImGui::Text("style.ItemInnerSpacing: %.2f,%.2f", style.ItemInnerSpacing.x, style.ItemInnerSpacing.y);

        if (copy_to_clipboard)
        {
            ImGui::LogText("\n```\n");
            ImGui::LogFinish();
        }
        ImGui::EndChild();
    }
    
    
    ImGui::End();
    return;
}






// *************************************************************************** //
//
//
//  3.4     TESTS, DEMOS, DEBUGGING, TEMPORARY APPLICATIONS...
// *************************************************************************** //
// *************************************************************************** //

//  "stream_test"
//
void App::stream_test(void)
{
    ImGui::Begin("Python stream test");

    // ---------------------------------------------------------------
    // 1)  Persistent state
    // ---------------------------------------------------------------
    // static utl::PyStream proc(app::PYTHON_COUNTER_FILEPATH);
    static utl::PyStream            proc(app::PYTHON_DUMMY_FPGA_FILEPATH);
    static bool                     started   = false;
    static char                     line_buf[256]{};
    static std::vector<std::string> log;            // console lines
    static float                    delay_s   = 1.0f; // seconds between packets

    // ---------------------------------------------------------------
    // 2)  Runtime‑adjustable delay
    // ---------------------------------------------------------------
    if (ImGui::SliderFloat("Delay (s)", &delay_s, 0.05f, 5.0f, "%.2f"))
    {
        // Send immediately while dragging *if* running
        if (started)
        {
            char cmd[48];
            std::snprintf(cmd, sizeof(cmd), "duration %.3f", delay_s);
            proc.send(cmd);                          // e.g. "duration 0.25\n"
        }
    }
    ImGui::SameLine();
    if (ImGui::Button("Apply") && started)           // extra button for mouse‑up commit
    {
        char cmd[48];
        std::snprintf(cmd, sizeof(cmd), "duration %.3f", delay_s);
        proc.send(cmd);
    }

    // ---------------------------------------------------------------
    // 3)  Start / Stop controls
    // ---------------------------------------------------------------
    if (!started)
    {
        if (ImGui::Button("Start process"))
        {
            started = proc.start();
            if (!started) ImGui::OpenPopup("launch_error");
        }
    }
    else
    {
        if (ImGui::Button("Stop process"))
        {
            proc.stop();
            started = false;
        }
        ImGui::SameLine();
        ImGui::TextDisabled("(running)");
    }

    if (ImGui::BeginPopupModal("launch_error", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("Failed to launch Python process!");
        if (ImGui::Button("OK")) ImGui::CloseCurrentPopup();
        ImGui::EndPopup();
    }

    ImGui::Separator();

    // ---------------------------------------------------------------
    // 4)  Manual line send (for debugging)
    // ---------------------------------------------------------------
    ImGui::InputText("send", line_buf, IM_ARRAYSIZE(line_buf),
                     ImGuiInputTextFlags_EnterReturnsTrue);
    ImGui::SameLine();
    if (ImGui::Button("Send") || ImGui::IsItemDeactivatedAfterEdit())
    {
        if (started && line_buf[0])
        {
            proc.send(line_buf);          // raw passthrough
            line_buf[0] = '\0';
        }
    }

    // ---------------------------------------------------------------
    // 5)  Poll stdout → decode & log
    // ---------------------------------------------------------------
    std::string raw;
    while (proc.try_receive(raw))
    {
        log.push_back("RAW: " + raw);

        if (auto pkt = utl::parse_packet(raw))
        {
            const auto& p = *pkt;
            char buf[256];
            std::snprintf(buf, sizeof(buf),
                          "DECODED  A=%d  B=%d  C=%d  D=%d  ABCD=%d  cycles=%d",
                          p.a(), p.b(), p.c(), p.d(), p.abcd(), p.cycles);
            log.emplace_back(buf);
        }
        else
        {
            log.emplace_back("!! parse error");
        }
    }

    // ---------------------------------------------------------------
    // 6)  Display log
    // ---------------------------------------------------------------
    ImGui::BeginChild("log", ImVec2(0, 0), true, ImGuiWindowFlags_HorizontalScrollbar);
    for (auto const& s : log)
        ImGui::TextUnformatted(s.c_str());
    if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
        ImGui::SetScrollHereY(1.0f);       // auto‑scroll
    ImGui::EndChild();

    ImGui::End();
}






//  TESTING TAB BAR...
// *************************************************************************** //
// *************************************************************************** //

//  "DefaultTabRenderFunc"
//
static void DefaultTabRenderFunc([[maybe_unused]] const char * uuid, [[maybe_unused]] bool * p_open, [[maybe_unused]] ImGuiWindowFlags flags) {
    if (!p_open)
        return;
        
    ImGui::Text("Here is some text on a window with no render function.");
    return;
}


//  "TestTabBar"
//
void App::TestTabBar(void)
{
    //  DEFINE MISC. VARIABLES...
    static std::vector<Tab_t>       DEF_TABS                    = {
        Tab_t(  "Tab 1",    true,   true,   ImGuiTabItemFlags_None,     nullptr),
        Tab_t(  "Tab 2",    true,   true,   ImGuiTabItemFlags_None,     nullptr)
    };
    
    

    static size_t                   N                           = 0;
    static bool                     SHOW_HELP_TABS              = true;
    static bool                     ENABLE_ADDING_TABS          = true;
    static ImGuiTabItemFlags        TABBAR_FLAGS                = ImGuiTabBarFlags_None | ImGuiTabBarFlags_AutoSelectNewTabs | ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_FittingPolicyResizeDown | ImGuiTabBarFlags_NoCloseWithMiddleMouseButton | ImGuiTabBarFlags_TabListPopupButton;


    //  BEGIN THE TAB BAR...
    if ( ImGui::BeginTabBar("MY_TAB_BAR", TABBAR_FLAGS) )
    {
        N   = DEF_TABS.size();


        //      1.      DRAW HELP-MENU TAB-BUTTON ITEM ("?")...
        if (SHOW_HELP_TABS) {
            if (ImGui::TabItemButton("?", ImGuiTabItemFlags_Leading | ImGuiTabItemFlags_NoTooltip))
                ImGui::OpenPopup("MyHelpMenu");
        }
        if (ImGui::BeginPopup("MyHelpMenu")) {
            ImGui::Selectable("Hello!");
            ImGui::EndPopup();
        }


        //      2.      DRAW THE "ADD-TAB" BUTTON ("+")...
        //  if (ENABLE_ADDING_TABS) {
        //      if (ImGui::TabItemButton("+", ImGuiTabItemFlags_Trailing | ImGuiTabItemFlags_NoTooltip))
        //          { AddNewTabFunc(); /* Add new tab */ }
        //  }



        //      2.3     DRAW EACH OF THE TAB ITEMS...
        size_t i = 0;
        for (auto & tab : DEF_TABS)
        {
            if ( ImGui::BeginTabItem( tab.uuid, (tab.no_close) ? nullptr : &tab.open, tab.flags ) )
            {
                if (tab.render_fn) {
                    tab.render_fn( tab.uuid, &tab.open, tab.flags );
                }
                else {
                    DefaultTabRenderFunc(tab.uuid, &tab.open, tab.flags);
                    //ImGui::Text("This is tab #%zu.", i++);
                }
                
            ImGui::EndTabItem();
            }// END "BeginTabItem".
        
        } // END "for auto & tab".


    ImGui::EndTabBar();
        
    } // END "BeginTabBar".

    return;
}






// *************************************************************************** //
//
//
//  3.4     OTHER FUNCTIONS...      | PRIVATE.
// *************************************************************************** //
// *************************************************************************** //

//  "ImPlot_Testing1"
//
void App::ImPlot_Testing1(void)
{
    constexpr int                                           NUM_CHANNELS                = 15;
    static std::array<utl::RollingBuffer, NUM_CHANNELS>     buffers;
    static float                                            max_counts[NUM_CHANNELS]    = {};
    static float                                            t                           = 0.0f;
    // Channel specification: mask (1–15), human-readable name, min/max from sample data
    struct ChannelSpec { int mask; const char* name; int min; int max; };
    static constexpr ChannelSpec channels[NUM_CHANNELS] = {
        {8,  "A",    76220, 82824},
        {4,  "B",    45668, 52491},
        {2,  "C",      535,   641},
        {1,  "D",      365,   441},
        {12, "AB",     205,   295},
        {10, "AC",       0,     5},
        {9,  "AD",       0,     5},
        {6,  "BC",       0,     4},
        {5,  "BD",       0,     3},
        {3,  "CD",       0,     0},
        {14, "ABC",      0,    11},
        {13, "ABD",      0,     4},
        {11, "ACD",      0,     4},
        {7,  "BCD",      0,     4},
        {15, "ABCD",    60,    99}
    };

    //  PLOTTING CONTROL WIDGETS...
    static float    history             = 30.0f;
    static float    clock_interval      = 1.00f;    //  integration window
    static float    clock_accumulator   = 0.0f;
    static float    row_height          = 35.0f;    //  Row height for sparkline plots.
    static float    uncertainty_pct     = 5.0f;     // Uncertainty control (%)
    static bool     animate             = false;
    ImGui::SliderFloat("Row Height (px)",           &row_height,        20.0f,  90.0f,      "%.0f px");
    ImGui::SliderFloat("History (s)",               &history,           5.0f,  90.0f,      "%.1f s");
    ImGui::SliderFloat("Integration Window (s)",    &clock_interval,    0.01f,   5.0f,       "%.2f s");
    ImGui::SliderFloat("Uncertainty (%)",           &uncertainty_pct,   1.0f,   15.0f,      "%.1f%%");
    
    if (animate)    // Control animation start/stop and clearing via PLAY/PAUSE toggle
    {
        if (ImGui::Button("Pause")) animate = false;
    } else {
        if (ImGui::Button("Record")) animate = true;
    }
    ImGui::SameLine();
    if (ImGui::Button("Clear Data")) {
        t = 0.0f;
        clock_accumulator = 0.0f;
        for (auto &buf : buffers)
            buf.Data.clear();
        // Reset peak counts
        for (int i = 0; i < NUM_CHANNELS; ++i)
            max_counts[i] = 0.0f;
    }
    
    
    // Update each buffer's span immediately when history changes
    for (auto& buf : buffers)
        buf.Span = history;

    if (animate) {
        // Advance the internal clock accumulator
        clock_accumulator += ImGui::GetIO().DeltaTime;

        // When the integration window elapses, step time and generate new points
        if (clock_accumulator >= clock_interval) {
            clock_accumulator -= clock_interval;
            t += clock_interval;

            // Removed duplicate count_ranges declaration.

            // Per-channel “last value” to produce smooth trends
            static bool init = false;
            static float last_counts[NUM_CHANNELS];
            if (!init) {
                for (int i = 0; i < NUM_CHANNELS; ++i) {
                    auto [minv, maxv] = std::pair<int,int>{ channels[i].min, channels[i].max };
                    last_counts[i] = 0.5f * (minv + maxv);
                }
                init = true;
            }

            // RNG for Poisson noise
            static std::mt19937_64 rng{std::random_device{}()};

            for (int i = 0; i < NUM_CHANNELS; ++i)
            {
                //  Sample using Poisson to mimic counting noise
                auto [minv, maxv] = std::pair<int,int>{ channels[i].min, channels[i].max };
                
                //  Use last_counts[i] as the mean for Poisson
                std::poisson_distribution<int> pd(static_cast<int>(last_counts[i]));
                int sampled = pd(rng);
                float count = float(sampled);
                
                //  Clamp to observed min/max
                count = std::clamp(count, float(minv), float(maxv));
                
                //  Apply uncertainty noise (% of max)
                float noise_range = float(maxv) * (uncertainty_pct * 0.01f);
                std::uniform_real_distribution<float> ud(-noise_range, noise_range);
                count += ud(rng);
                
                //  Re-clamp to valid bounds
                count = std::clamp(count, 0.0f, float(maxv));
                last_counts[i] = count;
                
                //  Update peak seen so far
                max_counts[i] = std::max(max_counts[i], count);
                buffers[i].AddPoint(t, count);
            }
        }
    }


    // Colormap selector using ImPlot's built-in button
    // Only include colormaps supported by this ImPlot version
    static const ImPlotColormap colormaps[] = {
        ImPlotColormap_Plasma,   // a.k.a. matplotlib "plasma"      (qual=false, n=11)
        ImPlotColormap_Viridis,  // a.k.a. matplotlib "viridis"     (qual=false, n=11)
        ImPlotColormap_Hot,      // a.k.a. matplotlib/MATLAB "hot"  (qual=false, n=11)
        ImPlotColormap_Cool,     // a.k.a. matplotlib/MATLAB "cool" (qual=false, n=11)
        ImPlotColormap_RdBu,     // red/blue, Color Brewer          (qual=false, n=11)
        ImPlotColormap_BrBG,     // brown/blue-green, Color Brewer  (qual=false, n=11)
        ImPlotColormap_Twilight, // a.k.a. matplotlib "twilight"    (qual=false, n=11)
        ImPlotColormap_Deep,     // a.k.a. seaborn deep             (qual=true,  n=10) (default)
        ImPlotColormap_Dark,     // a.k.a. matplotlib "Set1"        (qual=true,  n=9 )
        ImPlotColormap_Pastel,   // a.k.a. matplotlib "Pastel1"     (qual=true,  n=9 )
        ImPlotColormap_Paired,   // a.k.a. matplotlib "Paired"      (qual=true,  n=12)
        ImPlotColormap_Pink,     // a.k.a. matplotlib/MATLAB "pink" (qual=false, n=11)
        ImPlotColormap_Jet,      // a.k.a. MATLAB "jet"             (qual=false, n=11)
        ImPlotColormap_PiYG,     // pink/yellow-green, Color Brewer (qual=false, n=11)
        ImPlotColormap_Spectral, // color spectrum, Color Brewer    (qual=false, n=11)
        ImPlotColormap_Greys    // white/black                     (qual=false, n=2 )
    };


    static const int colormap_count = sizeof(colormaps) / sizeof(colormaps[0]);
    static int colormap_idx = 0; // default to Plasma.
    ImPlotColormap map = colormaps[colormap_idx];
    // Display button to switch colormap
    if (ImPlot::ColormapButton(ImPlot::GetColormapName(map), ImVec2(100, 0), map)) {
        colormap_idx = (colormap_idx + 1) % colormap_count;
        map = colormaps[colormap_idx];
        // Bust color cache for each trend sparkline so they update colors
        for (int k = 0; k < NUM_CHANNELS; ++k) {
            ImPlot::BustColorCache((std::string("##trend") + std::to_string(k)).c_str());
        }
    }
    ImGui::SameLine();
    ImGui::LabelText("##ColormapIndex", "Change Colormap");
    // Apply selected colormap
    ImPlot::PushColormap(map);
    if (ImGui::BeginTable("##CoincTable", 5,    ImGuiTableFlags_BordersOuter | ImGuiTableFlags_RowBg)) {
        ImGui::TableSetupColumn("Counter(s)",   ImGuiTableColumnFlags_WidthFixed,  80.0f);
        ImGui::TableSetupColumn("Max",          ImGuiTableColumnFlags_WidthFixed,  80.0f);
        ImGui::TableSetupColumn("Avg.",         ImGuiTableColumnFlags_WidthFixed,  80.0f);
        ImGui::TableSetupColumn("Count",        ImGuiTableColumnFlags_WidthFixed,  80.0f);
        ImGui::TableSetupColumn("Plot");
        ImGui::TableHeadersRow();
        
        for (int display_row = 0; display_row < NUM_CHANNELS; ++display_row)
        {
            const auto &    spec            = channels[display_row];
            int             buffer_idx      = spec.mask - 1;  // zero-based index into buffers


            ImGui::TableNextRow();

            //  Column 0: label
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("%s", spec.name);

            //  Column 1: peak count
            ImGui::TableSetColumnIndex(1);
            ImGui::Text("%.0f", max_counts[display_row]);

            //  Column 2: average count
            ImGui::TableSetColumnIndex(2);
            int n = static_cast<int>(buffers[buffer_idx].Data.size());
            float sum = 0.0f;
            for (int j = 0; j < n; ++j)
                sum += buffers[buffer_idx].Data[j].y;
                
            float avg = n > 0 ? sum / n : 0.0f;
            
            ImGui::Text("%.1f", avg);


            //  Column 3: latest count
            ImGui::TableSetColumnIndex(3);
            float curr = buffers[buffer_idx].Data.empty() ? 0.0f : buffers[buffer_idx].Data.back().y;
            ImGui::Text("%.0f", curr);


            //  Column 4: sparkline trend
            ImGui::TableSetColumnIndex(4);
            if (n > 0)
            {
                std::vector<float> ydata(n);
                for (int j = 0; j < n; ++j)
                    ydata[j] = buffers[buffer_idx].Data[j].y;
                    
                float raw_max = *std::max_element(ydata.begin(), ydata.end());
                float y_max   = raw_max * 1.1f;  // 10% headroom
                
                utl::Sparkline(
                    ("##trend" + std::to_string(display_row)).c_str(),
                    ydata.data(), n,
                    0.0f, y_max,
                    0,
                    ImPlot::GetColormapColor(display_row),
                    ImVec2(-1, row_height)
                );
            }
        }
        ImGui::EndTable();
        
        
        //  Extra vertical spacing before time axis
        //      Shared time axis at bottom
        ImGui::Dummy(ImVec2(0.0f, row_height));
        {
            ImPlotAxisFlags xflags = ImPlotAxisFlags_NoGridLines;
            if (ImPlot::BeginPlot("##TimeAxis", ImVec2(-1, 20))) {
                ImPlot::SetupAxes("Time (s)", nullptr, ImPlotAxisFlags_None, ImPlotAxisFlags_NoDecorations);
                ImPlot::SetupAxisLimits(ImAxis_X1, 0.0f, t, ImGuiCond_Always);
                ImPlot::EndPlot();
            }
        }
        ImPlot::PopColormap();
    }
    
    return;
}






//  "ImPlot_Testing2"
//
void App::ImPlot_Testing2(void)
{
    constexpr const int     size            = 100;  //  # OF DATA-POINTS ON GRAPH...
    static int              N               = 5;    //  # OF PLOTS.
    static float            row_height      = 35;   //  HEIGHT OF EACH ROW (Px)...

    static ImGuiTableFlags  flags           = ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV |
                                              ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable;
    static bool             anim            = true;
    static int              offset          = 0;
    
    
    ImGui::BulletText("Plots can be used inside of ImGui tables as another means of creating subplots.");
    ImGui::SliderInt("Number of Plots (N)",         &N,             1,   30,      "%2i");
    ImGui::SliderFloat("Row Height (Px)",           &row_height,   20,   90,      "%2.1f");
    
    ImGui::Checkbox("Animate", &anim);
    if (anim)
        offset = (offset + 1) % 100;
        
        
        
    if (ImGui::BeginTable("##table", 3, flags, ImVec2(-1,0)))
    {
        static float            data[size];
        std::vector<ImVec4>     colors      = cb::utl::GetColormapSamples(N, ImPlotColormap_Cool);
        
        
        ImGui::TableSetupColumn("Electrode",        ImGuiTableColumnFlags_WidthFixed,   75.0f);
        ImGui::TableSetupColumn("Voltage",          ImGuiTableColumnFlags_WidthFixed,   75.0f);
        ImGui::TableSetupColumn("EMG Signal");
        ImGui::TableHeadersRow();
        
        //ImPlot::PushColormap(ImPlotColormap_Cool);
        for (int row = 0; row < N; ++row)
        {
            ImGui::TableNextRow();
            
            srand(row);
            for (int i = 0; i < size; ++i)
                data[i] = utl::RandomRange(0.0f,10.0f);
                
            
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("EMG %d", row);
            ImGui::TableSetColumnIndex(1);
            ImGui::Text("%.3f V", data[offset]);
            ImGui::TableSetColumnIndex(2);
            ImGui::PushID(row);
            //utl::Sparkline("##spark", data, size, 0, 11.0f, offset,ImPlot::GetColormapColor(row), ImVec2(-1, 35));
            utl::Sparkline( "##spark", data, size, 0, 11.0f, offset,
                             colors[row],
                             ImVec2(-1, row_height) ); //ImVec2(-1, 35));
            ImGui::PopID();
        }
        //ImPlot::PopColormap();
        ImGui::EndTable();
    }
    

    return;
}



//  "ImPlot_Testing3"
//
void App::ImPlot_Testing3(void)
{
    constexpr float                 RMIN                = 0.0f;
    constexpr float                 RMAX                = 1.0f;
    static int                      N                   = 1;    //  # OF PLOTS.
    ImVec2                          mouse               = ImGui::GetMousePos();
    static float                    time                = 0;
    static float                    window              = 10.0f;
    static float                    row_height          = 120.0f;
    static ImPlotAxisFlags          flags               = ImPlotAxisFlags_NoHighlight|ImPlotAxisFlags_NoMenus;//ImPlotAxisFlags_NoTickLabels;
    static utl::ScrollingBuffer     data;
    
    //data[row].AddPoint(time, utl::RandomRange(RMIN, RMAX));
    data.AddPoint(time, mouse.y * 0.0005);
    //data.Offset = (int)window;
            
            
    ImGui::BulletText("Move your mouse to change the data!");
    ImGui::BulletText("This example assumes 60 FPS. Higher FPS requires larger buffer size.");
    
    
    time += ImGui::GetIO().DeltaTime;
    
    ImGui::SliderInt("Number of Plots (N)",         &N,             1,   30,        "%2i");
    ImGui::SliderFloat("Row Height (Px)",           &row_height,   45,  250,        "%2.1f");
    ImGui::SliderFloat("Window (s)",                &window,        1,   30,        "%.1f s");








    if (ImGui::BeginTable("##table", 3, ImGuiTableFlags_BordersOuter | ImGuiTableFlags_RowBg, ImVec2(-1,0)))
    {
        std::vector<ImVec4>     colors      = cb::utl::GetColormapSamples(N, ImPlotColormap_Cool);
        
        
        ImGui::TableSetupColumn("Column 1",         ImGuiTableColumnFlags_WidthFixed,   75.0f);
        ImGui::TableSetupColumn("Column 2",         ImGuiTableColumnFlags_WidthFixed,   75.0f);
        ImGui::TableSetupColumn("Plot");
        ImGui::TableHeadersRow();
        
        //ImPlot::PushColormap(ImPlotColormap_Cool);
        
        
        for (int row = 0; row < N; ++row)
        {
            ImGui::TableNextRow();
            
            //srand(row);
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("EMG %f", window);
            ImGui::TableSetColumnIndex(1);
            ImGui::Text("%.3f V", window);
            ImGui::TableSetColumnIndex(2);
            ImGui::PushID(row);
                
            if (row == N-1 || N==1)
                utl::ScrollingSparkline( time, window, data, flags, colors[row], ImVec2(-1, row_height) );
            else
                utl::ScrollingSparkline( time, window, data, ImPlotAxisFlags_NoDecorations, colors[row], ImVec2(-1, row_height) );
                                         
            ImGui::PopID();
        }
        //ImPlot::PopColormap();
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
}//   END OF "cb" NAMESPACE.











// *************************************************************************** //
// *************************************************************************** //
//
//  END.
