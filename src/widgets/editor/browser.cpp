/***********************************************************************************
*
*       *********************************************************************
*       ****            B R O W S E R . C P P  ____  F I L E             ****
*       *********************************************************************
*
*              AUTHOR:      Collin A. Bond.
*               DATED:      June 13, 2025.
*
**************************************************************************************
**************************************************************************************/
#include "widgets/editor/editor.h"



namespace cb {  //     BEGINNING NAMESPACE "cb"...
// *************************************************************************** //
// *************************************************************************** //



//  0.  STATIC / INTERAL LINKAGE TO THIS TU...
//
namespace {  //     BEGINNING NAMESPACE "cb"...
// *************************************************************************** //
// *************************************************************************** //


// Return true if a line already connects point indices a and b (order‑agnostic).
[[maybe_unused]]
static bool line_exists(const std::vector<Editor::Line>& lines, size_t a, size_t b)
{
    for (const Editor::Line& ln : lines)
        if ((ln.a == a && ln.b == b) || (ln.a == b && ln.b == a))
            return true;
    return false;
}

// Disconnect every edge that references the given point index.
[[maybe_unused]]
static void detach_point(std::vector<Editor::Line>& lines, size_t pi)
{
    lines.erase(std::remove_if(lines.begin(), lines.end(),
                               [pi](const Editor::Line& ln){ return ln.a == pi || ln.b == pi; }),
                 lines.end());
}

// Return index into m_points for a given vertex‑id (or -1 if not found)
[[maybe_unused]]
static int point_index_from_vid(const std::vector<Editor::Point>& pts, uint32_t vid)
{
    for (int i = 0; i < static_cast<int>(pts.size()); ++i)
        if (pts[i].v == vid) return i;
    return -1;
}



// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
}//   END OF "internal" NAMESPACE.












// *************************************************************************** //
//
//
//
//      0.      PUBLIC API FOR DRAWING BROWSER WINDOW...
// *************************************************************************** //
// *************************************************************************** //

//-------------------------------------------------------------------------
// PUBLIC FACADE – minimal wrapper to draw both columns side‑by‑side
//-------------------------------------------------------------------------

//  "DrawBrowser"
//      Main function to draw the entire "Browser" feature of the Editor app.
//      > This function is called by the client-code / caller-routiene so they have
//      > flexibility to customize which window they want to host the Browser inside, and etc.
//
void Editor::DrawBrowser(void)
{
    BrowserStyle &              BStyle                          = this->m_style.browser_style;
    //
    const bool                  allow_resize                    = ( ImGui::GetCurrentWindow()->DockTabIsVisible );
                                BStyle.DYNAMIC_CHILD_FLAGS      = (allow_resize)
                                                                    ? (BStyle.DYNAMIC_CHILD_FLAGS  | ImGuiChildFlags_ResizeX)
                                                                    : (BStyle.DYNAMIC_CHILD_FLAGS  & ~ImGuiChildFlags_ResizeX);
                          
#ifdef __CBAPP_DEBUG__
    //  this->_show_browser_color_edit_window();
#endif  //  __CBAPP_DEBUG__
    
    
    S.PushFont(Font::Small);
    
    
    
    //  2.  LEFTHAND BROWSER SELECTION MENU...
    ImGui::PushStyleVar     (ImGuiStyleVar_ChildBorderSize,     BStyle.ms_CHILD_BORDER1         );
    ImGui::PushStyleVar     (ImGuiStyleVar_ChildRounding,       BStyle.ms_CHILD_ROUND1          );
    ImGui::PushStyleColor   (ImGuiCol_ChildBg,                  BStyle.ms_CHILD_FRAME_BG1L      );
    //
    //
    //
        //  2.  OBJECT BROWSER'S SELECTOR COLUMN    (MAIN, LEFT-HAND MENU OF THE ENTIRE BROWSER)...
        ImGui::SetNextWindowSizeConstraints( BStyle.OBJ_SELECTOR_DIMS.limits.min, BStyle.OBJ_SELECTOR_DIMS.limits.max );
        ImGui::BeginChild("##Editor_Browser_ObjSelector", BStyle.OBJ_SELECTOR_DIMS.value, BStyle.DYNAMIC_CHILD_FLAGS);
            //
            S.PushFont(Font::Main);
                this->_draw_obj_selector_table();
            S.PopFont();
            //
            BStyle.OBJ_SELECTOR_DIMS.value.x     = ImGui::GetItemRectSize().x;
        ImGui::EndChild();
        ImGui::PopStyleColor();
        //
        //
        ImGui::SameLine();
        //
        //
        //  3.  OBJECT BROWSER'S INSPECTOR COLUMN   (MAIN, RIGHT-HAND MENU OF THE ENTIRE BROWSER)...
        ImGui::PushStyleColor(ImGuiCol_ChildBg,         BStyle.ms_CHILD_FRAME_BG1R);
        ImGui::BeginChild("##Editor_Browser_ObjInspector", ImVec2(0, 0), BStyle.STATIC_CHILD_FLAGS);
            _dispatch_obj_inspector_column();
        ImGui::EndChild();
    //
    //
    //
    ImGui::PopStyleColor();
    ImGui::PopStyleVar(2);  //  ImGuiStyleVar_ChildBorderSize, ImGuiStyleVar_ChildRounding
    
    
    S.PopFont();
    return;
}

//
//
// *************************************************************************** //
// *************************************************************************** //   END "PUBLIC BROWSER API".












// *************************************************************************** //
//
//
//
//      1.      MAIN INTERFACE FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

//  "_MECH_draw_controls"           formerly named:     "_draw_controls".
//
void Editor::_MECH_draw_controls(void)
{
    static constexpr const char *       uuid                = "##Editor_Controls_Columns";
    static constexpr int                ms_NC               = 10;
    static constexpr int                ms_NE               = 1;
    //
    static ImGuiOldColumnFlags          COLUMN_FLAGS        = ImGuiOldColumnFlags_None;
    const ImVec2                        WIDGET_SIZE         = ImVec2( -1,               ImGui::GetFrameHeight()               );
    static ImVec2                       BUTTON_SIZE         = ImVec2( 32,   WIDGET_SIZE.y );
    //
    constexpr ImGuiButtonFlags          BUTTON_FLAGS        = ImGuiOldColumnFlags_NoPreserveWidths;
    int                                 mode_i              = static_cast<int>(m_mode);
    
    this->S.PushFont( Font::Small );
   
    
    
   
   
    //  BEGIN COLUMNS...
    //
    ImGui::Columns(ms_NC, uuid, COLUMN_FLAGS);
    //
    //
    //
        //      1.      EDITOR STATE...
        this->S.column_label("State:");
        //
        ImGui::SetNextItemWidth( WIDGET_SIZE.x );
        if ( ImGui::Combo("##Editor_Controls_EditorState",      &mode_i,
                          ms_EDITOR_STATE_NAMES.data(),         static_cast<int>(Mode::COUNT)) )
        {
            m_mode = static_cast<Mode>(mode_i);
        }
        
        
        
        //      2.      SELECTION STATE...
        ImGui::NextColumn();        this->S.column_label("Selection State:");
        //
        //
        //
        static bool     custom_sel_state_available  = true;
        static bool     ss_surfaces                 = false;
        static bool     ss_edges                    = false;
        static bool     ss_vertices                 = false;
        //
        ImGui::BeginDisabled( !custom_sel_state_available );
        //
                //      2.1.        "SURFACE".
                //  if ( utl::IconButton( (ss_surfaces)  ? "vis_on"      : "vis_off",
                //                        (ss_surfaces)  ? ICON_FA_EYE   : ICON_FA_EYE_SLASH ) )
                //  {
                //      ss_surfaces = !ss_surfaces;
                //  }
                
            const ImU32 c0 = ImGui::GetColorU32(ImGuiCol_Text);
            const ImU32 c1 = ImGui::GetColorU32(ImGuiCol_TextDisabled);  // subtle hover alt
            const ImU32 c2 = ImGui::GetColorU32(ImGuiCol_Text);          // or same as c0
            //
            if ( utl::IconFlatButton( "vis", ICON_FA_EYE, c0, c1, c2 ) )
            {
                ss_surfaces = !ss_surfaces;
            }
            
            
            
            ImGui::PushItemWidth( BUTTON_SIZE.x );
            //
            {
                
                //
                //
                    //  if ( !ss_surfaces ) {
                    //      if ( utl::CButton("S",     this->S.SystemColor.Red     ) )
                    //          { ss_surfaces = !ss_surfaces; }
                    //  }
                    //  else {
                    //      if ( utl::CButton("S",      this->S.SystemColor.Green   ) )
                    //          { ss_surfaces = !ss_surfaces; }
                    //  }
                //
                //
                //      2.2.        "EDGE".
                ImGui::SameLine(0.0f, 0.0f);
                if ( !ss_edges ) {
                    if ( utl::CButton("E",     this->S.SystemColor.Red     ) )
                        { ss_edges = !ss_edges; }
                }
                else {
                    if ( utl::CButton("E",      this->S.SystemColor.Green   ) )
                        { ss_edges = !ss_edges; }
                }
                //
                //
                //      2.3.        "VERTEX".
                ImGui::SameLine(0.0f, 0.0f);
                if ( !ss_vertices ) {
                    if ( utl::CButton("V",     this->S.SystemColor.Red     ) )
                        { ss_vertices = !ss_vertices; }
                }
                else {
                    if ( utl::CButton("V",      this->S.SystemColor.Green   ) )
                        { ss_vertices = !ss_vertices; }
                }
            }
            //
            ImGui::PopItemWidth();
        //
        ImGui::EndDisabled();
    
    
    
        //      3.      GRID VISIBILITY...
        ImGui::NextColumn();        this->S.column_label("Show Grid:");
        //
        ImGui::SetNextItemWidth( BUTTON_SIZE.x );
        ImGui::Checkbox("##Editor_Controls_ShowGrid",           &m_grid.visible);



        //      4.      SNAP-TO-GRID...
        ImGui::NextColumn();        this->S.column_label("Snap-To-Grid:");
        //
        ImGui::SetNextItemWidth( BUTTON_SIZE.x );
        ImGui::Checkbox("##Editor_Controls_SnapToGrid",         &m_grid.snap_on);
        
        
        
        //      5.      GRID-LINE DENSITY...
        ImGui::NextColumn();        this->S.column_label("Grid Density:");
        //
        //
        //
        if ( ImGui::ArrowButtonEx( "##Editor_Controls_GridDensityDown",     ImGuiDir_Down,
                                   BUTTON_SIZE,                             BUTTON_FLAGS ) )
        {
            m_grid.snap_step *= 2.f;
        }
        //
        ImGui::SameLine(0.0f, 0.0f);
        //
        if ( ImGui::ArrowButtonEx("##Editor_Controls_GridDensityUp",        ImGuiDir_Up,
                          BUTTON_SIZE,                                      BUTTON_FLAGS) )
        {
            m_grid.snap_step = std::max(ms_GRID_STEP_MIN, m_grid.snap_step * 0.5f);
        }
        //
        ImGui::SameLine();
        //
        ImGui::Text("(%.1f)", m_grid.snap_step);
        



        //      6.      CLEAR ALL...
        ImGui::NextColumn();        this->S.column_label("Clear Data:");
        if ( ImGui::Button("Clear", WIDGET_SIZE) ) {
            ui::ask_ok_cancel( "Clear Data",
                               "This action will erase all unsaved data in the current session.\nDo you wish to proceed?",
                               [this]{ _clear_all(); } );
        }





        //      ?.      EMPTY SPACES FOR LATER...
        for (int i = ImGui::GetColumnIndex(); i < ms_NC - ms_NE; ++i) {
            ImGui::Dummy( ImVec2(0,0) );    ImGui::NextColumn();
        }



        //      X.      CANVAS SETTINGS...
        this->S.column_label("Settings:");
        if ( ImGui::Button("+", BUTTON_SIZE) ) {
            ui::open_preferences_popup( GetMenuID(PopupHandle::Settings), [this](popup::Context & ctx) { _draw_editor_settings(ctx); } );
        }
        
        
        
    //  popup::Draw();
    //
    //
    //
    ImGui::Columns(1);      //  END COLUMNS...
    
    
    this->S.PopFont();
   
    return;
}

//
//
// *************************************************************************** //
// *************************************************************************** //   END "MAIN INTERFACE".






// *************************************************************************** //
//
//
//
//      2.      MAIN BROWSER ORCHESTRATOR FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

//  "_dispatch_obj_inspector_column"
//      Simple wrapper that allows to pass a default argument WITHOUT having it declared static.
//
void Editor::_dispatch_obj_inspector_column(void)
    { return _dispatch_obj_inspector_column(this->m_trait_browser); }


//  "_dispatch_obj_inspector_column"
//
void Editor::_dispatch_obj_inspector_column(ObjectTrait & which_menu)
{
    BrowserStyle &                  BStyle              = this->m_style.browser_style;
    
    
    
    //      "DRAW 2A".      DRAW THE TRAIT-SELECTOR...
    ImGui::SetNextWindowSizeConstraints(    BStyle.TRAIT_SELECTOR_DIMS.limits.min,    BStyle.TRAIT_SELECTOR_DIMS.limits.max );
    ImGui::BeginChild("##Editor_Browser_TraitSelector", BStyle.OBJ_SELECTOR_DIMS.value,     BStyle.DYNAMIC_CHILD_FLAGS);
    //
        _draw_trait_selector(which_menu);
        BStyle.TRAIT_SELECTOR_DIMS.value.x       = ImGui::GetItemRectSize().x;
    //
    ImGui::EndChild();
    //
    //
    //
    ImGui::SameLine( 0.0f, BStyle.ms_CHILD_WINDOW_SAMELINE );
    //
    //
    //
    //      "DRAW 2B".      DRAW THE TRAIT-INSPECTOR PANEL...
    const float                     P1_w                = ImGui::GetContentRegionAvail().x;
    ImGui::BeginChild("##Editor_Browser_TraitInspector",    {P1_w, 0.0f},     BStyle.STATIC_CHILD_FLAGS);
    //
        _dispatch_trait_inspector( which_menu, this->S.ms_LeftLabel );
    //
    ImGui::EndChild();
    
    
    
    return;
}




//
//
// *************************************************************************** //
// *************************************************************************** //   END "BROWSER ORCHESTRATORS".












// *************************************************************************** //
//
//
//
//      3.      MAIN OBJECT-SELECTION STUFF...
// *************************************************************************** //
// *************************************************************************** //

//  "_draw_obj_selector_table"
//
void Editor::_draw_obj_selector_table(void)
{
    using                                   namespace           icon;
    constexpr ImGuiTableFlags               TABLE_FLAGS         = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_ScrollY; // | ImGuiTableFlags_RowBg;
    constexpr ImGuiTableColumnFlags         C_EYE               = ImGuiTableColumnFlags_NoHeaderLabel | ImGuiTableColumnFlags_WidthFixed;
    constexpr ImGuiTableColumnFlags         C_LOCK              = ImGuiTableColumnFlags_NoHeaderLabel | ImGuiTableColumnFlags_WidthFixed;
    constexpr ImGuiTableColumnFlags         C_NAME              = ImGuiTableColumnFlags_WidthStretch;
    constexpr ImGuiTableColumnFlags         C_DEL               = ImGuiTableColumnFlags_NoHeaderLabel | ImGuiTableColumnFlags_WidthFixed;
    //
    //
    constexpr const float                   SHADE               = 0.20f;
    static const ImU32                      col_text            = ImGui::GetColorU32(ImGuiCol_Text);
    static const ImU32                      col_text_uh         = cblib::utl::compute_shade(col_text, SHADE);    //  "_uh"   == "un-hovered".
    static const ImU32                      col_dim             = ImGui::GetColorU32(ImGuiCol_TextDisabled);
    static const ImU32                      col_dim_uh          = cblib::utl::compute_shade(col_dim, SHADE);    //  "_uh"   == "un-hovered".
    //
    //
    BrowserStyle &                          BStyle              = m_style.browser_style;
    BrowserState &                          BS                  = m_browser_S;
    ImDrawList *                            dl                  = ImGui::GetWindowDrawList();
    ImGuiListClipper                        clipper;


    //      1.      SEARCH-QUERY BOX...
    S.PushFont(Font::Main);
    ImGui::BeginDisabled(true);
    //
        ImGui::SetNextItemWidth(-FLT_MIN);
        if ( ImGui::InputTextWithHint( "##Editor_ObjSelector_ObjFilter",
                                       "filter",
                                       BS.m_obj_filter.InputBuf,
                                       IM_ARRAYSIZE( BS.m_obj_filter.InputBuf ) ) )
        {
            BS.m_obj_filter.Build();
            BS.m_obj_filter_dirty   = true;
        }
    //
    ImGui::EndDisabled();
        
    S.PopFont();
    ImGui::Separator();



    //      1.2.    RE-BUILD LIST OF SORTED INDICES...
    //  if ( BS.m_obj_filter_dirty || BS.m_obj_rows_paths_rev != m_paths_rev )
    //  {
    //      BS.m_obj_rows.clear();
    //      BS.m_obj_rows.reserve( m_paths.size() );
    //
    //      for (int i = 0; i < (int)m_paths.size(); ++i)
    //      {
    //          if ( BS.m_obj_filter.PassFilter(m_paths[i].label.c_str()) )
    //              { BS.m_obj_rows.push_back(i); }
    //      }
    //
    //      BS.m_obj_rows_paths_rev     = m_paths_rev;
    //      BS.m_obj_filter_dirty       = false;
    //  }



    (void)this->_ensure_paths_sorted_by_z_desc();
    

    //      2.      BEGIN THE TABLE TO PRESENT EACH OBJECT...
    if ( ImGui::BeginTable("##Editor_ObjSelector_ObjTable", 4, TABLE_FLAGS, ImVec2(0, -1)) )
    {
        ImGui::TableSetupColumn("Eye",      C_EYE,    CELL_SZ       );
        ImGui::TableSetupColumn("Lock",     C_LOCK,   CELL_SZ       );
        ImGui::TableSetupColumn("Name",     C_NAME                  );
        ImGui::TableSetupColumn("Del",      C_DEL,    CELL_SZ       );

        clipper.Begin( static_cast<int>(m_paths.size()), -1 );


        //          3.      DRAWING EACH OBJECT IN THE LEFT-HAND SELECTION COLUMN OF THE BROWSER...
        while ( clipper.Step() )
        {
            for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; ++i)
            {
                Path &                  path                = m_paths[i];
                bool                    selected            = m_sel.paths.count(static_cast<size_t>(i));
                bool                    mutable_path        = path.IsMutable();

                //  CASE 0 :    EARLY-OUT IF FILTER REMOVES OBJ.
                if ( !BS.m_obj_filter.PassFilter(path.label.c_str()) )      { continue; }


                //      4.      BEGIN THE ROW...
                ImGui::PushID(i);       //  ImGui::BeginGroup();
                ImGui::TableNextRow();
                //
                //
                //
                //  //      4.1.        "EYE" BUTTON (TO TOGGLE OBJECT'S VISIBILITY).
                    ImGui::TableSetColumnIndex(0);
                    {
                        ImGui::InvisibleButton("##Editor_Browser_ObjVisibilityButton", {CELL_SZ, CELL_SZ});
                        if ( ImGui::IsItemClicked() )   { path.visible = !path.visible; _prune_selection_mutability(); }
                        //
                        const bool hovered = ImGui::IsItemHovered();
                        //
                        ( (path.visible) ? draw_eye_icon : draw_eye_off_icon )
                            ( dl,
                              ImGui::GetItemRectMin(),
                              { CELL_SZ, CELL_SZ },
                              ( path.visible )
                                ? ( (hovered)   ? col_text  : col_text_uh       )
                                : ( (hovered)   ? col_dim   : col_dim_uh        )
                        );
                    }
                    //
                    //
                    //
                    //      4.2.        "LOCK" BUTTON (TO TOGGLE OBJECT'S LOCKED-STATE).
                    ImGui::TableSetColumnIndex(1);
                    {
                        ImGui::InvisibleButton("##Editor_Browser_ObjLockButton", {CELL_SZ, CELL_SZ});
                        if ( ImGui::IsItemClicked() )   { path.locked = !path.locked; _prune_selection_mutability(); }
                        //
                        const bool hovered = ImGui::IsItemHovered();
                        //
                        ( (path.locked) ? draw_lock_icon : draw_unlock_icon )
                            ( dl,
                              ImGui::GetItemRectMin(),
                              { CELL_SZ, CELL_SZ },
                              ( path.locked )
                                ? ( (hovered)   ? col_text  : col_text_uh       )
                                : ( (hovered)   ? col_dim   : col_dim_uh        )
                        );
                    }
                    //
                    //
                    //
                    //      4.3.        DRAW THE SELECTABLE FOR THIS OBJECT.
                    ImGui::TableSetColumnIndex(2);
                    this->_draw_obj_selectable( path, i, mutable_path, selected );
                    //
                    //
                    //
                    //      4.4.        DRAG / DROP TARGETS FOR RE-ORDERING EACH ROW.
                    //                      Make the *Selectable item itself* the drag source & drop target.
                    //                      Skip when this row is in rename mode so typing doesn't start drags.
                    if ( m_browser_S.m_obj_rename_idx != i )
                    {
                        //      DRAG-DROP SOURCE :      Only when user actually drags (prevents double-click conflicts).
                        if ( ImGui::IsItemActive() &&
                             ImGui::IsMouseDragging(ImGuiMouseButton_Left, 2.0f) &&
                             ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceNoPreviewTooltip) )
                        {
                            int src = i;
                            ImGui::SetDragDropPayload( "OBJ_ROW", &src, sizeof(src) );
                            ImGui::EndDragDropSource();
                        }
                        //
                        //      DRAG-DROP TARGET :      Same cell accepts the row payload.
                        if ( ImGui::BeginDragDropTarget() )
                        {
                            if ( const ImGuiPayload * p = ImGui::AcceptDragDropPayload("OBJ_ROW") )
                            {
                                int src = *static_cast<const int*>(p->Data);
                                _reorder_paths(src, i);

                                //  IMPORTANT:          //  we mutated m_paths; close scopes and return immediately.
                                ImGui::PopID();         //  pop the row PushID(i)
                                clipper.End();
                                ImGui::EndTable();
                                return;
                            }
                            ImGui::EndDragDropTarget();
                        }
                    }
                    //
                    //
                    //
                    //      4.5.        "DELETE" BUTTON.
                    ImGui::TableSetColumnIndex(3);
                    if ( !path.locked && selected )
                    {
                        //  utl::SmallCButton( BStyle.ms_DELETE_BUTTON_HANDLE, BStyle.ms_DELETE_BUTTON_COLOR );
                        //  utl::CButton( BStyle.ms_DELETE_BUTTON_HANDLE, BStyle.ms_DELETE_BUTTON_COLOR, {CELL_SZ, CELL_SZ} );
                        //
                        //
                        //
                        ImGui::InvisibleButton("##Editor_Browser_ObjDeleteButton", {CELL_SZ, CELL_SZ});
                        const bool hovered = ImGui::IsItemHovered();
                        draw_icon_background(
                            dl,
                            ImGui::GetItemRectMin(),
                            {CELL_SZ, CELL_SZ},
                            ( hovered )
                                ? (BStyle.ms_DELETE_BUTTON_COLOR | 0xFF000000u) : BStyle.ms_DELETE_BUTTON_COLOR,
                            8.0f * icon::BG_ROUNDING
                        );
                        //
                        //
                        if ( ImGui::IsItemClicked() )
                        {
                            _erase_path_and_orphans(static_cast<PathID>(i));
                            reset_selection();
                            BS.m_inspector_vertex_idx = -1;
                            _prune_selection_mutability();

                            ImGui::PopID();
                            clipper.End();
                            ImGui::EndTable();
                            return;
                        }
                    }
                //
                //
                //  END OF ROW.
                
                
                ImGui::PopID();
            //
            //
            }// END "ROW for-loop".
        //
        //
        }// END "while-loop" [CLIPPER]


