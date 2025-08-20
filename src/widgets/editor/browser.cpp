/***********************************************************************************
*
*       *********************************************************************
*       ****            B R O W S E R . C P P  ____  F I L E             ****
*       *********************************************************************
*
*              AUTHOR:      Collin A. Bond
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
//  1.  INITIALIZATION METHODS...
// *************************************************************************** //
// *************************************************************************** //

//  Default Constructor.
//
Editor::Editor(app::AppState & src)
    : CBAPP_STATE_NAME(src)
    , m_it( std::make_unique<Interaction>() )
{
    namespace           fs                          = std::filesystem;
    this->m_window_class.DockNodeFlagsOverrideSet   = ImGuiDockNodeFlags_HiddenTabBar;
    
    
    //  INITIALIZE EACH RESIDENT OVERLAY-WINDOW...
    for (size_t i = 0; i < static_cast<size_t>( Resident::COUNT ); ++i)
    {
        Resident idx = static_cast<Resident>(i);
        this->_dispatch_resident_draw_fn( idx );
    }
    
    
    
    //  2.  LOAD DEFAULT TESTS FROM FILE...
    if ( !m_editor_S.m_filepath.empty() && fs::exists(m_editor_S.m_filepath) && fs::is_regular_file(m_editor_S.m_filepath) )
    {
        this->S.m_logger.debug( std::format("Editor | loading from default file, \"{}\"", m_editor_S.m_filepath.string()) );
        this->load_async(m_editor_S.m_filepath);
    }
    else {
        this->S.m_logger.warning( std::format("Editor | unable to load default file, \"{}\"", m_editor_S.m_filepath.string()) );
    }
    
    
    
    //  3.  INITIALIZE FUNCTIONS FOR DEBUGGER OVERLAY WINDOW...
    //  using               DebugItem                   = DebuggerState::DebugItem;
    this->m_debugger.windows    = {{
            {   "Hit Detection",    true,       DebuggerState::ms_FLAGS,        [this]{ this->_debugger_hit_detection   (); }       }
        ,   {   "Interaction",      false,      DebuggerState::ms_FLAGS,        [this]{ this->_debugger_interaction     (); }       }
        ,   {   "More Info",        false,      DebuggerState::ms_FLAGS,        [this]{ this->_debugger_more_info       (); }       }
    }};


    
    return;
}


//  Destructor.
//
Editor::~Editor(void)
{
    this->_clear_all();
}


//  "_dispatch_resident_draw_fn"
//
void Editor::_dispatch_resident_draw_fn(Resident idx)
{
    ResidentEntry &     entry   = m_residents[idx];

    //  Attatch the correct callback function for each RESIDENTIAL OVERLAY WINDOW...
    switch (idx)
    {
        case Resident::Debugger :                   //  1.  DEBUGGER-TOOL OVERLAY.
        { entry.cfg.draw_fn   = [this]{ _draw_debugger_resident(); };       break;      }
        
        case Resident::Selection :                  //  2.  SELECTION OVERLAY.
        { entry.cfg.draw_fn   = [this]{ _draw_selection_resident(); };      break;      }
    
        case Resident::Shape :                      //  3.  SHAPE-TOOL OVERLAY.
        { entry.cfg.draw_fn   = [this]{ _draw_shape_resident(); };          break;      }
            
        default :                                   //  DEFAULT :  Failure.
        { IM_ASSERT(true && "Unknown resident index"); }
    }


    entry.id    = m_overlays.add_resident(entry.cfg, entry.style);
    entry.ptr   = m_overlays.get_resident(entry.id);   // now safe
    return;
}




// *************************************************************************** //
//
//
//  2.  IMPLEMENTATION...
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
                                                                    
    this->_show_browser_color_edit_window();
    
    
    S.PushFont(Font::Small);
    
    
    
    //  2.  LEFTHAND BROWSER SELECTION MENU...
    ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize,  BStyle.ms_CHILD_BORDER1);
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding,    BStyle.ms_CHILD_ROUND1);
    ImGui::PushStyleColor(ImGuiCol_ChildBg,             BStyle.ms_CHILD_FRAME_BG1L);
    //
    //
    //
        //  2.  OBJECT BROWSER'S SELECTOR COLUMN    (MAIN, LEFT-HAND MENU OF THE ENTIRE BROWSER)...
        ImGui::SetNextWindowSizeConstraints( BStyle.OBJ_SELECTOR_DIMS.limits.min, BStyle.OBJ_SELECTOR_DIMS.limits.max );
        ImGui::BeginChild("##Editor_Browser_ObjSelector", BStyle.OBJ_SELECTOR_DIMS.value, BStyle.DYNAMIC_CHILD_FLAGS);
            //
            this->_draw_obj_selector_table();
            //  _draw_obj_selector_column();
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



// *************************************************************************** //
//
//
//  2.  NEW INTERNAL BROWSER IMPLEMENTATIONS...
// *************************************************************************** //
// *************************************************************************** //

//  "_draw_obj_selector_column"
//
void Editor::_draw_obj_selector_column(void)
{
    using                                       namespace               icon;
    static constexpr ImGuiTableFlags            TABLE_FLAGS             = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_ScrollY | ImGuiTableFlags_RowBg;
    static constexpr ImGuiTableColumnFlags      C_EYE                   = ImGuiTableColumnFlags_NoHeaderLabel | ImGuiTableColumnFlags_WidthFixed;
    static constexpr ImGuiTableColumnFlags      C_LOCK                  = ImGuiTableColumnFlags_NoHeaderLabel | ImGuiTableColumnFlags_WidthFixed;
    static constexpr ImGuiTableColumnFlags      C_NAME                  = ImGuiTableColumnFlags_WidthStretch;
    static constexpr ImGuiTableColumnFlags      C_DEL                   = ImGuiTableColumnFlags_NoHeaderLabel | ImGuiTableColumnFlags_WidthFixed;
    //  static constexpr ImGuiSelectableFlags       SELECTABLE_FLAGS        = ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowDoubleClick;
    //
    const ImU32                                 col_text                = ImGui::GetColorU32(ImGuiCol_Text);
    const ImU32                                 col_dim                 = ImGui::GetColorU32(ImGuiCol_TextDisabled);
    //
    //
    BrowserStyle &                              BStyle                  = this->m_style.browser_style;
    BrowserState &                              BState                  = m_browser_S;                    // rename / focus state
    ImDrawList *                                dl                      = ImGui::GetWindowDrawList();
    ImGuiListClipper                            clipper;



    //  1.  SEARCH-QUERY BOX...
    S.PushFont(Font::Main);
    ImGui::SetNextItemWidth(-FLT_MIN);
    if ( ImGui::InputTextWithHint("##Editor_ObjSelector_ObjFilter", "filter", BState.m_obj_filter.InputBuf, IM_ARRAYSIZE( BState.m_obj_filter.InputBuf )) )
    { BState.m_obj_filter.Build(); }
    
    S.PopFont();
        
    ImGui::Separator();
    //clipper.Begin(static_cast<int>(m_paths.size()), -1);


    //  2.  BEGIN THE TABLE TO PRESENT EACH OBJECT...
    if ( ImGui::BeginTable("##Editor_ObjSelector_ObjTable", 4, TABLE_FLAGS, ImVec2(0, -1)) )
    {
        ImGui::TableSetupColumn ("Eye",             C_EYE,          CELL_SZ             );
        ImGui::TableSetupColumn ("Lock",            C_LOCK,         CELL_SZ             );
        ImGui::TableSetupColumn ("Selectable",      C_NAME                              );
        ImGui::TableSetupColumn ("Del",             C_DEL,          1.2f * CELL_SZ      );



        clipper.Begin( static_cast<int>(m_paths.size()), -1 );

        //  3.  DRAWING EACH OBJECT IN THE LEFT-HAND SELECTION COLUMN OF THE BROWSER...
        while ( clipper.Step() )
        {
            for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; ++i)
            {
                Path &                  path            = m_paths[i];
                const bool              selected        = m_sel.paths.count(static_cast<size_t>(i));
                const bool              mutable_path    = path.is_mutable();
                //  const bool          renaming       = (BState.m_renaming_idx == i);
                ImGuiSelectableFlags    sel_flags       = (!mutable_path) ? ImGuiSelectableFlags_Disabled | ImGuiSelectableFlags_AllowDoubleClick
                                                                          : ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowDoubleClick;
                
                
                //  CASE 0 :    EARLY-OUT IF FILTER REMOVES OBJ.
                if ( !BState.m_obj_filter.PassFilter(path.label.c_str()) )      { continue; }


                //  4.  BEGIN THE ROW...
                ImGui::PushID(i);
                //  ImGui::BeginGroup();
                ImGui::TableNextRow();


                //      4.1.        "EYE" BUTTON (TO TOGGLE OBJECT'S VISIBILITY).
                ImGui::TableSetColumnIndex(0);
                ImGui::InvisibleButton("##Editor_Browser_VisibilityButton", { CELL_SZ, CELL_SZ });
                if ( ImGui::IsItemClicked() )       { path.visible = !path.visible; _prune_selection_mutability(); }
                //
                //  Draw EYE or CLOSED-EYE Icon.
                (path.visible ? draw_eye_icon : draw_eye_off_icon)
                        ( dl, ImGui::GetItemRectMin(), {CELL_SZ, CELL_SZ},
                          path.visible ? col_text : col_dim );


                //      4.2.        "LOCK" BUTTON (TO TOGGLE OBJECT'S LOCKED-STATE).
                ImGui::TableSetColumnIndex(1);
                ImGui::InvisibleButton("##Editor_Browser_LockButton", { CELL_SZ, CELL_SZ });
                if ( ImGui::IsItemClicked() )       { path.locked = !path.locked; _prune_selection_mutability(); }
                //
                //  Draw LOCK or UNLOCK Icon.
                //      draw_icon_background(dl, ImGui::GetItemRectMin(), {CELL_SZ, CELL_SZ}, selected ? col_frame : 0);
                (path.locked ? draw_lock_icon : draw_unlock_icon)
                        ( dl, ImGui::GetItemRectMin(), {CELL_SZ, CELL_SZ},
                          path.locked ? col_text : col_dim );


            
                //      4.4.        EDITING THE NAME FOR THE PATH...
                ImGui::TableSetColumnIndex(2);
                ImGui::PushStyleColor( ImGuiCol_Text, (!mutable_path ? col_dim : col_text) ); // NEW – dim on invisible too
                //
                //
                //  ImGui::SetNextItemWidth(select_w);
                //  if ( ImGui::Selectable(path.label.c_str(), selected, sel_flags, {select_w, 0.0f}) )
                if ( ImGui::Selectable(path.label.c_str(), selected, sel_flags, {0.0f, 1.05f * CELL_SZ}) )
                {
                    const bool ctrl  = ImGui::GetIO().KeyCtrl;
                    const bool shift = ImGui::GetIO().KeyShift;

                    if (!ctrl && !shift) {
                        this->reset_selection();
                        if (mutable_path) m_sel.paths.insert(i);
                        m_browser_S.m_browser_anchor = i;
                    }
                    else if (shift && m_browser_S.m_browser_anchor >= 0) {
                        int lo = std::min(m_browser_S.m_browser_anchor, i);
                        int hi = std::max(m_browser_S.m_browser_anchor, i);
                        if (!ctrl) this->reset_selection();
                        for (int k = lo; k <= hi; ++k)
                            if (m_paths[k].is_mutable())
                                m_sel.paths.insert(k);
                    }
                    else if (ctrl && mutable_path) {
                        if (!m_sel.paths.erase(i)) m_sel.paths.insert(i);
                        m_browser_S.m_browser_anchor = i;
                    }
                    _rebuild_vertex_selection();
                }
                ImGui::PopStyleColor();


                //── 2.4 delete button (only when selected & unlocked) --------------
                ImGui::TableSetColumnIndex(3);
                if (!path.locked && selected)
                {
                    utl::SmallCButton(BStyle.ms_DELETE_BUTTON_HANDLE, BStyle.ms_DELETE_BUTTON_COLOR);
                    if (ImGui::IsItemClicked())
                    {
                        _erase_path_and_orphans(static_cast<PathID>(i));
                        this->reset_selection();
                        m_browser_S.m_inspector_vertex_idx = -1;
                        _prune_selection_mutability();

                        ImGui::PopID();
                        clipper.End();
                        ImGui::EndTable();
                        return;                         // safe early exit
                    }
                }

                ImGui::PopID();
            }// END "for-loop" [ROWS]
            
        }// END "while-loop" [CLIPPER]
        
        clipper.End();
        ImGui::EndTable();
    }
    
    
    return;
}


//  "_draw_obj_selector_table"
//
void Editor::_draw_obj_selector_table(void)
{
    using                                   namespace           icon;
    constexpr ImGuiTableFlags               TABLE_FLAGS         = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_ScrollY | ImGuiTableFlags_RowBg;
    constexpr ImGuiTableColumnFlags         C_EYE               = ImGuiTableColumnFlags_NoHeaderLabel | ImGuiTableColumnFlags_WidthFixed;
    constexpr ImGuiTableColumnFlags         C_LOCK              = ImGuiTableColumnFlags_NoHeaderLabel | ImGuiTableColumnFlags_WidthFixed;
    constexpr ImGuiTableColumnFlags         C_NAME              = ImGuiTableColumnFlags_WidthStretch;
    constexpr ImGuiTableColumnFlags         C_DEL               = ImGuiTableColumnFlags_NoHeaderLabel | ImGuiTableColumnFlags_WidthFixed;
    const ImU32                             col_text            = ImGui::GetColorU32(ImGuiCol_Text);
    const ImU32                             col_dim             = ImGui::GetColorU32(ImGuiCol_TextDisabled);
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
        ImGui::TableSetupColumn("Eye",  C_EYE,    CELL_SZ);
        ImGui::TableSetupColumn("Lock", C_LOCK,   CELL_SZ);
        ImGui::TableSetupColumn("Name", C_NAME);
        ImGui::TableSetupColumn("Del",  C_DEL,    1.2f * CELL_SZ);

        clipper.Begin( static_cast<int>(m_paths.size()), -1 );


        //          3.      DRAWING EACH OBJECT IN THE LEFT-HAND SELECTION COLUMN OF THE BROWSER...
        while ( clipper.Step() )
        {
            for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; ++i)
            {
                Path &                  path                = m_paths[i];
                bool                    selected            = m_sel.paths.count(static_cast<size_t>(i));
                bool                    mutable_path        = path.is_mutable();

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
                    ImGui::InvisibleButton("##Editor_Browser_ObjVisibilityButton", {CELL_SZ, CELL_SZ});
                    if ( ImGui::IsItemClicked() )   { path.visible = !path.visible; _prune_selection_mutability(); }
                    (path.visible ? draw_eye_icon : draw_eye_off_icon)
                        (dl, ImGui::GetItemRectMin(), {CELL_SZ, CELL_SZ},
                         path.visible ? col_text : col_dim);
                    //
                    //
                    //
                    //      4.2.        "LOCK" BUTTON (TO TOGGLE OBJECT'S LOCKED-STATE).
                    ImGui::TableSetColumnIndex(1);
                    ImGui::InvisibleButton("##Editor_Browser_ObjLockButton", {CELL_SZ, CELL_SZ});
                    if ( ImGui::IsItemClicked() )   { path.locked = !path.locked; _prune_selection_mutability(); }
                    (path.locked ? draw_lock_icon : draw_unlock_icon)
                        (dl, ImGui::GetItemRectMin(), {CELL_SZ, CELL_SZ},
                         path.locked ? col_text : col_dim);
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
                        utl::SmallCButton( BStyle.ms_DELETE_BUTTON_HANDLE, BStyle.ms_DELETE_BUTTON_COLOR );
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
    const bool                              renaming            = (BS.m_obj_rename_idx == idx);
    
    
    
    //  CASE 1 :    NORMAL "SELECTABLE" BEHAVIOR    [ NOT RENAMING ]...
    if ( !renaming )
    {
    
    
        //      1.1.    CREATE SELECTABLE.
        ImGui::PushStyleColor(ImGuiCol_Text, mutable_path ? col_text : col_dim);
        //
        //
            const bool      ctrl            = ImGui::GetIO().KeyCtrl;
            const bool      shift           = ImGui::GetIO().KeyShift;
            //
            const bool      single_click    = ImGui::Selectable(path.label.c_str(), selected, SEL_FLAGS, {0.0f, 1.05f * CELL_SZ});
            const bool      double_click    = mutable_path
                                                && ImGui::IsItemHovered()
                                                && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)
                                                && !ctrl
                                                && !shift;
        //
        //
        ImGui::PopStyleColor();
    
    
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
        if ( single_click )
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
                    if ( m_paths[k].is_mutable() )      { m_sel.paths.insert(k); }
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












// *************************************************************************** //
//
//
//
//  3.  INSPECTOR COLUMN DISPATCHER FUNCTIONS (RIGHT-HAND SIDE OF 2-COLUMN BROWSER DESIGN)...
// *************************************************************************** //
// *************************************************************************** //

//  "_dispatch_obj_inspector_column"
//
void Editor::_dispatch_obj_inspector_column(void)
{
    const size_t                    sel_paths           = this->m_sel.paths.size();



    //  CASE 0 :    EMPTY SELECTION...
    if ( sel_paths == 0 )
    {
        S.PushFont(Font::Main);
        ImGui::BeginDisabled(true);
            ImGui::SeparatorText("No selection...");
        ImGui::EndDisabled();
        S.PopFont();
        return;
    }
    
    if (sel_paths == 1)         { _draw_single_obj_inspector();     }
    else                        { _draw_multi_obj_inspector();      }
    
    return;
}


//  "_draw_single_obj_inspector"
//      Draws the right-hand, INSPECTOR COLUMN for the case: SELECTION CONTAINS ONLY ONE OBJECT.
//
void Editor::_draw_single_obj_inspector(void)
{
    constexpr ImGuiChildFlags       P1_FLAGS            = ImGuiChildFlags_AutoResizeX   | ImGuiChildFlags_AlwaysUseWindowPadding | ImGuiChildFlags_Borders;
    constexpr ImGuiChildFlags       C1_FLAGS            = ImGuiChildFlags_AutoResizeX   | ImGuiChildFlags_AlwaysUseWindowPadding | ImGuiChildFlags_Borders;
    BrowserStyle &                  BStyle              = this->m_style.browser_style;
    EditorState &                   EState              = this->m_editor_S;
    //
    //
    static bool                     browser_vis_cache   = EState.m_show_vertex_browser;
    //
    const size_t                    sel_idx             = *m_sel.paths.begin();   // only element
    Path &                          path                = m_paths[sel_idx];
    
    //  Compute adaptive column widths BEFORE we emit any child windows
    
    
    //  4.  "VERTEX BROWSER" FOR THE OBJECT...



    //  CASE 1 :    ONLY DRAW OBJECT BROWSER...
    if ( !EState.m_show_vertex_browser )
    {
        browser_vis_cache   = EState.m_show_vertex_browser;
        ImGui::PushStyleColor   (ImGuiCol_ChildBg,                  BStyle.ms_OBJ_INSPECTOR_FRAME_BG      );
        ImGui::BeginChild("##Editor_Browser_ObjectPropertiesPanel", BStyle.OBJ_PROPERTIES_INSPECTOR_DIMS.value,     BStyle.DYNAMIC_CHILD_FLAGS);
            ImGui::PopStyleColor(); //  ImGuiCol_ChildBg.
            _draw_obj_properties_panel(path, sel_idx);
            BStyle.OBJ_PROPERTIES_INSPECTOR_DIMS.value.x       = ImGui::GetItemRectSize().x;
        ImGui::EndChild();
        //
        //
        ImGui::SameLine( 0.0f, BStyle.ms_CHILD_WINDOW_SAMELINE );
        //
        //
        //  //  //  4B.     VERTEX SUB-BROWSER...
        const float                     P1_w                = ImGui::GetContentRegionAvail().x;
        ImGui::PushStyleVar     (ImGuiStyleVar_ChildBorderSize,     BStyle.ms_CHILD_BORDER2         );
        ImGui::PushStyleVar     (ImGuiStyleVar_ChildRounding,       BStyle.ms_CHILD_ROUND2          );
        ImGui::PushStyleColor   (ImGuiCol_ChildBg,                  BStyle.ms_CHILD_FRAME_BG2R      );
            //
            ImGui::BeginChild("##Editor_Browser_PayloadPropertiesPanel",     ImVec2(P1_w, 0.0f),    BStyle.STATIC_CHILD_FLAGS);
                    _draw_payload_properties_panel(path);
            ImGui::EndChild();
            //
        ImGui::PopStyleColor(); //  ImGuiCol_ChildBg.
        ImGui::PopStyleVar(2);   // ImGuiStyleVar_ChildBorderSize,  ImGuiStyleVar_ChildRounding.
    }
    //
    //
    //  CASE 2 :    DRAW OBJECT + VERTEX BROWSERS...
    else
    {
        ImGui::SetNextWindowSizeConstraints( BStyle.OBJ_PROPERTIES_INSPECTOR_DIMS.limits.min, ( browser_vis_cache != EState.m_show_vertex_browser )
                                                ? BStyle.OBJ_PROPERTIES_INSPECTOR_DIMS.value        //  Switched-BACK from FALSE to TRUE.
                                                : BStyle.OBJ_PROPERTIES_INSPECTOR_DIMS.limits.max );
        browser_vis_cache = EState.m_show_vertex_browser;
        
        //  4A.     OBJECT PROPERTIES PANEL.
        ImGui::PushStyleColor   (ImGuiCol_ChildBg,                  BStyle.ms_OBJ_INSPECTOR_FRAME_BG      );
        ImGui::BeginChild("##Editor_Browser_ObjectPropertiesPanel", BStyle.OBJ_PROPERTIES_INSPECTOR_DIMS.value,     BStyle.DYNAMIC_CHILD_FLAGS);
            ImGui::PopStyleColor(); //  ImGuiCol_ChildBg.
            _draw_obj_properties_panel(path, sel_idx);
            BStyle.OBJ_PROPERTIES_INSPECTOR_DIMS.value.x       = ImGui::GetItemRectSize().x;
        ImGui::EndChild();
        //
        //
        //
        ImGui::SameLine( 0.0f, BStyle.ms_CHILD_WINDOW_SAMELINE );
        //
        //
        //
        //  //  //  4B.     VERTEX SUB-BROWSER...
        const float                     P1_w                = ImGui::GetContentRegionAvail().x;
        ImGui::BeginChild("##Editor_Browser_VertexSubBrowser",      ImVec2(P1_w, 0.0f),     P1_FLAGS);
            //
            ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize,  BStyle.ms_CHILD_BORDER2);
            ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding,    BStyle.ms_CHILD_ROUND2);
            ImGui::PushStyleColor(ImGuiCol_ChildBg,             BStyle.ms_CHILD_FRAME_BG2L);
            //
            //
            //  //  4B.1.    LEFT-HAND VERTEX BROWSER.
                ImGui::SetNextWindowSizeConstraints( BStyle.VERTEX_SELECTOR_DIMS.limits.min, BStyle.VERTEX_SELECTOR_DIMS.limits.max );
                ImGui::BeginChild("##Editor_Browser_VertexSelectorColumn",      BStyle.VERTEX_SELECTOR_DIMS.value,    BStyle.DYNAMIC_CHILD_FLAGS);
                    _draw_vertex_selector_column(path);
                    BStyle.VERTEX_SELECTOR_DIMS.value.x        = ImGui::GetItemRectSize().x;
                ImGui::EndChild();
                ImGui::PopStyleColor();
                //
                //
                ImGui::SameLine(0.0f );
                //
                //
                //  4B.2.   RIGHT-HAND VERTEX BROWSER.
                const float             P1C1_w              = ImGui::GetContentRegionAvail().x;
                ImGui::PushStyleColor(ImGuiCol_ChildBg, BStyle.ms_CHILD_FRAME_BG2R);
                ImGui::BeginChild("##Editor_Browser_VertexInspectorColumn",     ImVec2(P1C1_w, 0.0f),    C1_FLAGS);
                    _draw_vertex_inspector_column(path);
                ImGui::EndChild();
                ImGui::PopStyleColor();
            //
            ImGui::PopStyleVar(2);   // border size, rounding
        //
        //
        //
        ImGui::EndChild();
    //
    }// END "Vertex Browser".
    
    
    return;
}


//  "_draw_multi_obj_inspector"
//      Draws the right-hand, INSPECTOR COLUMN for case of: SELECTION CONTAINS MORE THAN ONE OBJECT.
//
void Editor::_draw_multi_obj_inspector(void)
{
    static constexpr size_t         TITLE_SIZE              = 128;
    static char                     title [TITLE_SIZE];   // safe head-room
    //
    const size_t                    num_paths           = m_sel.paths.size();


    //      0.1.    UPDATE TITLE IF SELECTION CHANGED...
    S.PushFont(Font::Main);
    std::snprintf( title, TITLE_SIZE, "%zu Paths Selected", num_paths);
    ImGui::SeparatorText(title);
    S.PopFont();
    


    if ( ImGui::Button("Delete Selection", {150,0}) )
    {
        std::vector<size_t> idxs(m_sel.paths.begin(), m_sel.paths.end());
        std::sort(idxs.rbegin(), idxs.rend());
        for (size_t i : idxs) {
            if (i < m_paths.size())     { m_paths.erase(m_paths.begin() + static_cast<long>(i)); }
        }

        this->reset_selection();    // m_sel.clear();
        m_browser_S.m_inspector_vertex_idx = -1;
    }
    
    
    
    return;
}






// *************************************************************************** //
//
//
//
//  4.  VERTEX BROWSER FUNCTIONS (NESTED INSIDE THE MAIN PATH BROWSER)...
// *************************************************************************** //
// *************************************************************************** //

//  "_draw_vertex_selector_column"
//
void Editor::_draw_vertex_selector_column(Path & path)
{
    const int total = static_cast<int>(path.verts.size());
    ImGuiListClipper clipper; clipper.Begin(total, -1);

    while ( clipper.Step() )
    {
        for (int row = clipper.DisplayStart; row < clipper.DisplayEnd; ++row)
        {
            char lbl[8]; std::snprintf(lbl, sizeof(lbl), Vertex::ms_DEF_VERTEX_SELECTOR_FMT_STRING, row);
            bool selected = (row == m_browser_S.m_inspector_vertex_idx);
            if (ImGui::Selectable(lbl, selected))
                m_browser_S.m_inspector_vertex_idx = (selected ? -1 : row);      // toggle
        }
    }
    
    clipper.End();
    return;
}


//  "_draw_vertex_inspector_column"
//
void Editor::_draw_vertex_inspector_column(Path & path)
{
    static constexpr size_t     TITLE_SIZE      = 32ULL;
    BrowserStyle &              BStyle          = this->m_style.browser_style;
    const float &               LABEL_W         = BStyle.ms_BROWSER_OBJ_LABEL_WIDTH;
    const float &               WIDGET_W        = BStyle.ms_BROWSER_OBJ_WIDGET_WIDTH;
    
    
    //  CASE 0 :    NO VALID SELECTION...
    if ( m_browser_S.m_inspector_vertex_idx < 0 || m_browser_S.m_inspector_vertex_idx >= static_cast<int>(path.verts.size()) ) 
    { ImGui::BeginDisabled(true); ImGui::SeparatorText("Select a vertex from the left hand column..."); ImGui::EndDisabled(); return; }

                      
    //  0.  OBTAIN POINTER TO VERTEX...
    VertexID                    vid             = path.verts[static_cast<size_t>(m_browser_S.m_inspector_vertex_idx)];
    Vertex *                    v               = find_vertex_mut(m_vertices, vid);
    VertexID                    cache_id        = static_cast<VertexID>(-1);
    static char                 title [TITLE_SIZE];     // safe head-room
    
    
    //      0.1.    UPDATE TITLE IF SELECTION CHANGED...
    if (cache_id != vid) {
        cache_id        = vid;
        int retcode     = std::snprintf( title, TITLE_SIZE, Vertex::ms_DEF_VERTEX_TITLE_FMT_STRING, m_browser_S.m_inspector_vertex_idx, vid );
        
        if (retcode < 0) [[unlikely]] {//  Log a warning message if truncation takes place.
            auto message = std::format( "snprintf truncated Vertex title.\n"
                                        "vertex-ID: {}.  title: \"{}\".  buffer-size: {}.  return value: \"{}\".",
                                        vid, title, TITLE_SIZE, retcode );
            CB_LOG( LogLevel::Warning, message );
        }
    }
    
    
    
    //  CASE 1 :    STALE VERTEX...
    if ( !v )           { cache_id = static_cast<VertexID>(-1); ImGui::SeparatorText("[stale vertex]"); return; }



    //  1.  HEADER CONTENT...       "Vertex ID"
    //
    //ImGui::Text( Vertex::ms_DEF_VERTEX_LABEL_FMT_STRING, m_browser_S.m_inspector_vertex_idx, vid );
    ImGui::SeparatorText(title);


    //  2.  "DELETE" BUTTON...
    //  ImGui::SameLine();
    if ( ImGui::Button("Delete Vertex##Editor_VertexBrowser_DeleteVertexButton", {120,0}) )
    {
        _erase_vertex_and_fix_paths(vid);
        m_browser_S.m_inspector_vertex_idx = -1;
        _rebuild_vertex_selection();
    }
    
    
    
    //  3.  WIDGETS EDITS...
    {
        const float         grid            = m_style.GRID_STEP / m_cam.zoom_mag;
        const float         speed           = 0.1f * grid;
        bool                dirty           = false;
        int                 kind_idx        = static_cast<int>(v->kind);
        auto                snap            = [grid](float f){ return std::round(f / grid) * grid; };

        //      3.1.    Position:
        this->left_label("Position:", LABEL_W, WIDGET_W);
        dirty                              |= ImGui::DragFloat2("##Editor_VertexBrowser_Pos", &v->x, speed, -FLT_MAX, FLT_MAX, "%.3f");
        //
        if ( dirty && /*!ImGui::IsItemActive() && */ this->want_snap() ) {
            dirty       = false;
            ImVec2 s    = snap_to_grid({v->x, v->y});
            v->x        = s.x;
            v->y        = s.y;
        }


        //      3.2.    Bézier Handles / Control Points
        //ImGui::SeparatorText("Bezier Controls");
        ImGui::TextDisabled("Bezier Controls");
        //
        //              3.2A    ANCHOR TYPE (corner / smooth / symmetric):
        {
            this->left_label("Type:", LABEL_W, WIDGET_W);
            dirty = ImGui::Combo("##Editor_VertexBrowser_AnchorType", &kind_idx, ms_ANCHOR_TYPE_NAMES.data(), static_cast<int>( AnchorType::COUNT ));          // <- int, not enum
            //
            if (dirty) {
                v->kind     = static_cast<AnchorType>(kind_idx);
                dirty       = false;
            }
        }
        //
        //              3.2B    INWARD (from previous vertex):
        this->left_label("Inward:", LABEL_W, WIDGET_W);
        //
        dirty              = ImGui::DragFloat2("##Editor_VertexBrowser_InwardControl",     &v->in_handle.x,    speed,  -FLT_MAX,   FLT_MAX,    "%.3f");
        if ( dirty && !ImGui::IsItemActive() ) {
            v->in_handle.x      = snap(v->in_handle.x);
            v->in_handle.y      = snap(v->in_handle.y);
            mirror_handles<VertexID>(*v, /*dragging_out=*/false);
            dirty               = false;
        }
        //
        //              3.2C    OUTWARD (to next vertex):
        this->left_label("Outward:", LABEL_W, WIDGET_W);
        dirty               = ImGui::DragFloat2("##Editor_VertexBrowser_OutwardControl",    &v->out_handle.x,   speed,  -FLT_MAX,   FLT_MAX,    "%.3f");
        if ( dirty && !ImGui::IsItemActive() ) {
            v->out_handle.x     = snap(v->out_handle.x);
            v->out_handle.y     = snap(v->out_handle.y);
            mirror_handles<VertexID>(*v, /*dragging_out=*/true);  // keep smooth/symmetric rule
            dirty               = false;
        }
    }
    
    
    return;
}






