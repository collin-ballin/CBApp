/***********************************************************************************
*
*       ********************************************************************
*       ****              F D T D . C P P  ____  F I L E                ****
*       ********************************************************************
*              AUTHOR:      Collin A. Bond
*               DATED:      May 19, 2025.
*
**************************************************************************************
**************************************************************************************/
#include "app/graph_app/graph_app.h"



namespace cb { //     BEGINNING NAMESPACE "cb"...
// *************************************************************************** //
// *************************************************************************** //



//  0.      TYPENAME ALIASES FOR THIS TRANSLATION UNIT...
// *************************************************************************** //
// *************************************************************************** //
using                           FDTD_t                          = GraphApp::FDTD_t;
using                           value_type                      = GraphApp::value_type;

using                           re_array                        = GraphApp::re_array;
using                           re_frame                        = GraphApp::re_frame;
using                           im_array                        = GraphApp::im_array;
using                           im_frame                        = GraphApp::im_frame;



// *************************************************************************** //
//
//
//  1.      STATIC VARIABLES...
// *************************************************************************** //
// *************************************************************************** //

//          1.      MISC STUFF.
static bool                     ms_running                      = false;
//static std::atomic<bool>        data_ready(false);
//static std::once_flag           init_once;

//          2.      ANIMATION VARIABLES.
//  static bool                     playing                         = false;      // start paused
//  static float                    playback_fps                    = 30.0f;
//  static double                   last_time                       = 0.0;
//  static int                      current_frame                   = 0;






// *************************************************************************** //
//
//
//  2.      STAND-ALONE FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

//  "InitializeData"
//
void InitializeData(std::atomic<bool> & flag, FDTD_t & model)
{
    //model.run();
    flag.store(true, std::memory_order_release);
    return;
}


//  "StartDataInitAsync"
//
void GraphApp::StartDataInitAsync(void)
{
    std::thread([this]()
    {
        //InitializeData( data_ready, this->ms_model );
        
    }).detach();
    
    return;
}



//  "init_ctrl_rows"
//
void GraphApp::init_ctrl_rows(void)
{
    ms_FDTD_ROWS    = {
    //
    //
    //
        {"FDTD",                        []
            {// BEGIN.
                //      CASE 1 :    SCRIPT IS  **NOT**  RUNNING...
                if (!ms_running)
                {
                    if (ImGui::Button("Run Simulation")) // , ImVec2(ImGui::GetContentRegionAvail().x - pad, 0)) )
                    {
                        //
                        //  START THE MODEL...
                        //
                    }
                }
                else
                {
                    ImGui::PushStyleColor(ImGuiCol_Button,          ImVec4(0.800f, 0.216f, 0.180f, 1.00f) );
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered,   app::DEF_APPLE_RED );
                    if (ImGui::Button("Stop Simulation")) // , ImVec2(ImGui::GetContentRegionAvail().x - pad, 0)) )
                    {
                        //
                        //  STOP THE MODEL...
                        //
                    }
                    ImGui::PopStyleColor(2);
                }
            }// END.
        },
    //
        {"FDTD",                        []
            {// BEGIN.
            }// END.
        }
    //
    //
    //
    };//    END FDTD CTRLS.
    
    
    
    //  static int item_current_4 = 0;
    //  ImGuiComboFlags_PopupAlignLeft
    //  ImGuiComboFlags_HeightLargest
    //  const char* items[] = { "AAAA", "BBBB", "CCCC", "DDDD", "EEEE", "FFFF", "GGGG", "HHHH", "IIII", "JJJJ", "KKKK", "LLLLLLL", "MMMM", "OOOOOOO" };
    //  ImGui::Combo("combo 3 (one-liner)", &item_current_2, "aaaa\0bbbb\0cccc\0dddd\0eeee\0\0");
    //  ImGui::Combo("combo 4 (array)", &item_current_3, items, IM_ARRAYSIZE(items));
    //  ImGui::Combo("combo 5 (function)", &item_current_4, [](void* data, int n) { return ((const char**)data)[n]; }, items, IM_ARRAYSIZE(items));