        clipper.End();
        ImGui::EndTable();
    //
    //
    //
    }// END ALL.
    
    
    
    return;
}


//  "_draw_obj_selectable"
//
inline void Editor::_draw_obj_selectable( Path & path, const int idx, const bool mutable_path, const bool selected )
{
    using                                   namespace           icon;
    static constexpr ImGuiInputTextFlags    TEXT_FLAGS          = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll;
    const ImGuiSelectableFlags              SEL_FLAGS           = ( mutable_path )
                                                                    ? ( ImGuiSelectableFlags_AllowDoubleClick       )
                                                                    : ( ImGuiSelectableFlags_Disabled               );
    //
    const ImU32                             col_text            = ImGui::GetColorU32(ImGuiCol_Text);
    const ImU32                             col_dim             = ImGui::GetColorU32(ImGuiCol_TextDisabled);
    //
    //
    //
    BrowserState &                          BS                  = m_browser_S;
    const bool                              renaming            = (BS.m_obj_rename_idx      == idx);
    const bool                              canvas_hovered      = (BS.m_hovered_canvas_obj  == idx);
    
    
    
    //  CASE 1 :    NORMAL "SELECTABLE" BEHAVIOR    [ NOT RENAMING ]...
    if ( !renaming )
    {
    
    
        //      1.1.    CREATE SELECTABLE.
        ImGui::PushStyleColor(ImGuiCol_Text, mutable_path ? col_text : col_dim);
        //
        //
            const bool      ctrl                = ImGui::GetIO().KeyCtrl;
            const bool      shift               = ImGui::GetIO().KeyShift;
            bool            single_click        = false;
            //
            //
            if ( canvas_hovered ) [[unlikely]]
            {
                ImGui::PushStyleColor( ImGuiCol_HeaderActive, ImGui::GetColorU32(ImGuiCol_HeaderHovered) );
                    single_click = ImGui::Selectable(path.label.c_str(), selected, SEL_FLAGS, {0.0f, 1.05f * CELL_SZ});
                ImGui::PopStyleColor(1);
            }
            else
            { single_click = ImGui::Selectable(path.label.c_str(), selected, SEL_FLAGS, {0.0f, 1.05f * CELL_SZ}); }
            //
            //
            const bool      hovered         = ImGui::IsItemHovered(ImGuiHoveredFlags_None);
            const bool      double_click    = mutable_path
                                                && hovered
                                                && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)
                                                && !ctrl
                                                && !shift;
        //
        //
        ImGui::PopStyleColor();
    
        
        
        //      1.1A.   ADD TO BROWSER-HOVER CACHE.
        if ( mutable_path  &&  hovered )
        {
            BS.m_hovered_obj    = idx;
        }
                
    
        //      1.2.    DOUBLE CLICKED.
        if ( double_click )
        {
            BS.m_obj_rename_idx     = idx;
            std::strncpy( BS.m_name_buffer, path.label.c_str(), BrowserState::ms_MAX_PATH_TITLE_LENGTH );
                          
            BS.m_name_buffer[ BrowserState::ms_MAX_PATH_TITLE_LENGTH - 1 ] = '\0';
            BS.m_renaming_obj       = true;
            return;
        //
        }// END "DOUBLE CLICK".
    
    
        //      1.3.    SINGLE CLICKED.
        if ( single_click  &&  !renaming )
        {
            if ( !ctrl && !shift ) {
                reset_selection();
                if ( mutable_path )     { m_sel.paths.insert(idx); }
                BS.m_browser_anchor = idx;
            }
            //
            else if ( shift && BS.m_browser_anchor >= 0 )
            {
                int     lo      = std::min(BS.m_browser_anchor, idx);
                int     hi      = std::max(BS.m_browser_anchor, idx);
                
                if ( !ctrl )            { reset_selection(); }
                
                for (int k = lo; k <= hi; ++k) {
                    if ( m_paths[k].IsMutable() )       { m_sel.paths.insert(k); }
                }
            }
            //
            else if ( ctrl && mutable_path )
            {
                if ( !m_sel.paths.erase(idx) )            { m_sel.paths.insert(idx); }
                BS.m_browser_anchor = idx;
            }
            
            _rebuild_vertex_selection();
            
        //
        }// END "SINGLE CLICK".
    
        return;
        
    //
    //
    }// END "NORMAL SELECTABLE".
    


    //  CASE 2 :    NORMAL "SELECTABLE" BEHAVIOR...
    //
    {
        BS.m_renaming_obj                               = true;
        static int              last_rename_idx         = -1;
        
        
        //      2.1.    SET KEYBOARD FOCUS ON FIRST ENTRY IN ROW.
        if ( last_rename_idx != idx ) {
            ImGui::SetKeyboardFocusHere();
            last_rename_idx = idx;
        }
        //
        //
        //      2.2.    CREATE RE-NAME WIDGET AND LISTEN FOR USER ENTER/CANCEL SIGNALS.
        ImGui::PushItemWidth(-FLT_MIN);
            const bool      pressed_enter   = ImGui::InputText( "##Editor_ObjSelector_RenameObj", BS.m_name_buffer, BrowserState::ms_MAX_PATH_TITLE_LENGTH, TEXT_FLAGS );
            const bool      pressed_esc     = ImGui::IsKeyPressed(ImGuiKey_Escape);
            const bool      lost_focus      = (!ImGui::IsItemActive() && ImGui::IsItemDeactivated());
        ImGui::PopItemWidth();
        //
        //
        //      2.3A.   COMMIT CHANGES TO THE NAME.
        if ( pressed_enter || (lost_focus && !pressed_esc) ) {
            path.label              = BS.m_name_buffer;   // commit
            BS.m_obj_rename_idx     = -1;
            BS.m_renaming_obj       = false;
            last_rename_idx         = -1;
            return;
        }
        //
        //
        //      2.3B.   DISCARD CHANGES AND REVERT TO PREVIOUS NAME.
        if ( pressed_esc ) {
            BS.m_obj_rename_idx     = -1;
            BS.m_renaming_obj       = false;
            last_rename_idx         = -1;
            return;
        }
        
    //
    //
    }// END "RENAMING".

    
    return;
}

