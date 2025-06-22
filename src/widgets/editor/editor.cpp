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
//  2.  PUBLIC API...
// *************************************************************************** //
// *************************************************************************** //

//  "Begin"
//
void Editor::Begin(const char * id)
{
    //  1.      DRAW CANVAS RECTANGLE...
    m_avail             = ImGui::GetContentRegionAvail();
    m_avail.x           = std::max(m_avail.x, 50.f);
    m_avail.y           = std::max(m_avail.y, 50.f);
    m_p0                = ImGui::GetCursorScreenPos();
    m_p1                = { m_p0.x + m_avail.x, m_p0.y + m_avail.y };


    // current pixels-per-world-unit (cached for rest of frame)
    m_ppw               = m_cam.pixels_per_world(m_avail, m_world_bounds);


    ImDrawList *    dl  = ImGui::GetWindowDrawList();
    ImGuiIO &       io  = ImGui::GetIO();



    dl->AddRectFilled(m_p0, m_p1, IM_COL32(50, 50, 50, 255));
    dl->AddRect      (m_p0, m_p1, IM_COL32(255, 255, 255, 255));

    // ───────────────────────────────────────────────────── input capture zone
    ImGui::InvisibleButton(id, m_avail,
                           ImGuiButtonFlags_MouseButtonLeft |
                           ImGuiButtonFlags_MouseButtonRight);



    //  3.      MODE SWITCH BEHAVIORS...
    const bool      hovered         = ImGui::IsItemHovered();
    const bool      active          = ImGui::IsItemActive();
    const bool      space           = ImGui::IsKeyDown(ImGuiKey_Space);
    const bool      wheel           = (io.MouseWheel != 0.0f);
    bool            camera_update   = false;
    //
    if (hovered) {
        const bool shift  = io.KeyShift;
        const bool ctrl   = io.KeyCtrl;
        const bool alt    = io.KeyAlt;
        const bool super  = io.KeySuper;
        const bool no_mod = !ctrl && !shift && !alt && !super;

        if ( no_mod && ImGui::IsKeyPressed(ImGuiKey_V) )           m_mode = Mode::Default;
        if ( no_mod && ImGui::IsKeyPressed(ImGuiKey_N) )           m_mode = Mode::Point;
        if ( no_mod && ImGui::IsKeyPressed(ImGuiKey_P) )           m_mode = Mode::Pen;
        if ( no_mod && ImGui::IsKeyPressed(ImGuiKey_C) )           m_mode = Mode::Scissor;
        if ( no_mod && ImGui::IsKeyPressed(ImGuiKey_Equal) )       m_mode = Mode::AddAnchor;
        if ( no_mod && ImGui::IsKeyPressed(ImGuiKey_Minus) )       m_mode = Mode::RemoveAnchor;
        if (!ctrl &&  shift && !alt && !super &&
            ImGui::IsKeyPressed(ImGuiKey_C) )                      m_mode = Mode::EditAnchor;
    }
    if ( m_mode != Mode::Pen )    m_pen = {};     // leaving Pen resets its state


    //  4.      INTERACTION SNAPSHOTS...
    ImVec2          origin_scr      { m_p0.x - m_cam.pan.x * m_ppw,     m_p0.y - m_cam.pan.y * m_ppw };     // world (0,0) in px
    ImVec2          mouse_canvas    { io.MousePos.x - origin_scr.x,     io.MousePos.y - origin_scr.y };
    Interaction     it              { hovered, active, space, mouse_canvas, origin_scr, m_p0, dl };


    //  5.      CURSOR HINTS AND SHORTCUTS...
    if ( !space && hovered && _mode_has(CBCapabilityFlags_CursorHint) )     { _update_cursor_select(it); }
    
    
    //  6.      LOCAMOTION  | PANNING AND ZOOMING...
    //
    //          6A.     ZOOM IN/OUT.
    if ( !space && wheel && _mode_has(CBCapabilityFlags_Zoom) )         { _apply_wheel_zoom(it); }       // mouse wheel
    //
    //          6B.     PANNING.
    if ( space && hovered && _mode_has(CBCapabilityFlags_Pan) )
    {
        ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeAll);
        if ( ImGui::IsMouseDragging(ImGuiMouseButton_Left, 0.0f) )
        {
            //  float ppw = m_cam.pixels_per_world(m_avail, m_world_bounds);
            m_cam.pan.x        += io.MouseDelta.x / m_ppw;
            m_cam.pan.y        += io.MouseDelta.y / m_ppw;
            _clamp_scroll();
            camera_update       = true;
        }
    }
    //
    //          6C.     REFRESH ORIGIN AND CANVAS AFTER NAVIGATION...
    if (camera_update) {            //  Refresh origin & snapshot fields for downstream handlers
        origin_scr      = { m_p0.x - m_cam.pan.x * m_ppw, m_p0.y - m_cam.pan.y * m_ppw };
        it.origin       = origin_scr;
        it.canvas       = { io.MousePos.x - origin_scr.x, io.MousePos.y - origin_scr.y };
    }



    //  7.      GLOBAL SELECTION BEHAVIOR...
    if  ( !space && _mode_has(CBCapabilityFlags_Select) ) {
        _process_selection(it);
        
        if ( io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_J) )    //  JOINING CLOSED PATHS...
        { _join_selected_open_path(); }
    }


    //  8.      MODE/STATE/TOOL DISPATCHER...
    if ( !(space && ImGui::IsMouseDown(ImGuiMouseButton_Left)) ) {
        switch (m_mode) {
            case Mode::Default:      _handle_default        (it); break;
            case Mode::Line:         _handle_line           (it); break;
            case Mode::Point:        _handle_point          (it); break;
            case Mode::Pen:          _handle_pen            (it); break;
            case Mode::Scissor:      _handle_scissor        (it); break;
            case Mode::AddAnchor:    _handle_add_anchor     (it); break;
            case Mode::RemoveAnchor: _handle_remove_anchor  (it); break;
            case Mode::EditAnchor:   _handle_edit_anchor    (it); break;
            default: break;
        }
    }
    
    
    
                
                    
                    

    //  9.     RENDERING LOOP...
    dl->PushClipRect(m_p0, m_p1, true);

    _grid_handle_shortcuts();        // adjust world_step via hot-keys
    _grid_draw(dl, m_p0, m_avail);

    _draw_lines           (dl, origin_scr);
    _draw_paths           (dl, origin_scr);
    _draw_points          (dl, origin_scr);
    _draw_selection_overlay(dl, origin_scr);

    _handle_overlay(it);
    dl->PopClipRect();
    
    return;
}







