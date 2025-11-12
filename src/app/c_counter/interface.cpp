/***********************************************************************************
*
*       ********************************************************************
*       ****         I N T E R F A C E . C P P  _____  F I L E           ****
*       ********************************************************************
*              AUTHOR:      Collin A. Bond.
*               DATED:      July 4, 2025.
*
**************************************************************************************
**************************************************************************************/
#include "app/c_counter/c_counter.h"




namespace cb { //     BEGINNING NAMESPACE "cb"...
// *************************************************************************** //
// *************************************************************************** //



// *************************************************************************** //
//
//
//
//      1.      USER-INTERFACE...
// *************************************************************************** //
// *************************************************************************** //


//  "TAB_NewControls"
//
void CCounterApp::TAB_NewControls(void) noexcept
{
    namespace                       cc                  = ccounter;                             //  NAMESPACES AND ALIASES.
    namespace                       fs                  = std::filesystem;
    //
    static ImGuiSliderFlags         SLIDER_FLAGS        = ImGuiSliderFlags_AlwaysClamp;         //  STATICS.
    static auto                     labelcb             = this->S.ms_LeftLabel;
    //
    //  constexpr float                 margin              = 0.75f;                                //  CONSTANTS.
    //  constexpr float                 pad                 = 10.0f;
    //
    //
    //
    const ImVec2                    avail               = ImGui::GetContentRegionAvail();       //  LOCAL VARS.

    
    
    
    
    //      1.      COINCIDENCE WINDOW...
    //
    {
        labelcb("Coincidence Window");
        
        if ( ImGui::SliderScalar("##CoincidenceWindow",    ImGuiDataType_U64,      &m_coincidence_window.value,
                                &m_coincidence_window.limits.min, &m_coincidence_window.limits.max,  "%llu ticks", SLIDER_FLAGS) )
        {
            if (m_process_running)
            {
                char cmd[ms_CMD_MSG_SIZE];
                std::snprintf(cmd, ms_CMD_MSG_SIZE, "coincidence_window %llu\n", m_coincidence_window.value);
                m_python.send(cmd);
            }
        }
        
        
        
        
    
    }// END.
    
    
    
    
    
    
    
    
    
    //  if (...)
    //  {
    //      callback("In-Handle:");
    //      ImGui::PushItemWidth( ms_HALF_WIDTH );
    //          v.ui_InHandle       (WS_xmax, WS_ymax, speedx, speedy);
    //      ImGui::PopItemWidth();
    //  }
        
        
    
    /*
    ms_CTRL_ROWS            = {
    //
    //  1.  CONTROL PARAMETERS [TABLE]...

    //
        {"Coincidence Window",                  [this]
            {// BEGIN.
                ImGui::SetNextItemWidth( margin * ImGui::GetColumnWidth() );
                //if (ImGui::SliderInt("##CoincidenceWindow",       &m_coincidence_window,   8,    100,   "%03d clicks",    SLIDER_FLAGS) )
                
                if ( ImGui::SliderScalar("##CoincidenceWindow",    ImGuiDataType_U64,      &m_coincidence_window.value,
                                        &m_coincidence_window.limits.min, &m_coincidence_window.limits.max,  "%llu ticks", SLIDER_FLAGS) )
                {
                    if (m_process_running)    {
                        char cmd[ms_CMD_MSG_SIZE];
                        std::snprintf(cmd, ms_CMD_MSG_SIZE, "coincidence_window %llu\n", m_coincidence_window.value);
                        m_python.send(cmd);
                    }
                }
            }// END.
        },
    //
        {"Integration Window",                  [this]
            {// BEGIN.
                ImGui::SetNextItemWidth( margin * ImGui::GetColumnWidth() );
                if ( ImGui::SliderScalar("##IntegrationWindow",          ImGuiDataType_Double,       &m_integration_window.value,
                                    &m_integration_window.limits.min,     &m_integration_window.limits.max,     "%.3f seconds", SLIDER_FLAGS) ) {
                    if (m_process_running) {
                        char cmd[ms_CMD_MSG_SIZE];
                        std::snprintf(cmd, ms_CMD_MSG_SIZE, "integration_window %.3f\n", m_integration_window.value);
                        m_python.send(cmd);
                    }
                }
                ImGui::SameLine(0.0f, pad);
                if (ImGui::Button("Apply", ImVec2(ImGui::GetContentRegionAvail().x - pad, 0)) ) {
                    char cmd[ms_CMD_MSG_SIZE];
                    std::snprintf(cmd, ms_CMD_MSG_SIZE, "integration_window %.3f\n", cc::delay_s);
                    m_python.send(cmd);
                }
                ImGui::Dummy( ImVec2(pad, 0.0f) );
            }// END.
        },
    //
        {"History Length",                      [this]
            {// BEGIN.
                ImGui::SetNextItemWidth( margin * ImGui::GetColumnWidth() );
                ImGui::SliderScalar("##HistoryLength",                  ImGuiDataType_Double,           &m_history_length.value,
                                    &m_history_length.limits.min,       &m_history_length.limits.max,   "%.1f seconds", SLIDER_FLAGS);
                ImGui::SameLine(0.0f, pad);
                
                if ( ImGui::Button("Clear Plot", ImVec2(ImGui::GetContentRegionAvail().x - pad, 0)) ) {
                    for (auto & b : m_buffers) {
                        b.clear(); //b.Erase();
                    }
                    std::fill(std::begin(m_max_counts), std::end(m_max_counts), 0.f);
                }
                ImGui::Dummy( ImVec2(pad, 0.0f) );
            }// END.
        },
    //
        {"Average",                             [this]
            {// BEGIN.
            //
                const char *        avg_items []        = { "Last N samples", "Last T seconds" };
                int                 mode_idx            = (m_avg_mode == AvgMode::Samples ? 0 : 1);
            //
            //
                ImGui::SetNextItemWidth( margin * ImGui::GetColumnWidth() );
                if (m_avg_mode == AvgMode::Samples)
                {
                    ImGui::SliderScalar("##AverageDuration_Samples",        ImGuiDataType_U64,                  &m_avg_window_samp.value,
                                        &m_avg_window_samp.limits.min,      &m_avg_window_samp.limits.max,      "%llu samples", SLIDER_FLAGS);
                }
                else {
                    ImGui::SliderScalar("##AverageDuration_Time",           ImGuiDataType_Double,               &m_avg_window_sec.value,
                                        &m_avg_window_sec.limits.min,       &m_avg_window_sec.limits.max,      "%.2f seconds", SLIDER_FLAGS);
                }
                //
                //
                //
                ImGui::SameLine(0.0f, pad);
                ImGui::SetNextItemWidth( ImGui::GetContentRegionAvail().x - pad );
                if ( ImGui::Combo("##AverageModeSelector",   &mode_idx,      avg_items,      IM_ARRAYSIZE(avg_items)) )
                {
                    m_avg_mode = (mode_idx == 0 ? AvgMode::Samples : AvgMode::Seconds);
                }
                ImGui::Dummy( ImVec2(pad, 0.0f) );
                            
   
            }// END.
        },
    //
    //
        {"Misc.",                          [this]
            {// BEGIN.
            
                ImGui::Checkbox("Use Mutex Counts", &m_use_mutex_count);
                ImGui::SameLine();
                
                if ( ImGui::Button("Reset Averages") )      { this->_reset_average_values(); }
                
                ImGui::SameLine();
                
                if ( ImGui::Button("Reset Max") )           { this->_reset_max_values(); }
                
                ImGui::SameLine();
                ImGui::Checkbox("Plot Crawling", &m_smooth_scroll);
                
                
            }// END.
        }
    };
*/



    return;
}



//
//
//
// *************************************************************************** //
// *************************************************************************** //   END [[ 1.  "MAIN CONTROLS" ]].












// *************************************************************************** //
//
//
//
//      2.      OTHER TAB FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

//  "TAB_Controls"
//
void CCounterApp::TAB_Controls(void) noexcept
{
    //      1.      PRIMARY TABLE ENTRY...
    static utl::TableCFG<2>     ctrl_table_CFG          = {"CCCounterControlsTable"};
    utl::MakeCtrlTable(this->ms_CTRL_ROWS, ctrl_table_CFG);


    return;
}



//  "TAB_Appearance"
//
void CCounterApp::TAB_Appearance(void) noexcept
{
    static utl::TableCFG<2>     appearance_table_CFG     = {"CCounterAppearanceTable"};
    

    //      2.      APPEARANCE TABLE ENTRY...
    utl::MakeCtrlTable(this->ms_APPEARANCE_ROWS, appearance_table_CFG);
    
    return;
}



//
//
//
// *************************************************************************** //
// *************************************************************************** //   END [[ 2.  "OTHER TAB FUNCTIONS" ]].












// *************************************************************************** //
//
//
//
//      3.      MISC. GUI FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

//  "_MENU_cmap_selection"
//
bool CCounterApp::_MENU_cmap_selection(const ImPlotColormap map) noexcept
{
    // Size controls
    constexpr float           CMAP_PREVIEW_WIDTH_SCALE  = 1.00f;          // 1.0 = base width; >1 expand, <1 shrink
    constexpr float           CMAP_PREVIEW_MIN_W        = 350.0f;         // baseline button width before scaling
    constexpr float           CMAP_TABLE_HEIGHT         = 400.0f;         // finite height -> vertical scrollbar

    // One-column, scrollable table; window width slaved to preview button width
    constexpr ImGuiTableFlags FLAGS                     = ImGuiTableFlags_SizingStretchSame | ImGuiTableFlags_ScrollY;

    static ImVec2             s_cmap_preview_dims       = ImVec2(CMAP_PREVIEW_MIN_W, 45.0f);
    bool                      close                     = false;
    const int                 N                         = ImPlot::GetColormapCount();

    ImGui::SeparatorText("Colormap Selection");

    // Compute desired content/table width from preview button width
    const float table_w = ImMax(CMAP_PREVIEW_MIN_W, s_cmap_preview_dims.x) * CMAP_PREVIEW_WIDTH_SCALE;

    // On popup open, force the popup/window width to match the preview-controlled width
    if (ImGui::IsWindowAppearing()) {
        const ImGuiStyle& style = ImGui::GetStyle();
        const float desired_window_w = table_w + style.WindowPadding.x * 2.0f;
        ImGui::SetWindowSize(ImVec2(desired_window_w, 0.0f)); // height auto
    }

    // Scrollable table whose OUTER SIZE X sets the content width (slaved to preview width)
    if (!ImGui::BeginTable("ColormapSelectionTable", 1, FLAGS, ImVec2(table_w, CMAP_TABLE_HEIGHT)))
        return false;

    ImGui::TableSetupColumn("##CMap", ImGuiTableColumnFlags_WidthStretch, 1.0f);

    for (int i = 0; i < N && !close; ++i) {
        const ImPlotColormap idx      = static_cast<ImPlotColormap>(i);
        const bool           selected = (map == idx);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);

        ImGui::PushID(i);
        // Button width = current column's available width (accounts for scrollbar & padding)
        const float col_w = ImGui::GetContentRegionAvail().x;
        const float w     = ImMax(CMAP_PREVIEW_MIN_W, col_w); // fill the single column
        const ImVec2 sz   = ImVec2(w, s_cmap_preview_dims.y);

        if (ImPlot::ColormapButton(ImPlot::GetColormapName(idx), sz, idx)) {
            if (!selected) {
                this->m_cmap                   = idx;
                this->m_colormap_cache_invalid = true;
            }
            close = true;
        }
        ImGui::PopID();
    }