//
//
// *************************************************************************** //
// *************************************************************************** //   END "OBJ SELECTION".












// *************************************************************************** //
//
//
//
//      3.      FUNCTIONS FOR "TRAITS" OF BROWSER...
// *************************************************************************** //
// *************************************************************************** //

//  "_draw_trait_selector"
//
inline void Editor::_draw_trait_selector(ObjectTrait & which_menu)
{
    using                                       namespace               icon;
    static constexpr ImGuiTableFlags            TABLE_FLAGS             = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_ScrollY | ImGuiTableFlags_RowBg;
    static constexpr ImGuiTableColumnFlags      C1_FLAGS                = ImGuiTableColumnFlags_NoHeaderLabel | ImGuiTableColumnFlags_WidthStretch;
    static constexpr ImGuiSelectableFlags       SELECTABLE_FLAGS        = ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowDoubleClick;
    //
    //
    ImGuiListClipper                            clipper;
        

    this->S.PushFont(Font::Main);
    

    //      1.      BEGIN THE TABLE TO PRESENT EACH OBJECT...
    if ( ImGui::BeginTable("##Editor_TraitSelector_TraitTable", 1, TABLE_FLAGS, ImVec2(0, -1)) )
    {
        ImGui::TableSetupColumn ("Trait",         C1_FLAGS);



        clipper.Begin( static_cast<int>(ObjectTrait::COUNT), -1 );

        //      2.      DRAWING EACH OBJECT IN THE LEFT-HAND SELECTION COLUMN OF THE BROWSER...
        while ( clipper.Step() )
        {
            for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; ++i)
            {
                const ObjectTrait       trait           = static_cast<ObjectTrait>( i );
                const bool              selected        = ( which_menu == trait );
                
                
                //      3.      BEGIN THE ROW...
                ImGui::PushID(i);
                ImGui::TableNextRow();


                
                //      3.1.        DRAW THE NAME OF EACH TRAIT...
                ImGui::TableSetColumnIndex(0);
                if ( ImGui::Selectable( ms_OBJECT_TRAIT_NAMES[ trait ], selected, SELECTABLE_FLAGS, {0.0f, 1.05f * CELL_SZ}) )
                {
                    which_menu = trait;      //  Set the current trait to the selection.
                }
                
                

                ImGui::PopID();
            }// END "for-loop" [ROWS]
            
        }// END "while-loop" [CLIPPER]
        
        clipper.End();
        ImGui::EndTable();
    }
    
    
    this->S.PopFont();
    return;
}


