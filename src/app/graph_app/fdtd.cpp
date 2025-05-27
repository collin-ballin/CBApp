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
void InitializeData([[maybe_unused]] std::atomic<bool> & flag, [[maybe_unused]] FDTD_t & model)
{
    //model.run();
    flag.store(true, std::memory_order_release);
    return;
}


//  "StartDataInitAsync"
//
void GraphApp::StartDataInitAsync(void)
{
    //  std::thread([this]()
    //  {
    //      //InitializeData( data_ready, this->ms_model );
    //
    //  }).detach();
    
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