// *************************************************************************** //
//
//
//
//      INDIVIDUAL PANELS...
// *************************************************************************** //
// *************************************************************************** //

//  "_draw_obj_properties_panel"
//
void Editor::_draw_obj_properties_panel(Path & path, const size_t pidx)
{
    enum class ObjectType               { None = 0, Stroke, COUNT };
    //
    constexpr ImGuiColorEditFlags       COLOR_FLAGS             = ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaPreviewHalf;
    static constexpr const size_t &     BUFFER_SIZE             = BrowserState::ms_MAX_PATH_TITLE_LENGTH;
    //
    //
    //
    BrowserStyle &                      BStyle                  = this->m_style.browser_style;
    EditorState &                       EState                  = m_editor_S;
    BrowserState &                      BState                  = m_browser_S;
    //
    static char                         title                   [ BUFFER_SIZE ];   // safe head-room
    static PathID                       cache_id                = static_cast<PathID>(-1);
    //
    //
    //
    const float &                       LABEL_W                 = BStyle.ms_BROWSER_VERTEX_LABEL_WIDTH;
    const float &                       WIDGET_W                = BStyle.ms_BROWSER_VERTEX_WIDGET_WIDTH;
    const bool                          has_payload             = (path.kind != PathKind::None);
    const bool                          is_area                 = path.is_area();



    //  CASE 0 :    ERROR...
    if ( pidx >= m_paths.size() )   { ImGui::SeparatorText("[invalid object]"); cache_id = static_cast<PathID>(-1); return; }
    
    
    
    
    
    //      CASE 1 :    USER IS MODIFYING PATH-NAME...
    if ( BState.m_renaming_obj ) [[unlikely]]
    {
        cache_id        = -1;  //  Queue-up a RE-CACHE once user is finished modifying title.
        std::snprintf( title, BUFFER_SIZE, Path::ms_DEF_PATH_TITLE_FMT_STRING, BState.m_name_buffer, path.id );
    }
    //
    //
    //      CASE 2 :    IF ( NOT MODIFYING PATH )  *AND*  ( NEED TO RE-CACHE TITLE (IF SELECTION CHANGED) )...
    else if (cache_id != pidx )
    {
        cache_id        = static_cast<PathID>( pidx );
        int retcode     = std::snprintf( title, BrowserState::ms_MAX_PATH_TITLE_LENGTH, Path::ms_DEF_PATH_TITLE_FMT_STRING, path.label.c_str(), path.id );
        
        //  Log a warning message if truncation takes place.
        if (retcode < 0) [[unlikely]]
        {
            CB_LOG( LogLevel::Warning, std::format(
                "snprintf truncated Path title.\nPath ID: {}.  title: \"{}\".  buffer-size: {}.  return value: \"{}\".",
                 pidx, title, BUFFER_SIZE, retcode )
            );
        }
    }
    


    //  1A.     HEADER ENTRY / TITLE...
    S.PushFont(Font::Main);
    //
        ImGui::SeparatorText(title);
    //
    S.PopFont();
    
    
    //  1B.     SUBSEQUENT HEADER ENTRIES...
    //
        this->left_label("Show Vertices:",       LABEL_W,    WIDGET_W);
        ImGui::Checkbox("##Editor_VertexInspector_ShowVertexInspector",    &EState.m_show_vertex_browser);
    //
    //



    if ( !EState.m_show_vertex_browser && !has_payload ) {
    //
    //
    //
        ImGui::NewLine();
        
        //  2.  LINE-STROKE, AREA-FILL COLORS...
        {
            ImVec4          stroke_f            = u32_to_f4(path.style.stroke_color);
            ImVec4          fill_f              = u32_to_f4(path.style.fill_color);
            bool            stroke_dirty        = false;
            bool            fill_dirty          = false;


            this->left_label("Line Color:",         LABEL_W,    WIDGET_W);
            stroke_dirty                        = ImGui::ColorEdit4( "##Editor_VertexInspector_LineColor",    (float*)&stroke_f,  COLOR_FLAGS );
            //
            //
            this->left_label("Fill Color:",         LABEL_W,    WIDGET_W);
            ImGui::BeginDisabled( !is_area );
                fill_dirty                      = ImGui::ColorEdit4( "##Editor_VertexInspector_FillColor",    (float*)&fill_f,    COLOR_FLAGS );
            ImGui::EndDisabled();
            
            
            if (stroke_dirty)   { path.style.stroke_color = f4_to_u32(stroke_f); }
            if (fill_dirty)     { path.style.fill_color   = f4_to_u32(fill_f); }
            if (!is_area)       { path.style.fill_color  &= 0x00FFFFFF; }   // clear alpha
            
        }




        //  3.  LINE WIDTH...
        {
            static constexpr float  min_width   = 0.00;
            static constexpr float  max_width   = 32;
            bool                    dirty       = false;
            float                   w           = path.style.stroke_width;
            
            this->left_label("Stroke Weight:", LABEL_W, WIDGET_W);
            ImGui::SetNextItemWidth(200.0f);
            //
            //  CASE 1 :    Value < 2.0f
            if (w < 2.0f) {
                dirty = ImGui::InputFloat("##Editor_VertexBrowser_LineWidth", &w,   0.000f,     0.25f,      "%.4f px");
            }
            //
            //  CASE 2 :    2.0 <= Value.
            else {
                dirty = ImGui::InputFloat("##Editor_VertexBrowser_LineWidth", &w,   1.0f,       2.0f,       "%.2f px");
            }
            //
            //
            //
            if ( dirty ) {
                w = std::clamp(w, min_width, max_width);
                path.style.stroke_width = w;
            }
        }
        // ImGui::Separator();
        
        

    ImGui::NewLine();
    //
        //  4.  Z-ORDER...
        {
            this->left_label("Z-Index:", LABEL_W, WIDGET_W);
            ImGui::Text("%3u", path.z_index);
            
            if ( ImGui::SmallButton("Send To Back")     )       { send_selection_to_back();     }
            ImGui::SameLine();
            if ( ImGui::SmallButton("Send Backward")    )       { send_selection_backward();    }
            ImGui::SameLine();
            if ( ImGui::SmallButton("Bring Forward")    )       { bring_selection_forward();    }
            ImGui::SameLine();
            if ( ImGui::SmallButton("Bring To Front")   )       { bring_selection_to_front();   }
        }
    //
    //
    //
    }
    


    //  5.  PROPERTIES...
    this->_draw_payload_properties_panel(path);
    
    
    
    return;
}