//  "_dispatch_trait_inspector"
//
void Editor::_dispatch_trait_inspector(ObjectTrait & which_menu, const LabelFn & callback)
{
    const size_t                    N_paths             = this->m_sel.paths.size();
    const bool                      no_selection        = (N_paths == 0);
    const bool                      single              = (N_paths == 1);

    //  CASE 0 :    EMPTY SELECTION...
    if ( no_selection )
    {
        //this->S.HeadlineSeparatorText( ms_OBJECT_TRAIT_NAMES[ this->m_trait ] );
            ImGui::TextDisabled("No selection...");
        return;
    }



    if (single)                     { this->_dispatch_trait_inspector_single    (which_menu, callback);         }
    else                            { this->_dispatch_trait_inspector_multi     (which_menu, callback);         }

    return;
}


//  "_dispatch_trait_inspector_single"
//
inline void Editor::_dispatch_trait_inspector_single(ObjectTrait & which_menu, const LabelFn & callback)
{
    IM_ASSERT( !(this->m_sel.paths.size() != 1ULL)       && "single trait inspector called with selection != one" );

    const size_t                    sel_idx             = *m_sel.paths.begin();   // only element
    Path &                          path                = m_paths[sel_idx];
    
    
    
    
    //      DISPATCH THE APPROPRIATE PANEL DEPENDING ON WHICH "OBJECT-TRAIT" THE BROWSER IS ON...
    switch (which_menu)
    {
        case ObjectTrait::Vertices      : {         //  TRAIT #2:   VERTEX-EDITOR.
            _draw_vertex_panel(path, sel_idx, callback);
            break;
        }
        
        case ObjectTrait::Payload       : {         //  TRAIT #3:   OBJECT-PAYLOAD.
            _draw_payload_panel(path, sel_idx, callback);
            break;
        }
        
        default                         : {         //  TRAIT #1:   DEFAULT PROPERTIES.
            _draw_properties_panel_single(path, sel_idx, callback);
            break;
        }
    }
    
    
    
    
    return;
}


