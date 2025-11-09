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
    
    
    //      RE-ASSIGN COLORMAP COLORS IF USER HAS CHANGED COLOR-MAP SELECTION...
    if ( this->m_colormap_cache_invalid ) {
        this->_validate_colormap_cache();
    }


    //      2.      APPEARANCE TABLE ENTRY...
    utl::MakeCtrlTable(this->ms_APPEARANCE_ROWS, appearance_table_CFG);
    
    
    
    return;
}




//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "1.  MAIN ORCHESTRATORS".












// *************************************************************************** //
//
//
//
//      2.      OTHER INTERFACE FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

//  "_MENU_cmap_selection"
//
bool CCounterApp::_MENU_cmap_selection(const ImPlotColormap map) noexcept
{
    // Layout controls
    constexpr float                 CMAP_PREVIEW_WIDTH_SCALE    = 1.00f;    // 1.0 = fill remaining width
    constexpr float                 CMAP_PREVIEW_MIN_W          = 120.0f;   // clamp preview width
    constexpr float                 CMAP_POPUP_VIEW_H           = 260.0f;   // finite height for scrollable content
    constexpr ImGuiWindowFlags      CHILD_FLAGS                 = ImGuiWindowFlags_AlwaysVerticalScrollbar;

    constexpr ImGuiTableFlags       FLAGS                       = ImGuiTableFlags_SizingFixedFit;
    constexpr ImGuiSelectableFlags  SELECTABLE_FLAGS            = ImGuiSelectableFlags_SpanAllColumns;

    static float                    col_name_width              = -1.0f;   // cached
    static float                    row_height                  = 0.0f;    // cached after first preview draw


    //      Cache the widest name when the popup appears
    if ( ImGui::IsWindowAppearing() )
    {
        col_name_width      = 0.0f;
        const int count     = ImPlot::GetColormapCount();
        for (int i = 0; i < count; ++i) {
            const char *    nm      = ImPlot::GetColormapName(i);
            col_name_width          = ImMax(col_name_width, ImGui::CalcTextSize(nm ? nm : "").x);
        }
        col_name_width     *= 1.60f;        //  padding similar to the tool menu
        row_height          = 0.0f;         //  recalc on first draw
    }

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
                               , ImVec2(0.0f, (row_height > 0.0f)  ? row_height    : 0.0f)) )
        {
            close = true;
        }

        //      Column 1 :      Preview (gradient button)
        ImGui::TableSetColumnIndex(1);
        ImGui::PushID(i);
        const float     avail_w     = ImGui::GetContentRegionAvail().x;     //  within child -> respects scrollbar
        const float     w           = ImMax(CMAP_PREVIEW_MIN_W, avail_w * CMAP_PREVIEW_WIDTH_SCALE);
        if ( ImPlot::ColormapButton("##Preview", ImVec2(w, 0.0f), idx) ) {
            close = true;
        }
        if (row_height <= 0.0f) {
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



//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "2.  OTHER GUI FUNCTIONS".












// *************************************************************************** //
//
//
//
//      3.      TABBAR FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

//  "dispatch_plot_function"
//
void CCounterApp::dispatch_plot_function([[maybe_unused]] const std::string & uuid)
#ifdef DEF_REFACTOR_CC
//
{
    return;
}
//
# else
//
{
    bool                match       = false;
    const size_t        N           = ms_PLOT_TABS.size();
    size_t              idx         = N + 1;       //   Default:    if (NO MATCH):  "N < idx"

    //      1.      FIND THE INDEX AT WHICH THE TAB WITH NAME "uuid" IS FOUND...
    for (size_t i = 0; i < N && !match; ++i)
    {
        match           = ( uuid == ms_PLOT_TABS[i].uuid );
        if (match)      { idx = i; }
    }
    
    if (!match) return;
    
    
    //      DISPATCH EACH RENDER FUNCTION FOR EACH WINDOW OF THE APPLICATION...
    switch (idx)
    {
        //      0.  Model PARAMETERS...
        case 0:         {
            this->_Begin_IMPL();
            break;
        }
        //
        //
        //
        default: {
            break;
        }
    }
    
    return;
}
//
#endif  //  DEF_REFACTOR_CC  //




//  "dispatch_ctrl_function"
//
void CCounterApp::dispatch_ctrl_function(const std::string & uuid)
{
    bool            match       = false;
    const size_t    N           = ms_CTRL_TABS.size();
    size_t          idx         = N + 1;       //   Default:    if (NO MATCH):  "N < idx"

    //      1.      FIND THE INDEX AT WHICH THE TAB WITH NAME "uuid" IS FOUND...
    for (size_t i = 0; i < N && !match; ++i)
    {
        match = ( uuid == ms_CTRL_TABS[i].uuid );
        if (match)  { idx = i; }
    }
    if (!match) return;
    
    
    
    //      DISPATCH EACH RENDER FUNCTION FOR EACH WINDOW OF THE APPLICATION...
    switch (idx)
    {
        //      0.      INDIVIDUAL COUNTERS...
        case 0:         {
            _PlotSingles();
            break;
        }
        //
        //      1.      CCounter CONTROLS...
        case 1:         {
            this->TAB_Controls();
            break;
        }
        //
        //      2.      CCounter APPEARANCE SETTINGS...
        case 2:         {
            this->TAB_Appearance();
            break;
        }
        //
        //
        //      X.      DEFAULT SAFETY...
        default: {
            break;
        }
    }
    
    return;
}


//  "DefaultTabRenderFunc"
//
void CCounterApp::DefaultTabRenderFunc([[maybe_unused]] const char * uuid, [[maybe_unused]] bool * p_open, [[maybe_unused]] ImGuiWindowFlags flags) {
    if (!p_open)    { return; }
        
    ImGui::Text("Window Tab \"%s\".  Here is some default text dispatched by \"DefaultPlotTabRenderFunc()\".", uuid);
    return;
}


//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "3.  TABBAR FUNCTIONS".


















// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
}//   END OF "cb" NAMESPACE.















// *************************************************************************** //
// *************************************************************************** //  END.