    //  STEPSIZES ROWS...
    //
    ms_STEPSIZE_ROWS    = {
    //
        { "",   []
            {
                ImGui::TextUnformatted("Resolution");
            }
        }, // END ROW.
    //
        { "dx",                  [this]
            { // BEGIN LAMBDA.
                ImGui::SetNextItemWidth( ImGui::GetColumnWidth() );
                ImGui::SliderScalar("##FDTD_dx",    ImGuiDataType_Double,   &m_stepsizes.dx.value,      &m_stepsizes.dx.limits.min,     &m_stepsizes.dx.limits.max,     "%.1f spatial-units");
            } // END LAMBDA.
        }, // END ROW.
    //
        { "dy",                  [this]
            { // BEGIN LAMBDA.
                ImGui::BeginDisabled(true);
                ImGui::SetNextItemWidth( ImGui::GetColumnWidth() );
                ImGui::SliderScalar("##FDTD_dy",    ImGuiDataType_Double,   &m_stepsizes.dy.value,      &m_stepsizes.dy.limits.min,     &m_stepsizes.dy.limits.max,     "%.1f spatial-units");
                ImGui::EndDisabled();
            } // END LAMBDA.
        }, // END ROW.
    //
        { "dz",                  [this]
            { // BEGIN LAMBDA.
                ImGui::BeginDisabled(true);
                ImGui::SetNextItemWidth( ImGui::GetColumnWidth() );
                ImGui::SliderScalar("##FDTD_dz",    ImGuiDataType_Double,   &m_stepsizes.dz.value,      &m_stepsizes.dz.limits.min,     &m_stepsizes.dz.limits.max,     "%.1f spatial-units");
                ImGui::EndDisabled();
            } // END LAMBDA.
        }, // END ROW.
    //
        { "dt",                  [this]
            { // BEGIN LAMBDA.
                ImGui::SetNextItemWidth( ImGui::GetColumnWidth() );
                ImGui::SliderScalar("##FDTD_dt",    ImGuiDataType_Double,   &m_stepsizes.dt.value,      &m_stepsizes.dt.limits.min,     &m_stepsizes.dt.limits.max,     "%.1f temporal-units");
            } // END LAMBDA.
        }, // END ROW.
    //
        { "Sc",                  [this]
            { // BEGIN LAMBDA.
                ImGui::SetNextItemWidth( ImGui::GetColumnWidth() );
                ImGui::SliderScalar("##FDTD_Sc",    ImGuiDataType_Double,   &m_stepsizes.Sc.value,      &m_stepsizes.Sc.limits.min,     &m_stepsizes.Sc.limits.max,     "%.1f  Arb.");
            } // END LAMBDA.
        }, // END ROW.
    //
    //
    //
        { "",   []
            {
                ImGui::TextUnformatted("Simulated Region");
            }
        }, // END ROW.
    //
        { "NX",                 [this]
            { // BEGIN LAMBDA.
                ImGui::SetNextItemWidth( ImGui::GetColumnWidth() );
                ImGui::SliderScalar("##FDTD_NX",    ImGuiDataType_U64,      &m_steps.NX.value,      &m_steps.NX.limits.min,     &m_steps.NX.limits.max,  "%llu spatial-steps");
            } // END LAMBDA.
        }, // END ROW.
    //
        { "NY",                 [this]
            { // BEGIN LAMBDA.
                //ImGui::BeginDisabled(true);
                ImGui::SetNextItemWidth( ImGui::GetColumnWidth() );
                ImGui::SliderScalar("##FDTD_NY",    ImGuiDataType_U64,      &m_steps.NY.value,      &m_steps.NY.limits.min,     &m_steps.NY.limits.max,  "%llu spatial-steps");
                //ImGui::EndDisabled();
            } // END LAMBDA.
        }, // END ROW.
    //
        { "NZ",                 [this]
            { // BEGIN LAMBDA.
                ImGui::BeginDisabled(true);
                ImGui::SetNextItemWidth( ImGui::GetColumnWidth() );
                ImGui::SliderScalar("##FDTD_NZ",    ImGuiDataType_U64,      &m_steps.NZ.value,      &m_steps.NZ.limits.min,     &m_steps.NZ.limits.max,  "%llu spatial-steps");
                ImGui::EndDisabled();
            } // END LAMBDA.
        }, // END ROW.
    //
    //
    //
        { "NT",                 [this]
            { // BEGIN LAMBDA.
                ImGui::SetNextItemWidth( ImGui::GetColumnWidth() );
                ImGui::SliderScalar("##FDTD_NT",    ImGuiDataType_U64,      &m_steps.NT.value,      &m_steps.NT.limits.min,     &m_steps.NT.limits.max,  "%llu temporal-steps");
            } // END LAMBDA.
        } // END ROW.
    };
    
    
    
    
    
    
    
    
    
