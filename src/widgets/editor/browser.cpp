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
    if ( !this->m_filepath.empty() && fs::exists(this->m_filepath) && fs::is_regular_file(this->m_filepath) )
    {
        this->S.m_logger.debug( std::format("Editor | loading from default file, \"{}\"", this->m_filepath.string()) );
        this->load_async(this->m_filepath);
    }
    else {
        this->S.m_logger.debug( std::format("Editor | unable to load default file, \"{}\"", this->m_filepath.string()) );
    }

    
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
//OBJ_PROPERTIES_INSPECTOR_DIMS


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
        ImGui::TableSetupColumn ("Eye",         C_EYE,          CELL_SZ             );
        ImGui::TableSetupColumn ("Lock",        C_LOCK,         CELL_SZ             );
        ImGui::TableSetupColumn ("Name",        C_NAME                              );
        ImGui::TableSetupColumn ("Del",         C_DEL,          1.2f * CELL_SZ      );



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
    constexpr ImGuiTableColumnFlags         C_HANDLE            = ImGuiTableColumnFlags_NoHeaderLabel | ImGuiTableColumnFlags_WidthFixed;
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


    // filter input
    S.PushFont(Font::Main);
    ImGui::SetNextItemWidth(-FLT_MIN);
    if ( ImGui::InputTextWithHint( "##Editor_ObjSelector_ObjFilter",
                                   "filter",
                                   BS.m_obj_filter.InputBuf,
                                   IM_ARRAYSIZE(BS.m_obj_filter.InputBuf) ) )
        { BS.m_obj_filter.Build(); }
        
    S.PopFont();
    ImGui::Separator();



    if ( ImGui::BeginTable("##Editor_ObjSelector_ObjTable", 5, TABLE_FLAGS, ImVec2(0, -1)) )
    {
        ImGui::TableSetupColumn("Drag", C_HANDLE, CELL_SZ);
        ImGui::TableSetupColumn("Eye",  C_EYE,    CELL_SZ);
        ImGui::TableSetupColumn("Lock", C_LOCK,   CELL_SZ);
        ImGui::TableSetupColumn("Name", C_NAME);
        ImGui::TableSetupColumn("Del",  C_DEL,    1.2f * CELL_SZ);

        clipper.Begin(static_cast<int>(m_paths.size()), -1);

        while ( clipper.Step() )
        {
            for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; ++i)
            {
                Path &                  path                = m_paths[i];
                bool                    selected            = m_sel.paths.count(static_cast<size_t>(i));
                bool                    mutable_path        = path.is_mutable();

                if ( !BS.m_obj_filter.PassFilter(path.label.c_str()) )      { continue; }


                ImGui::PushID(i);
                ImGui::TableNextRow();


                // ── 1. Drag-handle column ──────────────────────────
                ImGui::TableSetColumnIndex(0);
                ImGui::PushID("drag");                       // <── NEW
                    utl::SmallCButton( BStyle.ms_DRAG_HANDLE_ICON, 0x00000000 );
                ImGui::PopID();                              // <── NEW
                //
                if ( ImGui::IsItemActive() && ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceNoPreviewTooltip) )
                {
                    ImGui::SetDragDropPayload( "OBJ_ROW", &i, sizeof(i) );
                    ImGui::EndDragDropSource();
                }
                if ( ImGui::BeginDragDropTarget() )
                {
                    if ( const ImGuiPayload * p = ImGui::AcceptDragDropPayload("OBJ_ROW") )
                    {
                        int src = *static_cast<const int*>(p->Data);
                        _reorder_paths(src, i);

                        // safe early exit after mutation
                        ImGui::PopID();
                        clipper.End();
                        ImGui::EndTable();
                        return;
                    }
                    ImGui::EndDragDropTarget();
                }



                // ── 2. Eye toggle ─────────────────────────────────
                ImGui::TableSetColumnIndex(1);
                ImGui::InvisibleButton("##Editor_Browser_ObjVisibilityButton", {CELL_SZ, CELL_SZ});
                if ( ImGui::IsItemClicked() )   { path.visible = !path.visible; _prune_selection_mutability(); }
                (path.visible ? draw_eye_icon : draw_eye_off_icon)
                    (dl, ImGui::GetItemRectMin(), {CELL_SZ, CELL_SZ},
                     path.visible ? col_text : col_dim);



                // ── 3. Lock toggle ───────────────────────────────
                ImGui::TableSetColumnIndex(2);
                ImGui::InvisibleButton("##Editor_Browser_ObjLockButton", {CELL_SZ, CELL_SZ});
                if ( ImGui::IsItemClicked() )   { path.locked = !path.locked; _prune_selection_mutability(); }
                (path.locked ? draw_lock_icon : draw_unlock_icon)
                    (dl, ImGui::GetItemRectMin(), {CELL_SZ, CELL_SZ},
                     path.locked ? col_text : col_dim);



                // ── 4. Name / selection / rename ─────────────────
                ImGui::TableSetColumnIndex(3);
                this->_draw_obj_selectable( path, i, mutable_path, selected );



                // ── 5. Delete (selected & unlocked) ──────────────
                ImGui::TableSetColumnIndex(4);
                if ( !path.locked && selected )
                {
                    utl::SmallCButton(BStyle.ms_DELETE_BUTTON_HANDLE, BStyle.ms_DELETE_BUTTON_COLOR);
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
                
                
                
                
                

                ImGui::PopID();
            }
        }

        clipper.End();
        ImGui::EndTable();
    }
}



