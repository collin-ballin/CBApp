/***********************************************************************************
*
*       *********************************************************************
*       ****             E D I T O R . C P P  ____  F I L E              ****
*       *********************************************************************
*
*              AUTHOR:      Collin A. Bond
*               DATED:      June 12, 2025.
*
**************************************************************************************
**************************************************************************************/
#include "widgets/editor/editor.h"



namespace cb { //     BEGINNING NAMESPACE "cb"...
// *************************************************************************** //
// *************************************************************************** //






//  0.      STATIC VARIABLES...
// *************************************************************************** //
// *************************************************************************** //


//  0.      STATIC FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //













// *************************************************************************** //
//
//
//
//  2.  "BEGIN" HELPERS...
// *************************************************************************** //
// *************************************************************************** //

//  "_mode_switch_hotkeys"
//
inline void Editor::_mode_switch_hotkeys([[maybe_unused]] const Interaction & it)
{
    ImGuiIO &           io                  = ImGui::GetIO();
    const bool          shift               = io.KeyShift,          ctrl    = io.KeyCtrl,
                        alt                 = io.KeyAlt,            super   = io.KeySuper;
    const bool          no_mod              = !ctrl && !shift && !alt && !super;
    const bool          only_shift          = !ctrl &&  shift && !alt && !super;
    const bool          lmb_held            = ImGui::IsMouseDown(ImGuiMouseButton_Left);
    //
    static auto &       shape_entry         = m_residents[Resident::Shape];
    static Overlay &    shape_resident      = *m_overlays.lookup_resident(shape_entry.id);
    //
    //  static auto &       selection_entry     = m_residents[Resident::Selection];
    //  static Overlay *    selection_resident  = m_overlays.lookup_resident(shape_entry.id);


    if ( it.space || !it.hovered || lmb_held )  return;   // Early-out if [SPACE], [NOT HOVERING OVER CANVAS], or [LMB IS HELD DOWN]...
    


    //  3.      MODE SWITCH BEHAVIORS   [NO KEY MODS]...
    if ( no_mod )
    {
        if ( ImGui::IsKeyPressed(ImGuiKey_V)                            )                   m_mode = Mode::Default;
        if ( ImGui::IsKeyPressed(ImGuiKey_N)                            )                   m_mode = Mode::Point;
        if ( ImGui::IsKeyPressed(ImGuiKey_P)                            )                   m_mode = Mode::Pen;
        if ( ImGui::IsKeyPressed(ImGuiKey_C)                            )                   m_mode = Mode::Scissor;
        if ( ImGui::IsKeyPressed(ImGuiKey_S)                            )                   m_mode = Mode::Shape;
        if ( ImGui::IsKeyPressed(ImGuiKey_Equal)                        )                   m_mode = Mode::AddAnchor;
        if ( ImGui::IsKeyPressed(ImGuiKey_Minus)                        )                   m_mode = Mode::RemoveAnchor;
    }
    //
    //  3.1     MODE SWITCH BEHAVIORS   [WITH SHIFT]...
    else if (only_shift)
    {
        if ( !ctrl && shift && !alt && !super && ImGui::IsKeyPressed(ImGuiKey_C)    )       m_mode = Mode::EditAnchor;
    }
    
    
    //  4.      CLEAR LINGERING STATE BEHAVIORS...
    if ( m_mode != Mode::Pen )    { this->reset_pen(); } // m_pen = {};     //  Leaving the Pen-Tool resets current path appending.
    shape_resident.visible          = ( m_mode == Mode::Shape );            //  Leaving the Shape-Tool closes the overlay window.
    
    
    //  5.      INVOKE GRID SHORTCUT BEHAVIORS...
    this->_grid_handle_shortcuts();
    
    //  bool lmbHeld = ImGui::IsMouseDown(ImGuiMouseButton_Left);
    //  bool lmbHeldRaw = io.MouseDown[0];          // 0 == left button
    //  float heldFor   = io.MouseDownDuration[0];


    return;
}


//  "_dispatch_mode_handler"
//
inline void Editor::_dispatch_mode_handler([[maybe_unused]] const Interaction & it)
{
    if ( !(it.space && ImGui::IsMouseDown(ImGuiMouseButton_Left)) )
    {
        switch (m_mode) {
            case Mode::Default:         _handle_default(it);           break;
            case Mode::Line:            _handle_line(it);              break;
            case Mode::Point:           _handle_point(it);             break;
            case Mode::Pen:             _handle_pen(it);               break;
            case Mode::Scissor:         _handle_scissor(it);           break;
            case Mode::Shape:           _handle_shape(it);             break;
            case Mode::AddAnchor:       _handle_add_anchor(it);        break;
            case Mode::RemoveAnchor:    _handle_remove_anchor(it);     break;
            case Mode::EditAnchor:      _handle_edit_anchor(it);       break;
            default: break;
        }
    }
        
    return;
}



// *************************************************************************** //
//
//
//
//  2.  PUBLIC API...
// *************************************************************************** //
// *************************************************************************** //

//  "Begin"
//
void Editor::Begin(const char * /*id*/)
{
    ImGuiIO &               io              = ImGui::GetIO();
    const bool              space           = ImGui::IsKeyDown(ImGuiKey_Space);
    const bool              zoom_enabled    = _mode_has(CBCapabilityFlags_Zoom) && (!io.MouseDown[0]);
    //
    m_avail                                 = ImGui::GetContentRegionAvail();       //  1. Canvas size & plot flags
    m_avail.x                               = std::max(m_avail.x, 50.f);
    m_avail.y                               = std::max(m_avail.y, 50.f);
    //
    ImPlotInputMap          backup          = ImPlot::GetInputMap();                //  Adjust ImPlot input map (backup, edit, restore at end)
    ImPlotInputMap &        map             = ImPlot::GetInputMap();                        //
    map.Pan                                 = ImGuiMouseButton_Left;                        //
    map.PanMod                              = space ? 0 : ImGuiMod_Ctrl;                    //  disable pan unless Space
    map.ZoomMod                             = zoom_enabled ? 0 : ImGuiMod_Ctrl;             //  disable zoom unless mode allows
    map.ZoomRate                            = 0.10f;                                        //


    this->pump_main_tasks();
    this->_handle_io();


    //  2.  CREATING THE CANVAS/GRID...
    //
    //          CASE 2A     : FAILURE TO CREATE CANVAS.
    if ( !ImPlot::BeginPlot("##Editor_CanvasGrid", ImVec2(m_avail.x, m_avail.y), m_plot_flags) ) {
        ImPlot::GetInputMap() = backup;
        return;
    }
    //
    //          CASE 2B     : SUCCESSFULLY CREATED THE "IMPLOT" PLOT...
    {
        //      3.0     CREATE VARIABLES FOR THIS FRAME CONTEXT...
    
    
    
        //      4.  CONFIGURE THE "IMPLOT" APPEARANCE...
        //
        ImPlot::SetupAxes(m_axes[0].uuid,           m_axes[1].uuid,             //  4A.     Axis Names & Flags.
                          m_axes[0].flags,          m_axes[1].flags);
        //
        ImPlot::SetupAxesLimits( m_world_bounds.min_x, m_world_bounds.max_x,    //  4B.     Auto-fit axes *before* any other ImPlot call.
                                 m_world_bounds.min_y, m_world_bounds.max_y, ImPlotCond_Once );
        //
        this->_update_grid_info();                                              //  4C.     Fetch Grid-Quantization Info.
        
        
        // ───────────────────────── 3. Per-frame context
        ImDrawList *        dl              = ImPlot::GetPlotDrawList();
        ImVec2              plotTL          = ImPlot::GetPlotPos();
        bool                hovered         = ImPlot::IsPlotHovered();
        bool                active          = ImPlot::IsPlotSelected();

        ImVec2              origin_scr      = plotTL;
        ImVec2              mouse_canvas    { io.MousePos.x - origin_scr.x,     io.MousePos.y - origin_scr.y };
        Interaction         it              { hovered, active, space, mouse_canvas, origin_scr, plotTL, dl };


        //  5.      MODE SWITCH BEHAVIORS...
        //
        this->_mode_switch_hotkeys(it);


        //  6.      CURSOR HINTS AND SHORTCUTS...
        //
        if ( space && _mode_has(CBCapabilityFlags_Pan) )
            { ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeAll); }
        //
        else if ( !space && hovered && _mode_has(CBCapabilityFlags_CursorHint) )
            { _update_cursor_select(it); }



        //  7.      GLOBAL SELECTION BEHAVIOR...
        //
        if  ( !space && _mode_has(CBCapabilityFlags_Select) )
        {
            _process_selection(it);
            
            if ( io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_J) )        { _join_selected_open_path(); }     //  JOINING CLOSED PATHS...
            
            if ( ImGui::IsKeyPressed(ImGuiKey_Escape) )                 { this->reset_selection(); }        //  [Esc]       CANCEL SELECTION...
            
            if ( io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_C) )        { this->copy_to_clipboard(); }      //  [CTRL+C]    COPY SELECTION...
        }
        
    
    
    
        //  8.      MODE/STATE/TOOL DISPATCHER...
        this->_dispatch_mode_handler(it);


        //  9.     RENDERING LOOP...
        ImPlot::PushPlotClipRect();
        //
        //
            _render_points(dl);           //  Already ported
            // _render_lines(dl);         //  Enable once ported
            _render_paths(dl);            //  Enable once ported
            _render_selection_highlight(dl);
        //
        //
        ImPlot::PopPlotClipRect();

        this->_clamp_plot_axes();
        this->_handle_overlays(it);



    }// END "IMPLOT".
    //
    ImPlot::EndPlot();
    ImPlot::GetInputMap() = backup;   // restore map
    
    
    
    this->_draw_io_overlay();
    //
    //
    //  show_icon_preview_window();
    //
    //
    
    
    return;
}






