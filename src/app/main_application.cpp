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
#include "implot.h"
#include "implot_internal.h"
#include <random>
#include <algorithm>



namespace cb { //     BEGINNING NAMESPACE "cb"...
// *************************************************************************** //
// *************************************************************************** //



// *************************************************************************** //
// *************************************************************************** //


//  "ShowAboutWindow"
//
void App::ShowAboutWindow([[maybe_unused]]   const char *        uuid,
                          [[maybe_unused]]   bool *              p_open,
                          [[maybe_unused]]   ImGuiWindowFlags    flags)
{
    static bool                     show_config_info    = false;
    static auto                     now                 = ImGui::GetTime();
    static ImGuiInputFlags          save_key_flags      = ImGuiInputFlags_None; //   | ~ImGuiInputFlags_Repeat; // Merged flags
    static const ImGuiKeyChord      SAVE_KEY            = ImGuiMod_Ctrl | ImGuiKey_S;
            

    ImGui::OpenPopup(uuid);
    
    
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    //ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2( 0.5f, 0.5f ));
    ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2( 0.5f, 0.5f ));
    if (ImGui::BeginPopupModal(uuid, p_open, ImGuiWindowFlags_AlwaysAutoResize))
    {

        
        
        ImGui::Text("CBApp Version %s (%s)", __CBAPP_VERSION__, __CBAPP_BUILD__);
        ImGui::Text("Developed by Collin Andrew Bond  (c)  2024-2025");



        ImGui::TextLinkOpenURL("Repository", "https://github.com/collin-ballin/CBApp");
        //  ImGui::SameLine();
        //  ImGui::TextLinkOpenURL("FAQ", "https://github.com/ocornut/imgui/blob/master/docs/FAQ.md");
        //  ImGui::SameLine();
        //  ImGui::TextLinkOpenURL("Wiki", "https://github.com/ocornut/imgui/wiki");
        //  ImGui::SameLine();
        //  ImGui::TextLinkOpenURL("Releases", "https://github.com/ocornut/imgui/releases");
        //  ImGui::SameLine();
        //  ImGui::TextLinkOpenURL("Funding", "https://github.com/ocornut/imgui/wiki/Funding");






        ImGui::Separator();
        
        //  1.  THIRD-PARTY REFERENCES...
        if ( ImGui::TreeNode("Third-Party Dependencies") ) {
        
            ImGui::Text("Dear ImGui %s (%d)", IMGUI_VERSION, IMGUI_VERSION_NUM);
            ImGui::Text("ImPlot %s", IMPLOT_VERSION);
        
        
            ImGui::TreePop();
        }
        
        
        
        ImGui::Separator();
        ImGui::Checkbox("Config/Build Information", &show_config_info);
        if (show_config_info)
        {
            this->get_build_info();
        }
        
 
   


    

        ImGui::EndPopup();
    }// END POP-UP.
    
    
    return;
}


//  "get_build_info"
//
void App::get_build_info(void) const
{
    constexpr unsigned int              NUM_LINES       = 18;
    [[maybe_unused]] ImGuiIO &          io              = ImGui::GetIO();
    [[maybe_unused]] ImGuiStyle &       style           = ImGui::GetStyle();





    ImGui::BulletText("Your current configuration is:");
    ImGui::Indent();
    ImGui::BulletText("ImDrawIdx: %d-bit", (int)(sizeof(ImDrawIdx) * 8));
    ImGui::BulletText("ImGuiBackendFlags_RendererHasVtxOffset: %s", (ImGui::GetIO().BackendFlags & ImGuiBackendFlags_RendererHasVtxOffset) ? "True" : "False");
    ImGui::Unindent();






    //  1.  CREATE A CHILD WINDOW TO HOST ALL THE INFORMATION...
    //
    bool        copy_to_clipboard   = ImGui::Button("Copy to clipboard");
    ImVec2      child_size          = ImVec2(0, ImGui::GetTextLineHeightWithSpacing() * NUM_LINES );
    
    ImGui::BeginChild(ImGui::GetID("cfg_info_1"), child_size, ImGuiChildFlags_FrameStyle);
    if (copy_to_clipboard) {
        ImGui::LogToClipboard();
        ImGui::LogText("```\n");    //  Back quotes will make text appears without formatting when pasting on GitHub
    }
    
        this->get_info1();
    

    //  END CHILD WINDOW.   COPY INFORMATION TO CLIPBOARD.
    if (copy_to_clipboard) {
        ImGui::LogText("\n```\n");
        ImGui::LogFinish();
    }
    ImGui::EndChild();
    
    
    
    //  WINDOW #2...
    //
    //ImGui::SameLine();
    ImGui::BeginChild(ImGui::GetID("cfg_info_2"), child_size, ImGuiChildFlags_FrameStyle);
    
        //this->get_info1();
    

    ImGui::EndChild();  //  END CHILD WINDOW.
    
    
    



    return;
}