//  "_draw_obj_selectable"
//
inline void Editor::_draw_obj_selectable( Path & path, const int idx, const bool mutable_path, const bool selected )
{
    using                                   namespace           icon;
    static constexpr ImGuiInputTextFlags    TEXT_FLAGS          = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll;
    const ImGuiSelectableFlags              SEL_FLAGS           = ( mutable_path )
                                                                    ? ( ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowDoubleClick     )
                                                                    : ( ImGuiSelectableFlags_Disabled | ImGuiSelectableFlags_AllowDoubleClick           );
    //
    const ImU32                             col_text            = ImGui::GetColorU32(ImGuiCol_Text);
    const ImU32                             col_dim             = ImGui::GetColorU32(ImGuiCol_TextDisabled);
    //
    //
    //
    BrowserState &                          BS                  = m_browser_S;
    const bool                              renaming            = (BS.m_obj_rename_idx == idx);
    
    
    
    //  CASE 1 :    RENAMING THE OBJECT IN THIS ROW...
    if (renaming)
    {
        BS.m_renaming_obj = true;                       //  flag blocks global shortcuts
        
        ImGui::PushItemWidth(-FLT_MIN);
        //
        //      CASE 1A :   Submit name-change upon [ ENTER ].
        if ( ImGui::InputText( "##Editor_ObjSelector_RenameObj",
                               BS.m_name_buffer,
                               IM_ARRAYSIZE(BS.m_name_buffer),
                               TEXT_FLAGS ) )
        {                                               // commit on Enter
            path.label              = BS.m_name_buffer;
            BS.m_obj_rename_idx     = -1;
            BS.m_renaming_obj       = false;
        }
        //
        //      CASE 1B :   Revert to previous name on [ ESC ] or FOCUS-LOSS.
        if ( ImGui::IsKeyPressed(ImGuiKey_Escape) || (!ImGui::IsItemActive() && ImGui::IsItemDeactivated()) )
        {
            BS.m_obj_rename_idx     = -1;
            BS.m_renaming_obj       = false;
        }
        ImGui::PopItemWidth();
    //
    //
    }// END "CASE 1".
    


    //  CASE 2 :    NORMAL "SELECTABLE" BEHAVIOR...
    else
    {
    //
        //      2.1.    CREATE SELECTABLE.
        ImGui::PushStyleColor(ImGuiCol_Text, mutable_path ? col_text : col_dim);
        //
        //
            const bool      ctrl        = ImGui::GetIO().KeyCtrl;
            const bool      shift       = ImGui::GetIO().KeyShift;
            //
            const bool      single_click    = ImGui::Selectable(path.label.c_str(), selected, SEL_FLAGS, {0.0f, 1.05f * CELL_SZ});
            const bool      double_click    = ctrl && shift && mutable_path && ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left); // mutable_path && ImGui::IsItemActivated() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left);
        //
        //
        ImGui::PopStyleColor();
        
        
        
        //      2.1A.   SINGLE-CLICK SELECTABLE LOGIC.
        if ( single_click && !double_click )
        {
        
            if ( !ctrl && !shift )
            {
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
        //
        }// END "SINGLE CLICK".
        //
        //
        //      2.2.    ALLOW DOUBLE-CLICK TO EDIT OBJECT NAME...
        else
        {
            BS.m_obj_rename_idx     = idx;
            std::strncpy( BS.m_name_buffer, path.label.c_str(), IM_ARRAYSIZE(BS.m_name_buffer) );
            BS.m_renaming_obj       = true;
        }// END "DOUBLE CLICK".
    //
    //
    //
    }// END "CASE 2".



    return;
}