/*{
    m_avail                         = ImGui::GetContentRegionAvail();
    m_avail.x                       = std::max(m_avail.x,   50.f);
    m_avail.y                       = std::max(m_avail.y,   50.f);
    m_p0                            = ImGui::GetCursorScreenPos();
    m_p1                            = { m_p0.x + m_avail.x,     m_p0.y + m_avail.y };
    m_ppw                           = m_cam.pixels_per_world( m_avail, m_world_bounds );  // cache for frame

    ImDrawList  * dl                = ImGui::GetWindowDrawList();
    ImGuiIO     & io                = ImGui::GetIO();


    dl->AddRectFilled(m_p0, m_p1,  IM_COL32(50,50,50,255));
    dl->AddRect      (m_p0, m_p1,  IM_COL32(255,255,255,255));

    // ── 1.  Refresh world bounds → clamp scroll BEFORE anything uses it -------------------
    //_update_world_extent();         // new helper – tracks min/max of all vertices
    _clamp_scroll();                // symmetric clamp (allows ± scroll)

    // ── 2.  Input capture zone ------------------------------------------------------------
    ImGui::InvisibleButton(id, m_avail, ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight);

    // ── 3.  Local hot‑keys (only while cursor over canvas) --------------------------------
    const bool  hovered     = ImGui::IsItemHovered();
    const bool  active      = ImGui::IsItemActive();
    const bool  space       = ImGui::IsKeyDown(ImGuiKey_Space);

    if (hovered) {
        const bool shift   = io.KeyShift;
        const bool ctrl    = io.KeyCtrl;
        const bool alt     = io.KeyAlt;
        const bool super   = io.KeySuper;
        const bool no_mod  = !ctrl && !shift && !alt && !super;

        if ( no_mod && ImGui::IsKeyPressed(ImGuiKey_V) )                                m_mode = Mode::Default;
        if ( no_mod && ImGui::IsKeyPressed(ImGuiKey_N) )                                m_mode = Mode::Point;
        if ( no_mod && ImGui::IsKeyPressed(ImGuiKey_P) )                                m_mode = Mode::Pen;
        if ( no_mod && ImGui::IsKeyPressed(ImGuiKey_C) )                                m_mode = Mode::Scissor;
        if ( no_mod && ImGui::IsKeyPressed(ImGuiKey_Equal) )                            m_mode = Mode::AddAnchor;
        if ( no_mod && ImGui::IsKeyPressed(ImGuiKey_Minus) )                            m_mode = Mode::RemoveAnchor;
        if ( !ctrl && shift && !alt && !super && ImGui::IsKeyPressed(ImGuiKey_C) )      m_mode = Mode::EditAnchor;
    }
    if (m_mode != Mode::Pen)  m_pen = {};     // reset pen state if we left Pen mode


    // ── 4.  Interaction snapshot ------------------------------------------
    ImVec2 origin    { m_p0.x + m_cam.pan.x, m_p0.y + m_cam.pan.y };
    ImVec2 mouse_can { io.MousePos.x - origin.x, io.MousePos.y - origin.y };
    Interaction it   { hovered, active, space, mouse_can, origin, m_p0, dl };


    // ── 5.  Cursor hints & join shortcut -----------------------------------
    if (!space && _mode_has(CBCapabilityFlags_CursorHint))
        _update_cursor_select(it);


    if (io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_J))
        _join_selected_open_path();




    _apply_wheel_zoom(it);

    // ── 6.  Space-bar pan UI feedback --------------------------------------
    if (space && hovered) ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeAll);


    // ── 7.  Space-drag panning ---------------------------------------------
    if (space && ImGui::IsMouseDragging(ImGuiMouseButton_Left, 0.0f))
    {
        float    ppw = m_ppw;
        m_cam.pan.x += io.MouseDelta.x; // / ppw;
        m_cam.pan.y += io.MouseDelta.y; // / ppw;
        
            //_clamp_pan(m_avail);
        
        //_clamp_scroll();


        //  OLD:
        //
        //          m_cam.pan.x += io.MouseDelta.x;
        //          m_cam.pan.y += io.MouseDelta.y;
        //
        //          //  refresh origin & snapshot fields for downstream handlers
        //          origin        = { m_p0.x + m_cam.pan.x, m_p0.y + m_cam.pan.y };
        //          it.origin     = origin;
        //          it.canvas     = { io.MousePos.x - origin.x, io.MousePos.y - origin.y };
    }


    // ── 8.  Global selection / drag ----------------------------------------
    if (!space && _mode_has(CBCapabilityFlags_Select))
        _process_selection(it);


    // ── 9.  Mode dispatcher (skip while panning) ---------------------------
    if ( !(space && ImGui::IsMouseDown(ImGuiMouseButton_Left)) )
    {
        switch (m_mode)
        {
            case Mode::Default:         _handle_default         (it);       break;
            case Mode::Line:            _handle_line            (it);       break;
            case Mode::Point:           _handle_point           (it);       break;
            case Mode::Pen:             _handle_pen             (it);       break;
            case Mode::Scissor:         _handle_scissor         (it);       break;
            case Mode::AddAnchor:       _handle_add_anchor      (it);       break;
            case Mode::RemoveAnchor:    _handle_remove_anchor   (it);       break;
            case Mode::EditAnchor:      _handle_edit_anchor     (it);       break;
            default: break;
        }
    }
    
    


    // ── 10.  Draw pass ------------------------------------------------------
    dl->PushClipRect(m_p0, m_p1, true);

    _grid_handle_shortcuts();     // Ctrl + / – adjust pitch
    _grid_draw           (dl, m_p0, m_avail);

    _draw_lines          (dl, origin);
    _draw_paths          (dl, origin);
    _draw_points         (dl, origin);
    _draw_selection_overlay(dl, origin);

    _handle_overlay(it);
    dl->PopClipRect();
    
    return;
}*/








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
        
    //  7.   ZOOM (mouse wheel) – only when not lassoing...
    //if (it.hovered && io.MouseWheel != 0.0f)        { this->_zoom_canvas(it); }
    //
    // _apply_wheel_zoom(it);
        
    //  8.   EDIT BEZIER CTRL POINTS IN DEFAULT STATE...
    //  if (m_pen.dragging_handle)                      { _pen_update_handle_drag(it); return; }    // continue an active handle drag
    //  if ( _pen_try_begin_handle_drag(it) )           { return; }                                 // Alt-click started a new drag
    
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
        m_points.push_back({ a, { COL_POINT_DEFAULT, DEFAULT_POINT_RADIUS, true } });
        m_points.push_back({ b, { COL_POINT_DEFAULT, DEFAULT_POINT_RADIUS, true } });

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
            if (Pos* v = find_vertex(m_vertices, last_path.verts[1]))
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
void Editor::_handle_pen(const Interaction & it)
{
    ImGuiIO & io = ImGui::GetIO();

    // 0. Escape aborts live path
    if (ImGui::IsKeyPressed(ImGuiKey_Escape)) { m_pen = {}; return; }

    //----------------------------------------------------------------
    // A. Handle‑drag in progress ------------------------------------
    //----------------------------------------------------------------  
    if (m_pen.dragging_handle) {
        _pen_update_handle_drag(it);
        return;                         // nothing else while dragging
    }

    //----------------------------------------------------------------
    // B. Pending click‑hold test ------------------------------------
    //----------------------------------------------------------------
    if (m_pen.pending_handle)                             // vertex placed, waiting
    {
        m_pen.pending_time += io.DeltaTime;     //  ++m_pen.pending_frames; // count frames
        
        //  Button released before time window → plain click
        if ( ImGui::IsMouseReleased(ImGuiMouseButton_Left) ) {
            m_pen.pending_handle = false;
            m_pen.pending_time   = 0.0f;
            return;
        }
    
        //  From the 2nd frame onward, test drag distance
        if ( m_pen.pending_time >= PEN_DRAG_TIME_THRESHOLD && ImGui::IsMouseDragPastThreshold(ImGuiMouseButton_Left, PEN_DRAG_MOVEMENT_THRESHOLD) )
        {
            _pen_begin_handle_drag( m_pen.pending_vid, /*out_handle=*/false, /*force_select=*/true );
            if ( Vertex * v = find_vertex_mut(m_vertices, m_pen.pending_vid) )
                v->in_handle = ImVec2(0,0);              // make handle visible
                
            m_pen.pending_handle = false;                // hand-off to drag logic
            m_pen.pending_time   = 0.0f;
            _pen_update_handle_drag(it);                 // first update
            return;
        }
        return;         // keep waiting (mouse still held but not past threshold)
    }

    //----------------------------------------------------------------
    // C. Alt‑mode : edit existing handles ---------------------------
    //----------------------------------------------------------------
    if ( alt_down() )
    {
        if ( ImGui::IsMouseClicked(ImGuiMouseButton_Left) && it.hovered )
        {
            int pi = _hit_point(it);
            if (pi >= 0) {
                _pen_begin_handle_drag(m_points[pi].v, /*out_handle=*/true, /*force_select=*/true);
                if ( Vertex * v = find_vertex_mut(m_vertices, m_drag_vid) )
                    v->out_handle = ImVec2(0,0);
            }
        }
        _draw_pen_cursor(io.MousePos, PEN_COL_NORMAL);   // yellow bullseye
        return;
    }

    //----------------------------------------------------------------
    // D. Normal pen behaviour  (endpoint continuation, new verts) ---
    //----------------------------------------------------------------
    bool     can_extend = false, prepend = false;
    size_t   end_path   = static_cast<size_t>(-1);
    int      pt_idx     = _hit_point(it);

    if (pt_idx >= 0) {
        uint32_t vid = m_points[pt_idx].v;
        if (auto ep = _endpoint_if_open(vid)) {
            can_extend = true;
            prepend    = ep->prepend;
            end_path   = ep->path_idx;
        }
    }

    // Cursor hint (green when hovering an open endpoint)
    if (it.hovered && !it.space) {
        _draw_pen_cursor(io.MousePos, can_extend ? PEN_COL_EXTEND : PEN_COL_NORMAL);
    }

    //----------------------------------------------------------------
    // Click handling ------------------------------------------------
    //----------------------------------------------------------------
    if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && it.hovered)
    {
        // A) pick open endpoint to continue
        if (can_extend && !m_pen.active) {
            m_pen.active     = true;
            m_pen.path_index = end_path;
            m_pen.prepend    = prepend;
            return;                                   // wait for next click
        }

        // B) live path exists → append / prepend / close
        if (m_pen.active) {
            _pen_append_or_close_live_path(it);       // **ensure this sets m_pen.last_vid**
            // set up possible click‑hold curvature on new vertex
            if (ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
                m_pen.pending_handle = true;
                m_pen.pending_vid    = m_pen.last_vid;
            }
            return;
        }

        // C) start new path
        ImVec2   ws  { it.canvas.x / m_cam.zoom_mag, it.canvas.y / m_cam.zoom_mag };
        uint32_t vid = _add_vertex(ws);
        _add_point_glyph(vid);

        Path p; p.verts.push_back(vid);
        m_paths.push_back(std::move(p));

        m_pen.active      = true;
        m_pen.path_index  = m_paths.size() - 1;
        m_pen.last_vid    = vid;
        m_pen.prepend     = false;

        // possible click‑hold curvature on first segment
        if (ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
            m_pen.pending_handle = true;
            m_pen.pending_vid    = vid;
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
    ImGuiIO & io = ImGui::GetIO();

    auto select_vertex = [&](uint32_t vid)
    {
        m_sel.clear();
        m_sel.vertices.insert(vid);

        // also select matching Point glyph so _draw_selected_handles() knows which one
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

    // 4. zoom wheel still active
    //   if (it.hovered && io.MouseWheel != 0.0f)
    //       _zoom_canvas(it);
    //  _apply_pan(it);          // Space + LMB drag (uses new Camera math)
    _apply_wheel_zoom(it);   // mouse wheel zoom, always uses m_cam
    
    
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
