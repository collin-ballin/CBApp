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
    this->m_window_class.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_HiddenTabBar;
    
    
    //  INITIALIZE EACH RESIDENT OVERLAY-WINDOW...
    for (size_t i = 0; i < static_cast<size_t>( Resident::COUNT ); ++i)
    {
        Resident idx = static_cast<Resident>(i);
        this->_dispatch_resident_draw_fn( idx );
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
    S.PushFont(Font::Small);
    
    
    //  2.  LEFTHAND BROWSER SELECTION MENU...
    ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize,  m_style.ms_CHILD_BORDER1);
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding,    m_style.ms_CHILD_ROUND1);
    ImGui::PushStyleColor(ImGuiCol_ChildBg,             m_style.ms_CHILD_FRAME_BG1L);
    //
    //
    //
        //  2.  OBJECT BROWSER'S SELECTOR COLUMN    (MAIN, LEFT-HAND MENU OF THE ENTIRE BROWSER)...
        ImGui::BeginChild("##Editor_Browser_Left", ImVec2(this->ms_LIST_COLUMN_WIDTH, 0), ImGuiChildFlags_Borders | ImGuiChildFlags_ResizeX);
            _draw_obj_selector_column();
        ImGui::EndChild();
        ImGui::PopStyleColor();
        //
        ImGui::SameLine();
        //
        //
        //  3.  OBJECT BROWSER'S INSPECTOR COLUMN   (MAIN, RIGHT-HAND MENU OF THE ENTIRE BROWSER)...
        ImGui::PushStyleColor(ImGuiCol_ChildBg,         m_style.ms_CHILD_FRAME_BG1R);
        ImGui::BeginChild("##Editor_Browser_Right", ImVec2(0, 0), ImGuiChildFlags_Borders);
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
                                 IM_ARRAYSIZE(m_browser_filter.InputBuf)) )
        { m_browser_filter.Build(); }
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
            if ( !m_browser_filter.PassFilter(path.label.c_str()) )      { continue; }


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
}