// *************************************************************************** //
//
//
//
//  3.  MAIN MANAGER FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

//  "_handle_default"
//
void Editor::_handle_default(const Interaction& it)
{
    [[maybe_unused]]    ImGuiIO &   io          = ImGui::GetIO();
    
    if (_try_begin_handle_drag(it)) {
        _pen_update_handle_drag(it);   // keep guide visible the very first frame
        return;                    // skip selection & mode handlers this frame
    }
    //  1.   EDIT BEZIER CTRL POINTS IN DEFAULT STATE...
    //  if (_try_begin_handle_drag(it)) return;
    //  if (m_dragging_handle) { _pen_update_handle_drag(it); return; }

    
    //  2.  BBOX HANDLE HOVER...
    if ( !m_boxdrag.active && m_hover_handle != -1 && ImGui::IsMouseClicked(ImGuiMouseButton_Left) ) {
        ImVec2 tl, br;
        if (_selection_bounds(tl, br))
            _start_bbox_drag(static_cast<uint8_t>(m_hover_handle), tl, br);
    }
        
        
    //  3.   Update BBOX...
    if (m_boxdrag.active)                           { _update_bbox(); }
    
    
    //  4.  IGNORE ALL INPUT IF SPACE KEY IS HELD DOWN...
    if (it.space)                                   { return; }


    // 5. LASSO START — begin only when selection is enabled and the mouse
    //    isn’t captured by another UI widget.
    if ( _mode_has(CBCapabilityFlags_Select)                      // ← NEW capability check
         && !m_lasso_active
         && it.hovered
         && ImGui::IsMouseClicked(ImGuiMouseButton_Left)
         && !_hit_any(it) )
    {
        _start_lasso_tool();
    }
    
    //  6.  LASSO UPDATE...
    if (m_lasso_active)                             { this->_update_lasso(it); return; }        // Skip zoom handling while dragging lasso
        
    return;
}


