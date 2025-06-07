/***********************************************************************************
*
*       ********************************************************************
*       ****                S T A T E . h  ____  F I L E                ****
*       ********************************************************************
*              AUTHOR:      Collin A. Bond
*               DATED:      May 10, 2025.
*
**************************************************************************************
**************************************************************************************/
#include "app/app.h"
#include "app/state/_state.h"



namespace cb { namespace app { //     BEGINNING NAMESPACE "cb"...
// *************************************************************************** //
// *************************************************************************** //



//  1.1     STRUCT INITIALIZATION FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //
    
//  Default Constructor.
//
AppState::AppState(void)
    : m_logger{Logger::instance()}
{
    size_t      i       = size_t(0);

    // 1.   INITIALIZE WINDOW INFOS...
    for (i = 0; i < static_cast<size_t>(Window_t::Count); ++i) {
        m_windows.data[i] = APPLICATION_WINDOW_INFOS[i];
    }

    // 2.   INITIALIZE APPLICATION FONTS...
    for (i = 0; i < static_cast<size_t>(Font::Count); ++i) {
        m_fonts.data[i] = nullptr;  // load later in your init()
    }
}


//  Default Destructor.
//
AppState::~AppState(void) = default;



// *************************************************************************** //
//
//
//  1.2     STRUCT UTILITY FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //
    
//  "SetDarkMode"
//
void AppState::SetDarkMode(void) {
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
// *************************************************************************** //
// *************************************************************************** //

//  "DockAtHome"
//
void AppState::DockAtHome(const Window & idx) {
    app::WinInfo &w = this->m_windows[static_cast<Window>(idx)];
    if (w.open)     ImGui::DockBuilderDockWindow(w.uuid.c_str(), this->m_main_dock_id);
    return;
}
//
void AppState::DockAtHome(const char * uuid)        { ImGui::DockBuilderDockWindow( uuid, m_main_dock_id ); }


//  "DockAtDetView"
//
void AppState::DockAtDetView(const Window & idx) {
    app::WinInfo &  w = this->m_windows[static_cast<Window>(idx)];
    if (w.open)     ImGui::DockBuilderDockWindow(w.uuid.c_str(), this->m_detview_dockspace_id);
    return;
}
//
void AppState::DockAtDetView(const char * uuid)     { ImGui::DockBuilderDockWindow( uuid, m_detview_dockspace_id ); }
    
    
    
    
//  "PushFont"
//
void AppState::PushFont( [[maybe_unused]] const Font & which) {
    ImGui::PushFont( this->m_fonts[which] );
    return;
}


//  "PopFont"
//
void AppState::PopFont(void) {
    ImGui::PopFont();
    return;
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
