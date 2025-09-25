/***********************************************************************************
*
*       *********************************************************************
*       ****     B R O W S E R _ P A N E L S . C P P  ____  F I L E      ****
*       *********************************************************************
*
*              AUTHOR:      Collin A. Bond.
*               DATED:      August 20, 2025.
*
**************************************************************************************
**************************************************************************************/
#include "widgets/editor/editor.h"



namespace cb {  //     BEGINNING NAMESPACE "cb"...
// *************************************************************************** //
// *************************************************************************** //








//      1.      "PROPERTIES" TRAIT / PANEL FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

//  "_draw_properties_panel_single"
//
void Editor::_draw_properties_panel_single(Path & path, const size_t pidx, const LabelFn & callback)
{
    enum class ObjectType               { None = 0, Stroke, COUNT };
    //
    constexpr ImGuiColorEditFlags       COLOR_FLAGS             = ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaPreviewHalf;
    static constexpr const size_t &     BUFFER_SIZE             = BrowserState::ms_MAX_PATH_TITLE_LENGTH;
    //
    //
    //
    //  BrowserStyle &                      BStyle                  = this->m_style.browser_style;
    //  EditorState &                       EState                  = m_editor_S;
    BrowserState &                      BState                  = m_browser_S;
    //
    static char                         title                   [ BUFFER_SIZE ];   // safe head-room
    static PathID                       cache_id                = static_cast<PathID>(-1);
    //
    //
    //
    //  const float &                       LABEL_W                 = BStyle.ms_BROWSER_VERTEX_LABEL_WIDTH;
    //  const float &                       WIDGET_W                = BStyle.ms_BROWSER_VERTEX_WIDGET_WIDTH;
    const bool                          is_area                 = path.IsArea();



    //      CASE 0 :    ERROR...
    if ( pidx >= m_paths.size() )   {
        ImGui::SeparatorText("[invalid object]");
        cache_id = static_cast<PathID>(-1);
        return;
    }
    
    
    
    
    
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
        //          ...
    //
    //



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


            callback("Line Color:");
            stroke_dirty                        = ImGui::ColorEdit4( "##Editor_VertexInspector_LineColor",    (float*)&stroke_f,  COLOR_FLAGS );
            //
            //
            callback("Fill Color:");
            ImGui::BeginDisabled( !is_area );
                fill_dirty                      = ImGui::ColorEdit4( "##Editor_VertexInspector_FillColor",    (float*)&fill_f,
                                                                     (is_area)  ? COLOR_FLAGS   : COLOR_FLAGS | ImGuiColorEditFlags_NoDragDrop );
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
            
            
            callback("Stroke Weight:");
            ImGui::PushItemWidth(200.0f);
            //
            //  CASE 1 :    Value < 2.0f
            if (w < 2.0f) {
                dirty = ImGui::InputFloat("##Editor_VertexBrowser_LineWidth", &w,   0.125f,     2.0f,       "%.3f px");
            }
            //
            //  CASE 2 :    2.0 <= Value.
            else {
                dirty = ImGui::InputFloat("##Editor_VertexBrowser_LineWidth", &w,   1.0f,       2.0f,       "%.2f px");
            }
            ImGui::PopItemWidth();
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
            callback("Z-Index:");
            ImGui::Text("%3u", path.z_index);
            
            //      if ( ImGui::SmallButton("Bring To Front")   )       { bring_selection_to_front();   }
            //
            //      if ( ImGui::SmallButton("Bring Forward")    )       { bring_selection_forward();    }
            //
            //      if ( ImGui::SmallButton("Send Backward")    )       { send_selection_backward();    }
            //
            //      if ( ImGui::SmallButton("Send To Back")     )       { send_selection_to_back();     }
        }
    //
    //
    //
    


    
    return;
}




//  "_draw_properties_panel_multi"
//      Draws the right-hand, INSPECTOR COLUMN for case of: SELECTION CONTAINS MORE THAN ONE OBJECT.
//
void Editor::_draw_properties_panel_multi([[maybe_unused]] const LabelFn & callback)
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



//
//
// *************************************************************************** //
// *************************************************************************** //   END "PROPERTIES" PANELS.







// *************************************************************************** //
//
//
//
//      2.      "VERTEX" TRAIT / PANEL FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