//  "_handle_line"
//
void Editor::_handle_line(const Interaction& it)
{
    if (it.space) return; // ignore while panning

    ImVec2 w = { it.canvas.x / m_cam.zoom_mag, it.canvas.y / m_cam.zoom_mag };

    // 1) Begin a brand‑new line/path on LMB press
    if (it.hovered && !m_drawing && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
    {
        uint32_t a = _add_vertex(w);
        uint32_t b = _add_vertex(w);
        // Create visible anchor glyphs so the endpoints behave like legacy points
        m_points.push_back({ a, { m_style.COL_POINT_DEFAULT, m_style.DEFAULT_POINT_RADIUS, true } });
        m_points.push_back({ b, { m_style.COL_POINT_DEFAULT, m_style.DEFAULT_POINT_RADIUS, true } });

        // Legacy Line container (kept for now)
        // m_lines.push_back({ a, b });

        // NEW: provisional Path (open polyline of length 2)
        Path p;
        p.verts = { a, b };
        p.closed = false;
        m_paths.push_back(std::move(p));

        m_drawing = true;
    }

    // 2) Rubber‑band the second vertex while the mouse is down
    if (m_drawing)
    {
        // Update the Path’s second vertex while rubber‑banding
        Path& last_path = m_paths.back();
        if (last_path.verts.size() >= 2)
        {
            if (Vertex* v = find_vertex(m_vertices, last_path.verts[1]))
            {
                v->x = w.x; v->y = w.y;
            }
        }

        // 3) Commit once the mouse button is released
        if (!ImGui::IsMouseDown(ImGuiMouseButton_Left))
            m_drawing = false;
    }
}


//  "_handle_point"
//
void Editor::_handle_point(const Interaction& it)
{
    // handle-drag branch (unchanged)
    if (m_pen.dragging_handle) {
        _pen_update_handle_drag(it);
        return;
    }

    // ── Click logic ──────────────────────────────────────────────────────
    if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && it.hovered)
    {
        // (A) If a path is already live → append / close as usual
        if (m_pen.active) {
            _pen_append_or_close_live_path(it);
            return;
        }

        // (B) No live path: did we click on an existing point?
        int pi = _hit_point(it);
        if (pi >= 0) {
            uint32_t vid = m_points[pi].v;
            if (auto idx = _path_idx_if_last_vertex(vid)) {
                // Continue that path on the NEXT click
                m_pen.active     = true;
                m_pen.path_index = *idx;
                m_pen.last_vid   = vid;
                return;          // wait for next click to add a vertex
            }
        }

        // (C) Otherwise start a brand-new path
        ImVec2 ws{ it.canvas.x / m_cam.zoom_mag, it.canvas.y / m_cam.zoom_mag };
        uint32_t vid = _add_vertex(ws);
        _add_point_glyph(vid);

        Path p;
        p.verts.push_back(vid);
        m_paths.push_back(std::move(p));

        m_pen.active     = true;
        m_pen.path_index = m_paths.size() - 1;
        m_pen.last_vid   = vid;
    }
}


