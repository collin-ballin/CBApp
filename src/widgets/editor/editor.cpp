/***********************************************************************************
*
*       *********************************************************************
*       ****             E D I T O R . C P P  ____  F I L E              ****
*       *********************************************************************
*
*              AUTHOR:      Collin A. Bond.
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

//  "_MECH_change_state"            formerly named:     "_mode_switch_hotkeys".
//
inline void Editor::_MECH_change_state([[maybe_unused]] const Interaction & it)
{
    //  I/O INPUTS...
    ImGuiIO &           io                  = ImGui::GetIO();
    const bool          shift               = io.KeyShift,          ctrl    = io.KeyCtrl,
                        alt                 = io.KeyAlt,            super   = io.KeySuper;
    const bool          no_mod              = !ctrl && !shift && !alt && !super;
    const bool          only_shift          = !ctrl &&  shift && !alt && !super;
    const bool          lmb_held            = ImGui::IsMouseDown(ImGuiMouseButton_Left);



    if ( it.space || !it.hovered || lmb_held )  { return; }   // Early-out if [SPACE], [NOT HOVERING OVER CANVAS], or [LMB IS HELD DOWN]...
    


    //  3.      MODE SWITCH BEHAVIORS   [NO KEY MODS]...
    if ( no_mod )
    {
        if ( ImGui::IsKeyPressed(ImGuiKey_V)                            )                   m_mode = Mode::Default;
        if ( ImGui::IsKeyPressed(ImGuiKey_H)                            )                   m_mode = Mode::Hand;
        if ( ImGui::IsKeyPressed(ImGuiKey_P)                            )                   m_mode = Mode::Pen;
        if ( ImGui::IsKeyPressed(ImGuiKey_C)                            )                   m_mode = Mode::Scissor;
        if ( ImGui::IsKeyPressed(ImGuiKey_S)                            )                   m_mode = Mode::Shape;
        if ( ImGui::IsKeyPressed(ImGuiKey_Equal)                        )                   m_mode = Mode::AddAnchor;
        if ( ImGui::IsKeyPressed(ImGuiKey_Minus)                        )                   m_mode = Mode::RemoveAnchor;
    }
    //
    //  3.1     MODE SWITCH BEHAVIORS   [WITH SHIFT]...
    else if ( only_shift )
    {
        if ( !ctrl && shift && !alt && !super && ImGui::IsKeyPressed(ImGuiKey_C)    )       m_mode = Mode::EditAnchor;
        
        //
        //      More SHIFT-KEY Handlers...
        //
    }
    
    
    //  4.      HANDLE LINGERING STATE BEHAVIORS...
    if ( m_mode != Mode::Pen )      { this->reset_pen(); } // m_pen = {};       //  Leaving the Pen-Tool resets current path appending.

    
    //  5.      INVOKE GRID SHORTCUT BEHAVIORS...
    this->_grid_handle_shortcuts();
    
    //  bool lmbHeld = ImGui::IsMouseDown(ImGuiMouseButton_Left);
    //  bool lmbHeldRaw = io.MouseDown[0];          // 0 == left button
    //  float heldFor   = io.MouseDownDuration[0];


    return;
}


//  "_MECH_dispatch_tool_handler"               formerly named:     "_dispatch_mode_handler".
//
inline void Editor::_MECH_dispatch_tool_handler([[maybe_unused]] const Interaction & it )
{
    if ( !(it.space && ImGui::IsMouseDown(ImGuiMouseButton_Left)) )
    {
        switch ( this->m_mode )
        {
            case Mode::Hand             :   { _handle_hand            (it);           break;    }
            case Mode::Pen              :   { _handle_pen             (it);           break;    }
            case Mode::Scissor          :   { _handle_scissor         (it);           break;    }
            case Mode::Shape            :   { _handle_shape           (it);           break;    }
            case Mode::AddAnchor        :   { _handle_add_anchor      (it);           break;    }
            case Mode::RemoveAnchor     :   { _handle_remove_anchor   (it);           break;    }
            case Mode::EditAnchor       :   { _handle_edit_anchor     (it);           break;    }
            //
            default                     :   { _handle_default         (it);           break;    }
        }
    }
        
    return;
}


//  "_per_frame_cache_begin"
//
inline void Editor::_per_frame_cache_begin(void) noexcept
{
    ImGuiIO &               io                      = ImGui::GetIO();
    //  EditorState &           ES                      = this->m_editor_S;
    //  BrowserState &          BS                      = this->m_browser_S;
    Interaction &           it                      = *m_it;
    EditorInteraction &     eit                     = it.obj;
    
    //      1.1.    IMPLOT STATE...
    const bool              space                   = ImGui::IsKeyDown(ImGuiKey_Space);
    const bool              hovered                 = ImPlot::IsPlotHovered();
    const bool              active                  = ImPlot::IsPlotSelected();
    eit.other_windows_open                          = (ImGui::GetTopMostPopupModal() != nullptr);
    //
    //
    //      1.2.    IMPLOT DATA...
    ImVec2                  plotTL                  = ImPlot::GetPlotPos();
    ImVec2                  origin_scr              = plotTL;
    ImVec2                  mouse_canvas            { io.MousePos.x - origin_scr.x,     io.MousePos.y - origin_scr.y };
    //
    //
    //      1.3.    SELECTION DATA...
    //  const size_t            N_obj_selected          = this->m_sel.paths.size();
    //
    //
    //      1.4.    OTHER DATA...
    ImDrawList *            dl                      = ImPlot::GetPlotDrawList();



    //      2.1.    EDITOR STATE...
    

    //      3.      ASSIGN UPDATED VALUES...
    it.hovered                      = hovered;
    it.active                       = active;
    it.space                        = space;
    //
    it.canvas                       = mouse_canvas;
    it.origin                       = origin_scr;
    it.tl                           = plotTL;
    //
    it.dl                           = dl;
    
    
    //      4.      ASSIGNMENTS FOR EDITOR-INTERACTION OBJECT...
    //
    //              4.1.        SET BIT-FIELD FOR EACH POP-UP WINDOW OPEN.
    for (size_t i = 0; i < ms_POPUP_INFOS.size(); ++i)
    {
        const auto &    info    = ms_POPUP_INFOS[i];
        bool            open    = false;
        
        if ( info.uuid )    { open = ImGui::IsPopupOpen(info.uuid); }
        
        set_bit(eit.open_menus, i, open);
	}
    //
    //              4.2.        SET BIT FOR OTHER MODAL (Settings, AskOKCancel, File Dialog, etc).
    set_bit( eit.open_menus, EditorPopupBits::Other, eit.other_windows_open );
    
    
    //
    //              4.2.        CACHE THE SELECTION STATE.
    //  ES.m_show_sel_overlay           = static_cast<bool>( N_obj_selected == 0 );
    //
    //  //  eit.empty_selection             = static_cast<bool>( this->m_sel.paths.size() == 0 );
    //  //  eit.single_obj_selection        = static_cast<bool>( this->m_sel.paths.size() == 1 );
    
    
    
    //      5.      OTHER PER-FRAME CACHE...
    //      ES.m_plot_limits                = ImPlot::GetPlotLimits();          //     current X/Y axes
    
    
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
    ImGuiIO &                           io          = ImGui::GetIO();
    [[maybe_unused]] EditorStyle &      EStyle      = this->m_style;
    EditorState &                       ES          = this->m_editor_S;
    Interaction &                       it          = *this->m_it;
    
    
    //      1.      FETCH INITIAL PER-FRAME VALUES (USER-INTERACTION INPUT, ETC)...
    //
    const bool              block_input             = it.BlockInput();
    const bool              block_shortcuts         = it.BlockShortcuts();
    const bool              space                   = ( !block_input  &&  ImGui::IsKeyDown(ImGuiKey_Space) );
    //
    const bool              pan_enabled_IMPL        = (  (!block_input)  &&  ( (this->m_mode == Mode::Hand)  ||  space)  ); // || (this->m_mode == Mode::Hand) )  );
    const bool              pan_enabled             = ( pan_enabled_IMPL  &&  _mode_has(CBCapabilityFlags_Pan) );
    const bool              zoom_enabled            = (  (!block_input)  &&  _mode_has(CBCapabilityFlags_Zoom)  &&   (!io.MouseDown[0]) );
    //
    //
    ImPlotInputMap          backup                  = ImPlot::GetInputMap();                            //  Adjust ImPlot input map (backup, edit, restore at end)
    ImPlotInputMap &        map                     = ImPlot::GetInputMap();
    //  ImPlotInputMap          m_state.m_backup        = ImPlot::GetInputMap();
    //
    //
    map.Pan                                         = ImGuiMouseButton_Left;
    map.PanMod                                      = (pan_enabled)     ? 0                         : ImGuiMod_Ctrl;        //  disable pan unless current TOOL allows PANNING...
    map.ZoomMod                                     = (zoom_enabled)    ? 0                         : ImGuiMod_Ctrl;        //  disable zoom unless current TOOL allows ZOOMING...
    map.ZoomRate                                    = ES.m_mousewheel_zoom_rate.value;    //



    //      2A.     HANDLE ANY I/O OPERATIONS BEFORE PLOT BEGINS...
    this->_MECH_pump_main_tasks();
    this->_MECH_drive_io();
    //
    //      2B.     DRAW THE EDITOR CONTROL BAR UI...
    this->_MECH_draw_controls();
    //
    //      2C.     STORE ADDITIONAL PER-FRAME VALUES (AFTER PLACING CONTROLBAR UI)...
    ES.m_avail                                      = ImGui::GetContentRegionAvail();       //  1. Canvas size & plot flags
    ES.m_avail.x                                    = std::max( ES.m_avail.x,   50.f );
    ES.m_avail.y                                    = std::max( ES.m_avail.y,   50.f );
    
    
    
    //      3.      CREATE THE MAIN IMPLOT "GRID" / "CANVAS"...
    //
    //      CASE 3A         : FAILURE TO CREATE CANVAS.
    if ( !ImPlot::BeginPlot("##Editor_CanvasGrid", ImVec2(ES.m_avail.x, ES.m_avail.y), m_plot_flags) )
    {
        ImPlot::GetInputMap() = backup;
        return;
    }
    //
    //      CASE 3B         : SUCCESSFULLY CREATED THE "IMPLOT" PLOT...
    {
        //          3.1.    CONFIGURE THE "IMPLOT" APPEARANCE, UPDATE GRID INFORMATION, ETC...
        this->_MECH_update_canvas(it);
        
        
        
        //          3.2.    UPDATE THE EDITOR'S CACHE / STORE "PER-FRAME" VALUES IN THE "Interaction" OBJECT FOR THIS FRAME...
        this->_per_frame_cache_begin();
        this->_selbox_rebuild_view_if_needed(it);   //  NEW: prepares bbox view cache (noop for now)


        //          3.3.    MODE SWITCH BEHAVIORS AND OVERLAY WINDOWS...
        this->_MECH_change_state(it);
        this->_MECH_draw_ui(it);



        //          3.4.    CURSOR HINTS AND SHORTCUTS...
        //  if ( space  &&  it.hovered  &&  _mode_has(CBCapabilityFlags_Pan) )
        if ( space  &&  !block_shortcuts  &&  _mode_has(CBCapabilityFlags_Pan) )
            { ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeAll); }
        //
        else if ( !space  &&  it.hovered  &&  !(m_dragging || m_boxdrag.active) )    //  ignore while dragging selection.
            { this->_MECH_hit_detection(it); }
        //
        //  else if ( !space && it.hovered && _mode_has(CBCapabilityFlags_CursorHint) )
        //      { this->_MECH_hit_detection(it); }



        //          3.5.    SELECTION BEHAVIOR...
        if  ( !space  &&  _mode_has(CBCapabilityFlags_Select) )
            { _MECH_process_selection(it); }



        //          3.6.    SELECTION BEHAVIOR...
        if  ( !space  &&  !block_input )
            { _MECH_query_shortcuts(it); }
        
    
        //          3.6.    MODE/STATE/TOOL DISPATCHER...
        this->_MECH_dispatch_tool_handler(it);


        //          3.7.    RENDERING LOOP...
        this->_MECH_render_frame(it);
    //
    //
    }// END "IMPLOT".
    //
    //
    //
    ImPlot::EndPlot();
    ImPlot::GetInputMap() = backup;   // restore map
    
    
    
    //  this->_draw_io_overlay();
    //
    //
    //  show_icon_preview_window();
    //
    //
    
    
    return;
}

//
//
// *************************************************************************** //
// *************************************************************************** //   END "MAIN API".






// *************************************************************************** //
//
//
//
//      3.      CORE MECHANIC-HANDLERS OF THE APPLICATION...
// *************************************************************************** //
// *************************************************************************** //


//  "_selbox_rebuild_view_if_needed"
//
void Editor::_selbox_rebuild_view_if_needed([[maybe_unused]] const Interaction & it)
{
    //  EditorState &           ES      = this->m_editor_S;
    EditorStyle &           Style       = this->m_style;
    BoxDrag::ViewCache &    V           = m_boxdrag.view;

    // Decide visibility: hide for single-vertex-only selection (matches your renderer)
    const bool has_paths_or_lines = !m_sel.paths.empty();
    const bool single_vertex_only = (m_sel.vertices.size() <= 1) && !has_paths_or_lines;

    ImVec2 tl_tight{}, br_tight{};
    if ( single_vertex_only || !_selection_bounds(tl_tight, br_tight, this->m_render_ctx) ) {
        V.visible   = false;
        V.valid     = true;
        V.hover_idx = -1;
        V.sel_seen   = m_rev_sel;
        V.geom_seen  = m_rev_geom;
        V.cam_seen   = m_rev_cam;
        V.style_seen = m_rev_style;
        return;
    }

    // Expand by pixel margin (so handles sit outside geometry)
    const auto [tl_ws, br_ws] = _expand_bbox_by_pixels(tl_tight, br_tight, Style.SELECTION_BBOX_MARGIN_PX);

    V.visible = true;
    V.tl_ws   = tl_ws;
    V.br_ws   = br_ws;
    V.tl_px   = world_to_pixels(tl_ws);
    V.br_px   = world_to_pixels(br_ws);

    // Compute 8 handle anchors (WS → PX) and their pixel hit-rects
    const ImVec2 c_ws{ (tl_ws.x + br_ws.x) * 0.5f, (tl_ws.y + br_ws.y) * 0.5f };
    const ImVec2 ws[8] = {
        tl_ws,
        { c_ws.x, tl_ws.y },
        { br_ws.x, tl_ws.y },
        { br_ws.x, c_ws.y },
        br_ws,
        { c_ws.x, br_ws.y },
        { tl_ws.x, br_ws.y },
        { tl_ws.x, c_ws.y }
    };

    const float r = m_style.HANDLE_BOX_SIZE;        // half-size in pixels
    for (int i = 0; i < 8; ++i) {
        V.handle_ws[i] = ws[i];
        const ImVec2 p = world_to_pixels(ws[i]);
        V.handle_px[i]      = p;
        V.handle_rect_px[i] = ImRect(ImVec2(p.x - r, p.y - r), ImVec2(p.x + r, p.y + r));
    }

    V.hover_idx  = -1;           // filled by hint stage next step
    V.valid      = true;

    // Stamp revs (we’ll start honoring these later if we want to skip recompute)
    V.sel_seen   = m_rev_sel;
    V.geom_seen  = m_rev_geom;
    V.cam_seen   = m_rev_cam;
    V.style_seen = m_rev_style;
    return;
}












//  "_MECH_update_canvas"       formerly: "_update_cursor_select"
//
inline void Editor::_MECH_update_canvas([[maybe_unused]] const Interaction & it)
{
    EditorState &       ES                  = this->m_editor_S;
    //
    static bool         show_grid_cache     = !m_grid.visible;
    
    
    //      1.      QUERY THE "SHOW GRID" STATUS...
    if ( show_grid_cache != m_grid.visible) [[unlikely]]
    {
        show_grid_cache = m_grid.visible;
        
        if ( show_grid_cache ) {    //  1A.     SET the flags.
            m_axes[0].flags     &= ~ImPlotAxisFlags_NoGridLines;
            m_axes[1].flags     &= ~ImPlotAxisFlags_NoGridLines;
        }
        else {                      //  1B.     REMOVE the flags.
            m_axes[0].flags     |= ImPlotAxisFlags_NoGridLines;
            m_axes[1].flags     |= ImPlotAxisFlags_NoGridLines;
        }
    }
    
    
    //      2.      SET THE INITIAL SIZE OF THE CANVAS...
    ImPlot::SetupAxesLimits( ES.ms_INITIAL_CANVAS_SIZE[0], ES.ms_INITIAL_CANVAS_SIZE[1], ES.ms_INITIAL_CANVAS_SIZE[2], ES.ms_INITIAL_CANVAS_SIZE[3], ImPlotCond_Once );
    
    
    //      3.      SETUP MIN + MAX AXIS DIMENSIONS...
    ImPlot::SetupAxisLimitsConstraints  (   ImAxis_X1,
                                            ES.m_world_size[0].Min()   - ES.m_world_slop[0].Value(),
                                            ES.m_world_size[0].Value() + ES.m_world_slop[0].Value()     );
    ImPlot::SetupAxisLimitsConstraints  (   ImAxis_Y1,
                                            ES.m_world_size[1].Min()   - ES.m_world_slop[1].Value(),
                                            ES.m_world_size[1].Value() + ES.m_world_slop[1].Value()     );
                                            
                                            
    //      4.      SETUP MIN + MAX AXIS "ZOOM" RESOLUTION...
    ImPlot::SetupAxisZoomConstraints    (   ImAxis_X1,      ES.m_zoom_size[0].Min(),            ES.m_zoom_size[0].Value()       );
    ImPlot::SetupAxisZoomConstraints    (   ImAxis_Y1,      ES.m_zoom_size[1].Min(),            ES.m_zoom_size[1].Value()       );
    
    
    //      5.      CONFIGURE THE "IMPLOT" APPEARANCE...
    ImPlot::SetupAxes(m_axes[0].uuid,           m_axes[1].uuid,             //  5A.     Axis Names & Flags.
                      m_axes[0].flags,          m_axes[1].flags
    );
    //  ImPlot::SetupAxes(m_axes[0].uuid,           m_axes[1].uuid,             //  5A.     Axis Names & Flags.
    //                    (m_grid.visible) ? m_axes[0].flags    : m_axes[0].flags | ImPlotAxisFlags_NoGridLines,
    //                    (m_grid.visible) ? m_axes[1].flags    : m_axes[1].flags | ImPlotAxisFlags_NoGridLines
    //  );
    
    
    //      6.      PER-FRAME CACHE OPERATIONS FOR IMPLOT CANVAS...
    ES.m_window_coords          = ImPlot::GetPlotLimits();                                          //  6A.     DOMAIN + RANGE OF PLOT      [ IN (X,Y) PLOT UNITS ].
    ES.m_window_size[0]         = std::abs(ES.m_window_coords.X.Max - ES.m_window_coords.X.Min);    //  6B.     SIZE OF THE PLOT            [ IN (X,Y) PLOT UNITS ].
    ES.m_window_size[1]         = std::abs(ES.m_window_coords.Y.Max - ES.m_window_coords.Y.Min);    //
    //
    ES.m_plot_px_dims           = ImPlot::GetPlotSize();                                            //  6C.     PIXEL SIZE OF THE PLOT      [ IN PIXEL-DIMENSIONS ].
    ES.m_plot_bbox              = ImRect( ImGui::GetItemRectMin(), ImGui::GetItemRectMax() );
    
    
    //      X.      REMAINING FUNCTIONS...
    this->_clamp_plot_axes();
    //
    this->_update_grid_info();                                              //  3C.     Fetch Grid-Quantization Info.



    return;
}


//  "_MECH_draw_ui"             formerly named:     "_handle_overlays".
//
inline void Editor::_MECH_draw_ui([[maybe_unused]] const Interaction & it)
{
    [[maybe_unused]] ImGuiIO &      io                  = ImGui::GetIO();
    EditorState &                   ES                  = this->m_editor_S;
    
    if ( ES.m_block_overlays )                          { return; }
    
    
    
    //  RESIDENTIAL WINDOWS...
    //
    static bool             debug_overlay_cache         = !ES.m_show_debug_overlay;                         //  1.  Debugger/Info Overlay.
    static auto &           debugger_entry              = m_residents[Resident::Debugger];
    static Overlay &        debugger_resident           = *m_ov_manager.lookup_resident(debugger_entry.id);
    //
    static bool             sel_overlay_cache           = !ES.m_show_sel_overlay;                           //  2.  Selection Overlay.
    static auto &           selection_entry             = m_residents[Resident::Selection];
    static Overlay &        selection_resident          = *m_ov_manager.lookup_resident(selection_entry.id);
    //
    static auto &           shape_entry                 = m_residents[Resident::Shape];                     //  3.  Shape Resident.
    static Overlay &        shape_resident              = *m_ov_manager.lookup_resident(shape_entry.id);
    //
    //
    //
    //  UI-RESIDENT OVERLAYS...
    static bool             ui_traits_overlay_cache     = !ES.m_show_debug_overlay;                         //  4.  UI-Traits Resident.
    static auto &           ui_traits_entry             = m_residents[Resident::UITraits];
    static Overlay &        ui_traits_resident          = *m_ov_manager.lookup_resident(ui_traits_entry.id);
    //
    static bool             ui_objects_overlay_cache    = !ES.m_show_ui_objects_overlay;                    //  5.  UI-Objects Resident.
    static auto &           ui_objects_entry            = m_residents[Resident::UIObjects];
    static Overlay &        ui_objects_resident         = *m_ov_manager.lookup_resident(ui_objects_entry.id);

    
    
    //      1.      UPDATE "DEBUGGER" OVERLAY...
    if ( ES.m_show_debug_overlay != debug_overlay_cache ) [[unlikely]] {
        debug_overlay_cache                     = ES.m_show_debug_overlay;
        debugger_resident.info.visible          = ES.m_show_debug_overlay;
    }
    

    //      2.      UPDATE SELECTION OVERLAY...
    if ( ES.m_show_sel_overlay != sel_overlay_cache ) [[unlikely]] {
        sel_overlay_cache                       = ES.m_show_sel_overlay;
        selection_resident.info.visible         = ES.m_show_sel_overlay;
    }
    if (selection_resident.info.visible) {
        ImVec2 tl, br;
        if ( _selection_bounds(tl, br, this->m_render_ctx) ) {
            selection_resident.cfg.anchor_ws = { (tl.x + br.x) * 0.5f, tl.y }; // bottom-centre in world
        }
    }
    
    
    //      3.      UPDATE "SHAPE" OVERLAY...
    shape_resident.info.visible                 = ( m_mode == Mode::Shape );                //  Leaving the Shape-Tool closes the overlay window.
    
    
    
    //      4.      UPDATE "UI-TRAITS" OVERLAY...
    if ( ES.m_show_ui_traits_overlay != ui_traits_overlay_cache ) [[unlikely]] {
        ui_traits_overlay_cache                 = ES.m_show_ui_traits_overlay;
        ui_traits_resident.info.visible         = ES.m_show_ui_traits_overlay;
    }
    
    
    
    //      5.      UPDATE "UI-OBJECTS" OVERLAY...
    if ( ES.m_show_ui_objects_overlay != ui_objects_overlay_cache ) [[unlikely]] {
        ui_objects_overlay_cache                = ES.m_show_ui_objects_overlay;
        ui_objects_resident.info.visible        = ES.m_show_ui_objects_overlay;
    }
    
    
    
    //      LAST:   DRAW EACH OVERLAY WINDOW...
    m_ov_manager.Begin(
        /* cursor      */ ImGui::GetIO().MousePos,
        /* full rect   */ ES.m_plot_bbox
    );

    return;
}