//  "_draw_vertex_panel"
//
void Editor::_draw_vertex_panel(Path & path, [[maybe_unused]] const size_t pidx, [[maybe_unused]] const LabelFn & callback)
{
    BrowserStyle &                  BStyle              = this->m_style.browser_style;



    ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize,  BStyle.ms_CHILD_BORDER2);
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding,    BStyle.ms_CHILD_ROUND2);
    ImGui::PushStyleColor(ImGuiCol_ChildBg,             BStyle.ms_VERTEX_SELECTOR_FRAME_BG);
    //
    //
    //  //  4B.1.    LEFT-HAND VERTEX BROWSER.
        ImGui::SetNextWindowSizeConstraints( BStyle.VERTEX_SELECTOR_DIMS.limits.min, BStyle.VERTEX_SELECTOR_DIMS.limits.max );
        ImGui::BeginChild("##Editor_Browser_VertexSelectorColumn",      BStyle.VERTEX_SELECTOR_DIMS.value,    BStyle.DYNAMIC_CHILD_FLAGS);
            _draw_vertex_selector_column(path, pidx);
            BStyle.VERTEX_SELECTOR_DIMS.value.x        = ImGui::GetItemRectSize().x;
        ImGui::EndChild();
        ImGui::PopStyleColor();
        //
        //
        ImGui::SameLine(0.0f);
        //
        //
        //  4B.2.   RIGHT-HAND VERTEX BROWSER.
        const float             P1C1_w              = ImGui::GetContentRegionAvail().x;
        ImGui::PushStyleColor(ImGuiCol_ChildBg, BStyle.ms_VERTEX_INSPECTOR_FRAME_BG);
        ImGui::BeginChild("##Editor_Browser_VertexInspectorColumn",     ImVec2(P1C1_w, 0.0f),    BStyle.STATIC_CHILD_FLAGS);
            _draw_vertex_inspector_column(path, callback);
        ImGui::EndChild();
        ImGui::PopStyleColor();
    //
    ImGui::PopStyleVar(2);   // border size, rounding
            
            
            
    return;
}


//  "_draw_vertex_selector_column"
//
void Editor::_draw_vertex_selector_column(Path & path, const size_t path_idx)
{
    using                                       namespace               icon;
    static constexpr ImGuiTableFlags            TABLE_FLAGS             = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_ScrollY | ImGuiTableFlags_RowBg;
    static constexpr ImGuiTableColumnFlags      C1_FLAGS                = ImGuiTableColumnFlags_NoHeaderLabel | ImGuiTableColumnFlags_WidthStretch;
    static constexpr ImGuiSelectableFlags       SELECTABLE_FLAGS        = ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowDoubleClick;
    //
    //
    //
    static PathID                               cache_ID                = path.id;
    //
    const int                                   N_vertices              = static_cast<int>( path.verts.size() );
    BrowserState &                              BS                      = this->m_browser_S;
    ImGuiListClipper                            clipper;



    //      0.      HEADER...
    this->S.HeadlineSeparatorText("Vertices");


    //  CASE 0 :    INVALID OBJECT (0 Vertices should NEVER occur)...
    if (N_vertices < 1) {
        IM_ASSERT(true && "vertex selector table was called with ZERO vertices in the path");
        ImGui::TextColored( this->S.SystemColor.Red, "%s", "[ STALE OBJECT ERROR ]");
        return;
    }
    
    
    
    //      1.      UPDATE CACHE...
    if ( cache_ID != path.id )
    {
        cache_ID                    = path.id;
        BS.m_inspector_vertex_idx   = -1;
    }
    

    //  2.  BEGIN THE TABLE TO PRESENT EACH OBJECT...
    if ( ImGui::BeginTable("##Editor_VertexBrowser_SelectorTable", 1, TABLE_FLAGS, ImVec2(0, -1)) )
    {
        ImGui::TableSetupColumn ("Selectable",         C1_FLAGS);


        clipper.Begin( N_vertices, -1 );


        //  3.  DRAWING EACH OBJECT IN THE LEFT-HAND SELECTION COLUMN OF THE BROWSER...
        while ( clipper.Step() )
        {
            for (int row = clipper.DisplayStart; row < clipper.DisplayEnd; ++row)
            {
                char                        buffer [Vertex::ms_MAX_VERTEX_NAME_LENGTH]          = {  };
                const bool                  selected        = ( row == BS.m_inspector_vertex_idx );
                
                
                std::snprintf(buffer, Vertex::ms_MAX_VERTEX_NAME_LENGTH, Vertex::ms_DEF_VERTEX_SELECTOR_FMT_STRING, row);
                
                
                
                //  4.  BEGIN THE ROW...
                //  ImGui::PushID(i);
                ImGui::TableNextRow();
                
                //      4.1.        DRAW THE SELECTABLE WIDGET FOR THE VERTEX...
                ImGui::TableSetColumnIndex(0);
                if ( ImGui::Selectable( buffer, selected, SELECTABLE_FLAGS, {0.0f, 1.05f * CELL_SZ}) )
                {
                    BS.m_inspector_vertex_idx   = ( (selected)  ? -1    : row );      //     toggle
                }
                const bool                  hovered         = ImGui::IsItemHovered(ImGuiHoveredFlags_None);
                //
                //      4.2.        CACHE THIS VERTEX AS THE HOVERED OBJECT...
                if ( hovered )
                {
                    //  m_hovered_vertex
                    BS.m_hovered_vertex = { path_idx, row };
                }
                
                

                //  ImGui::PopID();
                
                
                
            }// END "for-loop" [ROWS]
            
        }// END "while-loop" [CLIPPER]
        
        clipper.End();
        ImGui::EndTable();
    }
    
    
    return;
}