//  "_handle_pen"
//
void Editor::_handle_pen(const Interaction& it)
{
    ImGuiIO& io = ImGui::GetIO();

    // ───── 0.  [Esc] aborts live path
    //  if (ImGui::IsKeyPressed(ImGuiKey_Escape)) {
    //      this->reset_pen(); // m_pen = {};
    //      return;
    //  }
    
    

    // ───── A. Handle‑drag already in progress
    if (m_pen.dragging_handle) {
        _pen_update_handle_drag(it);
        return;                                     // nothing else this frame
    }

    // ───── B. Pending click‑hold test (detect curvature pull)
    if (m_pen.pending_handle)
    {
        m_pen.pending_time += io.DeltaTime;

        if (ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
            m_pen.pending_handle = false;
            m_pen.pending_time   = 0.0f;
            return;                                 // plain vertex click
        }

        if ( m_pen.pending_time >= PEN_DRAG_TIME_THRESHOLD &&
             ImGui::IsMouseDragPastThreshold(ImGuiMouseButton_Left,
                                             PEN_DRAG_MOVEMENT_THRESHOLD) )
        {
            _pen_begin_handle_drag(m_pen.pending_vid,
                                   /*out_handle=*/false,
                                   /*force_select=*/true);

            if (Vertex* v = find_vertex_mut(m_vertices, m_pen.pending_vid))
                v->in_handle = ImVec2(0,0);         // make new handle visible

            m_pen.pending_handle = false;
            m_pen.pending_time   = 0.0f;
            _pen_update_handle_drag(it);            // first update
            return;
        }
        return;     // still waiting (mouse held but not past threshold)
    }

    // ───── C.  Alt‑mode : edit existing handles (no new vertices)
    if (alt_down())
    {
        if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && it.hovered)
        {
            int pi = _hit_point(it);
            if (pi >= 0) {
                _pen_begin_handle_drag(m_points[pi].v,
                                        /*out_handle=*/true,
                                        /*force_select=*/true);
                if (Vertex* v = find_vertex_mut(m_vertices, m_drag_vid))
                    v->out_handle = ImVec2(0,0);
            }
        }
        _draw_pen_cursor(io.MousePos, m_style.PEN_COL_NORMAL);   // yellow bullseye
        return;
    }

    // ───── D. Normal pen behaviour (continue / add / close)
    bool   can_extend = false, prepend = false;
    size_t end_path   = static_cast<size_t>(-1);

    int pt_idx = _hit_point(it);
    if (pt_idx >= 0) {
        uint32_t vid = m_points[pt_idx].v;
        if (auto ep = _endpoint_if_open(vid)) {
            can_extend = true;
            prepend    = ep->prepend;
            end_path   = ep->path_idx;
        }
    }

    // Cursor hint colour
    if (it.hovered && !it.space)
        _draw_pen_cursor(io.MousePos,
                         can_extend ? m_style.PEN_COL_EXTEND : m_style.PEN_COL_NORMAL);

    // ───── E. Mouse‑click handling
    if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && it.hovered)
    {
        // (1) Pick open endpoint to continue
        if (can_extend && !m_pen.active) {
            m_pen.active     = true;
            m_pen.path_index = end_path;
            m_pen.prepend    = prepend;
            return;                                 // wait for next click
        }

        // (2) Live path exists → append / prepend / close
        if (m_pen.active) {
            _pen_append_or_close_live_path(it);     // updates m_pen.last_vid

            // Allow click‑hold curvature on the vertex we just added
            if (ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
                m_pen.pending_handle = true;
                m_pen.pending_vid    = m_pen.last_vid;
            }
            return;
        }

        // (3) Start a brand‑new path
        ImVec2          ws      = pixels_to_world(io.MousePos);   // NEW conversion
        VertexID        vid     = _add_vertex(ws);
        _add_point_glyph(vid);

        Path p;
        p.set_default_label(this->m_next_pid++);    // unique PathID
        p.verts.push_back(vid);
        m_paths.push_back(std::move(p));

        m_pen.active            = true;
        m_pen.path_index        = m_paths.size() - 1;
        m_pen.last_vid          = vid;
        m_pen.prepend           = false;

        //  Possible click‑hold curvature on first segment
        if ( ImGui::IsMouseDown(ImGuiMouseButton_Left) ) {
            m_pen.pending_handle    = true;
            m_pen.pending_vid       = vid;
        }
    }
    return;
}