//  "_dispatch_trait_inspector_multi"
//
inline void Editor::_dispatch_trait_inspector_multi(ObjectTrait & which_menu, const LabelFn & callback)
{
    IM_ASSERT( !(this->m_sel.paths.size() < 1ULL)       && "multi trait inspector called with selection not greater-than one" );
    
    
    
    //      DISPATCH THE APPROPRIATE PANEL DEPENDING ON WHICH "OBJECT-TRAIT" THE BROWSER IS ON...
    switch (which_menu)
    {
        
        case ObjectTrait::Vertices      : {         //  TRAIT #1:   "PROPERTIES".
            _draw_properties_panel_multi(callback);
            break;
        }
        
        default                         : {         //  REMAINING TRAITS ARE DISABLED FOR MULTI-SELECTION.
            ImGui::TextDisabled("Not implemented...");
            break;
        }
    }
    
    
    
    
    return;
}

//
//
// *************************************************************************** //
// *************************************************************************** //   END "TRAIT FUNCTIONS".












// *************************************************************************** //
//
//
//
//      X.      LEFTOVER ARTIFACTS      [ TO-BE-REMOVED SOON ]...
// *************************************************************************** //
// *************************************************************************** //











































// *************************************************************************** //
//
//
//
//      TEMPORARY STUFF...
// *************************************************************************** //
// *************************************************************************** //