//  "_draw_payload_properties_panel"
//
void Editor::_draw_payload_properties_panel(Path & path)
{
    const bool                          has_payload                 = path.kind != PathKind::None;


    ImGui::BeginDisabled(true);
        ImGui::SeparatorText("Payload");
    ImGui::EndDisabled();
    {
        this->left_label("Type:", 196.0f, 256.0f);    path.ui_kind();
        
        
        //  CASE 1 :    NO PAYLOAD TYPE...
        if (!has_payload)
        {
            ImGui::Indent();
                ImGui::TextDisabled("No payload data");
            ImGui::Unindent();
        }
        //
        //  CASE 2 :    DISPLAY THE PAYLOAD UI...
        else
        {
            path.ui_properties();
        }
    }



    return;
}














//  "_draw_vertex_properties_panel"
//
void Editor::_draw_vertex_properties_panel(void)
{
    return;
}

// *************************************************************************** //
//
//
// *************************************************************************** //
// *************************************************************************** //   END "INDIVIDUAL PANELS".















// *************************************************************************** //
//
//
//
//      NEW STUFF...
// *************************************************************************** //
// *************************************************************************** //

//  "_draw_trait_selector"
//
void Editor::_draw_trait_selector(void)
{
    using                                       namespace               icon;
    static constexpr ImGuiTableFlags            TABLE_FLAGS             = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_ScrollY | ImGuiTableFlags_RowBg;
    static constexpr ImGuiTableColumnFlags      C1_FLAGS                = ImGuiTableColumnFlags_NoHeaderLabel | ImGuiTableColumnFlags_WidthStretch;
    static constexpr ImGuiSelectableFlags       SELECTABLE_FLAGS        = ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowDoubleClick;
    //
    const ImU32                                 col_text                = ImGui::GetColorU32(ImGuiCol_Text);
    const ImU32                                 col_dim                 = ImGui::GetColorU32(ImGuiCol_TextDisabled);
    //
    //
    ImGuiListClipper                            clipper;



    //  1.  SEARCH-QUERY BOX...
    S.PushFont(Font::Main);
    ImGui::BeginDisabled(true);
    //
        ImGui::SeparatorText("Traits");
    //
    ImGui::EndDisabled();
    //ImGui::SetNextItemWidth(-FLT_MIN);
    
    S.PopFont();
        


    //  2.  BEGIN THE TABLE TO PRESENT EACH OBJECT...
    if ( ImGui::BeginTable("##Editor_TraitSelector_TraitTable", 1, TABLE_FLAGS, ImVec2(0, -1)) )
    {
        ImGui::TableSetupColumn ("C1",         C1_FLAGS);



        clipper.Begin( static_cast<int>(ObjectTrait::COUNT), -1 );

        //  3.  DRAWING EACH OBJECT IN THE LEFT-HAND SELECTION COLUMN OF THE BROWSER...
        while ( clipper.Step() )
        {
            for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; ++i)
            {   
                //  4.  BEGIN THE ROW...
                ImGui::PushID(i);
                ImGui::TableNextRow();


                


                //      4.1.        "EYE" BUTTON (TO TOGGLE OBJECT'S VISIBILITY).
                ImGui::TableSetColumnIndex(0);
                ImGui::TextDisabled( "%s", ms_OBJECT_TRAIT_NAMES[ static_cast<size_t>(i) ] );
                
                

                ImGui::PopID();
            }// END "for-loop" [ROWS]
            
        }// END "while-loop" [CLIPPER]
        
        clipper.End();
        ImGui::EndTable();
    }
    
    
    return;
}