    ImGui::EndTable();
    return !close; // keep open if nothing was chosen
}
//
//
//
/*
{
    // Layout controls
    constexpr float                 CMAP_PREVIEW_WIDTH_SCALE    = 1.00f;                    // 1.0 = fill remaining width
    constexpr float                 CMAP_POPUP_VIEW_H           = 260.0f;                   // finite height for scrollable content
    constexpr ImGuiWindowFlags      CHILD_FLAGS                 = ImGuiWindowFlags_AlwaysVerticalScrollbar;

    constexpr ImGuiTableFlags       FLAGS                       = ImGuiTableFlags_SizingFixedFit;
    constexpr ImGuiSelectableFlags  SELECTABLE_FLAGS            = ImGuiSelectableFlags_SpanAllColumns;
    //
    //
    static ImVec2                   s_cmap_preview_dims         = ImVec2(120.0f, 30.0f);    // clamp preview width
    static float                    col_name_width              = -1.0f;   // cached
    static float                    row_height                  = 0.0f;    // cached after first preview draw


    //      Cache the widest name when the popup appears
    //  if ( ImGui::IsWindowAppearing() )
    //  {
    //      col_name_width      = 0.0f;
    //      const int count     = ImPlot::GetColormapCount();
    //      for (int i = 0; i < count; ++i) {
    //          const char *    nm      = ImPlot::GetColormapName(i);
    //          col_name_width          = ImMax(col_name_width, ImGui::CalcTextSize(nm ? nm : "").x);
    //      }
    //      col_name_width     *= 1.60f;        //  padding similar to the tool menu
    //      row_height          = 0.0f;         //  recalc on first draw
    //  }


    ImGui::SeparatorText("Colormap Selection");

    //      Scrollable viewport inside the popup
    if ( !ImGui::BeginChild("##CMapScrollRegion", ImVec2(0.0f, CMAP_POPUP_VIEW_H), false, CHILD_FLAGS) ) {
        return true; // child not visible -> keep popup open
    }

    if ( !ImGui::BeginTable("ColormapSelectionTable", 2, FLAGS, ImVec2(0,0)) ) {
        ImGui::EndChild();
        return false;
    }

    ImGui::TableSetupColumn("##CMapName",    ImGuiTableColumnFlags_WidthFixed,   col_name_width);
    ImGui::TableSetupColumn("##CMapPreview", ImGuiTableColumnFlags_WidthStretch, 1.0f);

    bool            close       = false;
    const int       N           = ImPlot::GetColormapCount();



    for (int i = 0; i < N && !close; ++i)
    {
        const ImPlotColormap    idx         = static_cast<ImPlotColormap>(i);
        const bool              selected    = (map == idx);

        ImGui::TableNextRow();


        //      Column 0 :      Name (selectable)
        ImGui::TableSetColumnIndex(0);
        if ( ImGui::Selectable(  ImPlot::GetColormapName(idx), selected, SELECTABLE_FLAGS
                               , ImVec2(0.0f, s_cmap_preview_dims.y)) )
        {
            close = true;
        }


        //      Column 1 :      Preview (gradient button)
        ImGui::TableSetColumnIndex(1);
        ImGui::PushID(i);
        const float     avail_w     = ImGui::GetContentRegionAvail().x;     //  within child -> respects scrollbar
        const float     w           = ImMax(s_cmap_preview_dims.x, avail_w * CMAP_PREVIEW_WIDTH_SCALE);
        
        
        //  if ( ImPlot::ColormapButton("##Preview", s_cmap_preview_dims, idx) ) {
        //      close = true;
        //  }
        //
        if ( ImPlot::ColormapButton(ImPlot::GetColormapName(idx), s_cmap_preview_dims, idx) ) {
            close = true;
        }
        if (row_height < 0.0f) {
            row_height = ImGui::GetItemRectSize().y;        // Use the rendered preview button height as the consistent row height
        }
        ImGui::PopID();

        //  Apply selection
        if ( close && !selected ) {
            this->m_cmap                    = idx;
            this->m_colormap_cache_invalid  = true;
        }
    }

    ImGui::EndTable();
    ImGui::EndChild();

    return !close; // keep open if nothing was chosen
}
*/



//
//
//
// *************************************************************************** //
// *************************************************************************** //   END [[ 3.  "MISC. GUI FUNCTIONS" ]].












// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
}//   END OF "cb" NAMESPACE.















// *************************************************************************** //
// *************************************************************************** //  END.