    //  SOURCES ROWS...
    //
    ms_SOURCES_ROWS    = {
    //
        { "",   []
            {
                ImGui::TextUnformatted("Source");
            }
        }, // END ROW.
    //
        { "Type",   []
            {
                using                       Item            = std::pair<const char*, bool>;
                using                       Group           = std::pair<const char*, std::vector<Item>>;
                static std::vector<Group>   combo_groups    = {
                    {"Fundamental Sources", {
                        {"Gaussian Pulse",                              true},      {"Time-Harmonic / Monochromatic Plane Wave",    true},
                        {"Ricker Wavelet / Modulated Gaussian",         true},      {"Arbitrary Function Generator",                false}
                    }
                },
                    {"Temporal Profile Sources", {
                        {"Impulse / Dirac Delta",                       false},     {"Gated Sinusoid / Finite-Duration Tone",       false},
                        {"Chirped Pulse / Tunable-Band Tone",           false},     {"Time-Harmonic (Amplitude-Modulated)",         false},
                        {"Time-Harmonic (Frequency-Modulated)",         false},     {"Mathieu Beam",                                false}
                    }
                },
                    {"Spatial Distribution Sources", {
                        {"Point Dipole",                                false},     {"Line / Sheet",                                false},
                        {"Gaussian / Paraxial TEM_00",                  false},     {"Bessel / Vortex Beam",                        false},
                        {"Waveguide Port / TE_10 Computed Eigenmode",   false}
                    }
                },
                    {"Noise Sources", {
                        {"White Noise",                                 false},     {"Pink Noise",                                  false},
                        {"Shot / Poisson Noise",                        false},     {"Burst / Random Telegraph Noise",              false},
                        { "Schottky / Poisson Noise",                   false}
                    }
                },
                    {"Mode-Converter Sources", {
                        {"Ince-Gaussian (IG)",                          false},     {"Hermite-Gaussian (HG)",                       false},
                        {"Laguerre-Gaussian (LG)",                      false},     {"Bessel-Gaussian",                             false},
                        {"Airy Beam",                                   false},     {"Mathieu Beam",                                false}
                    }
                }
                //
                };  //  END.

                static ImGuiComboFlags      flags           = ImGuiComboFlags_PopupAlignLeft | ImGuiComboFlags_HeightLargest;
                static int                  item_current    = 0;
                const char *                current_label   = nullptr;
                int                         index           = 0;
                
                for (const auto & group : combo_groups)
                {
                    for (const auto & item : group.second) {
                        if (index == item_current) {
                            current_label = item.first;
                            break;
                        }
                        ++index;
                    }
                    if (current_label) break;
                }

                if (ImGui::BeginCombo("##FDTDSourceType", current_label, flags))
                {
                    int global_index = 0;
                    for (const auto& group : combo_groups) {
                        ImGui::TextDisabled("%s", group.first);
                        for (const auto& item : group.second) {
                            if (item.second) {
                                if (ImGui::Selectable(item.first, item_current == global_index)) {
                                    item_current = global_index;
                                }
                            }
                            else
                            {
                                ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyle().Colors[ImGuiCol_TextDisabled]);
                                ImGui::Selectable(item.first, false, ImGuiSelectableFlags_Disabled);
                                ImGui::PopStyleColor();
                            }
                            ++global_index;
                        }
                        ImGui::Separator();
                    }
                    ImGui::EndCombo();
                }
            
            }
        }, // END ROW.
    //
        { "Energy Propogation",   []
            {
                static const char *     items[]         = { "Soft", "Hard" };
                static int              selected        = 0;

                if (ImGui::BeginCombo("Combo", items[selected]))
                {
                    for (int i = 0; i < IM_ARRAYSIZE(items); ++i)
                    {
                        if (ImGui::Selectable(items[i], selected == i))
                            selected = i;
                    }
                    ImGui::EndCombo();
                }
            }
        }, // END ROW.
    //
    //
    //
        { "Wavelength",         [this]
            { // BEGIN LAMBDA.
                ImGui::SetNextItemWidth( ImGui::GetColumnWidth() );
                ImGui::SliderScalar("##FDTD_SourceWavelength",  ImGuiDataType_U64,                  &m_params.wavelength.value,
                                                                &m_params.wavelength.limits.min,    &m_params.wavelength.limits.max,  "%llu spatial-steps");
            } // END LAMBDA.
        }, // END ROW.
    //
        { "Duration",           [this]
            { // BEGIN LAMBDA.
                ImGui::SetNextItemWidth( ImGui::GetColumnWidth() );
                ImGui::SliderScalar("##FDTD_SourceDuration",    ImGuiDataType_U64,                  &m_params.duration.value,
                                                                &m_params.duration.limits.min,      &m_params.duration.limits.max,      "%llu # of periods");
            } // END LAMBDA.
        }, // END ROW.
        { "TF/SF Boundary",     []
            { // BEGIN LAMBDA.
                ImGui::SetNextItemWidth( ImGui::GetColumnWidth() );
                ImGui::Text("Boundary Conditions");
            } // END LAMBDA.
        }, // END ROW.
    //
    //
    //
        { "FDTD",                        []
            { // BEGIN LAMBDA.
                static bool temp = false;
                
                //      CASE 1 :    SCRIPT IS  **NOT**  RUNNING...
                if (!temp)
                {
                    if (ImGui::Button("Run Simulation")) // , ImVec2(ImGui::GetContentRegionAvail().x - pad, 0)) )
                    {
                        temp = true;
                        //
                        //  START THE MODEL...
                        //
                    }
                }
                else
                {
                    ImGui::PushStyleColor(ImGuiCol_Button,          ImVec4(0.800f, 0.216f, 0.180f, 1.00f) );
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered,   app::DEF_APPLE_RED );
                    if (ImGui::Button("Stop Simulation")) // , ImVec2(ImGui::GetContentRegionAvail().x - pad, 0)) )
                    {
                        temp = false;
                        //
                        //  STOP THE MODEL...
                        //
                    }
                    ImGui::PopStyleColor(2);
                }
            
            } // END LAMBDA.
        } // END ROW.
    };
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    return;
}


