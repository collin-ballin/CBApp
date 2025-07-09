/***********************************************************************************
*
*       ********************************************************************
*       ****           A P P E A R A N C E . h  ____  F I L E           ****
*       ********************************************************************
*              AUTHOR:      Collin A. Bond
*               DATED:      June 25, 2025.
*
**************************************************************************************
**************************************************************************************/
#include "app/app.h"
#include "app/state/state.h"



namespace cb { namespace app { //     BEGINNING NAMESPACE "cb"...
// *************************************************************************** //
// *************************************************************************** //

//      PROTOTYPES...
// *************************************************************************** //
// *************************************************************************** //

//  CUSTOM APP COLOR STYLES...
static void             set_style_laser410nm            (void);
//
//  IMGUI APP COLOR STYLES...
static void             ImGui_StyleColorsLight          (ImGuiStyle * dst = NULL);
static void             ImGui_StyleColorsDark           (ImGuiStyle * dst = NULL);
static void             ImGui_StyleColorsClassic        (ImGuiStyle * dst = NULL);



// *************************************************************************** //
//
//
//      1.3A    APPEARANCE HELPER FUNCTIONS:
// *************************************************************************** //
// *************************************************************************** //
    
//  "SetAppColorStyle"
//
void AppState::SetAppColorStyle(const AppColorStyle_t style)
{
    this->m_current_app_color_style   = style;
    
    switch (style)
    {
    //
    //  BASIC CUSTOM STYLES:
        case AppColorStyle_t::LightMode :           {
            ImGui_StyleColorsLight();
            break;
        }
        case AppColorStyle_t::DarkMode :            {
            ImGui_StyleColorsDark();
            break;
        }
    //
    //  EXTRA CUSTOM STYLES:
        case AppColorStyle_t::Laser_410NM :         {
            set_style_laser410nm();
            break;
        }
    //
    //  IMGUI STYLES:
        case AppColorStyle_t::ImGuiLight :          {
            ImGui_StyleColorsLight();
            break;
        }
        case AppColorStyle_t::ImGuiDark :           {
            ImGui_StyleColorsDark();
            break;
        }
        case AppColorStyle_t::ImGuiClassic :        {
            ImGui_StyleColorsClassic();
            break;
        }
    //
        default :               {
            break;
        }
    }


    return;
}


//  "SetPlotColorStyle"
//
void AppState::SetPlotColorStyle(const PlotColorStyle_t style) {
    this->m_current_plot_color_style   = style;
    
    switch (style)
    {
    //
    //  BASIC CUSTOM STYLES:
        case PlotColorStyle_t::LightMode :           {
            break;
        }
        case PlotColorStyle_t::DarkMode :            {
            break;
        }
    //
    //  EXTRA CUSTOM STYLES:
        //  case PlotColorStyle_t::CUSTOM1 :            {
        //      break;
        //  }
    //
    //  IMGUI STYLES:
        //  case PlotColorStyle_t::IMGUI1 :            {
        //      break;
        //  }
    //
        default :               {
            break;
        }
    }


    return;
}
    
    




//  "LoadCustomColorMaps"
//
void AppState::LoadCustomColorMaps(void) {
    IM_ASSERT( static_cast<int>( Cmap::IMPLOT_END ) == ImPlot::GetColormapCount() && "The first item in the ENUM \"Colormat_t\" does NOT start at the FIRST COLORMAP INDEX.");

    for (auto & map : info::DEF_COLORMAPS) {
        ImPlot::AddColormap( map.first, map.second.data(), static_cast<int>(map.second.size()), /*qual=*/false );
    }

    for (auto & map : info::DEF_COLORMAPS_SHORT) {
        ImPlot::AddColormap( map.first, map.second.data(), static_cast<int>(map.second.size()), /*qual=*/false );
    }

    return;
}















// *************************************************************************** //
//
//
//
//      STATIC HELPER FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //
    
    
    
    
    
//  "SetDarkMode"
//
void AppState::SetDarkMode(void) {
    ImGuiStyle &style = ImGui::GetStyle();
    style.Alpha								= 1.0f;
    style.AntiAliasedFill					= true;
    style.AntiAliasedLines					= true;
    style.AntiAliasedLinesUseTex			= true;
    style.CellPadding						= ImVec2(4.0f, 2.0f);
    style.ChildBorderSize					= 1.0f;
    style.ChildRounding						= 0.0f;
    style.CircleTessellationMaxError		= 0.30000001192092896f;
    style.ColumnsMinSpacing					= 6.0f;
    style.CurveTessellationTol				= 1.25f;
    style.DisabledAlpha						= 0.6000000238418579f;
    style.DisplaySafeAreaPadding			= ImVec2(16.0f, 3.0f);
    style.DisplayWindowPadding				= ImVec2(19.0f, 19.0f);
    style.DockingSeparatorSize				= 2.0f;
    style.FrameBorderSize					= 0.0f;
    style.FramePadding						= ImVec2(8.0f, 4.0f);
    style.FrameRounding						= 2.0f;
    style.GrabMinSize						= 16.0f;
    style.GrabRounding						= 2.0f;
    style.IndentSpacing						= 15.0f;
    style.ItemInnerSpacing.x				= 8.0f;
    style.ItemInnerSpacing.y				= 4.0f;
    style.ItemSpacing.x						= 14.0f;
    style.ItemSpacing.y						= 4.0f;
    style.MouseCursorScale					= 1.0f;
    style.PopupBorderSize					= 1.0f;
    style.PopupRounding						= 0.0f;
    style.ScrollbarRounding					= 12.0f;
    style.ScrollbarSize						= 15.0f;
    style.TabBarBorderSize					= 2.0f;
    style.TabBarOverlineSize				= 2.0f;
    style.TabBorderSize						= 1.0f;
    style.TabCloseButtonMinWidthSelected	= -1.0f;
    style.TabCloseButtonMinWidthUnselected	= 0.0f;
    style.TabRounding						= 6.0f;
    style.TouchExtraPadding					= ImVec2(0.0f, 0.0f);
    style.TreeLinesSize						= 1.0f;
    style.WindowBorderHoverPadding			= 4.0f;
    style.WindowBorderSize					= 1.0f;
    style.WindowMinSize						= ImVec2(32.0f, 32.0f);
    style.WindowPadding						= ImVec2(8.0f, 8.0f);
    style.WindowRounding					= 0.0f;
    style.WindowTitleAlign					= ImVec2(0.0f, 0.5f);
    style.Colors[0]							= ImGui::ColorConvertU32ToFloat4(0xFFFFFFFFu);
    style.Colors[1]							= ImGui::ColorConvertU32ToFloat4(0xFF808080u);
    style.Colors[2]							= ImGui::ColorConvertU32ToFloat4(0xFF242220u);
    style.Colors[3]							= ImGui::ColorConvertU32ToFloat4(0x00000000u);
    style.Colors[4]							= ImGui::ColorConvertU32ToFloat4(0xF0141414u);
    style.Colors[5]							= ImGui::ColorConvertU32ToFloat4(0x80806E6Eu);
    style.Colors[6]							= ImGui::ColorConvertU32ToFloat4(0x00000000u);
    style.Colors[7]							= ImGui::ColorConvertU32ToFloat4(0xFF5B5958u);
    style.Colors[8]							= ImGui::ColorConvertU32ToFloat4(0xC8E09161u);
    style.Colors[9]							= ImGui::ColorConvertU32ToFloat4(0xFFD87739u);
    style.Colors[10]						= ImGui::ColorConvertU32ToFloat4(0xFF201F1Eu);
    style.Colors[11]						= ImGui::ColorConvertU32ToFloat4(0xFF3A3836u);
    style.Colors[12]						= ImGui::ColorConvertU32ToFloat4(0x82000000u);
    style.Colors[13]						= ImGui::ColorConvertU32ToFloat4(0xFF242424u);
    style.Colors[14]						= ImGui::ColorConvertU32ToFloat4(0x87050505u);
    style.Colors[15]						= ImGui::ColorConvertU32ToFloat4(0xFF9B9B9Au);
    style.Colors[16]						= ImGui::ColorConvertU32ToFloat4(0xFF6D6D6Cu);
    style.Colors[17]						= ImGui::ColorConvertU32ToFloat4(0xFF828282u);
    style.Colors[18]						= ImGui::ColorConvertU32ToFloat4(0xFFAD5F2Eu);
    style.Colors[19]						= ImGui::ColorConvertU32ToFloat4(0xFF6C3C1Du);
    style.Colors[20]						= ImGui::ColorConvertU32ToFloat4(0xFF563017u);
    style.Colors[21]						= ImGui::ColorConvertU32ToFloat4(0xC8AD5F2Eu);
    style.Colors[22]						= ImGui::ColorConvertU32ToFloat4(0xFFD87739u);
    style.Colors[23]						= ImGui::ColorConvertU32ToFloat4(0xFFFA870Fu);
    style.Colors[24]						= ImGui::ColorConvertU32ToFloat4(0x4FFA9642u);
    style.Colors[25]						= ImGui::ColorConvertU32ToFloat4(0xCCFA9642u);
    style.Colors[26]						= ImGui::ColorConvertU32ToFloat4(0xFFFA9642u);
    style.Colors[27]						= ImGui::ColorConvertU32ToFloat4(0x80806E6Eu);
    style.Colors[28]						= ImGui::ColorConvertU32ToFloat4(0xC7BF661Au);
    style.Colors[29]						= ImGui::ColorConvertU32ToFloat4(0xFFBF661Au);
    style.Colors[30]						= ImGui::ColorConvertU32ToFloat4(0x33FA9642u);
    style.Colors[31]						= ImGui::ColorConvertU32ToFloat4(0xABFA9642u);
    style.Colors[32]						= ImGui::ColorConvertU32ToFloat4(0xF2FA9642u);
    style.Colors[33]						= ImGui::ColorConvertU32ToFloat4(0xFFFFFFFFu);
    style.Colors[34]						= ImGui::ColorConvertU32ToFloat4(0xFF1F1F1Fu);
    style.Colors[35]						= ImGui::ColorConvertU32ToFloat4(0xFF4F4E4Du);
    style.Colors[36]						= ImGui::ColorConvertU32ToFloat4(0xFF7E7D7Bu);
    style.Colors[37]						= ImGui::ColorConvertU32ToFloat4(0xFF9D9C9Au);
    style.Colors[38]						= ImGui::ColorConvertU32ToFloat4(0xFF5E5E5Cu);
    style.Colors[39]						= ImGui::ColorConvertU32ToFloat4(0xFF3A3836u);
    style.Colors[40]						= ImGui::ColorConvertU32ToFloat4(0xFF808080u);
    style.Colors[41]						= ImGui::ColorConvertU32ToFloat4(0xB3FA9642u);
    style.Colors[42]						= ImGui::ColorConvertU32ToFloat4(0x00333333u);
    style.Colors[43]						= ImGui::ColorConvertU32ToFloat4(0xFF9C9C9Cu);
    style.Colors[44]						= ImGui::ColorConvertU32ToFloat4(0xFF596EFFu);
    style.Colors[45]						= ImGui::ColorConvertU32ToFloat4(0xFF00B3E6u);
    style.Colors[46]						= ImGui::ColorConvertU32ToFloat4(0xFF0099FFu);
    style.Colors[47]						= ImGui::ColorConvertU32ToFloat4(0xFF333030u);
    style.Colors[48]						= ImGui::ColorConvertU32ToFloat4(0xFF594F4Fu);
    style.Colors[49]						= ImGui::ColorConvertU32ToFloat4(0xFF403B3Bu);
    style.Colors[50]						= ImGui::ColorConvertU32ToFloat4(0x00000000u);
    style.Colors[51]						= ImGui::ColorConvertU32ToFloat4(0x0FFFFFFFu);
    style.Colors[52]						= ImGui::ColorConvertU32ToFloat4(0xFFFA9642u);
    style.Colors[53]						= ImGui::ColorConvertU32ToFloat4(0x59FA9642u);
    style.Colors[54]						= ImGui::ColorConvertU32ToFloat4(0x80806E6Eu);
    style.Colors[55]						= ImGui::ColorConvertU32ToFloat4(0xE600FFFFu);
    style.Colors[56]						= ImGui::ColorConvertU32ToFloat4(0xFFFA9642u);
    style.Colors[57]						= ImGui::ColorConvertU32ToFloat4(0xB3FFFFFFu);
    style.Colors[58]						= ImGui::ColorConvertU32ToFloat4(0x33CCCCCCu);
    style.Colors[59]						= ImGui::ColorConvertU32ToFloat4(0x59CCCCCCu);

    return;
}


//  "SetLightMode"
//
void AppState::SetLightMode(void) {
    ImGuiStyle &        style               = ImGui::GetStyle();
    style.Alpha								= 1.0f;
    style.AntiAliasedFill					= true;
    style.AntiAliasedLines					= true;
    style.AntiAliasedLinesUseTex			= true;
    style.CellPadding						= ImVec2(4.0f, 2.0f);
    style.ChildBorderSize					= 1.0f;
    style.ChildRounding						= 0.0f;
    style.CircleTessellationMaxError		= 0.30000001192092896f;
    style.ColumnsMinSpacing					= 6.0f;
    style.CurveTessellationTol				= 1.25f;
    style.DisabledAlpha						= 0.6000000238418579f;
    style.DisplaySafeAreaPadding			= ImVec2(16.0f, 3.0f);
    style.DisplayWindowPadding				= ImVec2(19.0f, 19.0f);
    style.DockingSeparatorSize				= 2.0f;
    style.FrameBorderSize					= 0.0f;
    style.FramePadding						= ImVec2(8.0f, 4.0f);
    style.FrameRounding						= 2.0f;
    style.GrabMinSize						= 16.0f;
    style.GrabRounding						= 2.0f;
    style.IndentSpacing						= 15.0f;
    style.ItemInnerSpacing.x				= 8.0f;
    style.ItemInnerSpacing.y				= 4.0f;
    style.ItemSpacing.x						= 14.0f;
    style.ItemSpacing.y						= 4.0f;
    style.MouseCursorScale					= 1.0f;
    style.PopupBorderSize					= 1.0f;
    style.PopupRounding						= 0.0f;
    style.ScrollbarRounding					= 12.0f;
    style.ScrollbarSize						= 15.0f;
    style.TabBarBorderSize					= 2.0f;
    style.TabBarOverlineSize				= 2.0f;
    style.TabBorderSize						= 1.0f;
    style.TabCloseButtonMinWidthSelected	= -1.0f;
    style.TabCloseButtonMinWidthUnselected	= 0.0f;
    style.TabRounding						= 6.0f;
    style.TouchExtraPadding					= ImVec2(0.0f, 0.0f);
    style.TreeLinesSize						= 1.0f;
    style.WindowBorderHoverPadding			= 4.0f;
    style.WindowBorderSize					= 1.0f;
    style.WindowMinSize						= ImVec2(32.0f, 32.0f);
    style.WindowPadding						= ImVec2(8.0f, 8.0f);
    style.WindowRounding					= 0.0f;
    style.WindowTitleAlign					= ImVec2(0.0f, 0.5f);
    style.Colors[0]							= ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    style.Colors[1]							= ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
    style.Colors[2]							= ImVec4(0.125490203499794f, 0.13333334028720856f, 0.1411764770746231f, 1.0f);
    style.Colors[3]							= ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
    style.Colors[4]							= ImVec4(0.07999999821186066f, 0.07999999821186066f, 0.07999999821186066f, 0.9399999976158142f);
    style.Colors[5]							= ImVec4(0.4300000071525574f, 0.4300000071525574f, 0.5f, 0.5f);
    style.Colors[6]							= ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
    style.Colors[7]							= ImVec4(0.3450980484485626f, 0.3490196168422699f, 0.35686275362968445f, 1.0f);
    style.Colors[8]							= ImVec4(0.3789311647415161f, 0.5669782757759094f, 0.8784313797950745f, 0.7843137383460999f);
    style.Colors[9]							= ImVec4(0.2235294133424759f, 0.46666666865348816f, 0.8470588326454163f, 1.0f);
    style.Colors[10]						= ImVec4(0.11764705926179886f, 0.12156862765550613f, 0.125490203499794f, 1.0f);
    style.Colors[11]						= ImVec4(0.21176470816135406f, 0.21960784494876862f, 0.22745098173618317f, 1.0f);
    style.Colors[12]						= ImVec4(0.0f, 0.0f, 0.0f, 0.5099999904632568f);
    style.Colors[13]						= ImVec4(0.14000000059604645f, 0.14000000059604645f, 0.14000000059604645f, 1.0f);
    style.Colors[14]						= ImVec4(0.019999999552965164f, 0.019999999552965164f, 0.019999999552965164f, 0.5299999713897705f);
    style.Colors[15]						= ImVec4(0.6039215922355652f, 0.6078431606292725f, 0.6078431606292725f, 1.0f);
    style.Colors[16]						= ImVec4(0.42352941632270813f, 0.4274509847164154f, 0.4274509847164154f, 1.0f);
    style.Colors[17]						= ImVec4(0.5099999904632568f, 0.5099999904632568f, 0.5099999904632568f, 1.0f);
    style.Colors[18]						= ImVec4(0.18039216101169586f, 0.37254902720451355f, 0.6784313917160034f, 1.0f);
    style.Colors[19]						= ImVec4(0.11372549086809158f, 0.23529411852359772f, 0.42352941632270813f, 1.0f);
    style.Colors[20]						= ImVec4(0.09019608050584793f, 0.1882352977991104f, 0.33725491166114807f, 1.0f);
    style.Colors[21]						= ImVec4(0.18039216101169586f, 0.37254902720451355f, 0.6784313917160034f, 0.7843137383460999f);
    style.Colors[22]						= ImVec4(0.2235294133424759f, 0.46666666865348816f, 0.8470588326454163f, 1.0f);
    style.Colors[23]						= ImVec4(0.05999999865889549f, 0.5299999713897705f, 0.9800000190734863f, 1.0f);
    style.Colors[24]						= ImVec4(0.25999999046325684f, 0.5899999737739563f, 0.9800000190734863f, 0.3100000023841858f);
    style.Colors[25]						= ImVec4(0.25999999046325684f, 0.5899999737739563f, 0.9800000190734863f, 0.800000011920929f);
    style.Colors[26]						= ImVec4(0.25999999046325684f, 0.5899999737739563f, 0.9800000190734863f, 1.0f);
    style.Colors[27]						= ImVec4(0.4300000071525574f, 0.4300000071525574f, 0.5f, 0.5f);
    style.Colors[28]						= ImVec4(0.10000000149011612f, 0.4000000059604645f, 0.75f, 0.7799999713897705f);
    style.Colors[29]						= ImVec4(0.10000000149011612f, 0.4000000059604645f, 0.75f, 1.0f);
    style.Colors[30]						= ImVec4(0.25999999046325684f, 0.5899999737739563f, 0.9800000190734863f, 0.20000000298023224f);
    style.Colors[31]						= ImVec4(0.25999999046325684f, 0.5899999737739563f, 0.9800000190734863f, 0.6700000166893005f);
    style.Colors[32]						= ImVec4(0.25999999046325684f, 0.5899999737739563f, 0.9800000190734863f, 0.949999988079071f);
    style.Colors[33]						= ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    style.Colors[34]						= ImVec4(0.12156862765550613f, 0.12156862765550613f, 0.12156862765550613f, 1.0f);
    style.Colors[35]						= ImVec4(0.3019607961177826f, 0.30588236451148987f, 0.30980393290519714f, 1.0f);
    style.Colors[36]						= ImVec4(0.48235294222831726f, 0.4901960790157318f, 0.4941176474094391f, 1.0f);
    style.Colors[37]						= ImVec4(0.6039215922355652f, 0.6117647290229797f, 0.615686297416687f, 1.0f);
    style.Colors[38]						= ImVec4(0.3607843220233917f, 0.3686274588108063f, 0.3686274588108063f, 1.0f);
    style.Colors[39]						= ImVec4(0.21176470816135406f, 0.21960784494876862f, 0.22745098173618317f, 1.0f);
    style.Colors[40]						= ImVec4(0.501960813999176f, 0.501960813999176f, 0.501960813999176f, 1.0f);
    style.Colors[41]						= ImVec4(0.25999999046325684f, 0.5899999737739563f, 0.9800000190734863f, 0.699999988079071f);
    style.Colors[42]						= ImVec4(0.20000000298023224f, 0.20000000298023224f, 0.20000000298023224f, 0.0f);
    style.Colors[43]						= ImVec4(0.6100000143051147f, 0.6100000143051147f, 0.6100000143051147f, 1.0f);
    style.Colors[44]						= ImVec4(1.0f, 0.4300000071525574f, 0.3499999940395355f, 1.0f);
    style.Colors[45]						= ImVec4(0.8999999761581421f, 0.699999988079071f, 0.0f, 1.0f);
    style.Colors[46]						= ImVec4(1.0f, 0.6000000238418579f, 0.0f, 1.0f);
    style.Colors[47]						= ImVec4(0.1899999976158142f, 0.1899999976158142f, 0.20000000298023224f, 1.0f);
    style.Colors[48]						= ImVec4(0.3100000023841858f, 0.3100000023841858f, 0.3499999940395355f, 1.0f);
    style.Colors[49]						= ImVec4(0.23000000417232513f, 0.23000000417232513f, 0.25f, 1.0f);
    style.Colors[50]						= ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
    style.Colors[51]						= ImVec4(1.0f, 1.0f, 1.0f, 0.05999999865889549f);
    style.Colors[52]						= ImVec4(0.25999999046325684f, 0.5899999737739563f, 0.9800000190734863f, 1.0f);
    style.Colors[53]						= ImVec4(0.25999999046325684f, 0.5899999737739563f, 0.9800000190734863f, 0.3499999940395355f);
    style.Colors[54]						= ImVec4(0.4300000071525574f, 0.4300000071525574f, 0.5f, 0.5f);
    style.Colors[55]						= ImVec4(1.0f, 1.0f, 0.0f, 0.8999999761581421f);
    style.Colors[56]						= ImVec4(0.25999999046325684f, 0.5899999737739563f, 0.9800000190734863f, 1.0f);
    style.Colors[57]						= ImVec4(1.0f, 1.0f, 1.0f, 0.699999988079071f);
    style.Colors[58]						= ImVec4(0.800000011920929f, 0.800000011920929f, 0.800000011920929f, 0.20000000298023224f);
    style.Colors[59]						= ImVec4(0.800000011920929f, 0.800000011920929f, 0.800000011920929f, 0.3499999940395355f);

    return;
}



//  "set_style_laser410nm"
//
static void set_style_laser410nm(void)
{
    ImGuiStyle &style = ImGui::GetStyle();
    style.Alpha								= 1.0f;
    style.AntiAliasedFill					= true;
    style.AntiAliasedLines					= true;
    style.AntiAliasedLinesUseTex			= true;
    style.CellPadding						= ImVec2(4.0f, 2.0f);
    style.ChildBorderSize					= 1.0f;
    style.ChildRounding						= 0.0f;
    style.CircleTessellationMaxError		= 0.30000001192092896f;
    style.ColumnsMinSpacing					= 6.0f;
    style.CurveTessellationTol				= 1.25f;
    style.DisabledAlpha						= 0.6000000238418579f;
    style.DisplaySafeAreaPadding			= ImVec2(16.0f, 3.0f);
    style.DisplayWindowPadding				= ImVec2(19.0f, 19.0f);
    style.DockingSeparatorSize				= 2.0f;
    style.FrameBorderSize					= 0.0f;
    style.FramePadding						= ImVec2(8.0f, 4.0f);
    style.FrameRounding						= 2.0f;
    style.GrabMinSize						= 16.0f;
    style.GrabRounding						= 2.0f;
    style.IndentSpacing						= 15.0f;
    style.ItemInnerSpacing.x				= 8.0f;
    style.ItemInnerSpacing.y				= 4.0f;
    style.ItemSpacing.x						= 14.0f;
    style.ItemSpacing.y						= 4.0f;
    style.MouseCursorScale					= 1.0f;
    style.PopupBorderSize					= 1.0f;
    style.PopupRounding						= 0.0f;
    style.ScrollbarRounding					= 12.0f;
    style.ScrollbarSize						= 15.0f;
    style.TabBarBorderSize					= 2.0f;
    style.TabBarOverlineSize				= 2.0f;
    style.TabBorderSize						= 1.0f;
    style.TabCloseButtonMinWidthSelected	= -1.0f;
    style.TabCloseButtonMinWidthUnselected	= 0.0f;
    style.TabRounding						= 6.0f;
    style.TouchExtraPadding					= ImVec2(0.0f, 0.0f);
    style.TreeLinesSize						= 1.0f;
    style.WindowBorderHoverPadding			= 4.0f;
    style.WindowBorderSize					= 1.0f;
    style.WindowMinSize						= ImVec2(32.0f, 32.0f);
    style.WindowPadding						= ImVec2(8.0f, 8.0f);
    style.WindowRounding					= 0.0f;
    style.WindowTitleAlign					= ImVec2(0.0f, 0.5f);
    style.Colors[0]							= ImGui::ColorConvertU32ToFloat4(0xFFFFFFFFu);
    style.Colors[1]							= ImGui::ColorConvertU32ToFloat4(0xFF808080u);
    style.Colors[2]							= ImGui::ColorConvertU32ToFloat4(0xFF242220u);
    style.Colors[3]							= ImGui::ColorConvertU32ToFloat4(0xC3373431u);
    style.Colors[4]							= ImGui::ColorConvertU32ToFloat4(0xF0141414u);
    style.Colors[5]							= ImGui::ColorConvertU32ToFloat4(0x80806E6Eu);
    style.Colors[6]							= ImGui::ColorConvertU32ToFloat4(0x00000000u);
    style.Colors[7]							= ImGui::ColorConvertU32ToFloat4(0xFF5B5958u);
    style.Colors[8]							= ImGui::ColorConvertU32ToFloat4(0xC8E0D261u);
    style.Colors[9]							= ImGui::ColorConvertU32ToFloat4(0xFFD8C739u);
    style.Colors[10]						= ImGui::ColorConvertU32ToFloat4(0xFF201F1Eu);
    style.Colors[11]						= ImGui::ColorConvertU32ToFloat4(0xFF3A3836u);
    style.Colors[12]						= ImGui::ColorConvertU32ToFloat4(0x82000000u);
    style.Colors[13]						= ImGui::ColorConvertU32ToFloat4(0xFF242424u);
    style.Colors[14]						= ImGui::ColorConvertU32ToFloat4(0x87050505u);
    style.Colors[15]						= ImGui::ColorConvertU32ToFloat4(0xFF9B9B9Au);
    style.Colors[16]						= ImGui::ColorConvertU32ToFloat4(0xFF6D6D6Cu);
    style.Colors[17]						= ImGui::ColorConvertU32ToFloat4(0xFF828282u);
    style.Colors[18]						= ImGui::ColorConvertU32ToFloat4(0xFFB0FFB3u);
    style.Colors[19]						= ImGui::ColorConvertU32ToFloat4(0xFF6C3C1Du);
    style.Colors[20]						= ImGui::ColorConvertU32ToFloat4(0xFF563017u);
    style.Colors[21]						= ImGui::ColorConvertU32ToFloat4(0xADADA72Eu);
    style.Colors[22]						= ImGui::ColorConvertU32ToFloat4(0xFFD8BD39u);
    style.Colors[23]						= ImGui::ColorConvertU32ToFloat4(0xFFFAD20Fu);
    style.Colors[24]						= ImGui::ColorConvertU32ToFloat4(0x4FFA9642u);
    style.Colors[25]						= ImGui::ColorConvertU32ToFloat4(0xCCFAE042u);
    style.Colors[26]						= ImGui::ColorConvertU32ToFloat4(0xFFFAE042u);
    style.Colors[27]						= ImGui::ColorConvertU32ToFloat4(0x80806E6Eu);
    style.Colors[28]						= ImGui::ColorConvertU32ToFloat4(0xC73CBF1Au);
    style.Colors[29]						= ImGui::ColorConvertU32ToFloat4(0xFF1ABF45u);
    style.Colors[30]						= ImGui::ColorConvertU32ToFloat4(0x33FA9642u);
    style.Colors[31]						= ImGui::ColorConvertU32ToFloat4(0xAB91FE95u);
    style.Colors[32]						= ImGui::ColorConvertU32ToFloat4(0xF28EFF8Cu);
    style.Colors[33]						= ImGui::ColorConvertU32ToFloat4(0xFFFFFFFFu);
    style.Colors[34]						= ImGui::ColorConvertU32ToFloat4(0xFF1F1F1Fu);
    style.Colors[35]						= ImGui::ColorConvertU32ToFloat4(0xFF4F4E4Du);
    style.Colors[36]						= ImGui::ColorConvertU32ToFloat4(0xFF7E7D7Bu);
    style.Colors[37]						= ImGui::ColorConvertU32ToFloat4(0xFF9D9C9Au);
    style.Colors[38]						= ImGui::ColorConvertU32ToFloat4(0xFF5E5E5Cu);
    style.Colors[39]						= ImGui::ColorConvertU32ToFloat4(0xFF3A3836u);
    style.Colors[40]						= ImGui::ColorConvertU32ToFloat4(0xFF808080u);
    style.Colors[41]						= ImGui::ColorConvertU32ToFloat4(0xB3FA9642u);
    style.Colors[42]						= ImGui::ColorConvertU32ToFloat4(0x00333333u);
    style.Colors[43]						= ImGui::ColorConvertU32ToFloat4(0xFF9C9C9Cu);
    style.Colors[44]						= ImGui::ColorConvertU32ToFloat4(0xFF596EFFu);
    style.Colors[45]						= ImGui::ColorConvertU32ToFloat4(0xFF00B3E6u);
    style.Colors[46]						= ImGui::ColorConvertU32ToFloat4(0xFF0099FFu);
    style.Colors[47]						= ImGui::ColorConvertU32ToFloat4(0xFF333030u);
    style.Colors[48]						= ImGui::ColorConvertU32ToFloat4(0xFF594F4Fu);
    style.Colors[49]						= ImGui::ColorConvertU32ToFloat4(0xFF403B3Bu);
    style.Colors[50]						= ImGui::ColorConvertU32ToFloat4(0x00000000u);
    style.Colors[51]						= ImGui::ColorConvertU32ToFloat4(0x0FFFFFFFu);
    style.Colors[52]						= ImGui::ColorConvertU32ToFloat4(0xFFFA9642u);
    style.Colors[53]						= ImGui::ColorConvertU32ToFloat4(0x59FA9642u);
    style.Colors[54]						= ImGui::ColorConvertU32ToFloat4(0x80806E6Eu);
    style.Colors[55]						= ImGui::ColorConvertU32ToFloat4(0xE600FFFFu);
    style.Colors[56]						= ImGui::ColorConvertU32ToFloat4(0xFFFA9642u);
    style.Colors[57]						= ImGui::ColorConvertU32ToFloat4(0xB3FFFFFFu);
    style.Colors[58]						= ImGui::ColorConvertU32ToFloat4(0x33CCCCCCu);
    style.Colors[59]						= ImGui::ColorConvertU32ToFloat4(0x59CCCCCCu);

    return;
}





// *************************************************************************** //
//
//
//      IMGUI APP COLOR STYLES...
// *************************************************************************** //
// *************************************************************************** //

//  "ImGui_StyleColorsDark"
//
static void ImGui_StyleColorsDark(ImGuiStyle* dst)
{
    ImGuiStyle* style = dst ? dst : &ImGui::GetStyle();
    ImVec4* colors = style->Colors;

    colors[ImGuiCol_Text]                   = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_TextDisabled]           = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    colors[ImGuiCol_WindowBg]               = ImVec4(0.06f, 0.06f, 0.06f, 0.94f);
    colors[ImGuiCol_ChildBg]                = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_PopupBg]                = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
    colors[ImGuiCol_Border]                 = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
    colors[ImGuiCol_BorderShadow]           = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_FrameBg]                = ImVec4(0.16f, 0.29f, 0.48f, 0.54f);
    colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
    colors[ImGuiCol_FrameBgActive]          = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
    colors[ImGuiCol_TitleBg]                = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
    colors[ImGuiCol_TitleBgActive]          = ImVec4(0.16f, 0.29f, 0.48f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
    colors[ImGuiCol_MenuBarBg]              = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
    colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
    colors[ImGuiCol_CheckMark]              = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_SliderGrab]             = ImVec4(0.24f, 0.52f, 0.88f, 1.00f);
    colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_Button]                 = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
    colors[ImGuiCol_ButtonHovered]          = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_ButtonActive]           = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
    colors[ImGuiCol_Header]                 = ImVec4(0.26f, 0.59f, 0.98f, 0.31f);
    colors[ImGuiCol_HeaderHovered]          = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
    colors[ImGuiCol_HeaderActive]           = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_Separator]              = colors[ImGuiCol_Border];
    colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.10f, 0.40f, 0.75f, 0.78f);
    colors[ImGuiCol_SeparatorActive]        = ImVec4(0.10f, 0.40f, 0.75f, 1.00f);
    colors[ImGuiCol_ResizeGrip]             = ImVec4(0.26f, 0.59f, 0.98f, 0.20f);
    colors[ImGuiCol_ResizeGripHovered]      = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
    colors[ImGuiCol_ResizeGripActive]       = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
    colors[ImGuiCol_InputTextCursor]        = colors[ImGuiCol_Text];
    colors[ImGuiCol_TabHovered]             = colors[ImGuiCol_HeaderHovered];
    colors[ImGuiCol_Tab]                    = ImLerp(colors[ImGuiCol_Header],       colors[ImGuiCol_TitleBgActive], 0.80f);
    colors[ImGuiCol_TabSelected]            = ImLerp(colors[ImGuiCol_HeaderActive], colors[ImGuiCol_TitleBgActive], 0.60f);
    colors[ImGuiCol_TabSelectedOverline]    = colors[ImGuiCol_HeaderActive];
    colors[ImGuiCol_TabDimmed]              = ImLerp(colors[ImGuiCol_Tab],          colors[ImGuiCol_TitleBg], 0.80f);
    colors[ImGuiCol_TabDimmedSelected]      = ImLerp(colors[ImGuiCol_TabSelected],  colors[ImGuiCol_TitleBg], 0.40f);
    colors[ImGuiCol_TabDimmedSelectedOverline] = ImVec4(0.50f, 0.50f, 0.50f, 0.00f);
    colors[ImGuiCol_DockingPreview]         = colors[ImGuiCol_HeaderActive] * ImVec4(1.0f, 1.0f, 1.0f, 0.7f);
    colors[ImGuiCol_DockingEmptyBg]         = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    colors[ImGuiCol_PlotLines]              = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered]       = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
    colors[ImGuiCol_PlotHistogram]          = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered]   = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    colors[ImGuiCol_TableHeaderBg]          = ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
    colors[ImGuiCol_TableBorderStrong]      = ImVec4(0.31f, 0.31f, 0.35f, 1.00f);   // Prefer using Alpha=1.0 here
    colors[ImGuiCol_TableBorderLight]       = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);   // Prefer using Alpha=1.0 here
    colors[ImGuiCol_TableRowBg]             = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_TableRowBgAlt]          = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
    colors[ImGuiCol_TextLink]               = colors[ImGuiCol_HeaderActive];
    colors[ImGuiCol_TextSelectedBg]         = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
    colors[ImGuiCol_TreeLines]              = colors[ImGuiCol_Border];
    colors[ImGuiCol_DragDropTarget]         = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
    colors[ImGuiCol_NavCursor]              = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_NavWindowingHighlight]  = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    colors[ImGuiCol_NavWindowingDimBg]      = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
    colors[ImGuiCol_ModalWindowDimBg]       = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
}