// *************************************************************************** //
//
//
// *************************************************************************** //
// *************************************************************************** //   END "NEW STUFF".










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
        static int ID = 1;
        
        ImGui::PushID(ID++);
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
        this->left_label("[CUSTOM 1]:",     LABEL_W, WIDGET_W);
        ui::CustomColorEdit4( "##CUSTOM 1", BStyle.ms_CHILD_FRAME_BG1 );
        //
        CopyToClipboard(BStyle.ms_CHILD_FRAME_BG1);
        
        
        this->left_label("[CUSTOM 2]:",     LABEL_W, WIDGET_W);
        ui::HexColor4( "##CUSTOM 2", BStyle.ms_CHILD_FRAME_BG1 );
        //
        CopyToClipboard(BStyle.ms_CHILD_FRAME_BG1);
        
        
        
        ImGui::NewLine();
        /*
        this->left_label("ImGuiColorEditFlags_None:", LABEL_W, WIDGET_W);
        ImGui::ColorEdit4( "##ImGuiColorEditFlags_None",     (float*)&BStyle.ms_CHILD_FRAME_BG1,  ImGuiColorEditFlags_None );
        
        this->left_label("None + Float:", LABEL_W, WIDGET_W);
        ImGui::ColorEdit4( "##None + Float",     (float*)&BStyle.ms_CHILD_FRAME_BG1,  ImGuiColorEditFlags_None | ImGuiColorEditFlags_Float );
        
        this->left_label("None + Float + Hex:", LABEL_W, WIDGET_W);
        ImGui::ColorEdit4( "##None + Float + Hex",     (float*)&BStyle.ms_CHILD_FRAME_BG1,  ImGuiColorEditFlags_None | ImGuiColorEditFlags_Float | ImGuiColorEditFlags_DisplayHex );
        
        this->left_label("None + Float + Hex + NoLabel:", LABEL_W, WIDGET_W);
        ImGui::ColorEdit4( "##None + Float + Hex + NoLabel",     (float*)&BStyle.ms_CHILD_FRAME_BG1,  ImGuiColorEditFlags_None | ImGuiColorEditFlags_Float | ImGuiColorEditFlags_DisplayHex | ImGuiColorEditFlags_NoLabel);
        
        
        
        this->left_label("None + Float + NoInput:", LABEL_W, WIDGET_W);
        ImGui::ColorEdit4( "##None + Float + NoInput",     (float*)&BStyle.ms_CHILD_FRAME_BG1,  ImGuiColorEditFlags_None | ImGuiColorEditFlags_Float | ImGuiColorEditFlags_NoInputs );
        */
        
        
        
        
        
        
        ImGui::NewLine();
        ImGui::Separator();
        
        
        
        this->left_label("ms_CHILD_FRAME_BG1:", LABEL_W, WIDGET_W);
        if ( ImGui::ColorEdit4( "##ms_CHILD_FRAME_BG1",     (float*)&BStyle.ms_CHILD_FRAME_BG1,  COLOR_FLAGS ) )
        {  }
        CopyToClipboard(BStyle.ms_CHILD_FRAME_BG1L);
        
        
        this->left_label("ms_CHILD_FRAME_BG1L:", LABEL_W, WIDGET_W);
        if ( ImGui::ColorEdit4( "##ms_CHILD_FRAME_BG1L",     (float*)&BStyle.ms_CHILD_FRAME_BG1L,  COLOR_FLAGS ) )
        {  }
        CopyToClipboard(BStyle.ms_CHILD_FRAME_BG1L);
        
        
        this->left_label("ms_CHILD_FRAME_BG1R:", LABEL_W, WIDGET_W);
        if ( ImGui::ColorEdit4( "##ms_CHILD_FRAME_BG1R",     (float*)&BStyle.ms_CHILD_FRAME_BG1R,  COLOR_FLAGS ) )
        {  }
        CopyToClipboard(BStyle.ms_CHILD_FRAME_BG1R);
        
        
        
        
        
        
        this->left_label("ms_CHILD_FRAME_BG2:", LABEL_W, WIDGET_W);
        if ( ImGui::ColorEdit4( "##ms_CHILD_FRAME_BG2",     (float*)&BStyle.ms_CHILD_FRAME_BG2,  COLOR_FLAGS ) )
        {  }
        CopyToClipboard(BStyle.ms_CHILD_FRAME_BG2);
        
        
        this->left_label("ms_CHILD_FRAME_BG2L:", LABEL_W, WIDGET_W);
        if ( ImGui::ColorEdit4( "##ms_CHILD_FRAME_BG2L",     (float*)&BStyle.ms_CHILD_FRAME_BG2L,  COLOR_FLAGS ) )
        {  }
        CopyToClipboard(BStyle.ms_CHILD_FRAME_BG2L);
        
        
        this->left_label("ms_CHILD_FRAME_BG2R:", LABEL_W, WIDGET_W);
        if ( ImGui::ColorEdit4( "##ms_CHILD_FRAME_BG2R",     (float*)&BStyle.ms_CHILD_FRAME_BG2R,  COLOR_FLAGS ) )
        {  }
        CopyToClipboard(BStyle.ms_CHILD_FRAME_BG2R);
        
        

        this->left_label("ms_OBJ_INSPECTOR_FRAME_BG:", LABEL_W, WIDGET_W);
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