// *************************************************************************** //
//
//
//  3.      PLOTTING FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

//  "ShowFDTD"
//
void GraphApp::ShowFDTD(void)
{
    static utl::PlotCFG             cfg        = {
        {   "##1DFDTDPlot",             ImVec2(-1, -1),         ImPlotFlags_None | ImPlotFlags_NoTitle  },
        {
            { "x-Node Index  [m dx]",   ImPlotAxisFlags_None | ImPlotAxisFlags_AutoFit },
            { "Ez  [V / m]",            ImPlotAxisFlags_None | ImPlotAxisFlags_AutoFit }
        }
    };
    
    static ImPlotColormap                   PermECMap               = utl::CreateTransparentColormap(ImPlotColormap_Viridis, 0.4f, "MyPermittivityCMap");
    
    static ImPlotHeatmapFlags               eps_flags               = 0; //ImPlotHeatmapFlags_ColMajor;
    static ImPlotRect                       plot_bounds;
    
    
    //  1.  ONE-SHOT TO BEGIN FDTD SIMULATION...
    std::call_once(init_once, [this]{ this->StartDataInitAsync(); });


    //  2.  DRAWING USER-CONTROLS FOR FDTD SIMULATION...
    ImGui::PushID("AnimControls");
    bool ready = data_ready.load(std::memory_order_acquire);


    //  3.  IF DATA IS LOADING, DISPLAY A MESSAGE TO THE USER...
    if (!ready) {
        ImGui::Text("Initializing data... Please wait.");
    }



    ImGui::BeginDisabled(!ready);   //  IF DATA IS NOT READY, DISABLE THESE CONTROLS...
    {
        if (ready)
        {
            //
            //  CASE 1 :    DATA == *READY*.  ANIMATION == *PLAYING*.
            if (playing)
            {
                if (ImGui::Button("Pause")) {
                    playing = false;
                }
            }
            //
            //  CASE 2 :    READY, BUT *NOT* PLAYING...
            else
            {
                if (ImGui::Button("Start")) {
                    playing   = true;
                    last_time = ImGui::GetTime();
                }
            }
            ImGui::SameLine();
            ImGui::SliderFloat("Speed (fps)", &playback_fps, 0.1f, 240.0f, "%.1f");
            ImGui::SliderInt("Frame", &current_frame, 0, NT - 1);
        }
    }
    ImGui::EndDisabled();
    ImGui::PopID();


    // Update animation frame if running and data ready
    if (ready)
    {
        static float                    YLIMS [2]           = {-1.5f, 1.5f};
        static std::string              PLOT_LABEL          = "Ez(t)";
        double                          now                 = ImGui::GetTime();
        double                          delta               = now - last_time;
        //static re_frame &               ms_data             = *ms_model.get_E_time_data();
        //static float *                  ms_perm_E           = ms_model.get_perm_E_ptr();
        //static re_array &               ms_perm_E           = *ms_model.get_perm_E_data();
        //double static                   eps_min             = *std::min_element( ms_perm_E.begin(), ms_perm_E.end() );
        //double static                   eps_max             = *std::max_element( ms_perm_E.begin(), ms_perm_E.end() );
        static float                    eps_vrange[2]       = {};
        static bool                     first_frame         = true;

        //  if (first_frame) {
        //      for (size_t i = 0; i < NX; ++i)
        //          std::cout << "(" << i << ", " << ms_perm_E[i] << ")\n";
        //  }
        if (playing && delta >= 1.0 / playback_fps) {
            current_frame = (current_frame + 1) % NT;
            last_time     = now;
        }





        //  2.  CREATE THE PLOT...
        if ( utl::MakePlotCFG(cfg) )
        {
            //  4.  P3      | HEATMAP PLOT...
            //      {
            //          ImPlot::PushColormap(ImPlot::GetColormapIndex("Perm_E"));
            //          plot_bounds     = ImPlot::GetPlotLimits();
            //          ImPlot::PlotHeatmap("heat",
            //                  ms_perm_E,//(float*)ms_perm_E.data(),
            //                  1,
            //                  NX,
            //                  1.2f,   //YLIMS[0],
            //                  16.0f,   //YLIMS[1],
            //                  nullptr,
            //                  plot_bounds.Min(),
            //                  plot_bounds.Max(),
            //                  eps_flags);
            //          //
            //          ImPlot::PopColormap();
            //      }
            
            
            //  5.  PLOT  Ez(x, t)  ELECTRIC FIELD...
            //  ImPlot::SetNextLineStyle(   m_plot_colors[0],      3.0);
            //  ImPlot::PlotLine(
            //      PLOT_LABEL.c_str(),
            //      &ms_data[current_frame][0],
            //      NX,
            //      1.0,
            //      0.0,
            //      ImPlotLineFlags_None);
            //  ImPlot::EndPlot();
        }
        
        
        ImGui::PopID();
    }// END "ready".
        
        
    return;
}