//  "_handle_scissor"
//
void Editor::_handle_scissor(const Interaction & it)
{
    if (!it.hovered) return;

    // test once per frame
    std::optional<PathHit> hit = _hit_path_segment(it);

    // cursor hint (vertical beam) whenever a cut location is valid
    if (hit) ImGui::SetMouseCursor(ImGuiMouseCursor_TextInput);

    // left-click performs the cut
    if (hit && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
        _scissor_cut(*hit);
        
    return;
}


//  "_handle_add_anchor"
//
void Editor::_handle_add_anchor([[maybe_unused]] const Interaction & it)
{
    return;
}


//  "_handle_remove_anchor"
//
void Editor::_handle_remove_anchor([[maybe_unused]] const Interaction & it)
{
    return;
}


//  "_handle_edit_anchor"
//
void Editor::_handle_edit_anchor([[maybe_unused]] const Interaction & it)
{
    [[maybe_unused]] ImGuiIO & io = ImGui::GetIO();

    auto select_vertex = [&](uint32_t vid)
    {
        m_sel.clear();
        m_sel.vertices.insert(vid);

        // also select matching Point glyph so _render_selected_handles() knows which one
        for (size_t i = 0; i < m_points.size(); ++i)
            if (m_points[i].v == vid) { m_sel.points.insert(i); break; }
    };

    // 1. continue an active handle drag
    if (m_dragging_handle) {
        _pen_update_handle_drag(it);
        return;
    }

    // 2. left-click on an existing handle square  → begin drag
    if (_try_begin_handle_drag(it)) {
        select_vertex(m_drag_vid);            // make its handles visible
        _pen_update_handle_drag(it);          // draw guide first frame
        return;
    }

    // 3. left-click on a vertex  → pull / edit its out_handle
    if (it.hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
        int pi = _hit_point(it);              // glyph index under cursor
        if (pi >= 0) {
            uint32_t vid       = m_points[pi].v;
            select_vertex(vid);

            m_dragging_handle  = true;
            m_drag_vid         = vid;
            m_dragging_out     = true;        // start with out_handle
            if (Vertex * v = find_vertex_mut(m_vertices, vid))
                v->out_handle  = ImVec2(0,0); // make handle distinct from anchor

            return;                           // drag continues next frame
        }
    }

    
    return;
}




// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //

//  "_handle_overlays"
//
void Editor::_handle_overlays([[maybe_unused]] const Interaction & it)
{
    [[maybe_unused]] ImGuiIO & io = ImGui::GetIO();
    
    //  ImVec2 tl = it.origin;                       // top-left in screen px
    //  ImVec2 br = { tl.x + m_avail.x,
    //                tl.y + m_avail.y };            // bottom-right of canvas
    //
    //  m_overlays.Begin(
    //      /* world→pixel lambda */ [this](ImVec2 ws){ return world_to_pixels(ws); },
    //      /* cursor position   */ ImGui::GetIO().MousePos,
    //      /* canvas rectangle  */ ImRect(tl, br));
    
    

    ImVec2 bb_min = ImGui::GetItemRectMin();   // full ImPlot widget (axes included)
    ImVec2 bb_max = ImGui::GetItemRectMax();
    m_overlays.Begin(
        /* world→pixel */ [this](ImVec2 ws){ return world_to_pixels(ws); },
        /* cursor      */ ImGui::GetIO().MousePos,
        /* full rect   */ ImRect(bb_min, bb_max));          // ← use full item rect

    return;
}


//  "_handle_io"
//
void Editor::_handle_io(void)
{
    using                       Type            = cb::FileDialog::Type;
    //using                       Initializer     = cb::FileDialog::Initializer;


    //  1.  SAVE DIALOGUE...
    if ( m_sdialog_open.load(std::memory_order_acquire) ) {
        m_sdialog_open.store(false, std::memory_order_release);
        this->m_save_dialog.initialize(Type::Save, this->m_SAVE_DIALOG_DATA );
    }
    //
    if ( this->m_save_dialog.is_open() )
    {
        m_sdialog_open.store(false, std::memory_order_release);
        if ( this->m_save_dialog.Begin("Save Editor Session") ) {        // returns true when finished
            if ( auto path = this->m_save_dialog.result() )
                save_async( *path );        // your own handler
        }
    }
    
    
    //  2.  LOAD DIALOGUE...
    if ( m_odialog_open.load(std::memory_order_acquire) ) {
        m_odialog_open.store(false, std::memory_order_release);
        this->m_open_dialog.initialize(Type::Open, m_OPEN_DIALOG_DATA );
    }
    //
    if ( this->m_open_dialog.is_open() )
    {
        if ( this->m_open_dialog.Begin("Load session from file") ) {        // returns true when finished
            if ( auto path = this->m_open_dialog.result() )
                load_async( *path );        // your own handler
        }
    }



    return;
}












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
