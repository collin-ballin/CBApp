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








//  Destructor.
//
Editor::~Editor(void)
{
    this->_clear_all();
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
    static constexpr float      ms_BROWSER_BUTTON_SEP       = 8.0f;
    static constexpr float      ms_BROWSER_SELECTABLE_SEP   = 16.0f;
    //
    //  COLORS FROM CURRENT STYLE (Non-icon constants)
    const ImU32                 col_text                    = ImGui::GetColorU32(ImGuiCol_Text);
    const ImU32                 col_dim                     = ImGui::GetColorU32(ImGuiCol_TextDisabled);
    const ImU32                 col_frame                   = ImGui::GetColorU32(ImGuiCol_FrameBg);
    ImGuiListClipper            clipper;


    //  1.  SEARCH-QUERY BOX
    ImGui::SetNextItemWidth(-FLT_MIN);
    if ( ImGui::InputTextWithHint("##Editor_Browser_LeftFilter", "filter",
                                 m_browser_filter.InputBuf,
                                 IM_ARRAYSIZE(m_browser_filter.InputBuf)) )
        { m_browser_filter.Build(); }
        
        
    ImGui::Separator();


    clipper.Begin(static_cast<int>(m_paths.size()), -1);


    //  2.  DRAWING EACH OBJECT IN THE LEFT-HAND SELECTION COLUMN OF THE BROWSER...
    while ( clipper.Step() )
    {
        for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; ++i)
        {
            char                    label[12];
            Path &                  path            = m_paths[i];
            const bool              selected        = m_sel.paths.count(static_cast<size_t>(i));
            const bool              mutable_path    = path.is_mutable();          // NEW – cached
            ImGuiSelectableFlags    sel_flags       = (!mutable_path ? ImGuiSelectableFlags_Disabled | ImGuiSelectableFlags_AllowDoubleClick : ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowDoubleClick);
            
            
            //  CASE 0 :    EARLY-OUT IF FILTER REMOVES OBJ...
            if ( !m_browser_filter.PassFilter(path.label.c_str()) )      { continue; }



            //std::snprintf(label, sizeof(label), "Path %02d", i);
            ImGui::PushID(i);
            ImGui::BeginGroup();


            //  2.1.    "EYE" BUTTON (TO TOGGLE OBJECT'S VISIBILITY)...
            ImVec2      pos             = ImGui::GetCursorScreenPos();
            ImGui::InvisibleButton("##Editor_Browser_VisibilityButton", { CELL_SZ, CELL_SZ });
            if ( ImGui::IsItemClicked() )       { path.visible = !path.visible; _prune_selection_mutability(); }


            //  Draw EYE or CLOSED-EYE Icon.
            draw_icon_background(ImGui::GetWindowDrawList(), pos, { CELL_SZ, CELL_SZ }, selected ? col_frame : 0);
            if ( path.visible )                 { draw_eye_icon(ImGui::GetWindowDrawList(), pos, { CELL_SZ, CELL_SZ }, col_text); }
            else                                { draw_eye_off_icon(ImGui::GetWindowDrawList(), pos, { CELL_SZ, CELL_SZ }, col_dim); }

            ImGui::SameLine(0.0f, ms_BROWSER_BUTTON_SEP);


            //  2.2.    "LOCK" BUTTON (TO TOGGLE OBJECT'S LOCKED-STATE)...
            pos = ImGui::GetCursorScreenPos();
            ImGui::InvisibleButton("##Editor_Browser_LockButton", { CELL_SZ, CELL_SZ });
            if ( ImGui::IsItemClicked() )       { path.locked = !path.locked; _prune_selection_mutability(); }

            ImGui::SameLine(0.0f, ms_BROWSER_SELECTABLE_SEP);
            
            //  Draw LOCK or UNLOCK Icon.
            draw_icon_background(ImGui::GetWindowDrawList(), pos, { CELL_SZ, CELL_SZ }, selected ? col_frame : 0);
            if (path.locked)                    { draw_lock_icon(ImGui::GetWindowDrawList(), pos, { CELL_SZ, CELL_SZ }, col_text); }
            else                                { draw_unlock_icon(ImGui::GetWindowDrawList(), pos, { CELL_SZ, CELL_SZ }, col_dim); }



            ImGui::SameLine(0.0f, ms_BROWSER_SELECTABLE_SEP);     // Spacing before selectable object.
            
            
            
            //  3.1.    EDITING THE NAME FOR THE PATH...
            ImGui::PushStyleColor( ImGuiCol_Text, (!mutable_path ? col_dim : col_text) ); // NEW – dim on invisible too
            if (ImGui::Selectable(path.label.c_str(), selected, sel_flags))
            {
                const bool ctrl  = ImGui::GetIO().KeyCtrl;
                const bool shift = ImGui::GetIO().KeyShift;

                if (!ctrl && !shift) {
                    this->reset_selection();
                    if (mutable_path) m_sel.paths.insert(i);
                    m_browser_anchor = i;
                }
                else if (shift && m_browser_anchor >= 0) {
                    int lo = std::min(m_browser_anchor, i);
                    int hi = std::max(m_browser_anchor, i);
                    if (!ctrl) this->reset_selection();
                    for (int k = lo; k <= hi; ++k)
                        if (m_paths[k].is_mutable())
                            m_sel.paths.insert(k);
                }
                else if (ctrl && mutable_path) {
                    if (!m_sel.paths.erase(i)) m_sel.paths.insert(i);
                    m_browser_anchor = i;
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
    // 1) filter input --------------------------------------------------
    ImGui::SetNextItemWidth(-FLT_MIN);
    if (ImGui::InputTextWithHint("##Editor_Browser_LeftFilter", "filter",
                                 m_browser_filter.InputBuf,
                                 IM_ARRAYSIZE(m_browser_filter.InputBuf)))
        m_browser_filter.Build();

    ImGui::Separator();

    const int total = static_cast<int>(m_paths.size());
    ImGuiListClipper clipper;
    clipper.Begin(total, -1);


    while ( clipper.Step() )
    {
        for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; ++i)
        {
            char label[12]; std::snprintf(label, sizeof(label), "Path%02d", i);
            if (!m_browser_filter.PassFilter(label)) continue;

            bool selected = m_sel.paths.count(static_cast<size_t>(i));
            if (ImGui::Selectable(label, selected))
            {
                const bool ctrl  = ImGui::GetIO().KeyCtrl;
                const bool shift = ImGui::GetIO().KeyShift;

                if (!ctrl && !shift)
                {   this->reset_selection(); //m_sel.clear();
                    m_sel.paths.insert(i);  m_browser_anchor = i;
                }
                else if (shift && m_browser_anchor >= 0)
                {
                    int lo = std::min(m_browser_anchor, i), hi = std::max(m_browser_anchor, i);
                    if (!ctrl) {
                        this->reset_selection(); //m_sel.clear();
                    }
                    for (int k = lo; k <= hi; ++k)      { m_sel.paths.insert(k); }
                }
                else if (ctrl)
                {
                    if ( !m_sel.paths.erase(i) )        { m_sel.paths.insert(i); m_browser_anchor = i; }
                }

                _rebuild_vertex_selection();   // keep vertices in sync
            }
        }
    }
        
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
    if ( sel_paths == 0 )       { ImGui::TextDisabled("No selection."); return; }


    if (sel_paths == 1)         { _draw_single_obj_inspector();     }
    else                        { _draw_multi_obj_inspector();      }
    
    return;
}


//  "_draw_single_obj_inspector"
//      Draws the right-hand, INSPECTOR COLUMN for the case: SELECTION CONTAINS ONLY ONE OBJECT.
//
void Editor::_draw_single_obj_inspector(void)
{
    constexpr ImGuiChildFlags       P0_FLAGS            = ImGuiChildFlags_ResizeX       | ImGuiChildFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_NoScrollbar;
    constexpr ImGuiChildFlags       P1_FLAGS            = ImGuiChildFlags_AutoResizeX   | ImGuiChildFlags_AlwaysUseWindowPadding | ImGuiChildFlags_Borders;
    constexpr ImGuiChildFlags       C0_FLAGS            = ImGuiChildFlags_ResizeX       | ImGuiChildFlags_AlwaysUseWindowPadding | ImGuiChildFlags_Borders;
    constexpr ImGuiChildFlags       C1_FLAGS            = ImGuiChildFlags_AutoResizeX   | ImGuiChildFlags_AlwaysUseWindowPadding | ImGuiChildFlags_Borders;
    const ImGuiStyle &              style               = ImGui::GetStyle();


    const size_t                    sel_idx             = *m_sel.paths.begin();   // only element
    Path &                          path                = m_paths[sel_idx];
    
    
    //  Compute adaptive column widths BEFORE we emit any child windows
    const float                     TOTAL_W             = ImGui::GetContentRegionAvail().x;            // free width right now
    const float                     PADDING             = 2 * style.ItemSpacing.x;
    const float                     P0_w                = TOTAL_W * m_style.OBJ_PROPERTIES_REL_WIDTH - PADDING;
    

    
    


    //  4.  "VERTEX BROWSER" FOR THE OBJECT...
    float                           availY              = ImGui::GetContentRegionAvail().y;
    float                           sub_h               = 0.0f; // availY * m_style.ms_VERTEX_SUBBROWSER_HEIGHT;



    //  4A.     OBJECT PROPERTIES PANEL.
    ImGui::BeginChild("##Editor_Browser_ObjectPropertiesPanel", ImVec2(P0_w, 0.0f),     P0_FLAGS);
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
    ImGui::Text("%zu paths selected", m_sel.paths.size());
    ImGui::Separator();

    if (ImGui::Button("Delete Selections", {150,0}))
    {
        std::vector<size_t> idxs(m_sel.paths.begin(), m_sel.paths.end());
        std::sort(idxs.rbegin(), idxs.rend());
        for (size_t i : idxs)
            if (i < m_paths.size())
                m_paths.erase(m_paths.begin() + static_cast<long>(i));

        this->reset_selection();    // m_sel.clear();
        m_inspector_vertex_idx = -1;
    }
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
            char lbl[8]; std::snprintf(lbl, sizeof(lbl), Vertex::ms_DEF_VERTEX_FMT_STRING, row);
            bool selected = (row == m_inspector_vertex_idx);
            if (ImGui::Selectable(lbl, selected))
                m_inspector_vertex_idx = (selected ? -1 : row);      // toggle
        }
    clipper.End();
}



//  "_draw_vertex_inspector_column"
//
void Editor::_draw_vertex_inspector_column(Path & path)
{
    //  CASE 0 :    NO VALID SELECTION...
    if (m_inspector_vertex_idx < 0 ||
        m_inspector_vertex_idx >= static_cast<int>(path.verts.size()))
    {
        ImGui::TextDisabled("Select a vertex from the left hand column...");
        return;
    }

    //  0.  OBTAIN POINTER TO VERTEX...
    uint32_t    vid     = path.verts[static_cast<size_t>(m_inspector_vertex_idx)];
    Vertex *    v       = find_vertex_mut(m_vertices, vid);
    
    
    //  CASE 1 :    STALE VERTEX...
    if ( !v ) {
        ImGui::TextDisabled("[stale vertex]"); return;
    }



    //  1.  HEADER CONTENT...       "Vertex ID"
    //
    ImGui::Text( Vertex::ms_DEF_VERTEX_LABEL_FMT_STRING, m_inspector_vertex_idx, vid );
    
    ImGui::SameLine();
    
    if ( ImGui::Button("Delete Vertex##Editor_Browser_DeleteVertexButton", {120,0}) )
    {
        _erase_vertex_and_fix_paths(vid);
        m_inspector_vertex_idx = -1;
        _rebuild_vertex_selection();
    }
    
    
    
    ImGui::Separator();
    //
    //      1.1.    Constants:
    const float     grid        = m_style.GRID_STEP / m_cam.zoom_mag;
    const float     speed       = 0.1f * grid;
    auto            snap        = [grid](float f){ return std::round(f / grid) * grid; };
    bool            dirty       = false;
    int             kind_idx    = static_cast<int>(v->kind);



    //  2.  WIDGETS...
    //
    //      2.1.    Position:
    utl::LeftLabel("Position:");
    dirty                      |= ImGui::DragFloat2("##Editor_VertexBrowser_Pos", &v->x, speed, -FLT_MAX, FLT_MAX, "%.3f");
    //
    if ( dirty && /*!ImGui::IsItemActive() && */ this->want_snap() ) {
        dirty       = false;
        ImVec2 s    = snap_to_grid({v->x, v->y});
        v->x        = s.x;
        v->y        = s.y;
    }



    //      2.2.    Bézier Handles / Control Points
    ImGui::SeparatorText("Handles");
    //
    //              2.2A    ANCHOR TYPE (corner / smooth / symmetric):
    {
        utl::LeftLabel("Anchor Type:");
        dirty = ImGui::Combo("##Editor_VertexBrowser_AnchorType", &kind_idx,
                             ANCHOR_TYPE_NAMES.data(), static_cast<int>( AnchorType::COUNT ));          // <- int, not enum
        if (dirty) {
            v->kind     = static_cast<AnchorType>(kind_idx);
            dirty       = false;
        }
    }
    //
    //
    //              2.2B    OUTWARD (to next vertex):
    //
    utl::LeftLabel("Outward:");
    dirty               = ImGui::DragFloat2("##Editor_VertexBrowser_OutwardControl",    &v->out_handle.x,   speed,  -FLT_MAX,   FLT_MAX,    "%.3f");
    if ( dirty && !ImGui::IsItemActive() )
    {
        v->out_handle.x     = snap(v->out_handle.x);
        v->out_handle.y     = snap(v->out_handle.y);
        mirror_handles<VertexID>(*v, /*dragging_out=*/true);  // keep smooth/symmetric rule
        dirty               = false;
    }
    //
    //
    //              2.2C    INWARD (from previous vertex):
    utl::LeftLabel("Inward:");
    //
    dirty              = ImGui::DragFloat2("##Editor_VertexBrowser_InwardControl",     &v->in_handle.x,    speed,  -FLT_MAX,   FLT_MAX,    "%.3f");
    if ( dirty && !ImGui::IsItemActive() ) {
        v->in_handle.x      = snap(v->in_handle.x);
        v->in_handle.y      = snap(v->in_handle.y);
        mirror_handles<VertexID>(*v, /*dragging_out=*/false);
        dirty               = false;
    }






    ImGui::Separator();
    if (ImGui::Button("Delete Vertex", {120,0}))
    {
        _erase_vertex_and_fix_paths(vid);
        m_inspector_vertex_idx = -1;
        _rebuild_vertex_selection();
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




//  "_draw_obj_properties_panel"
//
void Editor::_draw_obj_properties_panel(Path & path, size_t pidx)
{
    constexpr ImGuiColorEditFlags   COLOR_FLAGS         = ImGuiColorEditFlags_NoInputs;
    const bool                      is_area             = path.is_area();
    
    //     "edit_u32_colour"  // ── colour editors ────────────────────────────────────────────────
    auto                            edit_u32_colour     = [&](const char * label, ImU32 & col_u32) {
        ImVec4  col_f   = ImGui::ColorConvertU32ToFloat4(col_u32);
        if ( ImGui::ColorEdit4(label, &col_f.x, COLOR_FLAGS) )      { col_u32 = ImGui::ColorConvertFloat4ToU32(col_f); }
    };


    
    //  CASE 0 :    ERROR...
    if ( pidx >= m_paths.size() ) {
        ImGui::TextDisabled("[invalid object]"); return;
    }



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
        stroke_dirty                        = ImGui::ColorEdit4( "##Editor_VertexInspector_LineColor",    (float*)&stroke_f,  COLOR_FLAGS );
        //
        //
        ImGui::BeginDisabled( !is_area );
            utl::LeftLabelSimple("Fill:");  ImGui::SameLine();
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
        
        utl::LeftLabelSimple("Line Width:");    ImGui::SameLine();
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
    ImGui::Separator();
    
    

    //  4.  Z-ORDER...
    {
        ImGui::SeparatorText("Z-Order");
        if ( ImGui::SmallButton("Send Backward") )      { send_selection_backward(); }
        
        ImGui::SameLine();
        
        if ( ImGui::SmallButton("Bring Forward") )      { bring_selection_forward(); }
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