//  "ImGui_StyleColorsClassic"
//
static void ImGui_StyleColorsLight(ImGuiStyle* dst)
{
    ImGuiStyle* style = dst ? dst : &ImGui::GetStyle();
    ImVec4* colors = style->Colors;

    colors[ImGuiCol_Text]                   = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_TextDisabled]           = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
    colors[ImGuiCol_WindowBg]               = ImVec4(0.94f, 0.94f, 0.94f, 1.00f);
    colors[ImGuiCol_ChildBg]                = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_PopupBg]                = ImVec4(1.00f, 1.00f, 1.00f, 0.98f);
    colors[ImGuiCol_Border]                 = ImVec4(0.00f, 0.00f, 0.00f, 0.30f);
    colors[ImGuiCol_BorderShadow]           = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_FrameBg]                = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
    colors[ImGuiCol_FrameBgActive]          = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
    colors[ImGuiCol_TitleBg]                = ImVec4(0.96f, 0.96f, 0.96f, 1.00f);
    colors[ImGuiCol_TitleBgActive]          = ImVec4(0.82f, 0.82f, 0.82f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(1.00f, 1.00f, 1.00f, 0.51f);
    colors[ImGuiCol_MenuBarBg]              = ImVec4(0.86f, 0.86f, 0.86f, 1.00f);
    colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.98f, 0.98f, 0.98f, 0.53f);
    colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.69f, 0.69f, 0.69f, 0.80f);
    colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.49f, 0.49f, 0.49f, 0.80f);
    colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.49f, 0.49f, 0.49f, 1.00f);
    colors[ImGuiCol_CheckMark]              = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_SliderGrab]             = ImVec4(0.26f, 0.59f, 0.98f, 0.78f);
    colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.46f, 0.54f, 0.80f, 0.60f);
    colors[ImGuiCol_Button]                 = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
    colors[ImGuiCol_ButtonHovered]          = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_ButtonActive]           = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
    colors[ImGuiCol_Header]                 = ImVec4(0.26f, 0.59f, 0.98f, 0.31f);
    colors[ImGuiCol_HeaderHovered]          = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
    colors[ImGuiCol_HeaderActive]           = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_Separator]              = ImVec4(0.39f, 0.39f, 0.39f, 0.62f);
    colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.14f, 0.44f, 0.80f, 0.78f);
    colors[ImGuiCol_SeparatorActive]        = ImVec4(0.14f, 0.44f, 0.80f, 1.00f);
    colors[ImGuiCol_ResizeGrip]             = ImVec4(0.35f, 0.35f, 0.35f, 0.17f);
    colors[ImGuiCol_ResizeGripHovered]      = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
    colors[ImGuiCol_ResizeGripActive]       = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
    colors[ImGuiCol_InputTextCursor]        = colors[ImGuiCol_Text];
    colors[ImGuiCol_TabHovered]             = colors[ImGuiCol_HeaderHovered];
    colors[ImGuiCol_Tab]                    = ImLerp(colors[ImGuiCol_Header],       colors[ImGuiCol_TitleBgActive], 0.90f);
    colors[ImGuiCol_TabSelected]            = ImLerp(colors[ImGuiCol_HeaderActive], colors[ImGuiCol_TitleBgActive], 0.60f);
    colors[ImGuiCol_TabSelectedOverline]    = colors[ImGuiCol_HeaderActive];
    colors[ImGuiCol_TabDimmed]              = ImLerp(colors[ImGuiCol_Tab],          colors[ImGuiCol_TitleBg], 0.80f);
    colors[ImGuiCol_TabDimmedSelected]      = ImLerp(colors[ImGuiCol_TabSelected],  colors[ImGuiCol_TitleBg], 0.40f);
    colors[ImGuiCol_TabDimmedSelectedOverline] = ImVec4(0.26f, 0.59f, 1.00f, 0.00f);
    colors[ImGuiCol_DockingPreview]         = colors[ImGuiCol_Header] * ImVec4(1.0f, 1.0f, 1.0f, 0.7f);
    colors[ImGuiCol_DockingEmptyBg]         = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    colors[ImGuiCol_PlotLines]              = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered]       = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
    colors[ImGuiCol_PlotHistogram]          = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered]   = ImVec4(1.00f, 0.45f, 0.00f, 1.00f);
    colors[ImGuiCol_TableHeaderBg]          = ImVec4(0.78f, 0.87f, 0.98f, 1.00f);
    colors[ImGuiCol_TableBorderStrong]      = ImVec4(0.57f, 0.57f, 0.64f, 1.00f);   // Prefer using Alpha=1.0 here
    colors[ImGuiCol_TableBorderLight]       = ImVec4(0.68f, 0.68f, 0.74f, 1.00f);   // Prefer using Alpha=1.0 here
    colors[ImGuiCol_TableRowBg]             = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_TableRowBgAlt]          = ImVec4(0.30f, 0.30f, 0.30f, 0.09f);
    colors[ImGuiCol_TextLink]               = colors[ImGuiCol_HeaderActive];
    colors[ImGuiCol_TextSelectedBg]         = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
    colors[ImGuiCol_TreeLines]              = colors[ImGuiCol_Border];
    colors[ImGuiCol_DragDropTarget]         = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
    colors[ImGuiCol_NavCursor]              = colors[ImGuiCol_HeaderHovered];
    colors[ImGuiCol_NavWindowingHighlight]  = ImVec4(0.70f, 0.70f, 0.70f, 0.70f);
    colors[ImGuiCol_NavWindowingDimBg]      = ImVec4(0.20f, 0.20f, 0.20f, 0.20f);
    colors[ImGuiCol_ModalWindowDimBg]       = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);
}




