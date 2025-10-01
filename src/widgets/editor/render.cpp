/***********************************************************************************
*
*       *********************************************************************
*       ****             R E N D E R . C P P  ____  F I L E              ****
*       *********************************************************************
*
*              AUTHOR:      Collin A. Bond.
*               DATED:      June 14, 2025.
*
**************************************************************************************
**************************************************************************************/
#include "widgets/editor/editor.h"



namespace cb { //     BEGINNING NAMESPACE "cb"...
// *************************************************************************** //
// *************************************************************************** //



// *************************************************************************** //
//
//
//      1.      RENDERING CORE MECHANICS OF THE WINDOW...
// *************************************************************************** //
// *************************************************************************** //

//  "_MECH_render_frame"        * NEW  _MECH  FUNCTION *
//
void Editor::_MECH_render_frame([[maybe_unused]] const Interaction & it) const
{
    using                           Layer           = ChannelCTX::Channel;
    const VertexStyle &             VS              = this->m_vertex_style;
    RenderCache &                   cache           = this->m_render_cache;

    

    ImPlot::PushPlotClipRect();
    ChannelCTX          CTX         (it.dl);
    VS                  .PushDL     (CTX.dl);
    //
    //
    //
    this->m_render_ctx.args.dl                      = CTX.dl;
    this->m_render_ctx.args.bezier_fill_steps       = this->m_style.ms_BEZIER_FILL_STEPS;
    this->m_render_ctx.args.bezier_segments         = this->m_style.ms_BEZIER_SEGMENTS;
    //
    //
    //
    this->_RENDER_update_render_cache();
    const std::span<const size_t>   z_view      ( cache.z_view.data(), cache.z_view.size() );
    //
    //  //      1.      RENDER "Grid" ELEMENTS.             [ Grid-Lines, Guides, etc ]...
        {
            ChannelCTX::Scope           scope       ( CTX,          Layer::Grid             );
            //  this->_RENDER_grid_channel              ( it.dl                                 );
        }
        
        
        //      2.      RENDER "Object" ELEMENTS.           [ AREA/FILL for each OBJECT (closed-path). ]...
        {
            ChannelCTX::Scope           scope       ( CTX,          Layer::Objects          );
            this->_RENDER_object_channel            ( z_view,       this->m_render_ctx      );
        }
        
        
        //      3.      RENDER "Highlights" ELEMENTS.       [ GLOW/HIGHLIGHT from Selection (ON-TOP of OBJ-FILL) ]...
        {
            ChannelCTX::Scope           scope       ( CTX,          Layer::Highlights       );
            //  this->render_selection_highlight        ( it.dl,        this->m_render_ctx      );
            this->_RENDER_highlights_channel        ( z_view,       this->m_render_ctx      );
        }
        
        
        //      4.      RENDER "Features" ELEMENTS.         [ STROKE/PERIMETER of each object (ON-TOP of OBJ-HIGHLIGHT) ]...
        {
            ChannelCTX::Scope           scope       ( CTX,          Layer::Features         );
            this->_RENDER_features_channel          ( z_view,       this->m_render_ctx      );
        }
        
        
        //      5.      RENDER "Accents" ELEMENTS.          [ VERTICES, POINTS, Accentuating Features (ON-TOP of PATH-STROKE) ]...
        {
            ChannelCTX::Scope           scope       ( CTX,          Layer::Accents          );
            //  this->_render_points                    ( z_view,       this->m_render_ctx      );
            this->_RENDER_accents_channel           ( z_view,       this->m_render_ctx      );
        }
        
        
        //      6.      RENDER "Glyph" ELEMENTS.            [ HANDLES, SELECTABLES, SNAPPING-LOCATIONS, USER-INTERACTIBLES (ON-TOP of ALL OBJECT GEOMETRY) ]...
        {
            ChannelCTX::Scope           scope       ( CTX,          Layer::Glyphs           );
            //  this->_RENDER_glyphs_channel            ( z_view,       this->m_render_ctx      );
        }
        
        
        //      7.      RENDER "Top" ELEMENTS.              [ TOP-MOST LAYER (RESERVED FOR CURSORS, ETC) ]...
        {
            ChannelCTX::Scope           scope       ( CTX,      Layer::Top          );
            //  this->_RENDER_top_channel            ( z_view,       this->m_render_ctx      );
        }
    //
    //
    //
    VS.PopDL();
    ImPlot::PopPlotClipRect();



    return;
}


//  "_RENDER_update_render_cache"
//
void Editor::_RENDER_update_render_cache(void) const noexcept
{
    auto  &         cache       = m_render_cache;
    auto  &         view        = cache.z_view;
    const size_t    N           = m_paths.size();

    //  Grow (or shrink) the view to match path count; re-use capacity.
    if ( view.size() != N )
    {
        view.resize(N);
        std::iota(view.begin(), view.end(), size_t{0});   // [0..N-1]
        cache.n_paths_last  = N;
        cache.dirty         = true;                        // content changed, must (re)sort
    }

    // Nothing to do if we’re clean and sizes match.
    if ( !cache.dirty )     { return; }

    //  Deterministic, stable-in-effect order: sort by z_index, then PathID.
    //  Use std::sort (in-place) with a tie-breaker instead of stable_sort (avoids extra allocations).
    std::sort( view.begin(), view.end(), [&](size_t ia, size_t ib) {
        const Path &     a       = m_paths[ia];
        const Path &     b       = m_paths[ib];

        if ( a.z_index != b.z_index )    { return a.z_index < b.z_index; }

        return a.id < b.id; // tie-breaker for deterministic render
    } );



    cache.dirty = false;

    return;
}



//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "MAIN RENDER FUNCTIONS".






// *************************************************************************** //
//
//
//
//      2.      NEW RENDERING FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

//  "_RENDER_grid_channel"
//     [ Grid-Lines, Guides, etc ]...
//
inline void Editor::_RENDER_grid_channel(std::span<const size_t> /*z_view*/, const RenderCTX & /*ctx*/) const noexcept
{
    return;
}


//  "_RENDER_object_channel"
//      [ AREA/FILL for each OBJECT (closed-path). ]...
//
inline void Editor::_RENDER_object_channel(std::span<const size_t> z_view, const RenderCTX & ctx) const noexcept
{
    for (size_t idx : z_view)
    {
        const Path &        path            = this->m_paths[idx];
        const bool          should_render   = ( path.IsVisible()  &&  path.IsArea()  &&  path.FillIsVisible() );
        
        if ( should_render )
            { path.render_fill_area(ctx); }
    }
    
    return;
}


//  "_RENDER_highlights_channel"
//      [ GLOW/HIGHLIGHT from Selection (ON-TOP of OBJ-FILL) ]...
//
inline void Editor::_RENDER_highlights_channel([[maybe_unused]] std::span<const size_t> z_view, const RenderCTX & ctx) const noexcept
{
    [[maybe_unused]] static const ImU32 &   hl_color    = this->m_style.HIGHLIGHT_COLOR;
    static const float                      hl_width    = this->m_style.HIGHLIGHT_WIDTH;
    static PathStyle                        hl_style    = {
        /*  stroke_color    */        this->m_style.HIGHLIGHT_COLOR
        /*  fill_color      */      , this->m_style.HIGHLIGHT_COLOR
        /*  stroke_width    */      , this->m_style.HIGHLIGHT_WIDTH
    };
    const BrowserState &                    BS          = this->m_browser_S;



    //      1.      RENDER HIGHLIGHT FOR SELECTED OBJECT...
    for (size_t idx : m_sel.paths)
    {
        const Path &        path            = this->m_paths[idx];
        const bool          should_render   = ( path.IsVisible() );
        
        if ( should_render )
        {
            hl_style.stroke_width = path.style.stroke_width + hl_width;
            path.render_highlight(hl_style, ctx);
        }
    }



    //      2.      RENDER SELECTION B-BOX...
    this->_render_selection_bbox        (ctx.args.dl);

    //      3.      RENDER SELECTED HANDLES B-BOX...
    this->_render_selected_handles      (ctx.args.dl);



    //      X.      RENDER AUXILIARY HIGHLIGHTS  [ FROM BROWSER ]...
    if ( BS.HasAuxiliarySelection() )
    {
        _render_auxiliary_highlights( ctx.args.dl );
    }


    return;
}


//  "_RENDER_features_channel"
//      [ STROKE/PERIMETER of each object (ON-TOP of OBJ-HIGHLIGHT) ]...
//
inline void Editor::_RENDER_features_channel(std::span<const size_t> z_view, const RenderCTX & ctx) const noexcept
{
    //      1.      TEXT...
    for ( size_t idx : z_view )
    {
        const Path &    path            = m_paths[idx];
        const bool      should_render   = ( path.IsVisible()  &&  path.IsPath()  &&  path.StrokeIsVisible() );  //  ( path.IsVisible()  &&  path.IsPath()  &&  path.StrokeIsVisible() );
        
        if ( should_render )    { path.render_stroke(ctx); }
    }
    
    
    //      2.      IF USING PEN-TOOL,  DRAW THE CURRENT PATH...
    if ( this->m_pen.active  &&  this->m_pen.path_index.has_value() )
    {
        this->m_paths[ (*this->m_pen.path_index) ].render_stroke(ctx);
    }
    
    return;
}
        
        
        
        
        

//  "_RENDER_accents_channel"
//      [ VERTICES, POINTS, Accentuating Features (ON-TOP of PATH-STROKE) ]...
//
inline void Editor::_RENDER_accents_channel(std::span<const size_t> z_view, const RenderCTX & ctx) const noexcept
{
    //  const size_t        N_vertices          = this->m_vertices.size();
    //  const auto &        hovered             = this->m_sel.hovered;
    
    
    
    
    
    //      1.      STATE-DEPENDENT VERTEX DRAWING...
    switch (this->m_mode)
    {
        //      CASE 1 :    Show Vertices for  *ALL*  OBJECTS...
        case Mode::AddAnchor    :
        case Mode::RemoveAnchor :
        {
            this->_ACCENTS_all_objects      (z_view, ctx);
            break;
        }
        //
        //
        //      CASE 2 :    Show  *ALL*  HANDLES...
        case Mode::EditAnchor   :
        {
            this->_ACCENTS_all_handles      (z_view, ctx);
            break;
        }
        //
        //
        //      CASE 3 :    Show  *ALL*  HANDLES...
        case Mode::Pen          :
        {
            this->_ACCENTS_geometry         (z_view, ctx);
            break;
        }
        //
        //
        //      DEFAULT :   ONLY Show Vertices for  *SELECTED*  OBJECTS...
        case Mode::Default      :
        default                 : {
            this->_ACCENTS_default          (z_view, ctx);
            break;
        }
    }
    
    return;
}


//  "_ACCENTS_default"
//
inline void Editor::_ACCENTS_default(std::span<const size_t> /* z_view */, const RenderCTX & ctx) const noexcept
{
    const size_t        N_vertices          = this->m_vertices.size();
    const size_t        N_paths             = this->m_paths.size();
    this->PushVertexStyle( VertexStyleType::Default );
    
    
    
    //      1.      DRAW OBJECT UNDER CURSOR...
    if ( this->m_sel.hovered.has_value() )
    {
        switch ( this->m_sel.hovered->type )
        {
            case Hit::Type::Vertex      : {
                if ( this->m_sel.hovered->index < N_vertices )
                    { this->m_vertices[ this->m_sel.hovered->index ].render(this->m_vertex_style); }
            
                break;
            }
            //
            case Hit::Type::Edge        :
            case Hit::Type::Surface     : {
                if ( this->m_sel.hovered->index < N_paths )
                    { this->m_paths[ this->m_sel.hovered->index ].render_vertices(ctx, this->m_vertex_style); }
                break;
            }
            //
            //
            default :       { break; }
        }
    }
 
    //      2.      IF USING PEN-TOOL,  DRAW EACH VERTEX FOR THE CURRENT PATH...
    if ( this->m_pen.active  &&  this->m_pen.path_index.has_value() )
    {
        this->m_paths[ (*this->m_pen.path_index) ].render_vertices(ctx, this->m_vertex_style);
    }
    this->PopVertexStyle();
 
    
    
    //      3.      ALWAYS DRAW VERTICES FOR HIGHLIGHTED OBJECTS...
    this->PushVertexStyle( VertexStyleType::Highlight );
    for (size_t idx : this->m_sel.paths)
    {
        const Path &        path            = this->m_paths[idx];
        const bool          should_render   = ( path.IsVisible()  &&  path.IsPath() );
        if ( should_render )
            { path.render_vertices(ctx, this->m_vertex_style); }
    }
    
    this->PopVertexStyle();
    return;
}


//  "_ACCENTS_geometry"
//
inline void Editor::_ACCENTS_geometry(std::span<const size_t> /* z_view */, const RenderCTX & ctx) const noexcept
{
    const size_t        N_vertices          = this->m_vertices.size();
    const size_t        N_paths             = this->m_paths.size();
    this->PushVertexStyle( VertexStyleType::Default );
    
    
    
    //      1.      DRAW OBJECT UNDER CURSOR...
    if ( this->m_sel.hovered.has_value() )
    {
        switch ( this->m_sel.hovered->type )
        {
            case Hit::Type::Vertex      : {
                if ( this->m_sel.hovered->index < N_vertices )
                    { this->m_vertices[ this->m_sel.hovered->index ].render(this->m_vertex_style); }
            
                break;
            }
            //
            case Hit::Type::Edge        :
            case Hit::Type::Surface     : {
                if ( this->m_sel.hovered->index < N_paths )
                    { this->m_paths[ this->m_sel.hovered->index ].render_vertices(ctx, this->m_vertex_style); }
                break;
            }
            //
            //
            default :       { break; }
        }
    }
 
    //      2.      IF USING PEN-TOOL,  DRAW EACH VERTEX FOR THE CURRENT PATH...
    if ( this->m_pen.active  &&  this->m_pen.path_index.has_value() )
    {
        //  this->m_paths[ (*this->m_pen.path_index) ].render_vertices(ctx, this->m_vertex_style);
        this->m_paths[ (*this->m_pen.path_index) ].render_vertices_all(ctx, this->m_vertex_style);
    }
    this->PopVertexStyle();
 
    
    
    //      3.      ALWAYS DRAW VERTICES FOR HIGHLIGHTED OBJECTS...
    this->PushVertexStyle( VertexStyleType::Highlight );
    for (size_t idx : this->m_sel.paths)
    {
        const Path &        path            = this->m_paths[idx];
        const bool          should_render   = ( path.IsVisible()  &&  path.IsPath() );
        if ( should_render )
            { path.render_vertices(ctx, this->m_vertex_style); }
    }
 
    this->PopVertexStyle();
    return;
}


//  "_ACCENTS_all_objects"
//
inline void Editor::_ACCENTS_all_objects(std::span<const size_t> /* z_view */, const RenderCTX & ctx) const noexcept
{
    this->PushVertexStyle( VertexStyleType::Default );
    
    //      1.      DRAW VERTICES FOR ALL OBJECTS...
    for (const Path & path : this->m_paths)
    {
        const bool          should_render   = ( path.IsVisible()  &&  path.IsPath() );
        if ( should_render )
            { path.render_vertices(ctx, this->m_vertex_style); }
    }
    
    this->PopVertexStyle();
    return;
}


//  "_ACCENTS_all_handles"
//
inline void Editor::_ACCENTS_all_handles(std::span<const size_t> /* z_view */, const RenderCTX & ctx) const noexcept
{
    this->PushVertexStyle( VertexStyleType::Default );
    
    //      1.      DRAW  *ALL*  VERTICES...
    for (const Path & path : this->m_paths)
    {
        const bool          should_render   = ( path.IsVisible()  &&  path.IsPath() );
        if ( should_render )
            { path.render_vertices(ctx, this->m_vertex_style); }
    }
    this->PopVertexStyle();
    
    
    
    this->PushVertexStyle( VertexStyleType::Highlight );
    for (size_t idx : this->m_sel.paths)
    {
        const Path &        path            = this->m_paths[idx];
        const bool          should_render   = ( path.IsVisible()  &&  path.IsPath() );
        if ( should_render )
            { path.render_vertices_all(ctx, this->m_vertex_style); }
    }
 
    this->PopVertexStyle();
    return;
}







//  "_RENDER_glyphs_channel"
//      [ HANDLES, SELECTABLES, SNAPPING-LOCATIONS, USER-INTERACTIBLES (ON-TOP of ALL OBJECT GEOMETRY) ]...
//
inline void Editor::_RENDER_glyphs_channel(std::span<const size_t> /*z_view*/, const RenderCTX & /*ctx*/) const noexcept
{
    return;
}
        
//  "_RENDER_top_channel"
//      [ TOP-MOST LAYER (RESERVED FOR CURSORS, ETC) ]...
//
inline void Editor::_RENDER_top_channel(std::span<const size_t> /*z_view*/, const RenderCTX & /*ctx*/) const noexcept
{
    return;
}



//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "NEW RENDERING".






// *************************************************************************** //
//
//
//
//      2.      PRIMARY RENDERING OPERATIONS...
// *************************************************************************** //
// *************************************************************************** //


//  "_render_paths"
//
void Editor::_render_paths(ImDrawList * dl) const 
{
    //  1.  BUILD THE DRAW-LIST (USE VISIBLE PATHS ONLY)...
    std::vector<const Path*>    draw_vec;
    draw_vec.reserve(m_paths.size());


    for (const Path & p : m_paths) {
        if (p.visible)              { draw_vec.push_back(&p); }// NEW visibility filter
    }


    //  2.  STABLE-SORT BY Z-INDEX (Low Z: Background → High Z: Foreground)...
    std::stable_sort( draw_vec.begin(), draw_vec.end(),
                      [](const Path* a, const Path* b) { return a->z_index < b->z_index; } );


    //  3.  DRAW EACH PATH IN SORTED ORDER...
    for (const Path * pp : draw_vec)
    {
        const Path &    p   = *pp;
        const size_t    N   = p.verts.size();
        if (N < 2) continue;

        // ───── Filled-area pass (only for closed paths with non-transparent fill)
        if (p.IsArea() && (p.style.fill_color & 0xFF000000))
        {
            dl->PathClear();

            for (size_t i = 0; i < N; ++i)
            {
                const Vertex* a = find_vertex(m_vertices, p.verts[i]);
                const Vertex* b = find_vertex(m_vertices, p.verts[(i + 1) % N]);
                if (!a || !b) continue;

                if (!is_curved<VertexID>(a, b)) {
                    ImVec2 pa = world_to_pixels({ a->x, a->y });
                    dl->PathLineTo(pa);
                }
                else {
                    for (int step = 0; step <= m_style.ms_BEZIER_FILL_STEPS; ++step) {
                        float  t  = static_cast<float>(step) / m_style.ms_BEZIER_FILL_STEPS;
                        ImVec2 wp = cubic_eval<VertexID>(a, b, t);
                        dl->PathLineTo(world_to_pixels(wp));
                    }
                }
            }
            dl->PathFillConvex(p.style.fill_color);
        }

        // ───── Lambda to draw one segment (straight or cubic)
        auto draw_seg = [&](const Vertex * a, const Vertex * b)
        {
            const bool curved = is_curved<VertexID>(a, b);

            if (!curved) {
                dl->AddLine(world_to_pixels({ a->x, a->y }),
                            world_to_pixels({ b->x, b->y }),
                            p.style.stroke_color,
                            p.style.stroke_width);
            }
            else {
                ImVec2 P0 = world_to_pixels({ a->x,                                     a->y                                });
                ImVec2 P1 = world_to_pixels({ a->x + a->m_bezier.out_handle.x,          a->y + a->m_bezier.out_handle.y     });
                ImVec2 P2 = world_to_pixels({ b->x + b->m_bezier.in_handle.x,           b->y + b->m_bezier.in_handle.y      });
                ImVec2 P3 = world_to_pixels({ b->x,                                     b->y                                });

                dl->AddBezierCubic(P0, P1, P2, P3,
                                   p.style.stroke_color,
                                   p.style.stroke_width,
                                   m_style.ms_BEZIER_SEGMENTS);   // 0 ⇒ default tessellation
            }
        };

        // ───── Stroke contiguous segments
        for ( size_t i = 0; i < N - 1; ++i ) {
            if ( const Vertex* a = find_vertex(m_vertices, p.verts[i]) ) {
                if ( const Vertex* b = find_vertex(m_vertices, p.verts[i + 1]) )    { draw_seg(a, b); }
            }
        }

        // Close the loop if required
        if (p.closed) {
            if ( const Vertex* a = find_vertex(m_vertices, p.verts.back()) ) {
                if ( const Vertex* b = find_vertex(m_vertices, p.verts.front()) )   { draw_seg(a, b); }
            }
        }
    }
    
    return;
}


//  "_render_points"
//
void Editor::_render_points(ImDrawList * dl) const
{
    for (size_t i = 0; i < m_points.size(); ++i)
    {
        const Point &       pt      = m_points[i];
        if (!pt.sty.visible)        { continue; }

        const Vertex *      v       = find_vertex(m_vertices, pt.v);
        if ( !v )                   { continue; }




        ImU32 col = (m_dragging && m_sel.points.count(i))
                        ? m_style.COL_POINT_HELD
                        : pt.sty.color;

        ImVec2 pix = world_to_pixels({ v->x, v->y });   // NEW transform
        dl->AddCircleFilled(pix, pt.sty.radius, col, 12);
    }
    
    return;
}

//
//
// *************************************************************************** //
// *************************************************************************** //   END "PRIMARY RENDER OPs".






// *************************************************************************** //
//
//
//
//      3.      "SELECTION HIGHLIGHT" RENDERING / INTERACTIBLES...
// *************************************************************************** //
// *************************************************************************** //

//  "render_selection_highlight"
//      Draw outlines for selected primitives plus bbox/handles.
//
void Editor::render_selection_highlight(ImDrawList * dl, const RenderCTX & ctx) const noexcept
{
    //  const ImU32 &               col         = m_style.COL_SELECTION_OUT;
    const BrowserState &        BS          = this->m_browser_S;


    this->_render_selection_objects     (ctx);
    this->_render_selection_bbox        (dl);
    this->_render_selected_handles      (dl);
    //  this->_render_selected_handles      (dl);
    
    if ( BS.HasAuxiliarySelection() )
    {
        _render_auxiliary_highlights(dl);
    }
    
    return;
}




//  "_render_selected_objects"
//
inline void Editor::_render_selection_objects(const RenderCTX & ctx) const noexcept
{
    //  static const ImU32 &        col         = this->m_style.HIGHLIGHT_COLOR;
    static const float          w           = this->m_style.HIGHLIGHT_WIDTH;
    static PathStyle            style       = {
        /*  stroke_color    */        this->m_style.HIGHLIGHT_COLOR
        /*  fill_color      */      , this->m_style.HIGHLIGHT_COLOR
        /*  stroke_width    */      , this->m_style.HIGHLIGHT_WIDTH
    };


    //      1.      ITERATE THROUGH EACH SELECTED PATH...
    for (size_t idx : m_sel.paths)
    {
        const Path &        path            = this->m_paths[idx];
        const bool          should_render   = ( path.IsVisible() );
        
        
        if ( should_render )
        {
            style.stroke_width = path.style.stroke_width + w;
            path.render_highlight(style, ctx);
        }
    }
    
    return;
}





/*
{
    const ImU32 &               col                         = m_style.COL_SELECTION_OUT;
    //  constexpr float             ms_VERTEX_HIGHLIGHT_WIDTH   = 2.0f;
    //  constexpr float             ms_PATH_HIGHLIGHT_WIDTH     = 2.0f;



    //      1.      HIGHLIGHT SELECTED PATHS...
    for (size_t idx : m_sel.paths)
    {
        if ( idx >= m_paths.size() )            { continue; }
        const Path &        p           = m_paths[idx];
        const size_t        N           = p.verts.size();
        if (N < 2)                              { continue; }


        auto                draw_seg    = [&](const Vertex & a, const Vertex & b)
        {
            const float w = p.style.stroke_width + 2.0f;
            if ( Vertex::SegmentIsCurved(a, b) )    //  is_curved<VertexID>(a,b)
            {
                ImVec2 P0 = this->world_to_pixels({ a.x,                               a.y                                });
                ImVec2 P1 = this->world_to_pixels({ a.x + a.m_bezier.out_handle.x,     a.y + a.m_bezier.out_handle.y      });
                ImVec2 P2 = this->world_to_pixels({ b.x + b.m_bezier.in_handle.x,      b.y + b.m_bezier.in_handle.y       });
                ImVec2 P3 = this->world_to_pixels({ b.x,                               b.y                                });
                dl->AddBezierCubic(P0, P1, P2, P3, col, w, m_style.ms_BEZIER_SEGMENTS);
            }
            else
            {
                dl->AddLine( this->world_to_pixels({ a.x, a.y }), this->world_to_pixels({ b.x, b.y }), col, w );
            }
        };



        for (size_t i = 0; i < N - 1; ++i)
        {
            if ( const Vertex * a = find_vertex(m_vertices, p.verts[i]) )
            {
                if ( const Vertex * b = find_vertex(m_vertices, p.verts[i+1]) )
                    draw_seg(*a, *b);
            }
        }

        if ( p.closed )
        {
            if ( const Vertex * a = find_vertex(m_vertices, p.verts.back()) )
            {
                if ( const Vertex * b = find_vertex(m_vertices, p.verts.front()) )
                    draw_seg(*a, *b);
            }
        }
    }


    //      2.      HIGHLIGHT SELECTED POINTS...
    this->PushVertexStyle( VertexStyleType::Highlight );
    for (size_t idx : m_sel.points)
    {
        if ( idx >= m_points.size() )       { continue; }       //  CASE 0 :    INVALID POINT...
        
        
        //      1.      GET REFERENCE TO THE POINT.  OBTAIN THIS POINT'S VERTEX...
        const Point &           point       = m_points[idx];
        const Vertex *          v_ptr       = find_vertex( this->m_vertices, point.v );
        
        if ( !v_ptr )                       { continue; }       //  CASE 1 :    INVALID VERTEX...
    
        const Vertex &          vtx         = *(v_ptr);
        vtx.render( this->m_vertex_style );
    
    }
    this->PopVertexStyle();



    return;
}*/


/*{
    const ImU32 &               col                         = m_style.COL_SELECTION_OUT;
    //  constexpr float             ms_VERTEX_HIGHLIGHT_WIDTH   = 2.0f;
    //  constexpr float             ms_PATH_HIGHLIGHT_WIDTH     = 2.0f;



    //      1.      HIGHLIGHT SELECTED PATHS...
    for (size_t idx : m_sel.paths)
    {
        if ( idx >= m_paths.size() )            { continue; }
        const Path &        p           = m_paths[idx];
        const size_t        N           = p.verts.size();
        if (N < 2)                              { continue; }


        auto                draw_seg    = [&](const Vertex & a, const Vertex & b)
        {
            const float w = p.style.stroke_width + 2.0f;
            if ( is_curved<VertexID>(a,b)  )
            {
                ImVec2 P0 = this->world_to_pixels({ a.x,                               a.y                                });
                ImVec2 P1 = this->world_to_pixels({ a.x + a.m_bezier.out_handle.x,     a.y + a.m_bezier.out_handle.y      });
                ImVec2 P2 = this->world_to_pixels({ b.x + b.m_bezier.in_handle.x,      b.y + b.m_bezier.in_handle.y       });
                ImVec2 P3 = this->world_to_pixels({ b.x,                               b.y                                });
                dl->AddBezierCubic(P0, P1, P2, P3, col, w, m_style.ms_BEZIER_SEGMENTS);
            }
            else
            {
                dl->AddLine( this->world_to_pixels({ a->x, a->y }), this->world_to_pixels({ b->x, b->y }), col, w );
            }
        };

        for (size_t i = 0; i < N - 1; ++i)
        {
            if ( const Vertex* a = find_vertex(m_vertices, p.verts[i]) )
            {
                if (const Vertex * b = find_vertex(m_vertices, p.verts[i+1]))
                    draw_seg(a, b);
            }
        }

        if ( p.closed ) {
            if ( const Vertex * a = find_vertex(m_vertices, p.verts.back()) )
            {
                if ( const Vertex* b = find_vertex(m_vertices, p.verts.front()) )
                    draw_seg(a, b);
            }
        }
    }


    //      2.      HIGHLIGHT SELECTED POINTS...
    this->PushVertexStyle( VertexStyleType::Highlight );
    for (size_t idx : m_sel.points)
    {
        if ( idx >= m_points.size() )       { continue; }       //  CASE 0 :    INVALID POINT...
        
        
        //      1.      GET REFERENCE TO THE POINT.  OBTAIN THIS POINT'S VERTEX...
        const Point &           point       = m_points[idx];
        const Vertex *          v_ptr       = find_vertex( this->m_vertices, point.v );
        
        if ( !v_ptr )                       { continue; }       //  CASE 1 :    INVALID VERTEX...
    
        const Vertex &          vtx         = *(v_ptr);
        vtx.render( this->m_vertex_style );
    
    }
    this->PopVertexStyle();
    
    
    //  for (size_t idx : m_sel.points)
    //  {
    //      if ( idx >= m_points.size() )       { continue; }
    //      const Point &           pt  = m_points[idx];
    //
    //      if (const Vertex *      v   = find_vertex(m_vertices, pt.v))
    //      {
    //          ImVec2  scr     = world_to_pixels({ v->x, v->y });
    //          dl->AddCircle( scr,
    //                         pt.sty.radius + ms_PATH_HIGHLIGHT_WIDTH,        // small outset
    //                         col, 0, ms_PATH_HIGHLIGHT_WIDTH  );               // thickness 2 px
    //      }
    //  }

    return;
}*/




//  "_render_selected_handles"
//
inline void Editor::_render_selected_handles(ImDrawList * /* dl */) const noexcept
{
    for (const Vertex & v : m_vertices)
    {
        if ( !m_show_handles.count(v.id) )      { continue; }   // ← NEW visibility mask
        v.render_all( this->m_vertex_style );
    }


    return;
}
/*
{
    auto ws2px = [this](ImVec2 w){ return world_to_pixels(w); };

    for (const Vertex & v : m_vertices)
    {
        if ( !m_show_handles.count(v.id) )      { continue; }   // ← NEW visibility mask

        ImVec2 a = ws2px({ v.x, v.y });

        auto draw_handle = [&](const ImVec2 & off)
        {
            if ( (off.x == 0.f)  &&  (off.y == 0.f) )   { return; }
            
            ImVec2 h = ws2px({ v.x + off.x, v.y + off.y });
            dl->AddLine(a, h, m_style.ms_HANDLE_COLOR, 1.0f);
            dl->AddRectFilled({ h.x - m_style.ms_HANDLE_SIZE, h.y - m_style.ms_HANDLE_SIZE },
                              { h.x + m_style.ms_HANDLE_SIZE, h.y + m_style.ms_HANDLE_SIZE },
                              m_style.ms_HANDLE_COLOR);
        };

        draw_handle(v.m_bezier.out_handle);
        draw_handle(v.m_bezier.in_handle);
    }
    
    return;
}*/


//  "_render_selection_bbox"
//
inline void Editor::_render_selection_bbox(ImDrawList * dl) const noexcept
{
    constexpr int           N               = static_cast<int>( BoxDrag::Anchor::COUNT );
    const auto &            view            = m_boxdrag.view;
    BoxDrag::Anchor         handle          = static_cast<BoxDrag::Anchor>( 0 );
    
    
    //  CASE 0 :    SELECTION BOX IS INACTIVE...
    if ( !view.visible )                   { return; }

    //      Draw expanded bbox (from cache)
    const ImVec2            p0              = world_to_pixels(view.tl_ws);
    const ImVec2            p1              = world_to_pixels(view.br_ws);
    
    
    dl->AddRect( p0, p1,
                 m_style.SELECTION_BBOX_COL, 0.0f,
                 ImDrawFlags_None, m_style.SELECTION_BBOX_TH );


    //      Draw the 8 handles using cached rectangles and cached hover index
    for ( int i = 0; i < N; handle = static_cast<BoxDrag::Anchor>(++i) )
    {
        const bool          hovered     = (handle == view.hover_idx);      // ← read, don’t compute
        const ImRect &      r           = view.handle_rect_px[handle];
        dl->AddRectFilled( r.Min, r.Max, (hovered)
                                ? m_style.ms_HANDLE_HOVER_COLOR
                                : m_style.ms_HANDLE_COLOR
        );
    }
    
    return;
}

/*{
    const bool has_paths_or_lines = !m_sel.paths.empty() || !m_sel.lines.empty();
    const bool single_vertex_only = (m_sel.vertices.size() <= 1) && !has_paths_or_lines;
    if (single_vertex_only) return;

    ImVec2 tl_tight, br_tight;
    if (!_selection_bounds(tl_tight, br_tight, this->m_render_ctx)) { m_hover_handle = -1; return; }

    // NEW: robust expansion (pixel-space min/max)
    const auto [tl, br] = _expand_bbox_by_pixels(tl_tight, br_tight, this->m_style.SELECTION_BBOX_MARGIN_PX);

    auto ws2px = [this](ImVec2 w){ return world_to_pixels(w); };
    ImVec2 p0 = ws2px(tl);
    ImVec2 p1 = ws2px(br);

    dl->AddRect(p0, p1, m_style.SELECTION_BBOX_COL, 0.0f,
                ImDrawFlags_None, m_style.SELECTION_BBOX_TH);

    // Handle positions from expanded WS box
    ImVec2 hw{ (tl.x + br.x) * 0.5f, (tl.y + br.y) * 0.5f };
    const ImVec2 ws[8] = {
        tl, { hw.x, tl.y }, { br.x, tl.y }, { br.x, hw.y },
        br, { hw.x, br.y }, { tl.x, br.y }, { tl.x, hw.y }
    };

    m_hover_handle = -1;
    for (int i = 0; i < 8; ++i)
    {
        ImVec2 s = ws2px(ws[i]);
        ImVec2 min{ s.x - m_style.HANDLE_BOX_SIZE, s.y - m_style.HANDLE_BOX_SIZE };
        ImVec2 max{ s.x + m_style.HANDLE_BOX_SIZE, s.y + m_style.HANDLE_BOX_SIZE };

        bool hovered = ImGui::IsMouseHoveringRect(min, max);
        if (hovered) m_hover_handle = i;

        dl->AddRectFilled(min, max, hovered ? m_style.ms_HANDLE_HOVER_COLOR
                                            : m_style.ms_HANDLE_COLOR);
    }
    
    return;
}*/


/*{
    const bool      has_paths_or_lines      = !m_sel.paths.empty() || !m_sel.lines.empty();
    const bool      single_vertex_only      = (m_sel.vertices.size() <= 1) && !has_paths_or_lines;
    
    if ( single_vertex_only )               { return; }

    ImVec2      tl, br;
    if ( !_selection_bounds(tl, br, this->m_render_ctx) )       { m_hover_handle = -1; return; }

    auto        ws2px       = [this](ImVec2 w){ return world_to_pixels(w); };
    ImVec2      p0          = ws2px(tl);
    ImVec2      p1          = ws2px(br);

    dl->AddRect(p0, p1, m_style.SELECTION_BBOX_COL, 0.0f,
                ImDrawFlags_None, m_style.SELECTION_BBOX_TH);

    // handle positions
    ImVec2 hw{ (tl.x + br.x) * 0.5f, (tl.y + br.y) * 0.5f };
    const ImVec2 ws[8] = {
        tl,
        { hw.x, tl.y },
        { br.x, tl.y },
        { br.x, hw.y },
        br,
        { hw.x, br.y },
        { tl.x, br.y },
        { tl.x, hw.y }
    };

    m_hover_handle = -1;
    for (int i = 0; i < 8; ++i)
    {
        ImVec2 s = ws2px(ws[i]);
        ImVec2 min{ s.x - m_style.HANDLE_BOX_SIZE, s.y - m_style.HANDLE_BOX_SIZE };
        ImVec2 max{ s.x + m_style.HANDLE_BOX_SIZE, s.y + m_style.HANDLE_BOX_SIZE };

        bool hovered = ImGui::IsMouseHoveringRect(min, max);
        if (hovered) m_hover_handle = i;

        dl->AddRectFilled(min, max, hovered ? m_style.ms_HANDLE_HOVER_COLOR
                                            : m_style.ms_HANDLE_COLOR);
    }
    
    return;
}*/



//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "MAIN HIGHLIGHT".






// *************************************************************************** //
//
//
//
//      4.      "AUXILIARY" HIGHLIGHT RENDERING...
// *************************************************************************** //
// *************************************************************************** //

//  "_render_auxiliary_highlights"
//
inline void Editor::_render_auxiliary_highlights(ImDrawList * dl) const noexcept
{
    const BrowserState &            BS              = this->m_browser_S;
    const int &                     pidx            = BS.m_hovered_obj;
    const std::pair<int,int> &      vidx            = BS.m_hovered_vertex;
    //
    const bool                      draw_object     = ( pidx >= 0 );
    const bool                      draw_handle     = ( (vidx.first >= 0)  &&  (vidx.second >= 0) );
    //
    this->PushVertexStyle( VertexStyleType::Highlight );
    
    
    
    //      1.      RENDER THE OBJECT...
    if ( draw_object )
    {
        const Path & path   = this->m_paths[ static_cast<size_t>( pidx ) ];
        _auxiliary_highlight_object(path, dl);
    }
    
    
    //      2.      RENDER THE HANDLE...
    if ( draw_handle )
    {
        const Path &        v_path      = this->m_paths[ static_cast<size_t>(vidx.first) ];
        const VertexID      vid         = v_path.verts[ static_cast<size_t>(vidx.second) ];
        const Vertex *      v           = find_vertex_mut( m_vertices, vid );     IM_ASSERT( v != nullptr );
        
        _auxiliary_highlight_handle( *v, dl );
    }
    
    
    this->PopVertexStyle();
    BS.ClearAuxiliarySelection();
    return;
}


//  "_auxiliary_highlight_object"
//
inline void Editor::_auxiliary_highlight_object(const Path & p, ImDrawList * dl) const noexcept
{
    const ImU32 &               col             = ImGui::GetColorU32(ImGuiCol_FrameBgHovered); //  .AUX_HIGHLIGHT_COLOR;
    const float &               w               = this->m_style.HIGHLIGHT_WIDTH;
    //  const ImU32 &               col         = m_style.AUX_HIGHLIGHT_COLOR;
    //  const float                 w           = p.style.stroke_width + 2.0f;
    //
    //
    //
    const size_t                N               = p.verts.size();
    //
    //
    //  "ws2px"
    auto                        ws2px           = [](ImVec2 w) {
        ImPlotPoint pp = ImPlot::PlotToPixels(ImPlotPoint(w.x, w.y));
        return ImVec2{ static_cast<float>(pp.x), static_cast<float>(pp.y) };
    };
    //
    //  "draw_seg"
    auto                        draw_seg        = [&](const Vertex * a, const Vertex * b)
    {
        if ( !a || !b )         { return; }
        if ( is_curved<VertexID>(a, b) ) {
            ImVec2 P0 = ws2px({ a->x,                                   a->y                                });
            ImVec2 P1 = ws2px({ a->x + a->m_bezier.out_handle.x,        a->y + a->m_bezier.out_handle.y     });
            ImVec2 P2 = ws2px({ b->x + b->m_bezier.in_handle.x,         b->y + b->m_bezier.in_handle.y      });
            ImVec2 P3 = ws2px({ b->x,                                   b->y                                });
            dl->AddBezierCubic(P0, P1, P2, P3, col, w, m_style.ms_BEZIER_SEGMENTS);
        }
        else
        {
            dl->AddLine( ws2px({ a->x, a->y }), ws2px({ b->x, b->y }), col, w );
        }
    };



    for (size_t i = 0; i + 1 < N; ++i)
    {
        const Vertex *      a   = find_vertex(m_vertices, p.verts[i]);
        const Vertex *      b   = find_vertex(m_vertices, p.verts[i + 1]);
        draw_seg(a, b);
    }
    if ( p.closed )
    {
        const Vertex *      a   = find_vertex(m_vertices, p.verts.back());
        const Vertex *      b   = find_vertex(m_vertices, p.verts.front());
        draw_seg(a, b);
    }
    
    
    
    return;
}


//  "_auxiliary_highlight_handle"
//
inline void Editor::_auxiliary_highlight_handle(const Vertex & v, ImDrawList * /*dl*/) const noexcept
{
    //  const ImU32 &               col             = ImGui::GetColorU32(ImGuiCol_FrameBgHovered);  //  m_style.AUX_HIGHLIGHT_COLOR;
    //  const float &               w               = this->m_style.AUX_HIGHLIGHT_WIDTH;            //  p.style.stroke_width + 2.0f;
    
    
    //      2.      DRAW HOVERED VERTEX...
    v.render_all( this->m_vertex_style );
    
    
    
    //  const ImVec2    a       = ws2px({ v.x, v.y });
    //  draw_handle             ( v, v.out_handle, a );
    //  draw_handle             ( v, v.in_handle,  a );
    
    
    return;
}



//
//
// *************************************************************************** //
// *************************************************************************** //   END "AUXILIARY HIGHLIGHT".

























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
