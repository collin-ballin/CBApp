/***********************************************************************************
*
*       *********************************************************************
*       ****    M A I N _ A P P L I C A T I O N . C P P  ____  F I L E   ****
*       *********************************************************************
*
*              AUTHOR:      Collin A. Bond.
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
//
//
//  3.4     TESTS, DEMOS, DEBUGGING, TEMPORARY APPLICATIONS...
// *************************************************************************** //
// *************************************************************************** //

//  "PyStreamTest"
//
void App::PyStreamTest(void)
{
    return;
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
    static std::array< const char *, 6>     PLOT_NAMES      = {
        "One", "Two", "Three", "Four", "Five", "Six"
    };
    //static constexpr const char *           uuid            = PLOT_NAMES[0];
    [[maybe_unused]] static ImGuiID         plot_ID         = 0;
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
    //  static float                    YLIMS[2]        = {-1.5f, 1.5f};
    
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
    
    
    //  static const char *             plot_name       = "##TestingPlotCFG";
    //  ImPlotPlot *                    plot_ptr        = nullptr;
    //  static ImGuiID                  plot_ID         = 0;
    
    
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
    // Runtime‑tunable totals.
    static int total_buttons   = 6;   // how many widgets overall
    static int columns_per_row = 4;   // NC (columns per row)

    ImGui::SliderInt("Total Buttons",   &total_buttons,   1, 24, "%d", ImGuiSliderFlags_AlwaysClamp);
    ImGui::SliderInt("Columns / Row",   &columns_per_row, 1, 8,  "%d", ImGuiSliderFlags_AlwaysClamp);

    // Labels pool (wraps if total_buttons exceeds length).
    static const char* labels_pool[] = {
        "Alpha", "Beta SuperLong", "Gamma", "Delta", "Epsilon Extra",
        "Zeta", "Eta WideLabel", "Theta", "Iota", "Kappa VeryVeryLongLabel",
        "Lambda", "Mu", "Nu", "Xi", "Omicron", "Pi"
    };
    constexpr int LABEL_POOL_SIZE = sizeof(labels_pool) / sizeof(labels_pool[0]);

    ImVec2 btn_size = ImVec2(64, 64);

    // Compute rows required.
    int rows = (total_buttons + columns_per_row - 1) / columns_per_row;

    for (int r = 0; r < rows; ++r)
    {
        // Build per‑row label array (empty strings for unused trailing cells).
        const int COL_CAP = 8;                 // slider's max columns; adjust if needed
        static const char* row_labels[COL_CAP];

        for (int c = 0; c < columns_per_row; ++c)
        {
            int global_idx = r * columns_per_row + c;
            if (global_idx < total_buttons)
                row_labels[c] = labels_pool[global_idx % LABEL_POOL_SIZE];
            else
                row_labels[c] = ""; // empty placeholder
        }

        std::string row_uuid = "##CollimatedRow" + std::to_string(r);

        utl::collimated_row(
            row_uuid.c_str(),
            columns_per_row,          // always draw full grid width for alignment
            btn_size,
            row_labels,
            columns_per_row,
            [&](int col_idx, float x, const ImVec2& size, const char* /*lbl*/)
            {
                int global_idx = r * columns_per_row + col_idx;
                if (global_idx >= total_buttons)
                    return; // leave cell empty

                ImGui::SetCursorPosX(x);
                std::string id = "##Btn" + std::to_string(global_idx);
                ImGui::Button(id.c_str(), size);
            });
    }
}















// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
}//   END OF "cb" NAMESPACE.






//  UNDEF THE MACROS WE USED EARLIER...
//
#ifdef MAKE_GMAIL_COMPOSE_URL_IMPL
#  undef MAKE_GMAIL_COMPOSE_URL_IMPL
#endif

#ifdef MAKE_GMAIL_URL
#  undef MAKE_GMAIL_URL
#endif



// *************************************************************************** //
// *************************************************************************** //  END.