//  "_show_browser_color_edit_window"
//
void Editor::_show_browser_color_edit_window(void)
{
    const float                     LABEL_W             = 1.8 * m_style.ms_SETTINGS_LABEL_WIDTH;
    const float &                   WIDGET_W            = m_style.ms_SETTINGS_WIDGET_WIDTH;
    constexpr ImGuiColorEditFlags   COLOR_FLAGS         = ImGuiColorEditFlags_NoInputs;
    //
    BrowserStyle &                  BStyle              = this->m_style.browser_style;
    //
    //
    //
    auto                            CopyToClipboard     = [](const ImVec4 & c)
    {
        static int id = 1;
        
        ImGui::PushID(id++);
        char    buf[96];
        
        ImGui::SameLine();
        if ( ImGui::SmallButton("copy") )
        {
            std::snprintf(buf, sizeof(buf),
                          "ImVec4( %.3ff, %.3ff, %.3ff, %.3ff);",
                          c.x, c.y, c.z, c.w);
            ImGui::SetClipboardText(buf);
        }
        ImGui::PopID();
        return;
    };
    
    
    S.PushFont(Font::Small);
    ImGui::Begin("browser color edit window", nullptr, ImGuiWindowFlags_None);
    //
    //
    //
        this->S.labelf("[CUSTOM 1]:",     LABEL_W, WIDGET_W);
        ui::CustomColorEdit4( "##CUSTOM 1", BStyle.ms_CHILD_FRAME_BG1 );
        //
        CopyToClipboard(BStyle.ms_CHILD_FRAME_BG1);
        
        
        this->S.labelf("[CUSTOM 2]:",     LABEL_W, WIDGET_W);
        ui::HexColor4( "##CUSTOM 2", BStyle.ms_CHILD_FRAME_BG1 );
        //
        CopyToClipboard(BStyle.ms_CHILD_FRAME_BG1);
        
        
        
        ImGui::NewLine();
        /*
        this->S.labelf("ImGuiColorEditFlags_None:", LABEL_W, WIDGET_W);
        ImGui::ColorEdit4( "##ImGuiColorEditFlags_None",     (float*)&BStyle.ms_CHILD_FRAME_BG1,  ImGuiColorEditFlags_None );
        
        this->S.labelf("None + Float:", LABEL_W, WIDGET_W);
        ImGui::ColorEdit4( "##None + Float",     (float*)&BStyle.ms_CHILD_FRAME_BG1,  ImGuiColorEditFlags_None | ImGuiColorEditFlags_Float );
        
        this->S.labelf("None + Float + Hex:", LABEL_W, WIDGET_W);
        ImGui::ColorEdit4( "##None + Float + Hex",     (float*)&BStyle.ms_CHILD_FRAME_BG1,  ImGuiColorEditFlags_None | ImGuiColorEditFlags_Float | ImGuiColorEditFlags_DisplayHex );
        
        this->S.labelf("None + Float + Hex + NoLabel:", LABEL_W, WIDGET_W);
        ImGui::ColorEdit4( "##None + Float + Hex + NoLabel",     (float*)&BStyle.ms_CHILD_FRAME_BG1,  ImGuiColorEditFlags_None | ImGuiColorEditFlags_Float | ImGuiColorEditFlags_DisplayHex | ImGuiColorEditFlags_NoLabel);
        
        
        
        this->S.labelf("None + Float + NoInput:", LABEL_W, WIDGET_W);
        ImGui::ColorEdit4( "##None + Float + NoInput",     (float*)&BStyle.ms_CHILD_FRAME_BG1,  ImGuiColorEditFlags_None | ImGuiColorEditFlags_Float | ImGuiColorEditFlags_NoInputs );
        */
        
        
        
        
        
        
        ImGui::NewLine();
        ImGui::Separator();
        
        
        
        this->S.labelf("ms_CHILD_FRAME_BG1:", LABEL_W, WIDGET_W);
        if ( ImGui::ColorEdit4( "##ms_CHILD_FRAME_BG1",     (float*)&BStyle.ms_CHILD_FRAME_BG1,  COLOR_FLAGS ) )
        {  }
        CopyToClipboard(BStyle.ms_CHILD_FRAME_BG1L);
        
        
        this->S.labelf("ms_CHILD_FRAME_BG1L:", LABEL_W, WIDGET_W);
        if ( ImGui::ColorEdit4( "##ms_CHILD_FRAME_BG1L",     (float*)&BStyle.ms_CHILD_FRAME_BG1L,  COLOR_FLAGS ) )
        {  }
        CopyToClipboard(BStyle.ms_CHILD_FRAME_BG1L);
        
        
        this->S.labelf("ms_CHILD_FRAME_BG1R:", LABEL_W, WIDGET_W);
        if ( ImGui::ColorEdit4( "##ms_CHILD_FRAME_BG1R",     (float*)&BStyle.ms_CHILD_FRAME_BG1R,  COLOR_FLAGS ) )
        {  }
        CopyToClipboard(BStyle.ms_CHILD_FRAME_BG1R);
        
        
        
        
        
        
        this->S.labelf("ms_CHILD_FRAME_BG2:", LABEL_W, WIDGET_W);
        if ( ImGui::ColorEdit4( "##ms_CHILD_FRAME_BG2",     (float*)&BStyle.ms_CHILD_FRAME_BG2,  COLOR_FLAGS ) )
        {  }
        CopyToClipboard(BStyle.ms_CHILD_FRAME_BG2);
        
        
        this->S.labelf("ms_CHILD_FRAME_BG2L:", LABEL_W, WIDGET_W);
        if ( ImGui::ColorEdit4( "##ms_CHILD_FRAME_BG2L",     (float*)&BStyle.ms_CHILD_FRAME_BG2L,  COLOR_FLAGS ) )
        {  }
        CopyToClipboard(BStyle.ms_CHILD_FRAME_BG2L);
        
        
        this->S.labelf("ms_CHILD_FRAME_BG2R:", LABEL_W, WIDGET_W);
        if ( ImGui::ColorEdit4( "##ms_CHILD_FRAME_BG2R",     (float*)&BStyle.ms_CHILD_FRAME_BG2R,  COLOR_FLAGS ) )
        {  }
        CopyToClipboard(BStyle.ms_CHILD_FRAME_BG2R);
        
        

        this->S.labelf("ms_OBJ_INSPECTOR_FRAME_BG:", LABEL_W, WIDGET_W);
        if ( ImGui::ColorEdit4( "##ms_OBJ_INSPECTOR_FRAME_BG",     (float*)&BStyle.ms_OBJ_INSPECTOR_FRAME_BG,  COLOR_FLAGS ) )
        {  }
        CopyToClipboard(BStyle.ms_OBJ_INSPECTOR_FRAME_BG);
    //
    //
    //
    ImGui::End();



    S.PopFont();
    return;
}

// *************************************************************************** //
//
//
// *************************************************************************** //
// *************************************************************************** //   END "TEMPORARY STUFF".





















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