//  "get_info2"
//
void App::get_info2(void) const
{
    return;
}




//  "get_info1"
//
void App::get_info1(void) const
{
    constexpr unsigned int              NUM_LINES       = 18;
    [[maybe_unused]] ImGuiIO &          io              = ImGui::GetIO();
    [[maybe_unused]] ImGuiStyle &       style           = ImGui::GetStyle();



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




    //  3.  IMGUI CONFIG STUFF...
    //
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






    return;
}






// *************************************************************************** //
//
//
//  3.4     TESTS, DEMOS, DEBUGGING, TEMPORARY APPLICATIONS...
// *************************************************************************** //
// *************************************************************************** //

//  "PyStreamTest"
//
void App::PyStreamTest(void)
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


//----------------------------------
// Helper: hosting mode selector
//----------------------------------
enum class TabHostMode {
    StaticTabBar,      // Option 2 – fixed tab‑bar
    DockableWindows    // Option 3 – dockable windows
};

//----------------------------------
// App::TestTabBar()
//----------------------------------
void App::TestTabBar()
{
    //----------------------------------------------------------
    // 1. Persistent tab list
    //----------------------------------------------------------
    static std::vector<Tab_t> tabs = {
        Tab_t("Tab 1", true,  true, ImGuiTabItemFlags_None, nullptr),
        Tab_t("Tab 2", true,  true, ImGuiTabItemFlags_None, nullptr)
    };

    static TabHostMode host_mode  = TabHostMode::StaticTabBar;
    static bool        redock_all = false; // one‑shot flag

    //----------------------------------------------------------
    // 2. Mode switcher + redock button
    //----------------------------------------------------------
    ImGui::SeparatorText("Tab Host Mode");
    ImGui::RadioButton("TabBar + Tabs",      reinterpret_cast<int*>(&host_mode), static_cast<int>(TabHostMode::StaticTabBar));
    ImGui::SameLine();
    ImGui::RadioButton("DockSpace + Window", reinterpret_cast<int*>(&host_mode), static_cast<int>(TabHostMode::DockableWindows));

    if (host_mode == TabHostMode::DockableWindows) {
        ImGui::SameLine();
        if (ImGui::Button("Redock all"))
            redock_all = true;
    }
    ImGui::Separator();

    //----------------------------------------------------------
    // 3. Rendering path A: single tab‑bar (static)
    //----------------------------------------------------------
    if (host_mode == TabHostMode::StaticTabBar)
    {
        constexpr ImGuiTabBarFlags TABBAR_FLAGS =
              ImGuiTabBarFlags_AutoSelectNewTabs
            | ImGuiTabBarFlags_Reorderable
            | ImGuiTabBarFlags_FittingPolicyResizeDown
            | ImGuiTabBarFlags_NoCloseWithMiddleMouseButton
            | ImGuiTabBarFlags_TabListPopupButton;

        if (ImGui::BeginTabBar("MY_TAB_BAR", TABBAR_FLAGS))
        {
            //----------------------- Leading help tab ("?")
            if (ImGui::TabItemButton("?", ImGuiTabItemFlags_Leading | ImGuiTabItemFlags_NoTooltip))
                ImGui::OpenPopup("HelpMenu");
            if (ImGui::BeginPopup("HelpMenu"))
            {
                ImGui::TextUnformatted("Hello from the help tab!\nAdd instructions here.");
                ImGui::EndPopup();
            }

            //----------------------- User‑defined tabs
            for (Tab_t& tab : tabs)
            {
                if (!ImGui::BeginTabItem(tab.get_uuid(),
                                          tab.no_close ? nullptr : &tab.open,
                                          tab.flags))
                    continue;

                if (tab.render_fn)
                    tab.render_fn(tab.get_uuid(), &tab.open, ImGuiWindowFlags_None);
                else
                    DefaultTabRenderFunc(tab.get_uuid(), &tab.open, ImGuiWindowFlags_None);

                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }
        return; // early exit; nothing else to do in this mode
    }

    //----------------------------------------------------------
    // 4. Rendering path B: dockable windows
    //----------------------------------------------------------
    const ImGuiID dockspace_id = ImGui::GetID("APP_DOCKSPACE");
    ImGui::DockSpace(dockspace_id, ImVec2(0, 0), ImGuiDockNodeFlags_None);

    // Optional: class to tweak the appearance of windows once docked
    static ImGuiWindowClass docked_window_class;
    docked_window_class.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_HiddenTabBar; // hide internal tab bar when docked

    for (Tab_t& tab : tabs)
    {
        if (!tab.open)
            continue;

        // 4‑A. Pre‑dock / redock into the root
        ImGuiCond cond = redock_all ? ImGuiCond_Always : ImGuiCond_FirstUseEver;
        ImGui::SetNextWindowDockID(dockspace_id, cond);
        redock_all = false; // reset after first use

        // 4‑B. Apply window class (e.g., hide tab‑bar when docked)
        ImGui::SetNextWindowClass(&docked_window_class);

        // 4‑C. Translate TabItem flags to Window flags (unsaved marker)
        ImGuiWindowFlags win_flags = 0;
        if (tab.flags & ImGuiTabItemFlags_UnsavedDocument)
            win_flags |= ImGuiWindowFlags_UnsavedDocument;

        // 4‑D. Begin window (no close button if tab.no_close)
        if (ImGui::Begin(tab.get_uuid(),
                         tab.no_close ? nullptr : &tab.open,
                         win_flags))
        {
            if (tab.render_fn)
                tab.render_fn(tab.get_uuid(), &tab.open, win_flags);
            else
                DefaultTabRenderFunc(tab.get_uuid(), &tab.open, win_flags);
        }
        ImGui::End();
    }
}






// *************************************************************************** //
//
//
//  3.4     OTHER FUNCTIONS...      | PRIVATE.
// *************************************************************************** //
// *************************************************************************** //

ImPlotPoint SinewaveGetter(int i, void* data) {
    float f = *(float*)data;
    return ImPlotPoint(i,sinf(f*i));
}

ImVec4 GetLastItemColor() {
    ImPlotContext& gp = *GImPlot;
    if (gp.PreviousItem)
        return ImGui::ColorConvertU32ToFloat4(gp.PreviousItem->Color);
    return ImVec4();
}


void Demo_LegendPopups() {
    ImGui::BulletText("You can implement legend context menus to inject per-item controls and widgets.");
    ImGui::BulletText("Right click the legend label/icon to edit custom item attributes.");

    static float  frequency = 0.1f;
    static float  amplitude = 0.5f;
    static ImVec4 color     = ImVec4(1,1,0,1);
    static float  alpha     = 1.0f;
    static bool   line      = false;
    static float  thickness = 1;
    static bool   markers   = false;
    static bool   shaded    = false;

    static float vals[101];
    for (int i = 0; i < 101; ++i)
        vals[i] = amplitude * sinf(frequency * i);

    if (ImPlot::BeginPlot("Right Click the Legend")) {
        ImPlot::SetupAxesLimits(0,100,-1,1);
        // rendering logic
        ImPlot::PushStyleVar(ImPlotStyleVar_FillAlpha, alpha);
        if (!line) {
            ImPlot::SetNextFillStyle(color);
            ImPlot::PlotBars("Right Click Me", vals, 101);
        }
        else {
            if (markers) ImPlot::SetNextMarkerStyle(ImPlotMarker_Square);
            ImPlot::SetNextLineStyle(color, thickness);
            ImPlot::PlotLine("Right Click Me", vals, 101);
            if (shaded) ImPlot::PlotShaded("Right Click Me",vals,101);
        }
        ImPlot::PopStyleVar();
        // custom legend context menu
        if (ImPlot::BeginLegendPopup("Right Click Me")) {
            ImGui::SliderFloat("Frequency",&frequency,0,1,"%0.2f");
            ImGui::SliderFloat("Amplitude",&amplitude,0,1,"%0.2f");
            ImGui::Separator();
            ImGui::ColorEdit3("Color",&color.x);
            ImGui::SliderFloat("Transparency",&alpha,0,1,"%.2f");
            ImGui::Checkbox("Line Plot", &line);
            if (line) {
                ImGui::SliderFloat("Thickness", &thickness, 0, 5);
                ImGui::Checkbox("Markers", &markers);
                ImGui::Checkbox("Shaded",&shaded);
            }
            ImPlot::EndLegendPopup();
        }
        ImPlot::EndPlot();
    }
}












/*
// Iterates through all registered ImPlots and displays their IDs
// Returns true if at least one plot was found.
bool GetPlotID(const char * name) {
    // Access the global ImPlot context
    ImPlotContext & ctx = *ImPlot::GetCurrentContext();

    // Determine number of registered plots
    int plotCount = ctx.Plots.GetBufSize();

    // Iterate and display each Plot's ID
    for (int i = 0; i < plotCount; ++i) {
        ImPlotPlot * plot = ctx.Plots.GetByIndex(i);
        
        
        if (plot) {
            //name
            // Display Plot ID in the UI
            ImGui::Text("Plot ID: %u", static_cast<unsigned>(plot->ID));
        }
    }

    // Return true if any plots were processed
    return (plotCount > 0);
}
*/

























//  "ImPlot_Testing0ALT"
//
void App::ImPlot_Testing0ALT() {
    // ---------------------------------------------------------------------
    // User‑interface controls
    // ---------------------------------------------------------------------
    static constexpr size_t                 ms_BUFF_SIZE    = 256;
    static constexpr int                    rows            = 2;
    static constexpr int                    cols            = 3;
    static constexpr int                    NUM             = rows * cols;
    static std::array< const char *, 6>     PLOT_NAMES      = {
        "One", "Two", "Three", "Four", "Five", "Six"
    };
    //static constexpr const char *           uuid            = PLOT_NAMES[0];
    static ImGuiID                          plot_ID         = 0;
    static bool                             first_frame     = true;


    // Global flags for the ImPlot demo
    static ImPlotSubplotFlags flags = ImPlotSubplotFlags_ShareItems;
    ImGui::CheckboxFlags("ImPlotSubplotFlags_ShareItems", (unsigned int*)&flags, ImPlotSubplotFlags_ShareItems);
    ImGui::CheckboxFlags("ImPlotSubplotFlags_ColMajor", (unsigned int*)&flags, ImPlotSubplotFlags_ColMajor);
    ImGui::BulletText("Drag and drop items from the legend onto plots (except for 'common')");

    // ---------------------------------------------------------------------
    // NEW: programmatic show/hide of every series (mimics legend‑icon clicks)
    // ---------------------------------------------------------------------
    static bool hide_all = false;            // current visibility state  (false = shown)
    static bool apply_toggle = false;        // set true for one frame when button pressed

    if (ImGui::Button(hide_all ? "Show All Series" : "Hide All Series")) {
        hide_all     = !hide_all;            // flip state
        apply_toggle = true;                 // apply to every item this frame only
    }

    // ---------------------------------------------------------------------
    // Sub‑plot grid configuration
    // ---------------------------------------------------------------------
    static int id[] = {0,1,2,3,4,5};         // subplot index assignment for 6 data series
    static int curj = -1;                    // series currently being dragged

    if (ImPlot::BeginSubplots("##ItemSharing", rows, cols, ImVec2(-1,400), flags))
    {
        ImPlot::SetupLegend(ImPlotLocation_South, ImPlotLegendFlags_Sort | ImPlotLegendFlags_Horizontal);

        for (int i = 0; i < rows * cols; ++i)
        {
            if ( ImPlot::BeginPlot(PLOT_NAMES[i]) )
            {
                
                
                
                if (first_frame) {
                    plot_ID = ImPlot::GetCurrentPlot()->ID;
                }
                
                
                
                // -----------------------------------------------------------------
                // "common" sine wave (always present in every subplot)
                // -----------------------------------------------------------------
                float fc = 0.01f;
                if (apply_toggle)
                    ImPlot::HideNextItem(hide_all, ImGuiCond_Always);
                ImPlot::PlotLineG("common", SinewaveGetter, &fc, 1000);

                // -----------------------------------------------------------------
                // Six draggable series, only the one whose id[j]==i is drawn here
                // -----------------------------------------------------------------
                for (int j = 0; j < 6; ++j) {
                    if (id[j] == i) {
                        char  label[ms_BUFF_SIZE];
                        float fj = 0.01f * (j + 2);
                        snprintf(label, sizeof(label), "data%d", j);

                        if (apply_toggle)
                            ImPlot::HideNextItem(hide_all, ImGuiCond_Always);
                        ImPlot::PlotLineG(label, SinewaveGetter, &fj, 1000);

                        // Make the legend‑entry icon a drag source
                        if (ImPlot::BeginDragDropSourceItem(label)) {
                            curj = j;
                            ImGui::SetDragDropPayload("MY_DND", nullptr, 0);
                            ImPlot::ItemIcon(ImPlot::GetLastItemColor());
                            ImGui::SameLine();
                            ImGui::TextUnformatted(label);
                            ImPlot::EndDragDropSource();
                        }
                    }
                }

                // Accept payload: re‑assign series j to subplot i
                if (ImPlot::BeginDragDropTargetPlot()) {
                    if (ImGui::AcceptDragDropPayload("MY_DND"))
                        id[curj] = i;
                    ImPlot::EndDragDropTarget();
                }

                ImPlot::EndPlot();
            }
        }
        ImPlot::EndSubplots();
    }

    // Reset one‑frame flag after applying
    if (apply_toggle)
        apply_toggle = false;
        
        
        
        
     
    //
    if (first_frame)
    {
        first_frame = false;
        utl::DisplayAllPlotIDs();
    }




    return;
}






#ifndef CHECKBOX_FLAG
#   define CHECKBOX_FLAG(flags, flag) ImGui::CheckboxFlags(#flag, (unsigned int*)&flags, flag)
#endif

//  "ImPlot_Testing0"
//
void App::ImPlot_Testing0(void)
{
    static utl::PlotCFG             cfg             = { {"##TestingPlotCFG"} };
    
    //  DATA INFORMATION...
    static constexpr size_t         NX              = 200ULL;
    static float                    data[NX]        = {0.0f};
    static float                    amp             = 1.0f;
    static float                    freq            = 1.0f;
    static float                    time            = 0.0f;
    
    //  PLOT INFORMATION...
    static std::string              PLOT_LABEL      = "Data";
    static ImVec4                   PLOT_COLOR      = app::DEF_APPLE_RED;
    static float                    PLOT_LW         = 3.0f;
    static float                    YLIMS[2]        = {-1.5f, 1.5f};
    
    //  P2  DATA...
    static ImPlotStairsFlags        p2_flags        = 0;
    static int                      p2_count        = 21;
    static float                    p2_scale[2]     = {0.005,   0};
    
    //  P3  DATA...
    static ImPlotHeatmapFlags       p3_flags        = 0;
    static float                    p3_vrange[2]    = {0.0,     1.0};
    static ImPlotRect               plot_bounds;
    
    static ImPlotColormap           CustomCMap      = utl::CreateTransparentColormap(ImPlotColormap_Viridis, 0.4f, "MyCustomCMap");
    static bool                     first_frame     = true;
    
    
    static const char *             plot_name       = "##TestingPlotCFG";
    ImPlotPlot *                    plot_ptr        = nullptr;
    static ImGuiID                  plot_ID         = 0;
    
    
    //  0.  CONTROL WIDGETS...
    ImGui::SeparatorText("Controls");
    //
    ImGui::SetNextItemOpen(true, ImGuiCond_Once);
    if ( ImGui::TreeNode("Source") ) {
        ImGui::SliderFloat("Amp",           &amp,           -2.0f,      2.0f,           "%.3f");
        ImGui::SliderFloat("Freq",          &freq,          0.001,      5,              "%.3f");
        ImGui::TreePop();
    }
    
    
    //  1.  PLOT P2...
    ImGui::SetNextItemOpen(false, ImGuiCond_Once);
    if ( ImGui::TreeNode("Stair Step") ) {
        CHECKBOX_FLAG(p2_flags, ImPlotStairsFlags_Shaded);
        ImGui::SliderInt("Count",           &p2_count,      1,          NX,                             "%1d");
        ImGui::SliderFloat("XScale",        &p2_scale[0],   0.0f,       10.0f,                           "%.3f");
        ImGui::SliderFloat("XStart",        &p2_scale[1],   0,          static_cast<float>(NX-1),       "%.3f");
        
        //p2_scale[0] = static_cast<float>(NX) / p2_count;
        
        //ImGui::SliderFloat2("Scale",        p2_scale,      0.0f,        1.0f,           "%.3f");
        ImGui::TreePop();
    }
    
    //  2.  PLOT P3...
    ImGui::SetNextItemOpen(false, ImGuiCond_Once);
    if ( ImGui::TreeNode("Heat Map") ) {
    
        ImGui::SliderFloat2("VRange",        p3_vrange,      -1.5f,        1.5f,           "%.3f");
    
        ImGui::TreePop();
    }
    
    
    
    
    
    //  1.  GENERATE DATA...
    time    = ImGui::GetTime();
    utl::sinusoid_wave_IMPL_1D(data, time, amp, freq);
    


    //  2.  CREATE THE PLOT...
    ImGui::NewLine();   ImGui::SeparatorText("Plots");
    //
    if ( utl::MakePlotCFG(cfg) )
    {
        
    
    //  ImGui::PushID(cfg.plot_uuid);
    //  if ( ImPlot::BeginPlot(cfg.plot_uuid, cfg.plot_size, cfg.plot_flags) )
    //  {
    //      //  3.  CONFIGURE THE PLOT APPEARANCE...
    //      ImPlot::SetupAxes(cfg.axis_labels[0],       cfg.axis_labels[1],
    //                        cfg.axis_flags[0],        cfg.axis_flags[1]);
    //      ImPlot::SetupLegend(cfg.legend_loc,         cfg.legend_flags);
    //
    //
    //      //  ImPlot::SetupAxisLimits(ImAxis_X1, 0, NX - 1,           ImGuiCond_Always);
    //      //  ImPlot::SetupAxisLimits(ImAxis_Y1, YLIMS[0], YLIMS[1],  ImGuiCond_Always);
    //
        
        //  4.  P3      | HEATMAP PLOT...
        {
            ImPlot::PushColormap(CustomCMap);
            plot_bounds     = ImPlot::GetPlotLimits();
            ImPlot::PlotHeatmap("heat",
                    data,
                    1,
                    NX,
                    p3_vrange[0],
                    p3_vrange[1],
                    nullptr,
                    plot_bounds.Min(),
                    plot_bounds.Max(),
                    p3_flags);
            
            ImPlot::PopColormap();
        }
            
        //  5.  P2      | PLOT STAIR-STEP PLOT...
        //  {
        //      ImPlot::SetNextMarkerStyle(ImPlotMarker_Circle);
        //      //ImPlot::SetNextFillStyle(IMPLOT_AUTO_COL, 0.25f);
        //      ImPlot::PlotStairs("Post Step (default)", data, p2_count, p2_scale[0], p2_scale[1], p2_flags);
        //  }

        
        //  0.  PLOT THE LINE...
        //
        ImPlot::SetNextLineStyle(PLOT_COLOR,      PLOT_LW);
        ImPlot::PlotLine(
            PLOT_LABEL.c_str(),
            data,
            NX,
            1.0,
            0.0,
            ImPlotLineFlags_None);

        //  END OF PLOT.
        ImPlot::EndPlot();
    }
        
        
        
        
     
    //
    if (first_frame)
    {
        first_frame = false;
        //  std::tie(plot_ptr, plot_ID) = utl::GetPlot(plot_name);
        //  utl::DisplayAllPlotIDs();
    }
    
    
    
    //ImGui::PopID();
    return;
}













// *************************************************************************** //
//
//
//      BROWSER TESTING...
// *************************************************************************** //
// *************************************************************************** //


// A simple record type we want to browse & edit
struct Person {
    std::string     name;
    int             age    = 30;
    bool            alive  = true;
};


//  "Test_Browser"
//
void App::Test_Browser(void)
{
    static std::vector<Person>      people                  = {       //  1.  In your application state (not every frame):
        {"Ada",         36,         true},
        {"Grace",       85,         false},
        {"Linus",       54,         true}
    };


    //  2.  Property-editor lambda (called once per frame for the selected item)
    static auto                     draw_person_properties  = [](Person& p){
        ImGui::InputText("Name", &p.name);
        ImGui::SliderInt("Age", &p.age, 0, 120);
        ImGui::Checkbox("Alive", &p.alive);
    };
    
    
    //  3.  Instantiate the widget (flat list selector in this example)
    static CB_Browser<Person>       browser(
        people,
        draw_person_properties,
        [](const Person& p){ return p.name; }
    );
    
    //  static cb::ui::CB_Browser<Person> browser{
    //      .items           = &people,                 // pass address of container
    //      .draw_properties = draw_person_properties,
    //      .label_provider  = [](const Person& p){ return p.name; }
    //  };


    // 4.  Draw the browser every frame
    browser.draw();


    // 5.  React to selection outside the widget if you like
    if (auto* person = browser.selected()) {
        /* ... */
    }






    return;
}



// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //

//  "Test_Editor"
//
void App::Test_Editor(void)
{


    m_editor.Begin();

    m_editor.DrawPointBrowser(this->m_editor_WinInfo.uuid.c_str(), &this->m_editor_WinInfo.open, this->m_editor_WinInfo.flags );

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
