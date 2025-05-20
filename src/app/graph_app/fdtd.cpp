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
    static utl::PlotCFG                     cfg                     = {"##1DFDTDPlot"};
    static ImVec2                           m_mst_plot_size         = ImVec2(-1, -1);
    static std::array<const char *, 2>      ms_mst_axis_labels      = {"x-Node Index  [m dx]", "Ez  [V / m]"};
    static ImPlotColormap                   PermECMap               = utl::CreateTransparentColormap(ImPlotColormap_Viridis, 0.4f, "MyPermittivityCMap");
    
    
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
            ImGui::SliderFloat("Speed (fps)", &playback_fps, 0.1f, 120.0f, "%.1f");
            ImGui::SliderInt("Frame", &current_frame, 0, NT - 1);
        }
    }
    ImGui::EndDisabled();
    ImGui::PopID();


    // Update animation frame if running and data ready
    if (ready)
    {
        static std::pair<float,float>   YLIMS           = {-1.5f, 1.5f};
        static std::string              PLOT_LABEL      = "Ez(t)";
        double                          now             = ImGui::GetTime();
        double                          delta           = now - last_time;
        static re_frame &               ms_data         = *ms_model.get_E_time_data();
        
        if (playing && delta >= 1.0 / playback_fps) {
            current_frame = (current_frame + 1) % NT;
            last_time     = now;
        }


        //  2.  CREATE THE PLOT...
        ImGui::PushID(cfg.plot_uuid);
        if ( ImPlot::BeginPlot(cfg.plot_uuid, cfg.plot_size, cfg.plot_flags) )
        {
            //  3.  CONFIGURE THE PLOT APPEARANCE...
            ImPlot::SetupAxes(cfg.axis_labels[0],       cfg.axis_labels[1],
                              cfg.axis_flags[0],        cfg.axis_flags[1]);
            ImPlot::SetupLegend(cfg.legend_loc,         cfg.legend_flags);

            
            
            
            
            
            
            //  5.  PLOT  Ez(x, t)  ELECTRIC FIELD...
            ImPlot::SetNextLineStyle(   m_plot_colors[0],      3.0);
            ImPlot::PlotLine(
                PLOT_LABEL.c_str(),
                &ms_data[current_frame][0],
                NX,
                1.0,
                0.0,
                ImPlotLineFlags_None);
            ImPlot::EndPlot();
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



    //  1.  PRIMARY TABLE ENTRY...
    //
    if (ImGui::BeginTable("SketchFDTD", 2, flags))
    {
        if (freeze_column || freeze_header)
            ImGui::TableSetupScrollFreeze(freeze_column ? 1 : 0, freeze_header ? 1 : 0);


        ImGui::TableSetupColumn("Label",    col0_flags,     LABEL_COLUMN_WIDTH);
        ImGui::TableSetupColumn("Widget",   col1_flags,     stretch_column_1 ? 1.0f : WIDGET_COLUMN_WIDTH);
        ImGui::TableHeadersRow();

        for (const auto & row : ms_FDTD_ROWS) {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::AlignTextToFramePadding();
            ImGui::TextUnformatted(row.label);
            ImGui::TableSetColumnIndex(1);
            row.render();
        }

        ImGui::EndTable();
    }

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