//  "_MECH_drive_io"             formerly named:     "_handle_io".
//
inline void Editor::_MECH_drive_io(void)
{
    namespace           fs                  = std::filesystem;
    //using             Initializer         = cb::FileDialog::Initializer;
    //  ImGuiIO &           io                  = ImGui::GetIO();
    EditorState &       ES                  = this->m_editor_S;



    //      0.      TOOL-TIP I/O MESSAGING...
    if ( ES.m_show_io_message.load(std::memory_order_acquire) ) {
        ES.DisplayIOStatus();
    }


    //      1.      SAVE DIALOGUE...
    if ( ES.m_sdialog_open.load(std::memory_order_acquire) )
    {
        ES.m_sdialog_open.store(false, std::memory_order_release);
        this->m_save_dialog.initialize(this->m_SAVE_DIALOG_DATA );
    }
    //
    if ( this->m_save_dialog.is_open() )
    {
        ES.m_sdialog_open.store(false, std::memory_order_release);
        
        if ( this->m_save_dialog.Begin() )      // returns true when finished
        {
        
            //  CASE 1.1. :     FILE DIALOG SELECTED A FILE.
            if ( auto path = this->m_save_dialog.result() )
            {
                const bool result   = save_async( *path );        // your own handler
                
                
                //  CASE 1.1A.      SAVE SUCCESS...
                if (result)
                {
                    ES.m_filepath       = *path;
                    CB_LOG( LogLevel::Info, "Editor | saved data to file \"{}\"", ES.m_filepath.filename().string() );
                }
                //
                //  CASE 1.1B.      SAVE FAILURE...
                else
                {
                    CB_LOG( LogLevel::Error, "Editor | failure to save data to file \"{}\"", (*path).filename().string() );
                    //
                    //  ES.m_filepath       = fs::path{   };
                    //  this->RESET_ALL();
                }
                
        
            }
            //
            //  CASE 1.2. :     USER HAS CANCELLED FILE DIALOG.
            else
            {
                CB_LOG( LogLevel::Debug, "Editor | cancelled file dialog menu (\"save file\")" );
            }
            
        }
    
    
    }
    
    
    
    //      2.      LOAD DIALOGUE...
    if ( ES.m_odialog_open.load(std::memory_order_acquire) )
    {
        ES.m_odialog_open.store(false, std::memory_order_release);
        this->m_open_dialog.initialize(m_OPEN_DIALOG_DATA );
    }
    //
    if ( this->m_open_dialog.is_open() )
    {
        if ( this->m_open_dialog.Begin() )      // returns true when finished
        {
        
            //  CASE 2.1. :     FILE DIALOG SELECTED A FILE.
            if ( auto path = this->m_open_dialog.result() )
            {
                this->RESET_ALL();
                const bool result   = load_async( *path );        // your own handler
                
                
                
                //  CASE 2.1A.      LOAD SUCCESS...
                if (result)
                {
                    ES.m_filepath       = *path;
                    CB_LOG( LogLevel::Info, "Editor | loaded session from file \"{}\"", ES.m_filepath.filename().string() );
                }
                //
                //  CASE 2.1B.      LOAD FAILURE...
                else
                {
                    CB_LOG( LogLevel::Error, "Editor | failure to load file \"{}\"", (*path).filename().string() );
                    ES.m_filepath       = fs::path{   };
                    this->RESET_ALL();
                }
                
            }
            //
            //  CASE 2.2. :     USER HAS CANCELLED FILE DIALOG.
            else
            {
                CB_LOG( LogLevel::Debug, "Editor | cancelled file dialog menu (\"open file\")" );
            }
            
        }
    }


    //  popup::Begin();
    return;
}


