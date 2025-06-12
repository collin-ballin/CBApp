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
#include "widgets/_editor.h"



namespace cb { //     BEGINNING NAMESPACE "cb"...
// *************************************************************************** //
// *************************************************************************** //


//───── PRIVATE UTILS ─────────────────────────────────────────────────────—
static constexpr float GRID_STEP = 64.0f;
static constexpr float HIT_THRESH_SQ = 6.0f * 6.0f;



//-------------------------------------------------------------------------
//  Helper to fetch vertex by id (const / non‑const)
//-------------------------------------------------------------------------
static Pos*       find_vertex(std::vector<Pos>& verts, uint32_t id) {
    for (auto &v : verts) if (v.id == id) return &v; return nullptr;
}
static const Pos* find_vertex(const std::vector<Pos>& verts, uint32_t id) {
    for (auto &v : verts) if (v.id == id) return &v; return nullptr;
}



//=============================================================================
//  Editor : public API
//=============================================================================
void Editor::Begin(const char* id)
{
    const float bar_h = ImGui::GetFrameHeight() + ImGui::GetStyle().ItemSpacing.y;

    // ------------------ Layout rect ------------------
    ImVec2 p0 = ImGui::GetCursorScreenPos();
    ImVec2 avail = ImGui::GetContentRegionAvail();
    avail.x = std::max(avail.x, 50.f);
    avail.y = std::max(avail.y, 50.f);
    if (avail.y > bar_h) avail.y -= bar_h;
    ImVec2 p1 = { p0.x + avail.x, p0.y + avail.y };

    ImDrawList* dl = ImGui::GetWindowDrawList();
    ImGuiIO& io    = ImGui::GetIO();

    dl->AddRectFilled(p0, p1, IM_COL32(50, 50, 50, 255));
    dl->AddRect      (p0, p1, IM_COL32(255, 255, 255, 255));

    // Capture area
    ImGui::InvisibleButton(id, avail, ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight);
    bool hovered = ImGui::IsItemHovered();
    bool active  = ImGui::IsItemActive();
    bool space   = ImGui::IsKeyDown(ImGuiKey_Space);

    ImVec2 origin = { p0.x + m_scroll.x, p0.y + m_scroll.y };
    ImVec2 mouse_canvas = { io.MousePos.x - origin.x, io.MousePos.y - origin.y };

    Interaction it{ hovered, active, space, mouse_canvas, origin, p0, dl };

    // Cursor + pan with Space+LMB
    if (space && hovered)
        ImGui::SetMouseCursor(ImGui::IsMouseDown(ImGuiMouseButton_Left) ? ImGuiMouseCursor_ResizeAll : ImGuiMouseCursor_Hand);

    if (space && ImGui::IsMouseDragging(ImGuiMouseButton_Left, 0.0f))
    {
        m_scroll.x += io.MouseDelta.x;
        m_scroll.y += io.MouseDelta.y;
    }

    // Mode‑specific processing (skip while panning)
    if (!(space && ImGui::IsMouseDown(ImGuiMouseButton_Left)))
    {
        switch (m_mode)
        {
        case Mode::Default: _handle_default(it); break;
        case Mode::Line:    _handle_line(it);    break;
        case Mode::Point:   _handle_point(it);   break;
        default: break;
        }
    }

    // ------------------ Rendering ------------------
    dl->PushClipRect(p0, p1, true);
    if (m_show_grid) _draw_grid(dl, p0, avail);
    _draw_lines(dl, origin);
    _draw_points(dl, origin);
    dl->PopClipRect();

    // ------------------ Control bar ----------------
    _draw_controls({ p0.x, p1.y + ImGui::GetStyle().ItemSpacing.y });
    ImGui::Dummy({ avail.x, bar_h });
}

//=============================================================================
//  Mode handlers
//=============================================================================
void Editor::_handle_default(const Interaction& it)
{
    ImGuiIO& io = ImGui::GetIO();
    if (it.hovered && io.MouseWheel != 0.0f)
    {
        float new_zoom = std::clamp(m_zoom * (1.0f + io.MouseWheel * 0.1f), 0.25f, 4.0f);
        if (std::fabs(new_zoom - m_zoom) > 1e-6f)
        {
            ImVec2 world = { (io.MousePos.x - it.origin.x) / m_zoom, (io.MousePos.y - it.origin.y) / m_zoom };
            m_zoom = new_zoom;
            ImVec2 new_origin = { io.MousePos.x - world.x * m_zoom, io.MousePos.y - world.y * m_zoom };
            m_scroll.x = new_origin.x - it.tl.x;
            m_scroll.y = new_origin.y - it.tl.y;
        }
    }
}

void Editor::_handle_line(const Interaction& it)
{
    if (it.space) return; // ignore while panning

    ImVec2 w = { it.canvas.x / m_zoom, it.canvas.y / m_zoom };

    if (it.hovered && !m_drawing && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
    {
        uint32_t a = _add_vertex(w);
        uint32_t b = _add_vertex(w);
        m_lines.push_back({ a, b });
        m_drawing = true;
    }

    if (m_drawing)
    {
        Pos* vb = find_vertex(m_vertices, m_lines.back().b);
        if (vb) { vb->x = w.x; vb->y = w.y; }
        if (!ImGui::IsMouseDown(ImGuiMouseButton_Left)) m_drawing = false;
    }
}

void Editor::_handle_point(const Interaction& it)
{
    if (it.space) return;

    ImVec2 w = { it.canvas.x / m_zoom, it.canvas.y / m_zoom };

    if (m_drag_point >= 0)
    {
        Pos* v = find_vertex(m_vertices, m_points[m_drag_point].v);
        if (v) { v->x = w.x; v->y = w.y; }
        if (!ImGui::IsMouseDown(ImGuiMouseButton_Left)) m_drag_point = -1;
        return;
    }

    if (it.hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
    {
        int hit = _hit_point(it);
        if (hit >= 0) m_drag_point = hit;
        else          _add_point(w);
    }
}

//=============================================================================
//  Data helpers
//=============================================================================
uint32_t Editor::_add_vertex(ImVec2 w)
{
    m_vertices.push_back({ m_next_id++, w.x, w.y });
    return m_vertices.back().id;
}

void Editor::_add_point(ImVec2 w)
{
    uint32_t vid = _add_vertex(w);
    m_points.push_back({ vid, { IM_COL32(0,255,0,255), 4.f, true } });
}

int Editor::_hit_point(const Interaction& it) const
{
    for (size_t i = 0; i < m_points.size(); ++i)
    {
        const Pos* v = find_vertex(m_vertices, m_points[i].v);
        if (!v) continue;
        ImVec2 scr{ it.origin.x + v->x * m_zoom, it.origin.y + v->y * m_zoom };
        float dx = scr.x - ImGui::GetIO().MousePos.x;
        float dy = scr.y - ImGui::GetIO().MousePos.y;
        if (dx*dx + dy*dy <= HIT_THRESH_SQ) return static_cast<int>(i);
    }
    return -1;
}

//=============================================================================
//  Rendering helpers
//=============================================================================
void Editor::_draw_grid(ImDrawList* dl, const ImVec2& p0, const ImVec2& sz) const
{
    float step = GRID_STEP * m_zoom;
    float x0 = std::fmod(m_scroll.x, step);
    for (float x = x0; x < sz.x; x += step)
        dl->AddLine({ p0.x + x, p0.y }, { p0.x + x, p0.y + sz.y }, IM_COL32(200,200,200,40));
    float y0 = std::fmod(m_scroll.y, step);
    for (float y = y0; y < sz.y; y += step)
        dl->AddLine({ p0.x, p0.y + y }, { p0.x + sz.x, p0.y + y }, IM_COL32(200,200,200,40));
}

void Editor::_draw_lines(ImDrawList* dl, const ImVec2& origin) const
{
    for (const auto& ln : m_lines)
    {
        const Pos* a = find_vertex(m_vertices, ln.a);
        const Pos* b = find_vertex(m_vertices, ln.b);
        if (!a || !b) continue;
        dl->AddLine({ origin.x + a->x * m_zoom, origin.y + a->y * m_zoom },
                    { origin.x + b->x * m_zoom, origin.y + b->y * m_zoom },
                    ln.color, ln.thickness);
    }
}

void Editor::_draw_points(ImDrawList* dl, const ImVec2& origin) const
{
    for (size_t i = 0; i < m_points.size(); ++i)
    {
        const Point& pt = m_points[i]; if (!pt.sty.visible) continue;
        const Pos* v = find_vertex(m_vertices, pt.v); if (!v) continue;
        ImU32 col = (static_cast<int>(i) == m_drag_point) ? IM_COL32(255,100,0,255) : pt.sty.color;
        dl->AddCircleFilled({ origin.x + v->x * m_zoom, origin.y + v->y * m_zoom }, pt.sty.radius, col);
    }
}




//=============================================================================
//  UI controls & clear
//=============================================================================
void Editor::_draw_controls(const ImVec2& pos)
{
    ImGui::SetCursorScreenPos(pos);
    ImGui::BeginGroup();

    // Mode selector
    int mode_i = static_cast<int>(m_mode);
    ImGui::SetNextItemWidth(110.0f);
    ImGui::Combo("Mode", &mode_i, MODE_LABELS.data(),
                 static_cast<int>(Mode::Count));
    m_mode = static_cast<Mode>(mode_i);

    // Grid toggle
    ImGui::SameLine(0.0f, 15.0f);
    ImGui::Checkbox("Grid", &m_show_grid);

    // Clear button
    ImGui::SameLine(0.0f, 15.0f);
    if (ImGui::Button("Clear"))
        _clear_all();

    ImGui::EndGroup();
}

// ────────────────────────── clear everything ─────────────────────────────
void Editor::_clear_all()
{
    m_vertices.clear();
    m_points.clear();
    m_lines.clear();
    m_drawing     = false;
    m_drag_point  = -1;
    m_next_id     = 1;
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