/*
{
    using namespace icon;                      // pull in CELL_SZ, etc.
    //
    //  COLORS FROM CURRENT STYLE (Non-icon constants)
    static constexpr auto       ms_DELETE_BUTTON_COLOR      = IM_COL32(  255,  0,  0, 255);
    static constexpr float      ms_DELETE_BUTTON_ROUNDING   = 0.0f;
    const ImU32                 col_text                    = ImGui::GetColorU32(ImGuiCol_Text);
    const ImU32                 col_dim                     = ImGui::GetColorU32(ImGuiCol_TextDisabled);
    const ImU32                 col_frame                   = ImGui::GetColorU32(ImGuiCol_FrameBg);
    ImDrawList *                dl                          = ImGui::GetWindowDrawList();
    ImGuiListClipper            clipper;


    //  1.  SEARCH-QUERY BOX...
    S.PushFont(Font::Main);
    ImGui::SetNextItemWidth(-FLT_MIN);
    if ( ImGui::InputTextWithHint("##Editor_Browser_LeftFilter", "filter",
                                 m_browser_filter.InputBuf,
                                 IM_ARRAYSIZE(m_obj_filter.InputBuf)) )
        { m_obj_filter.Build(); }
    S.PopFont();
        
    ImGui::Separator();


    clipper.Begin(static_cast<int>(m_paths.size()), -1);


    //  2.  DRAWING EACH OBJECT IN THE LEFT-HAND SELECTION COLUMN OF THE BROWSER...
    while ( clipper.Step() )
    {
        for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; ++i)
        {
            Path &                  path            = m_paths[i];
            const bool              selected        = m_sel.paths.count(static_cast<size_t>(i));
            const bool              mutable_path    = path.is_mutable();          // NEW – cached
            ImGuiSelectableFlags    sel_flags       = (!mutable_path ? ImGuiSelectableFlags_Disabled | ImGuiSelectableFlags_AllowDoubleClick : ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowDoubleClick);
            
            
            //  CASE 0 :    EARLY-OUT IF FILTER REMOVES OBJ...
            if ( !m_obj_filter.PassFilter(path.label.c_str()) )      { continue; }


            //  3.    BEGIN ROW...
            ImGui::PushID(i);
            ImGui::BeginGroup();


            //  3.1.    "EYE" BUTTON (TO TOGGLE OBJECT'S VISIBILITY)...
            ImVec2      pos             = ImGui::GetCursorScreenPos();
            ImGui::InvisibleButton("##Editor_Browser_VisibilityButton", { CELL_SZ, CELL_SZ });
            if ( ImGui::IsItemClicked() )       { path.visible = !path.visible; _prune_selection_mutability(); }
            //
            //  Draw EYE or CLOSED-EYE Icon.
            draw_icon_background(dl, pos, { CELL_SZ, CELL_SZ }, selected ? col_frame : 0);
            if ( path.visible )                 { draw_eye_icon(dl,     pos, { CELL_SZ, CELL_SZ }, col_text); }
            else                                { draw_eye_off_icon(dl, pos, { CELL_SZ, CELL_SZ }, col_dim); }


            ImGui::SameLine(0.0f, m_style.ms_BROWSER_BUTTON_SEP);


            //  3.2.    "LOCK" BUTTON (TO TOGGLE OBJECT'S LOCKED-STATE)...
            pos = ImGui::GetCursorScreenPos();
            ImGui::InvisibleButton("##Editor_Browser_LockButton", { CELL_SZ, CELL_SZ });
            if ( ImGui::IsItemClicked() )       { path.locked = !path.locked; _prune_selection_mutability(); }
            //
            ImGui::SameLine(0.0f, m_style.ms_BROWSER_SELECTABLE_SEP);
            //
            //  Draw LOCK or UNLOCK Icon.
            draw_icon_background(dl, pos, { CELL_SZ, CELL_SZ }, selected ? col_frame : 0);
            if (path.locked)                    { draw_lock_icon(dl,    pos, { CELL_SZ, CELL_SZ }, col_text); }
            else                                { draw_unlock_icon(dl,  pos, { CELL_SZ, CELL_SZ }, col_dim); }



            ImGui::SameLine(0.0f, m_style.ms_BROWSER_SELECTABLE_SEP);     // Spacing before selectable object.
            
            
            //  3.3.    "DELETE" BUTTON...
            const float     availX          = ImGui::GetContentRegionAvail().x;
            const float     del_button_w    = 1.2f * CELL_SZ;
            const auto      prev_pos        = ImGui::GetCursorScreenPos();
            pos                             = prev_pos + ImVec2(availX - del_button_w, 0.0f);
            //
            //
            if (!path.locked && selected) {
                ImGui::SetCursorScreenPos(pos);
                ImGui::InvisibleButton( "##Editor_Browser_DeleteButton", { CELL_SZ, CELL_SZ } );
                if ( ImGui::IsItemClicked() )
                {
                    _erase_path_and_orphans(static_cast<PathID>(i)); // ← your helper
                    this->reset_selection();
                    m_browser_S.m_inspector_vertex_idx  = -1;
                    _prune_selection_mutability();      //  ??  DO I NEED THIS  ??
                }
                //
                //  Draw EYE or CLOSED-EYE Icon.
                draw_icon_background(dl, pos, { CELL_SZ, CELL_SZ }, !path.locked ? ms_DELETE_BUTTON_COLOR : 0, ms_DELETE_BUTTON_ROUNDING);
            }
            
            
            
            //  3.4.    EDITING THE NAME FOR THE PATH...
            ImGui::SetCursorScreenPos(prev_pos);
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
            
            
            
            
            ImGui::EndGroup();
            ImGui::PopID();
            
        }// END "for-loop"
        
    }// END "while-loop"
    
    
    clipper.End();
    return;
}*/












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
    const size_t    sel_paths       = this->m_sel.paths.size();

    //  CASE 0 :    EMPTY SELECTION...
    if ( sel_paths == 0 ) {
        S.PushFont(Font::Main); ImGui::BeginDisabled(true);
        ImGui::SeparatorText("Select an object from the left hand column...");
        ImGui::EndDisabled(); S.PopFont(); return;
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
    //
    BrowserStyle &                  BStyle              = this->m_style.browser_style;
    const size_t                    sel_idx             = *m_sel.paths.begin();   // only element
    Path &                          path                = m_paths[sel_idx];
    
    
    //  Compute adaptive column widths BEFORE we emit any child windows
    
    
    //  4.  "VERTEX BROWSER" FOR THE OBJECT...
    float                           sub_h               = 0.0f; // availY * m_style.ms_VERTEX_SUBBROWSER_HEIGHT;



    //  4A.     OBJECT PROPERTIES PANEL.
        ImGui::SetNextWindowSizeConstraints( BStyle.OBJ_PROPERTIES_INSPECTOR_DIMS.limits.min, BStyle.OBJ_PROPERTIES_INSPECTOR_DIMS.limits.max );
    ImGui::BeginChild("##Editor_Browser_ObjectPropertiesPanel", BStyle.OBJ_PROPERTIES_INSPECTOR_DIMS.value,     BStyle.DYNAMIC_CHILD_FLAGS);
        _draw_obj_properties_panel(path, sel_idx);     // <-- NEW helper you just wrote
        BStyle.OBJ_PROPERTIES_INSPECTOR_DIMS.value.x       = ImGui::GetItemRectSize().x;
    ImGui::EndChild();
    ImGui::SameLine( 0.0f );
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
            ImGui::BeginChild("##Editor_Browser_VertexInspectorColumn",     ImVec2(P1C1_w, sub_h),    C1_FLAGS);
                _draw_vertex_inspector_column(path);
            ImGui::EndChild();
            ImGui::PopStyleColor();
        //
        ImGui::PopStyleVar(2);   // border size, rounding
    //
    //
    //
    ImGui::EndChild();
    
    
    return;
}