//
//
// *************************************************************************** //
// *************************************************************************** //   END "CORE MECHANICS".






// *************************************************************************** //
//
//
//
//      4.      TOOL / "MODE" HANDLER FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

//  "_handle_default"
//
inline void Editor::_handle_default(const Interaction & it)
{
    [[maybe_unused]] ImGuiIO & io = ImGui::GetIO();

    // 0) Bezier control-handle drag (Alt + LMB) — unchanged
    if ( _try_begin_handle_drag(it) ) {
        _pen_update_handle_drag(it);                 // keep guide visible on first frame
        return;                                      // skip selection & other handlers this frame
    }


    //  1.   EDIT BEZIER CTRL POINTS IN DEFAULT STATE...
    if ( !m_boxdrag.active
         && _mode_has(CBCapabilityFlags_Select)
         && m_boxdrag.view.visible
         && m_boxdrag.view.hover_idx >= 0
         && ImGui::IsMouseClicked(ImGuiMouseButton_Left) )
    {
        // NOTE: view.tl_ws/br_ws are already the expanded bbox corners.
        // Ensure _start_bbox_drag treats these as final (i.e., does NOT re-expand).
        _start_bbox_drag(static_cast<uint8_t>(m_boxdrag.view.hover_idx),
                         m_boxdrag.view.tl_ws, m_boxdrag.view.br_ws);
        return;                                      // consume click; no lasso this frame
    }


    // 2) Active BBox drag — update and exit early
    if ( m_boxdrag.active ) {
        _update_bbox();
        return;                                      // while dragging, ignore other inputs
    }


    // 3) Ignore input while Space (camera pan) is held
    if (it.space) {
        return;
    }


    // 4) Lasso start (selection engine) — only when not over a handle or other hit
    if (_mode_has(CBCapabilityFlags_Select)
        && !m_lasso_active
        && it.hovered
        && ImGui::IsMouseClicked(ImGuiMouseButton_Left)
        && m_boxdrag.view.hover_idx < 0            // NEW: don't lasso from over BBox handle
        && !_hit_any(it))                          // keep existing pick test
    {
        _start_lasso_tool();
    }


    // 5) Lasso update — unchanged
    if (m_lasso_active) {
        _update_lasso(it);
        return;
    }

    return;
}