//  "_draw_vertex_inspector_column"
//
void Editor::_draw_vertex_inspector_column(Path & path, [[maybe_unused]] const LabelFn & callback)
{
    static constexpr size_t     TITLE_SIZE      = 32ULL;
    //  BrowserStyle &              BStyle          = this->m_style.browser_style;
    //
    const int                   idx             = m_browser_S.m_inspector_vertex_idx;
    const int                   n               = static_cast<int>(path.verts.size());

    
    //      CASE 0 :    NO VALID SELECTION...
    if ( !(0 <= idx  &&  idx < n) ) {
        this->S.PushFont(Font::Main);
            ImGui::TextDisabled("No vertex selected...");
        this->S.PopFont();
        return;
    }

                      
    //      0.          OBTAIN POINTER TO VERTEX...
    VertexID                    vid             = path.verts[static_cast<size_t>(m_browser_S.m_inspector_vertex_idx)];
    Vertex *                    v               = find_vertex_mut(m_vertices, vid);
    VertexID                    cache_id        = static_cast<VertexID>(-1);
    static char                 title [TITLE_SIZE];     // safe head-room
    
    
    //      0.1.    UPDATE TITLE IF SELECTION CHANGED...
    if ( cache_id != vid )
    {
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
    if ( !v )           {
        cache_id = static_cast<VertexID>(-1);
        ImGui::TextColored( this->S.SystemColor.Red, "%s", "[ STALE VERTEX ERROR ]");
        return;
    }


    //  1.  HEADER CONTENT...       "Vertex ID"
    ImGui::SeparatorText(title);


    //  2.  "DELETE" BUTTON...
    if ( ImGui::Button("Delete Vertex##Editor_VertexBrowser_DeleteVertexButton", {120,0}) )
    {
        _erase_vertex_and_fix_paths(vid);
        m_browser_S.m_inspector_vertex_idx = -1;
        _rebuild_vertex_selection();
    }
    
    
    //  3.  INVOKE FUNCTION TO DRAW REMAINING VERTEX PROPERTIES...
    this->_draw_vertex_properties_panel( *v, callback );
    
    
    return;
}


//  "_draw_vertex_properties_panel"
//
inline void Editor::_draw_vertex_properties_panel(Vertex & v, const LabelFn & callback)
{
    ImGuiStyle &                    style               = ImGui::GetStyle();
    EditorState &                   ES                  = this->m_editor_S;
    //
    static ImVec2                   ms_SEP_WIDTH        = ImVec2( 5, style.ItemSpacing.y );
    const float                     ms_HALF_WIDTH       = 0.5 * ( ms_WIDGET_WIDTH - ms_SEP_WIDTH.x );
    constexpr float                 SPEED_SCALE         = 0.001f;
    [[maybe_unused]] const float    grid                = m_style.GRID_STEP / m_cam.zoom_mag;
    //  auto                            snap                = [/*grid*/](const double & x_, const double & y_) -> ImVec2 {
    //      float   x     = x_;//  std::round(x_ / grid) * grid;
    //      float   y     = y_;//  std::round(y_ / grid) * grid;
    //      return ImVec2{ x, y };
    //  };
    
    const float                     speedx              = SPEED_SCALE * ES.m_window_size[0];
    const float                     speedy              = SPEED_SCALE * ES.m_window_size[1];
    const double &                  WS_xmax             = ES.m_world_size[0].value;
    const double &                  WS_ymax             = ES.m_world_size[1].value;
    const bool                      quadratic           = ( v.IsQuadratic() );



    ImGui::PushStyleVar     ( ImGuiStyleVar_ItemSpacing,    ms_SEP_WIDTH        );
    {
    //
    //
    //
    //  //      3.1.    Position:
        ImGui::PushItemWidth( ms_HALF_WIDTH );
            v.ui_Position       (WS_xmax, WS_ymax, speedx, speedy);
        ImGui::PopItemWidth();
        
        
        //      3.2.    Bézier Handles / Control Points
        ImGui::NewLine();
        ImGui::TextDisabled("Bezier Controls");
        //
        //              3.2A    ANCHOR TYPE (corner / smooth / symmetric):
        {
            callback("Type:");
            v.ui_CurvatureType();
        }
            
            
        //              3.2B.   In-Handle:
        if ( !quadratic ) {
            callback("In-Handle:");
            ImGui::PushItemWidth( ms_HALF_WIDTH );
                v.ui_InHandle       (WS_xmax, WS_ymax, speedx, speedy);
            ImGui::PopItemWidth();
        }
        //              3.2C.   Out-Handle:
        //                      (A)     QUADRATIC BEZIER CURVE.
        if ( quadratic ) {
            callback("Control:");
            ImGui::PushItemWidth( ms_HALF_WIDTH );
                v.ui_OutHandle      (WS_xmax, WS_ymax, speedx, speedy);
            ImGui::PopItemWidth();
        }
        //                      (B)     CUBIC BEZIER CURVE.
        else {
            callback("Out-Handle:");
            ImGui::PushItemWidth( ms_HALF_WIDTH );
                v.ui_OutHandle      (WS_xmax, WS_ymax, speedx, speedy);
            ImGui::PopItemWidth();
        }
    //
    //
    //
    }
    ImGui::PopStyleVar();   //  ImGuiStyleVar_ItemSpacing
    
    
    return;
}

/* {
    ImGuiStyle &                    style               = ImGui::GetStyle();
    EditorState &                   ES                  = this->m_editor_S;
    //
    static ImVec2                   ms_SEP_WIDTH        = ImVec2( 5, style.ItemSpacing.y );
    const float                     ms_HALF_WIDTH       = 0.5 * ( ms_WIDGET_WIDTH - ms_SEP_WIDTH.x );
    constexpr float                 SPEED_SCALE         = 0.001f;
    [[maybe_unused]] const float    grid                = m_style.GRID_STEP / m_cam.zoom_mag;
    auto                            snap                = [grid](const double & x_, const double & y_) -> ImVec2 {
        float   x     = x_;//  std::round(x_ / grid) * grid;
        float   y     = y_;//  std::round(y_ / grid) * grid;
        return ImVec2{ x, y };
    };
    
    const float                     speedx              = SPEED_SCALE * ES.m_window_size[0];
    const float                     speedy              = SPEED_SCALE * ES.m_window_size[1];
    const double &                  WS_xmax             = ES.m_world_size[0].value;
    const double                    WS_xmin             = -WS_xmax;
    const double &                  WS_ymax             = ES.m_world_size[1].value;
    const double                    WS_ymin             = -WS_ymax;
    //
    //
    //
    int                             kind_idx            = static_cast<int>( v.GetCurvatureType() );
    //
    ImVec2                          position            = v.GetXYPosition();
    double                          pos_x               = static_cast<double>( position.x       );
    double                          pos_y               = static_cast<double>( position.y       );
    //
    ImVec2                          in_handle           = v.GetInHandle();
    double                          ih_x                = static_cast<double>( in_handle.x      );
    double                          ih_y                = static_cast<double>( in_handle.y      );
    //
    ImVec2                          out_handle          = v.GetOutHandle();
    double                          oh_x                = static_cast<double>( out_handle.x     );
    double                          oh_y                = static_cast<double>( out_handle.y     );
    //
    bool                            dirty1              = false;
    bool                            dirty2              = false;


    ImGui::PushStyleVar     ( ImGuiStyleVar_ItemSpacing,    ms_SEP_WIDTH        );
    {
    //
    //
    //
    //  //      3.1.    Position:
        callback("Position:");
        {
            ImGui::PushItemWidth    ( ms_HALF_WIDTH                                     );
            {
                v.ui_Position(WS_xmax, WS_ymax, speedx, speedy);
            
                //  dirty1  = s_draw_vertex_slider( "##Vertex_Position_X", pos_x, speedx, WS_xmin, WS_xmax );
                //  ImGui::SameLine(0.0f, ms_SEP_WIDTH);
                //  dirty2  = s_draw_vertex_slider( "##Vertex_Position_Y", pos_y, speedy, WS_ymin, WS_ymax );
                //
                //  //      UPDATE VALUE.
                //  if ( dirty1 || dirty2 ) {
                //      v.SetXYPosition( snap(pos_x, pos_y) );
                //      dirty1 = false; dirty2 = false;
                //  }
            }
            ImGui::PopItemWidth();
        }
        
        
        //      3.2.    Bézier Handles / Control Points
        ImGui::NewLine();
        ImGui::TextDisabled("Bezier Controls");
        //
        //              3.2A    ANCHOR TYPE (corner / smooth / symmetric):
        {
            callback("Type:");
            dirty1 = ImGui::Combo("##Vertex_AnchorType", &kind_idx, ms_BEZIER_CURVATURE_TYPE_NAMES.data(), static_cast<int>( BezierCurvatureType::COUNT ));          // <- int, not enum
            //
            if (dirty1) {
                v.SetCurvatureType( static_cast<BezierCurvatureType>(kind_idx) );
                dirty1 = false;
            }
        }
            
            
        //      3.2A.   In-Handle:
        callback("In-Handle:");
        {
            ImGui::PushItemWidth( ms_HALF_WIDTH );
            {
                dirty1  = s_draw_vertex_slider( "##Vertex_InHandle_X", ih_x, speedx, WS_xmin, WS_xmax );
                ImGui::SameLine(0.0f);
                dirty2  = s_draw_vertex_slider( "##Vertex_InHandle_Y", ih_y, speedy, WS_ymin, WS_ymax );
                
                //      UPDATE VALUE.
                if ( dirty1 || dirty2 ) {
                    v.SetInHandle( snap(ih_x, ih_y) );
                    dirty1 = false; dirty2 = false;
                }
            }
            ImGui::PopItemWidth();
        }
        
        
        //      3.2B.   Out-Handle:
        callback("Out-Handle:");
        {
            ImGui::PushItemWidth( ms_HALF_WIDTH );
            {
                dirty1  = s_draw_vertex_slider( "##Vertex_OutHandle_X", oh_x, speedx, WS_xmin, WS_xmax );
                ImGui::SameLine(0.0f);
                dirty2  = s_draw_vertex_slider( "##Vertex_OutHandle_Y", oh_y, speedy, WS_ymin, WS_ymax );
                
                //      UPDATE VALUE.
                if ( dirty1 || dirty2 ) {
                    v.SetOutHandle( snap(oh_x, oh_y) );
                    dirty1 = false; dirty2 = false;
                }
            }
            ImGui::PopItemWidth();
        }
    //
    //
    //
    ImGui::PopStyleVar();   //  ImGuiStyleVar_ItemSpacing
    
    
    return;
}
*/



//
//
// *************************************************************************** //
// *************************************************************************** //   END "VERTEX" PANELS.







// *************************************************************************** //
//
//
//
//      3.      "PAYLOAD" TRAIT / PANEL FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

//  "_draw_payload_panel"
//
void Editor::_draw_payload_panel(Path & path, [[maybe_unused]] const size_t pidx, const LabelFn & callback)
{
    const bool                          has_payload                 = path.kind != PathKind::None;


    S.DisabledSeparatorText("Payload");
    {
        callback("Type:");
        path.ui_kind();
        
        
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



//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "PAYLOAD" PANELS.
























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