/*{
    constexpr ImGuiChildFlags       P0_FLAGS            = ImGuiChildFlags_AutoResizeX   | ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_NoScrollbar;
    constexpr ImGuiChildFlags       P1_FLAGS            = ImGuiChildFlags_AutoResizeX   | ImGuiChildFlags_AlwaysUseWindowPadding | ImGuiChildFlags_Borders;
    constexpr ImGuiChildFlags       C0_FLAGS            = ImGuiChildFlags_ResizeX       | ImGuiChildFlags_AlwaysUseWindowPadding | ImGuiChildFlags_Borders;
    constexpr ImGuiChildFlags       C1_FLAGS            = ImGuiChildFlags_AutoResizeX   | ImGuiChildFlags_AlwaysUseWindowPadding | ImGuiChildFlags_Borders;
    //
    BrowserStyle &                  BStyle             = this->m_style.browser_style;
    const size_t                    sel_idx             = *m_sel.paths.begin();   // only element
    Path &                          path                = m_paths[sel_idx];
    
    
    //  Compute adaptive column widths BEFORE we emit any child windows
    const ImVec2                    Avail               = ImGui::GetContentRegionAvail();
    //const float                     P0_w                = TOTAL_W * m_style.OBJ_PROPERTIES_REL_WIDTH - PADDING;
    const float                     P0_w                = 0.65f * ImGui::GetContentRegionAvail().x;
    
    
    //  4.  "VERTEX BROWSER" FOR THE OBJECT...
    float                           sub_h               = 0.0f; // availY * m_style.ms_VERTEX_SUBBROWSER_HEIGHT;



    //  4A.     OBJECT PROPERTIES PANEL.
    ImGui::BeginChild("##Editor_Browser_ObjectPropertiesPanel", ImVec2(P0_w, Avail.y),     P0_FLAGS);
        _draw_obj_properties_panel(path, sel_idx);     // <-- NEW helper you just wrote
    ImGui::EndChild();
    ImGui::SameLine(0.0f );
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
            ImGui::BeginChild("##Editor_Browser_VertexSelectorColumn",      ImVec2(0.0f, sub_h),    C0_FLAGS);
                _draw_vertex_selector_column(path);
            ImGui::EndChild();
            ImGui::PopStyleColor();
            //
            ImGui::SameLine(0.0f );
            //
            //  4B.2.   RIGHT-HAND VERTEX BROWSER.
            const float             P1C1_w              = ImGui::GetContentRegionAvail().x;
            ImGui::PushStyleColor(ImGuiCol_ChildBg, BStyle.ms_CHILD_FRAME_BG2R);
            ImGui::BeginChild("##Editor_Browser_VertexInspectorColumn",     ImVec2(P1C1_w, sub_h),    C1_FLAGS);
                _draw_vertex_inspector_column(path);
            ImGui::EndChild();
            ImGui::PopStyleColor();
        //
        ImGui::PopStyleVar(2);   // border size, rounding
    //
    //
    //
    ImGui::EndChild();
    
    
    return;
}*/