/*{
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
        if ( _selection_bounds(tl, br, this->m_render_ctx) )
            { _start_bbox_drag(static_cast<uint8_t>(m_hover_handle), tl, br); }
    }
        
        
    //  3.   Update BBOX...
    if (m_boxdrag.active)                           { _update_bbox(); }
    
    
    //  4.  IGNORE ALL INPUT IF SPACE KEY IS HELD DOWN...
    if ( it.space )                                 { return; }


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
}*/


//  "_handle_hand"
//
inline void Editor::_handle_hand([[maybe_unused]] const Interaction & it)
{
    ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeAll);
    return;
}



//  "_handle_line"
//
inline void Editor::_handle_line(const Interaction & it)
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
inline void Editor::_handle_point(const Interaction& it)
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
inline void Editor::_handle_pen(const Interaction& it)
{
    ImGuiIO & io = ImGui::GetIO();

    // ───── 0.  [Esc] aborts live path
    //  if (ImGui::IsKeyPressed(ImGuiKey_Escape)) {
    //      this->reset_pen(); // m_pen = {};
    //      return;
    //  }
    
    

    // ───── A. Handle‑drag already in progress
    if ( m_pen.dragging_handle )
    {
        _pen_update_handle_drag(it);
        return;                                     // nothing else this frame
    }


    // ───── B. Pending click‑hold test (detect curvature pull)
    if ( m_pen.pending_handle )
    {
        m_pen.pending_time += io.DeltaTime;

        if ( ImGui::IsMouseReleased(ImGuiMouseButton_Left) ) {
            m_pen.pending_handle = false;
            m_pen.pending_time   = 0.0f;
            return;                                 // plain vertex click
        }
        if ( (m_pen.pending_time >= m_style.PEN_DRAG_TIME_THRESHOLD)  &&
             ImGui::IsMouseDragPastThreshold( ImGuiMouseButton_Left, m_style.PEN_DRAG_MOVEMENT_THRESHOLD ) )
        {
            _pen_begin_handle_drag(m_pen.pending_vid,
                                   /*out_handle=*/false,
                                   /*force_select=*/true);

            if ( Vertex * v = find_vertex_mut(m_vertices, m_pen.pending_vid) )
                { v->m_bezier.in_handle = ImVec2(0,0); }        // make new handle visible

            m_pen.pending_handle = false;
            m_pen.pending_time   = 0.0f;
            _pen_update_handle_drag(it);            // first update
            return;
        }
        return;     // still waiting (mouse held but not past threshold)
    }


    // ───── C.  Alt‑mode : edit existing handles (no new vertices)
    if ( alt_down() )
    {
        if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && it.hovered)
        {
            int pi = _hit_point(it);
            if ( pi >= 0 )
            {
                _pen_begin_handle_drag(m_points[pi].v,
                                        /*out_handle=*/true,
                                        /*force_select=*/true);
                if (Vertex* v = find_vertex_mut(m_vertices, m_drag_vid))
                    v->m_bezier.out_handle = ImVec2(0,0);
            }
        }
        _draw_pen_cursor(io.MousePos, m_style.PEN_COL_NORMAL);   // yellow bullseye
        return;
    }


    // ───── D. Normal pen behaviour (continue / add / close)
    bool        can_extend      = false, prepend = false;
    size_t      end_path        = static_cast<size_t>(-1);
    int         pt_idx          = _hit_point(it);
    if ( pt_idx >= 0 )
    {
        uint32_t vid = m_points[pt_idx].v;
        if ( auto ep = _endpoint_if_open(vid) ) {
            can_extend = true;
            prepend    = ep->prepend;
            end_path   = ep->path_idx;
        }
    }


    // Cursor hint colour
    if ( it.hovered  &&  !it.space )
    {
        _draw_pen_cursor( io.MousePos, (can_extend)
                                            ? m_style.PEN_COL_EXTEND : m_style.PEN_COL_NORMAL );
    }


    // ───── E. Mouse‑click handling
    if ( ImGui::IsMouseClicked(ImGuiMouseButton_Left)  &&  it.hovered )
    {
        // (1) Pick open endpoint to continue
        if ( can_extend  &&  !m_pen.active )
        {
            m_pen.active     = true;
            m_pen.path_index = end_path;
            m_pen.prepend    = prepend;
            return;                                 // wait for next click
        }

        // (2) Live path exists → append / prepend / close
        if ( m_pen.active )
        {
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

        Path            p;
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
inline void Editor::_handle_scissor(const Interaction & it)
{
    if ( !it.hovered )      { return; }


    // test once per frame
    std::optional<PathHit> hit = _hit_path_segment(it);


    // cursor hint (vertical beam) whenever a cut location is valid
    if ( hit )
        { ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW); }

    // left-click performs the cut
    if ( hit  &&  ImGui::IsMouseClicked(ImGuiMouseButton_Left) )
        { this->_scissor_cut(*hit); }
        
    return;
}


//  "_handle_add_anchor"
//
inline void Editor::_handle_add_anchor([[maybe_unused]] const Interaction & it)
{
    VertexID                    vid         = 0;
    Path *                      path        = nullptr;                          //  Helper returning Path *
    std::optional<PathHit>      p_hit       = _hit_path_segment(it);
    

    if ( !p_hit )                                           { return; }



    path                                    = &m_paths[p_hit->path_idx];
    
    //  1.  EARLY OUT IF: (1) PATH IS NOT MUTABLE (Locked, Invisible),  (2) CURSOR IS ABOVE A VERTEX...
    if ( !path->IsMutable() || (_hit_point(it) > 0) )      { return; }



    //  CASE 1 :    RENDER PREVIEW...
    it.dl->AddCircleFilled( world_to_pixels(p_hit->pos_ws),     m_style.VERTEX_PREVIEW_RADIUS,
                            m_style.VERTEX_PREVIEW_COLOR,       m_style.ms_VERTEX_NUM_SEGMENTS );
    //
    //  CASE 2 :    INSERT VERTEX   (Check condition TWICE so we can use the preview as an accurate reference)...
    if ( ImGui::IsMouseClicked(ImGuiMouseButton_Left) ) {
        vid                                     = _add_vertex(p_hit->pos_ws);               //  Add new vertex...
        _add_point_glyph(vid);
        path->insert_vertex_after(p_hit->seg_idx, vid);                                     //  Insert into path right after seg_idx...
    }


    //
    //  TODO:
    //      Re-compute Bézier handles around the split (TODO)
    //
    
    return;
}


//  "_handle_remove_anchor"
//
inline void Editor::_handle_remove_anchor([[maybe_unused]] const Interaction & it)
{
    int                 point_idx       = -1;
    VertexID            vid             = 0;
    Path *              path            = nullptr;                              //  Helper returning Path *


    //  0.  Respond only to a fresh LMB click on the canvas.
    if ( !ImGui::IsMouseClicked(ImGuiMouseButton_Left) )    { return; }
        
        
    //  1.  Hit-test for a point glyph under the cursor
    point_idx                           = _hit_point(it);                       //  Returns glyph index, or -1.
    if (point_idx < 0)                                      { return; }         //  No vertex hit.


    //  2.  Locate owning path and check mutability
    vid                                 = m_points[point_idx].v;
    path                                = parent_path_of_vertex_mut(vid);       //  Helper returning Path *
    
    
    if ( !path || !path->IsMutable() )                      { return; }         //  Hidden / locked → ignore


    //  3.  Remove glyph from list
    m_points.erase( m_points.begin() + point_idx );



    //  4.  Remove vertex from every container
    _erase_vertex_record_only(vid);                // helper: removes from m_vertices & handles

    //  5.  Repair path topology; if path now invalid, drop it entirely
    if ( !path->remove_vertex(vid) ) {
        m_paths.erase( std::remove_if(m_paths.begin(), m_paths.end(),    //  Path has <2 verts; erase it from m_paths
                       [path](const Path& p){ return &p == path; }),
                       m_paths.end() );
    }

    //  6.  Selection hygiene: purge any IDs that vanished
    _prune_selection_mutability();
    return;
}


//  "_handle_edit_anchor"
//
inline void Editor::_handle_edit_anchor([[maybe_unused]] const Interaction & it)
{
    [[maybe_unused]] ImGuiIO & io = ImGui::GetIO();

    auto select_vertex = [&](uint32_t vid)
    {
        this->reset_selection();    // m_sel.clear();
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
                v->m_bezier.out_handle  = ImVec2(0,0); // make handle distinct from anchor

            return;                           // drag continues next frame
        }
    }

    
    return;
}

//
//
// *************************************************************************** //
// *************************************************************************** //   END "TOOL HANDLERS".






// *************************************************************************** //
//
//
//
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
