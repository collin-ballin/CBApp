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

//using                           re_array                        = GraphApp::re_array;
//using                           re_frame                        = GraphApp::re_frame;
//using                           im_array                        = GraphApp::im_array;
//using                           im_frame                        = GraphApp::im_frame;



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
    model.run();
    flag.store(true, std::memory_order_release);
    return;
}


//  "StartDataInitAsync"
//
void GraphApp::StartDataInitAsync(void)
{
    std::thread([this]()
    {
        InitializeData( data_ready, this->ms_model );
    }).detach();
    
    return;
}



// *************************************************************************** //
//
//
//  3.      PLOTTING FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

//  "ShowPlayback"
//
void GraphApp::ShowPlayback(void)
{
    constexpr ImVec4                ET_COLOR                = ImVec4(0.910f,    0.145f,     0.184f,     1.000f);
    constexpr ImVec4                EF_COLOR                = ImVec4(0.1451f,   0.909f,     0.835f,     1.000f);
    constexpr float                 SCALE_WIDTH             = 100.0f;
    ImVec2                          avail                   = ImGui::GetContentRegionAvail();
    ImVec2                          SCALE_DIMS              = ImVec2(SCALE_WIDTH, -1);
    
    if ( this->m_SHOW_PLOTS[0] && this->m_SHOW_PLOTS[1] ) {
        SCALE_DIMS                      = ImVec2(SCALE_WIDTH, static_cast<float>(avail.y/2.0));
        fdtd_1D_time_cfg.graph.size     = { static_cast<float>(avail.x - SCALE_WIDTH - 2*ImGui::GetStyle().ItemSpacing.x), static_cast<float>(avail.y/2.0) };
    }
    else {
        if ( this->m_SHOW_PLOTS[0] ) {
            SCALE_DIMS                      = ImVec2(SCALE_WIDTH, -1);
            fdtd_1D_time_cfg.graph.size     = { static_cast<float>(avail.x - SCALE_WIDTH - 2*ImGui::GetStyle().ItemSpacing.x), -1 };
        }
        else {
        }
    }
    
    

    static ImPlotHeatmapFlags       eps_flags               = 0; //ImPlotHeatmapFlags_ColMajor;
    static ImPlotRect               plot_bounds;
    
    
    //  1.  ONE-SHOT TO BEGIN FDTD SIMULATION...
    std::call_once(init_once, [this]{ this->StartDataInitAsync(); });


    //  2.  DRAWING USER-CONTROLS FOR FDTD SIMULATION...
    ImGui::PushID("AnimControls");
        m_playback.ready = data_ready.load(std::memory_order_acquire);
        //  3.  IF DATA IS LOADING, DISPLAY A MESSAGE TO THE USER...
        if (!m_playback.ready) {
            ImGui::Text("Computing FDTD Simulation...  Please wait.");
        }
    ImGui::PopID();


    // Update animation frame if running and data ready
    if (m_playback.ready)
    {
        static float                    YLIMS [2]           = {-1.8f, 1.8f};
        static std::string              ET_PLOT_LABEL       = "E_{z}(time)";
        static std::string              EF_PLOT_LABEL       = "E_{z}(omega)";
        double                          now                 = ImGui::GetTime();
        double                          delta               = now - m_playback.last_time;
        
        static re_array                 ms_perm_E           = cblib::math::make_real_vector( ms_model.get_eps_r() );
        static re_array                 ms_frequencies      = ms_model.get_fourier_freqs();
        static re_frame &               ms_Ez_T_data        = *ms_model.get_E_time_data();
        static re_frame &               ms_Ez_F_data        = *ms_model.get_E_freq_data();
        
        static float                    perm_lims [2]       = {1.0, 16.0f};
        static float                    eps_vrange[2]       = {};
        static bool                     first_frame         = true;

        if (m_playback.playing && delta >= 1.0 / m_playback.fps) {
            m_playback.frame.value      = (m_playback.frame.value + 1) % m_playback.frame.limits.max;
            m_playback.last_time        = now;
        }



        //  2.  CREATE THE PLOT...
        if ( this->m_SHOW_PLOTS[0] )
        {
            if ( utl::MakePlotCFG(fdtd_1D_time_cfg) )
            {
                //  4.  P3      | HEATMAP PLOT...
                {
                    ImPlot::SetupAxisLimits(ImAxis_X1, 0, NX-1, ImGuiCond_Always);
                    ImPlot::SetupAxisLimits(ImAxis_Y1, YLIMS[0], YLIMS[1], ImGuiCond_Once);
                    ImPlot::PushColormap(ImPlot::GetColormapIndex("Perm_E"));
                    plot_bounds     = ImPlot::GetPlotLimits();
                    ImPlot::PlotHeatmap("Relative Permitivitty (Real)",
                            ms_perm_E.data(), //  ms_perm_E,//(float*)ms_perm_E.data(),
                            1,
                            NX,
                            perm_lims[0],  //  1.2f,   //YLIMS[0],
                            perm_lims[1],  //  16.0f,   //YLIMS[1],
                            nullptr,
                            plot_bounds.Min(),
                            plot_bounds.Max(),
                            eps_flags);
                    //
                }
                
                
                //  5.  PLOT  Ez(x, t)  ELECTRIC FIELD...
                ImPlot::SetNextLineStyle( ET_COLOR,      3.0);
                ImPlot::PlotLine(
                    ET_PLOT_LABEL.c_str(),
                    &ms_Ez_T_data[ m_playback.frame.value ][0],
                    NX,
                    1.0,
                    0.0,
                    ImPlotLineFlags_None);
                //
                ImPlot::EndPlot();// END PLOT.
                
                //  6.  DRAW THE COLORMAP SCALE...
                ImGui::SameLine();
                ImPlot::ColormapScale("Relative Permitivitty (Real)", perm_lims[0], perm_lims[1], SCALE_DIMS, "%.0f");
                            
                            
            ImPlot::PopColormap();
            }// END PLOT.
        }




        //  3.  CREATE THE FREQUENCY-DOMAIN PLOT...
        if ( this->m_SHOW_PLOTS[1] )
        {
            if ( utl::MakePlotCFG(fdtd_1D_freq_cfg) )
            {
                // 6. FREQUENCY-DOMAIN SPECTRUM PLOT using Fourier frequencies
                //ImPlot::SetupAxisScale(ImAxis_X1, ImPlotScale_Log10);
                ImPlot::SetupAxisLimits(ImAxis_X1, ms_frequencies[0], ms_frequencies[ ms_frequencies.size()-1 ], ImGuiCond_Always);
                ImPlot::SetupAxisLimits(ImAxis_Y1, 0.00f, 1.10f, ImGuiCond_Once);
                ImPlot::SetNextFillStyle(EF_COLOR, 0.4f);
                ImPlot::SetNextLineStyle(EF_COLOR, 3.0f);
                const int M = static_cast<int>(ms_frequencies.size());
                ImPlot::PlotLine(
                    EF_PLOT_LABEL.c_str(),
                    ms_frequencies.data(),
                    ms_Ez_F_data[m_playback.frame.value].data(),
                    M);
                ImPlot::EndPlot();
            }
        }
                        
                        








    }// END "ready".
        
        
    return;
}



// *************************************************************************** //
//
//
//  4.      CONTROL-PANEL FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

//  "ShowPlaybackControls"
//
void GraphApp::ShowPlaybackControls(void)
{
    
    
    //  5.  PLAYBACK TABLE...
    utl::MakeCtrlTable(this->ms_PLAYBACK_ROWS,   m_playback_table_CFG);

    

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