/*{
    constexpr ImGuiColorEditFlags   COLOR_FLAGS     = ImGuiColorEditFlags_NoInputs;
    const ImGuiStyle &              style           = ImGui::GetStyle();
    //
    const size_t                    pidx            = *m_sel.paths.begin();
    
    
    //  CASE 0 :    ERROR...
    if ( pidx >= m_paths.size() ) {
        ImGui::TextDisabled("[invalid object]"); return;
    }
    Path &          path            = m_paths[pidx];
    bool            is_area         = path.is_area();



    //  1.  HEADER-ENTRY AND "DELETE" BUTTON...
    //  I   mGui::Text("%s. %zu  (%zu vertices)", pidx, path.verts.size());
    ImGui::Text( "%s (%zu vertices)", path.label.c_str(), path.verts.size() );
    ImGui::SameLine();
    if (ImGui::Button("Delete Object"))
    {
        _erase_path_and_orphans( static_cast<PathID>(pidx) );   // ← replaces direct m_paths.erase()
        m_sel.clear();
        m_inspector_vertex_idx = -1;
        return;
    }
    ImGui::Separator();



    //  2.  LINE-STROKE, AREA-FILL COLORS...
    {
        ImVec4          stroke_f            = u32_to_f4(path.style.stroke_color);
        ImVec4          fill_f              = u32_to_f4(path.style.fill_color);
        bool            stroke_dirty        = false;
        bool            fill_dirty          = false;


        utl::LeftLabelSimple("Stroke:");    ImGui::SameLine();
        stroke_dirty                        = ImGui::ColorEdit4( "##Editor_VertexBrowser_LineColor",    (float*)&stroke_f,  COLOR_FLAGS );
        //
        ImGui::SameLine();
        ImGui::BeginDisabled( !is_area );
            utl::LeftLabelSimple("Fill:");  ImGui::SameLine();
            fill_dirty                      = ImGui::ColorEdit4( "##Editor_VertexBrowser_FillColor",    (float*)&fill_f,    COLOR_FLAGS );
        ImGui::EndDisabled();
        
        if (stroke_dirty)   { path.style.stroke_color = f4_to_u32(stroke_f); }
        if (fill_dirty)     { path.style.fill_color   = f4_to_u32(fill_f); }
        if (!is_area)       { path.style.fill_color  &= 0x00FFFFFF; }   // clear alpha

        ImGui::SameLine(); 
    }


    //  3.  LINE WIDTH...
    {
        static constexpr float  min_width   = 0.25;
        static constexpr float  max_width   = 32;
        bool                    dirty       = false;
        float                   w           = path.style.stroke_width;
        
        utl::LeftLabelSimple("Line Width:");    ImGui::SameLine();
        ImGui::SetNextItemWidth(200.0f);
        //
        //  CASE 1 :    Value < 2.0f
        if (w < 2.0f) {
            dirty = ImGui::InputFloat("##Editor_VertexBrowser_LineWidth", &w,   0.125f,   0.25f,      "%.3f px");
        }
        //
        //  CASE 2 :    2.0 <= Value.
        else {
            dirty = ImGui::InputFloat("##Editor_VertexBrowser_LineWidth", &w,   1.0f,     2.0f,       "%.1f px");
        }
        //
        //
        //
        if ( dirty ) {
            w = std::clamp(w, min_width, max_width);
            path.style.stroke_width = w;
        }
    }
    ImGui::Separator();



    //  4.  "VERTEX BROWSER" FOR THE OBJECT...
    float availY = ImGui::GetContentRegionAvail().y;
    float sub_h  = availY * m_style.ms_VERTEX_SUBBROWSER_HEIGHT; 
    float filler = availY - sub_h - style.WindowPadding.y;
    
    if (filler > 0.0f)
        { ImGui::Dummy(ImVec2(0, filler)); }

    ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize,  m_style.ms_CHILD_BORDER2);
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding,    m_style.ms_CHILD_ROUND2);
    ImGui::PushStyleColor(ImGuiCol_ChildBg,             m_style.ms_CHILD_FRAME_BG2L);
    //
    //
    //  //  4.1.    LEFT-HAND VERTEX BROWSER.
        ImGui::BeginChild("##VertexPanel_Left", ImVec2(ms_LIST_COLUMN_WIDTH, sub_h), ImGuiChildFlags_Borders | ImGuiChildFlags_ResizeX);
            _draw_vertex_selector_column(path);
        ImGui::EndChild();
        ImGui::PopStyleColor();
        //
        ImGui::SameLine();
        //
        //  4.2.    RIGHT-HAND VERTEX BROWSER.
        ImGui::PushStyleColor(ImGuiCol_ChildBg, m_style.ms_CHILD_FRAME_BG2R);
        ImGui::BeginChild("##VertexPanel_Right", ImVec2(0, sub_h),
                          ImGuiChildFlags_Borders);
            _draw_vertex_inspector_column(path);
        ImGui::EndChild();
        ImGui::PopStyleColor();
    //
    //
    ImGui::PopStyleVar(2);   // border size, rounding
    
    
    return;
}*/






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