// *************************************************************************** //
//
//
//  4.      CONTROL-PANEL FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

//  "ShowFDTDControls"
//
void GraphApp::ShowFDTDControls(void)
{
    //  CONSTANTS...
    static constexpr float              LABEL_COLUMN_WIDTH      = 200.0f;
    static constexpr float              WIDGET_COLUMN_WIDTH     = 250.0f;

    //  INTERACTIVE VARIABLES...

    //  TABLE GLOBAL FLAGS...
    static bool                         freeze_header           = false;
    static bool                         freeze_column           = false;
    static bool                         stretch_column_1        = true;

    //  COLUMN-SPECIFIC FLAGS...
    static ImGuiTableColumnFlags        col0_flags              = ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize;
    static ImGuiTableColumnFlags        col1_flags              = stretch_column_1 ? ImGuiTableColumnFlags_WidthStretch : ImGuiTableColumnFlags_WidthFixed;
    static ImGuiTableFlags              flags                   = ImGuiTableFlags_None | ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable | ImGuiTableFlags_NoKeepColumnsVisible; //| ImGuiTableFlags_ScrollX;
        
    static const utl::WidgetRow         rows[]                  = {
        {"FDTD",                        []
            {// BEGIN.
                //      CASE 1 :    SCRIPT IS  **NOT**  RUNNING...
                if (!ms_running)
                {
                    if (ImGui::Button("Run Simulation")) // , ImVec2(ImGui::GetContentRegionAvail().x - pad, 0)) )
                    {
                        //
                        //  START THE MODEL...
                        //
                    }
                }
                else
                {
                    ImGui::PushStyleColor(ImGuiCol_Button,          ImVec4(0.800f, 0.216f, 0.180f, 1.00f) );
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered,   app::DEF_APPLE_RED );
                    if (ImGui::Button("Stop Simulation")) // , ImVec2(ImGui::GetContentRegionAvail().x - pad, 0)) )
                    {
                        //
                        //  STOP THE MODEL...
                        //
                    }
                    ImGui::PopStyleColor(2);
                }
            
            }// END.
        }
    };

    static utl::TableCFG<2>    fdtd_table_CFG    = {
        "FDTDControls"
    };





    //  1.  PRIMARY TABLE ENTRY...
    //
    utl::MakeCtrlTable(rows, fdtd_table_CFG);




    return;
}










// *************************************************************************** //
//
//
//  ?.      UTILITY FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //
























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