//  "ImGui_StyleColorsClassic"
//      Those light colors are better suited with a thicker font than the default one + FrameBorder
//
static void ImGui_StyleColorsClassic(ImGuiStyle* dst)
{
    ImGuiStyle* style = dst ? dst : &ImGui::GetStyle();
    ImVec4* colors = style->Colors;

    colors[ImGuiCol_Text]                   = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
    colors[ImGuiCol_TextDisabled]           = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
    colors[ImGuiCol_WindowBg]               = ImVec4(0.00f, 0.00f, 0.00f, 0.85f);
    colors[ImGuiCol_ChildBg]                = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_PopupBg]                = ImVec4(0.11f, 0.11f, 0.14f, 0.92f);
    colors[ImGuiCol_Border]                 = ImVec4(0.50f, 0.50f, 0.50f, 0.50f);
    colors[ImGuiCol_BorderShadow]           = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_FrameBg]                = ImVec4(0.43f, 0.43f, 0.43f, 0.39f);
    colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.47f, 0.47f, 0.69f, 0.40f);
    colors[ImGuiCol_FrameBgActive]          = ImVec4(0.42f, 0.41f, 0.64f, 0.69f);
    colors[ImGuiCol_TitleBg]                = ImVec4(0.27f, 0.27f, 0.54f, 0.83f);
    colors[ImGuiCol_TitleBgActive]          = ImVec4(0.32f, 0.32f, 0.63f, 0.87f);
    colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.40f, 0.40f, 0.80f, 0.20f);
    colors[ImGuiCol_MenuBarBg]              = ImVec4(0.40f, 0.40f, 0.55f, 0.80f);
    colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.20f, 0.25f, 0.30f, 0.60f);
    colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.40f, 0.40f, 0.80f, 0.30f);
    colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.40f, 0.40f, 0.80f, 0.40f);
    colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.41f, 0.39f, 0.80f, 0.60f);
    colors[ImGuiCol_CheckMark]              = ImVec4(0.90f, 0.90f, 0.90f, 0.50f);
    colors[ImGuiCol_SliderGrab]             = ImVec4(1.00f, 1.00f, 1.00f, 0.30f);
    colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.41f, 0.39f, 0.80f, 0.60f);
    colors[ImGuiCol_Button]                 = ImVec4(0.35f, 0.40f, 0.61f, 0.62f);
    colors[ImGuiCol_ButtonHovered]          = ImVec4(0.40f, 0.48f, 0.71f, 0.79f);
    colors[ImGuiCol_ButtonActive]           = ImVec4(0.46f, 0.54f, 0.80f, 1.00f);
    colors[ImGuiCol_Header]                 = ImVec4(0.40f, 0.40f, 0.90f, 0.45f);
    colors[ImGuiCol_HeaderHovered]          = ImVec4(0.45f, 0.45f, 0.90f, 0.80f);
    colors[ImGuiCol_HeaderActive]           = ImVec4(0.53f, 0.53f, 0.87f, 0.80f);
    colors[ImGuiCol_Separator]              = ImVec4(0.50f, 0.50f, 0.50f, 0.60f);
    colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.60f, 0.60f, 0.70f, 1.00f);
    colors[ImGuiCol_SeparatorActive]        = ImVec4(0.70f, 0.70f, 0.90f, 1.00f);
    colors[ImGuiCol_ResizeGrip]             = ImVec4(1.00f, 1.00f, 1.00f, 0.10f);
    colors[ImGuiCol_ResizeGripHovered]      = ImVec4(0.78f, 0.82f, 1.00f, 0.60f);
    colors[ImGuiCol_ResizeGripActive]       = ImVec4(0.78f, 0.82f, 1.00f, 0.90f);
    colors[ImGuiCol_InputTextCursor]        = colors[ImGuiCol_Text];
    colors[ImGuiCol_TabHovered]             = colors[ImGuiCol_HeaderHovered];
    colors[ImGuiCol_Tab]                    = ImLerp(colors[ImGuiCol_Header],       colors[ImGuiCol_TitleBgActive], 0.80f);
    colors[ImGuiCol_TabSelected]            = ImLerp(colors[ImGuiCol_HeaderActive], colors[ImGuiCol_TitleBgActive], 0.60f);
    colors[ImGuiCol_TabSelectedOverline]    = colors[ImGuiCol_HeaderActive];
    colors[ImGuiCol_TabDimmed]              = ImLerp(colors[ImGuiCol_Tab],          colors[ImGuiCol_TitleBg], 0.80f);
    colors[ImGuiCol_TabDimmedSelected]      = ImLerp(colors[ImGuiCol_TabSelected],  colors[ImGuiCol_TitleBg], 0.40f);
    colors[ImGuiCol_TabDimmedSelectedOverline] = ImVec4(0.53f, 0.53f, 0.87f, 0.00f);
    colors[ImGuiCol_DockingPreview]         = colors[ImGuiCol_Header] * ImVec4(1.0f, 1.0f, 1.0f, 0.7f);
    colors[ImGuiCol_DockingEmptyBg]         = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    colors[ImGuiCol_PlotLines]              = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered]       = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogram]          = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered]   = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    colors[ImGuiCol_TableHeaderBg]          = ImVec4(0.27f, 0.27f, 0.38f, 1.00f);
    colors[ImGuiCol_TableBorderStrong]      = ImVec4(0.31f, 0.31f, 0.45f, 1.00f);   // Prefer using Alpha=1.0 here
    colors[ImGuiCol_TableBorderLight]       = ImVec4(0.26f, 0.26f, 0.28f, 1.00f);   // Prefer using Alpha=1.0 here
    colors[ImGuiCol_TableRowBg]             = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_TableRowBgAlt]          = ImVec4(1.00f, 1.00f, 1.00f, 0.07f);
    colors[ImGuiCol_TextLink]               = colors[ImGuiCol_HeaderActive];
    colors[ImGuiCol_TextSelectedBg]         = ImVec4(0.00f, 0.00f, 1.00f, 0.35f);
    colors[ImGuiCol_TreeLines]              = colors[ImGuiCol_Border];
    colors[ImGuiCol_DragDropTarget]         = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
    colors[ImGuiCol_NavCursor]              = colors[ImGuiCol_HeaderHovered];
    colors[ImGuiCol_NavWindowingHighlight]  = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    colors[ImGuiCol_NavWindowingDimBg]      = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
    colors[ImGuiCol_ModalWindowDimBg]       = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);
}







// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
} }//   END OF "cb" :: "app" NAMESPACE.






// *************************************************************************** //
// *************************************************************************** //
//
//  END.