/*{
    constexpr ImGuiColorEditFlags   COLOR_FLAGS     = ImGuiColorEditFlags_NoInputs;
    const ImGuiStyle &              style           = ImGui::GetStyle();
    //
    const size_t                    pidx            = *m_sel.paths.begin();
    
    
    //  CASE 0 :    ERROR...
    if ( pidx >= m_paths.size() ) {
        ImGui::TextDisabled("[invalid object]"); return;
    }
    Path &          path            = m_paths[pidx];
    bool            is_area         = path.is_area();



    //  1.  HEADER-ENTRY AND "DELETE" BUTTON...
    //  I   mGui::Text("%s. %zu  (%zu vertices)", pidx, path.verts.size());
    ImGui::Text( "%s (%zu vertices)", path.label.c_str(), path.verts.size() );
    ImGui::SameLine();
    if (ImGui::Button("Delete Object"))
    {
        _erase_path_and_orphans( static_cast<PathID>(pidx) );   // ← replaces direct m_paths.erase()
        m_sel.clear();
        m_browser_S.m_inspector_vertex_idx = -1;
        return;
    }
    ImGui::Separator();



    //  2.  LINE-STROKE, AREA-FILL COLORS...
    {
        ImVec4          stroke_f            = u32_to_f4(path.style.stroke_color);
        ImVec4          fill_f              = u32_to_f4(path.style.fill_color);
        bool            stroke_dirty        = false;
        bool            fill_dirty          = false;


        utl::LeftLabelSimple("Stroke:");    ImGui::SameLine();
        stroke_dirty                        = ImGui::ColorEdit4( "##Editor_VertexBrowser_LineColor",    (float*)&stroke_f,  COLOR_FLAGS );
        //
        ImGui::SameLine();
        ImGui::BeginDisabled( !is_area );
            utl::LeftLabelSimple("Fill:");  ImGui::SameLine();
            fill_dirty                      = ImGui::ColorEdit4( "##Editor_VertexBrowser_FillColor",    (float*)&fill_f,    COLOR_FLAGS );
        ImGui::EndDisabled();
        
        if (stroke_dirty)   { path.style.stroke_color = f4_to_u32(stroke_f); }
        if (fill_dirty)     { path.style.fill_color   = f4_to_u32(fill_f); }
        if (!is_area)       { path.style.fill_color  &= 0x00FFFFFF; }   // clear alpha

        ImGui::SameLine(); 
    }


    //  3.  LINE WIDTH...
    {
        static constexpr float  min_width   = 0.25;
        static constexpr float  max_width   = 32;
        bool                    dirty       = false;
        float                   w           = path.style.stroke_width;
        
        utl::LeftLabelSimple("Line Width:");    ImGui::SameLine();
        ImGui::SetNextItemWidth(200.0f);
        //
        //  CASE 1 :    Value < 2.0f
        if (w < 2.0f) {
            dirty = ImGui::InputFloat("##Editor_VertexBrowser_LineWidth", &w,   0.125f,   0.25f,      "%.3f px");
        }
        //
        //  CASE 2 :    2.0 <= Value.
        else {
            dirty = ImGui::InputFloat("##Editor_VertexBrowser_LineWidth", &w,   1.0f,     2.0f,       "%.1f px");
        }
        //
        //
        //
        if ( dirty ) {
            w = std::clamp(w, min_width, max_width);
            path.style.stroke_width = w;
        }
    }
    ImGui::Separator();



    //  4.  "VERTEX BROWSER" FOR THE OBJECT...
    float availY = ImGui::GetContentRegionAvail().y;
    float sub_h  = availY * m_style.ms_VERTEX_SUBBROWSER_HEIGHT; 
    float filler = availY - sub_h - style.WindowPadding.y;
    
    if (filler > 0.0f)
        { ImGui::Dummy(ImVec2(0, filler)); }

    ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize,  m_style.ms_CHILD_BORDER2);
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding,    m_style.ms_CHILD_ROUND2);
    ImGui::PushStyleColor(ImGuiCol_ChildBg,             m_style.ms_CHILD_FRAME_BG2L);
    //
    //
    //  //  4.1.    LEFT-HAND VERTEX BROWSER.
        ImGui::BeginChild("##VertexPanel_Left", ImVec2(ms_LIST_COLUMN_WIDTH, sub_h), ImGuiChildFlags_Borders | ImGuiChildFlags_ResizeX);
            _draw_vertex_selector_column(path);
        ImGui::EndChild();
        ImGui::PopStyleColor();
        //
        ImGui::SameLine();
        //
        //  4.2.    RIGHT-HAND VERTEX BROWSER.
        ImGui::PushStyleColor(ImGuiCol_ChildBg, m_style.ms_CHILD_FRAME_BG2R);
        ImGui::BeginChild("##VertexPanel_Right", ImVec2(0, sub_h),
                          ImGuiChildFlags_Borders);
            _draw_vertex_inspector_column(path);
        ImGui::EndChild();
        ImGui::PopStyleColor();
    //
    //
    ImGui::PopStyleVar(2);   // border size, rounding
    
    
    return;
}*/