/* {
//  "_draw_obj_selector_column"
//
void Editor::_draw_obj_selector_column(void)
{
    using namespace icon;
    const ImU32 col_text  = ImGui::GetColorU32(ImGuiCol_Text);
    const ImU32 col_dim   = ImGui::GetColorU32(ImGuiCol_TextDisabled);
    const ImU32 col_frame = ImGui::GetColorU32(ImGuiCol_FrameBg);

    //–––– rename-mode state (static, survives frames) ––––//
    static int  rename_idx   = -1;          // -1 → no row in rename
    static char rename_buf[64] = {};

    //–––– filter box ––––//
    ImGui::SetNextItemWidth(-FLT_MIN);
    if (ImGui::InputTextWithHint("##Editor_Browser_LeftFilter", "filter",
                                 m_browser_filter.InputBuf,
                                 IM_ARRAYSIZE(m_browser_filter.InputBuf)))
        m_browser_filter.Build();
    ImGui::Separator();

    ImGuiListClipper clipper;
    clipper.Begin(static_cast<int>(m_paths.size()), -1);

    while (clipper.Step())
        for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; ++i)
        {
            Path&  path          = m_paths[i];
            bool   selected      = m_sel.paths.count(static_cast<size_t>(i));
            bool   mutable_path  = path.is_mutable();
            auto   sel_flags     = mutable_path
                                   ? ImGuiSelectableFlags_SpanAllColumns
                                     | ImGuiSelectableFlags_AllowDoubleClick
                                   : ImGuiSelectableFlags_Disabled
                                     | ImGuiSelectableFlags_AllowDoubleClick;

            if (!m_browser_filter.PassFilter(path.label.c_str()))
                continue;

            ImGui::PushID(i);
            ImGui::BeginGroup();                 // full row

            // eye toggle //
            ImVec2 pos = ImGui::GetCursorScreenPos();
            ImGui::InvisibleButton("##eye", {CELL_SZ, CELL_SZ});
            if (ImGui::IsItemClicked()) { path.visible = !path.visible; _prune_selection_mutability(); }
            draw_icon_background(ImGui::GetWindowDrawList(), pos, {CELL_SZ, CELL_SZ}, selected ? col_frame : 0);
            (path.visible ? draw_eye_icon : draw_eye_off_icon)
                (ImGui::GetWindowDrawList(), pos, {CELL_SZ, CELL_SZ}, path.visible ? col_text : col_dim);
            ImGui::SameLine(0.0f, m_style.ms_BROWSER_BUTTON_SEP);

            // lock toggle //
            pos = ImGui::GetCursorScreenPos();
            ImGui::InvisibleButton("##lock", {CELL_SZ, CELL_SZ});
            if (ImGui::IsItemClicked()) { path.locked = !path.locked; _prune_selection_mutability(); }
            ImGui::SameLine(0.0f, m_style.ms_BROWSER_SELECTABLE_SEP);
            draw_icon_background(ImGui::GetWindowDrawList(), pos, {CELL_SZ, CELL_SZ}, selected ? col_frame : 0);
            (path.locked ? draw_lock_icon : draw_unlock_icon)
                (ImGui::GetWindowDrawList(), pos, {CELL_SZ, CELL_SZ}, path.locked ? col_text : col_dim);

            ImGui::SameLine(0.0f, m_style.ms_BROWSER_SELECTABLE_SEP);

            // label or rename field //
            bool row_in_rename = (rename_idx == i);
            if (row_in_rename)
            {
                ImGui::PushItemWidth(-FLT_MIN);
                if (ImGui::InputText("##ren", rename_buf, IM_ARRAYSIZE(rename_buf),
                                     ImGuiInputTextFlags_EnterReturnsTrue
                                     | ImGuiInputTextFlags_AutoSelectAll))
                {                       // commit on Enter
                    path.label = rename_buf;
                    rename_idx = -1;
                }
                if (!ImGui::IsItemActive() && ImGui::IsItemDeactivated())
                    rename_idx = -1;     // commit on focus loss
                if (ImGui::IsKeyPressed(ImGuiKey_Escape))
                    rename_idx = -1;     // cancel
                ImGui::PopItemWidth();
            }
            else
            {
                ImGui::PushStyleColor(ImGuiCol_Text,
                                      mutable_path ? col_text : col_dim);
                if (ImGui::Selectable(path.label.c_str(), selected, sel_flags))
                    _handle_selection_click(i, mutable_path);      // ← your existing helper
                ImGui::PopStyleColor();

                // start rename on double-click //
                if (mutable_path && ImGui::IsItemHovered()
                    && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
                {
                    rename_idx = i;
                    std::strncpy(rename_buf, path.label.c_str(),
                                 IM_ARRAYSIZE(rename_buf));
                }
            }

            ImGui::EndGroup();
            ImGui::PopID();
        }

    clipper.End();
}
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
    //constexpr ImGuiChildFlags       P0_FLAGS            = ImGuiChildFlags_ResizeX       | ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_NoScrollbar;
    constexpr ImGuiChildFlags       P0_FLAGS            = ImGuiChildFlags_AutoResizeX   | ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_NoScrollbar;
    constexpr ImGuiChildFlags       P1_FLAGS            = ImGuiChildFlags_AutoResizeX   | ImGuiChildFlags_AlwaysUseWindowPadding | ImGuiChildFlags_Borders;
    constexpr ImGuiChildFlags       C0_FLAGS            = ImGuiChildFlags_ResizeX       | ImGuiChildFlags_AlwaysUseWindowPadding | ImGuiChildFlags_Borders;
    constexpr ImGuiChildFlags       C1_FLAGS            = ImGuiChildFlags_AutoResizeX   | ImGuiChildFlags_AlwaysUseWindowPadding | ImGuiChildFlags_Borders;
    const ImGuiStyle &              style               = ImGui::GetStyle();

    const size_t                    sel_idx             = *m_sel.paths.begin();   // only element
    Path &                          path                = m_paths[sel_idx];
    
    
    //  Compute adaptive column widths BEFORE we emit any child windows
    const ImVec2                    Avail               = ImGui::GetContentRegionAvail();
    const float                     TOTAL_W             = Avail.x;            // free width right now
    const float                     PADDING             = 2 * style.ItemSpacing.x;
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
        ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize,  m_style.ms_CHILD_BORDER2);
        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding,    m_style.ms_CHILD_ROUND2);
        ImGui::PushStyleColor(ImGuiCol_ChildBg,             m_style.ms_CHILD_FRAME_BG2L);
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
            ImGui::PushStyleColor(ImGuiCol_ChildBg, m_style.ms_CHILD_FRAME_BG2R);
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
        for (int row = clipper.DisplayStart; row < clipper.DisplayEnd; ++row)
        {
            char lbl[8]; std::snprintf(lbl, sizeof(lbl), Vertex::ms_DEF_VERTEX_SELECTOR_FMT_STRING, row);
            bool selected = (row == m_browser_S.m_inspector_vertex_idx);
            if (ImGui::Selectable(lbl, selected))
                m_browser_S.m_inspector_vertex_idx = (selected ? -1 : row);      // toggle
        }
    clipper.End();
}



//  "_draw_vertex_inspector_column"
//
void Editor::_draw_vertex_inspector_column(Path & path)
{
    static constexpr size_t     TITLE_SIZE      = 32ULL;
    const float &               LABEL_W         = m_style.ms_BROWSER_OBJ_LABEL_WIDTH;
    const float &               WIDGET_W        = m_style.ms_BROWSER_OBJ_WIDGET_WIDTH;
    
    
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
void Editor::_draw_obj_properties_panel(Path & path, const PathID pidx)
{
    constexpr ImGuiColorEditFlags   COLOR_FLAGS         = ImGuiColorEditFlags_NoInputs;
    //
    const bool                      is_area             = path.is_area();
    const float &                   LABEL_W             = m_style.ms_BROWSER_VERTEX_LABEL_WIDTH;
    const float &                   WIDGET_W            = m_style.ms_BROWSER_VERTEX_WIDGET_WIDTH;
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
        cache_id        = pidx;
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
    //  ImGui::NewLine();
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