//  "_draw_obj_properties_panel"
//
void Editor::_draw_obj_properties_panel(Path & path, const size_t pidx)
{
    constexpr ImGuiColorEditFlags   COLOR_FLAGS         = ImGuiColorEditFlags_NoInputs;
    //
    BrowserStyle &                  BStyle              = this->m_style.browser_style;
    const bool                      is_area             = path.is_area();
    const float &                   LABEL_W             = BStyle.ms_BROWSER_VERTEX_LABEL_WIDTH;
    const float &                   WIDGET_W            = BStyle.ms_BROWSER_VERTEX_WIDGET_WIDTH;
    //
    static constexpr size_t         TITLE_SIZE          = 128;
    static char                     title [TITLE_SIZE];   // safe head-room
    static PathID                   cache_id            = static_cast<PathID>(-1);
    const bool                      PAYLOAD             = path.kind != PathKind::Default;
    
    //     "edit_u32_colour"  // ── colour editors ────────────────────────────────────────────────
    [[maybe_unused]] auto           edit_u32_colour     = [&](const char * label, ImU32 & col_u32) {
        ImVec4  col_f   = ImGui::ColorConvertU32ToFloat4(col_u32);
        if ( ImGui::ColorEdit4(label, &col_f.x, COLOR_FLAGS) )      { col_u32 = ImGui::ColorConvertFloat4ToU32(col_f); }
    };



    S.PushFont(Font::Main);

    //  CASE 0 :    ERROR...
    if ( pidx >= m_paths.size() )   { ImGui::SeparatorText("[invalid object]"); S.PopFont(); cache_id = static_cast<PathID>(-1); return; }
    
    
    
    
    
    
    //      0.1.    UPDATE TITLE IF SELECTION CHANGED...
    if (cache_id != pidx )           {
        cache_id        = static_cast<PathID>( pidx );
        int retcode     = std::snprintf( title, TITLE_SIZE, Path::ms_DEF_PATH_TITLE_FMT_STRING, path.label.c_str(), path.id );
        
        if (retcode < 0) [[unlikely]] {//  Log a warning message if truncation takes place.
            auto message = std::format( "snprintf truncated Path title.\n"
                                        "Path ID: {}.  title: \"{}\".  buffer-size: {}.  return value: \"{}\".",
                                        pidx, title, TITLE_SIZE, retcode );
            CB_LOG( LogLevel::Warning, message );
        }
    }
    


    //  1.  HEADER-ENTRY AND "DELETE" BUTTON...
    //  ImGui::Text( "%s (%zu vertices)", path.label.c_str(), path.verts.size() );
    ImGui::SeparatorText(title);
    
    //  2.  "DELETE" BUTTON...
    S.PopFont();



    if (!PAYLOAD) {
    //
    //
    //
        //  2.  LINE-STROKE, AREA-FILL COLORS...
        {
            ImVec4          stroke_f            = u32_to_f4(path.style.stroke_color);
            ImVec4          fill_f              = u32_to_f4(path.style.fill_color);
            bool            stroke_dirty        = false;
            bool            fill_dirty          = false;


            this->left_label("Line Color:", LABEL_W, WIDGET_W);
            stroke_dirty                        = ImGui::ColorEdit4( "##Editor_VertexInspector_LineColor",    (float*)&stroke_f,  COLOR_FLAGS );
            //
            //
            ImGui::BeginDisabled( !is_area );
                this->left_label("Fill Color:", LABEL_W, WIDGET_W);
                fill_dirty                      = ImGui::ColorEdit4( "##Editor_VertexInspector_FillColor",    (float*)&fill_f,    COLOR_FLAGS );
            ImGui::EndDisabled();
            
            if (stroke_dirty)   { path.style.stroke_color = f4_to_u32(stroke_f); }
            if (fill_dirty)     { path.style.fill_color   = f4_to_u32(fill_f); }
            if (!is_area)       { path.style.fill_color  &= 0x00FFFFFF; }   // clear alpha

        }




        //  3.  LINE WIDTH...
        {
            static constexpr float  min_width   = 0.25;
            static constexpr float  max_width   = 32;
            bool                    dirty       = false;
            float                   w           = path.style.stroke_width;
            
            this->left_label("Line Color:", LABEL_W, WIDGET_W);
            ImGui::SetNextItemWidth(200.0f);
            //
            //  CASE 1 :    Value < 2.0f
            if (w < 2.0f) {
                dirty = ImGui::InputFloat("##Editor_VertexBrowser_LineWidth", &w,   0.125f,   0.25f,      "%.4f px");
            }
            //
            //  CASE 2 :    2.0 <= Value.
            else {
                dirty = ImGui::InputFloat("##Editor_VertexBrowser_LineWidth", &w,   1.0f,     2.0f,       "%.2f px");
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
    ImGui::NewLine();
    ImGui::BeginDisabled(true);
    ImGui::SeparatorText("Properties");
    ImGui::EndDisabled();
    {
        this->left_label("Payload:", 196.0f, 256.0f);    path.ui_kind();
        
        if (!PAYLOAD)   { ImGui::Separator(); }
        
        if (path.kind == PathKind::Default) {
            ImGui::TextDisabled("None");
        }
        else {
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
//
//      NEW STUFF...
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
