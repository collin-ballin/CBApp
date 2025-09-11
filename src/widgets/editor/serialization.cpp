/***********************************************************************************
*
*       *********************************************************************
*       ****      S E R I A L I Z A T I O N . C P P  ____  F I L E       ****
*       *********************************************************************
*
*              AUTHOR:      Collin A. Bond.
*               DATED:      August 25, 2025.
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
//
//      1.      MAIN FUNCTIONS FOR "SYSTEM PREFERENCES" AND OTHER UI-MENUS...
// *************************************************************************** //
// *************************************************************************** //

//  "_draw_editor_settings"
//
void Editor::_draw_editor_settings([[maybe_unused]] popup::Context & ctx)
{
    [[maybe_unused]] ImGuiStyle &   style               = ImGui::GetStyle();
    EditorState &                   ES                  = this->m_editor_S;
    EditorStyle &                   Style               = this->m_style;
    //
    [[maybe_unused]] float &        LABEL_W             = m_style.ms_SETTINGS_LABEL_WIDTH;
    [[maybe_unused]] float &        WIDGET_AVAIL        = m_style.ms_SETTINGS_WIDGET_AVAIL;
    [[maybe_unused]] float &        WIDGET_W            = m_style.ms_SETTINGS_WIDGET_WIDTH;
    this->m_style.ms_SETTINGS_INDENT_SPACING_CACHE      = 4.0f * style.IndentSpacing;
    S.PushFont(Font::Small);
    
    this->m_style.ms_SETTINGS_SMALL_LINE_HEIGHT         = 0.5f * ImGui::GetTextLineHeight();
    WIDGET_AVAIL                                        = ImGui::GetContentRegionAvail().x - LABEL_W;
    WIDGET_W                                            = WIDGET_AVAIL;
    //
    SettingsData                    args                = {
        ES, Style, LABEL_W, WIDGET_W
    };
    
    
    

    //  1.  SAVE/LOAD SERIALIZATION...
    ImGui::SetNextItemOpen(true, ImGuiCond_Once);
    if ( ImGui::CollapsingHeader("Project Data") ) {
        this->_settings_H1( args );
    }
    
    
    //  2.  EDITOR SETTINGS...
    ImGui::SetNextItemOpen(true, ImGuiCond_Once);
    if ( ImGui::CollapsingHeader("Editor Settings") ) {
        this->_settings_H2( args );
    }


    //  3.  USER PREFERENCES...
    ImGui::SetNextItemOpen(false, ImGuiCond_Once);
    if ( ImGui::CollapsingHeader("User Preferences") ) {
        this->_settings_H3( args );
    }


    //  4.  OPERATIONS...
    ImGui::SetNextItemOpen(false, ImGuiCond_Once);
    if ( ImGui::CollapsingHeader("Operations") ) {
        this->_settings_H4( args );
    }
    
    
    
    S.PopFont();
    return;
}



// *************************************************************************** //
//
//
//
//      HELPER FUNCTIONS FOR EDITOR SETTINGS...
// *************************************************************************** //
// *************************************************************************** //



// *************************************************************************** //
//
//          1.      "SERIALIZE"...
// *************************************************************************** //
// *************************************************************************** //

//  "_settings_H1"
//
inline void Editor::_settings_H1(SettingsData & args)
{
    namespace                   fs                      = std::filesystem;
    const bool                  has_file                = this->has_file();
    
    
    
    //      1.      PROJECT NAME...
    S.PushFont(Font::Main);
    ImGui::Indent();            args.Style.PushSettingsWidgetW(1);
    {
    //
    //
        const ImGuiInputTextFlags   INPUT_FLAGS     = ImGuiInputTextFlags_ReadOnly | ImGuiInputTextFlags_CharsNoBlank;
        
        
        ImGui::PushStyleColor( ImGuiCol_Text, (has_file) ? S.SystemColor.Blue : S.SystemColor.Gray );
            this->S.labelf("Current File:",             args.LABEL_W,    args.WIDGET_W);
        ImGui::PopStyleColor(); //  ImGuiCol_Text
        ImGui::SetNextItemWidth( -1.0f );
        //
        //
        if ( has_file )
        {
            ImGui::PushStyleColor( ImGuiCol_FrameBg, S.SystemColor.Blue );     //  ImGui::TextColored( app::DEF_APPLE_BLUE, "%s", args.ES.m_filepath.filename().string().c_str() );
                ImGui::InputTextWithHint( "##H1_Filename", "filename", &args.ES.m_project_name, INPUT_FLAGS );
            ImGui::PopStyleColor(); //  ImGuiCol_FrameBg
        }
        else
        {
            ImGui::TextDisabled( "%s", ms_NO_ASSIGNED_FILE_STRING );
        }
    //
    //
    }
    ImGui::NewLine();
    ImGui::Unindent();          args.Style.PopSettingsWidgetW();
    S.PopFont();
        
    
    
    //      2.      PROJECT DATA...
    //  ImGui::SetNextItemOpen(true, ImGuiCond_Once);
    //  if ( ImGui::TreeNode("Canvas") )
    //  {
    //  //
    ImGui::Indent();        args.Style.PushSettingsWidgetW(1);
        {
            this->_H1_project_data( args );
        }
        //
        //
        //
    ImGui::Unindent();      args.Style.PopSettingsWidgetW();
    //      //
    ImGui::NewLine();
    //      ImGui::TreePop();
    //  }

    return;
}


//  "_H1_project_data"
//
inline void Editor::_H1_project_data(SettingsData & args)
{
    constexpr float             SLIDER_SEP      = 10;
    const float                 SLIDER_W        = 0.5f * (args.WIDGET_W - SLIDER_SEP);
    const ImGuiInputTextFlags   INPUT_FLAGS     = ImGuiInputTextFlags_CharsScientific | ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_AutoSelectAll;
    //  ImGuiInputTextFlags_CallbackEdit


    this->S.labelf("World Size:", args.LABEL_W, SLIDER_W);                           //  1.2A.   WINDOW LIMITS.
    //
    //
    ImGui::SetNextItemWidth( SLIDER_W );
    ImGui::InputScalar( "##Mechanics_World_Limits_ConstraintsX",                            //  1.2A-1.   WINDOW LIMITS X.
                        ImGuiDataType_Double,
                        &args.ES.m_world_size[0].value,
                        /* small step = */ &args.ES.ms_INPUT_DOUBLE_INCREMENTS[0],
                        /* big step   = */ &args.ES.ms_INPUT_DOUBLE_INCREMENTS[1],
                        "%.0f px",
                        INPUT_FLAGS );
    args.ES.m_world_size[0].Clamp();
    //
    ImGui::SameLine(0, SLIDER_SEP);         ImGui::SetNextItemWidth( SLIDER_W );
    ImGui::InputScalar( "##Mechanics_World_Limits_ConstraintsY",                            //  1.2A-2.   WINDOW LIMITS Y.
                        ImGuiDataType_Double,
                        &args.ES.m_world_size[1].value,
                        &args.ES.ms_INPUT_DOUBLE_INCREMENTS[0],
                        &args.ES.ms_INPUT_DOUBLE_INCREMENTS[1],
                        "%.0f px",
                        INPUT_FLAGS );
    args.ES.m_world_size[1].Clamp();


    
    this->S.labelf("Zoom Constraints:", args.LABEL_W, SLIDER_W);             //  2.1B.     CANVAS ZOOM-LEVEL LIMITS.
    //
    //
    //
    ImGui::SetNextItemWidth( SLIDER_W );
    ImGui::InputScalar( "##Mechanics_Zoom_Limits_X",
                        ImGuiDataType_Double,
                        &args.ES.m_zoom_size[0].value,
                        /* small step = */ &args.ES.ms_INPUT_DOUBLE_INCREMENTS[0],
                        /* big step   = */ &args.ES.ms_INPUT_DOUBLE_INCREMENTS[1],
                        "%.0f px",
                        INPUT_FLAGS );
    args.ES.m_zoom_size[0].Clamp();
    //
    //
    //
    ImGui::SameLine(0, SLIDER_SEP);         ImGui::SetNextItemWidth( SLIDER_W );
    ImGui::InputScalar( "##Mechanics_Zoom_Limits_Y",
                        ImGuiDataType_Double,
                        &args.ES.m_zoom_size[1].value,
                        &args.ES.ms_INPUT_DOUBLE_INCREMENTS[0],
                        &args.ES.ms_INPUT_DOUBLE_INCREMENTS[1],
                        "%.0f px",
                        INPUT_FLAGS );
    args.ES.m_zoom_size[1].Clamp();

    return;
}

//
//
// *************************************************************************** //
// *************************************************************************** //   END "H1. SERIALIZE".






// *************************************************************************** //
//
//          2.      "BEHAVIOR AND STATE"...
// *************************************************************************** //
// *************************************************************************** //

//  "_settings_H2"
//
inline void Editor::_settings_H2(SettingsData & args)
{
    //  BrowserState &                  BState              = this->m_browser_S;
    //
    //constexpr ImGuiSliderFlags      SLIDER_FLAGS        = ImGuiSliderFlags_AlwaysClamp;
    //constexpr ImGuiColorEditFlags   COLOR_FLAGS         = ImGuiColorEditFlags_NoInputs;
    
    
    
    //      1.      STATE...
    this->S.DisabledSeparatorText("State");
    //
    //
    //
        this->_H2_state( args );
    //
    //
    //  END "STATE".
    
    
    
    //      2.      MECHANICS...
    ImGui::NewLine();
    this->S.DisabledSeparatorText("Mechanics");
    //
    //
    //
        this->_H2_mechanics( args );
    //
    //
    //  END "STATE".
    
    
    
    ImGui::NewLine();
    return;
}




//  "_H2_state"
//
inline void Editor::_H2_state(SettingsData & args)
{
    //  BrowserState &                  BState              = this->m_browser_S;
    //
    //constexpr ImGuiSliderFlags      SLIDER_FLAGS        = ImGuiSliderFlags_AlwaysClamp;
    //constexpr ImGuiColorEditFlags   COLOR_FLAGS         = ImGuiColorEditFlags_NoInputs;
    
    
    
    ImGui::SetNextItemOpen(false, ImGuiCond_Once);
    if ( ImGui::TreeNode("Behaviors") )
    {
        ImGui::Indent();        args.Style.PushSettingsWidgetW(1);


        //      1.      "Grid"
        ImGui::SetNextItemOpen(false, ImGuiCond_Once);
        if ( ImGui::TreeNode("Grid") )
        {
            ImGui::Indent();        args.Style.PushSettingsWidgetW(2);
            //
            //
                //      1.      GRID.
                //  args.Style.SmallNewLine();       ImGui::TextDisabled("Lasso Tool");
                //
                //
                this->S.labelf("Snap-To-Grid:",                         args.LABEL_W,               args.WIDGET_W);       //  1.1.        SNAP-TO-GRID.
                ImGui::Checkbox("##H2_Grid_SnapToGrid",                 &m_grid.snap_on);
                
                this->S.labelf("Show Grid:",                            args.LABEL_W,               args.WIDGET_W);       //  1.2.        SHOW GRID.
                ImGui::Checkbox("##H2_Grid_ShowGrid",                   &m_grid.visible);
                
                
                ImGui::BeginGroup();
                    this->S.labelf("Lock X&Y Dimensions:",              args.LABEL_W,               args.WIDGET_W);       //  1.3.        GRID ASPECT RATIO.
                    ImGui::CheckboxFlags("##H2_Grid_EqualGrid",         &m_plot_flags,              ImPlotFlags_Equal );
                ImGui::EndGroup();
                //
                ImGui::SetItemTooltip("(x, y) pairs are constrained to have the same units/pixel value.");
                
                
                this->S.labelf("Show Frame:",                           args.LABEL_W,               args.WIDGET_W);       //  1.5.        HIDE FRAME.
                ImGui::CheckboxFlags("##H2_Grid_ShowFrame",             &m_plot_flags,              ImPlotFlags_NoFrame );
                
                
                this->S.labelf("Hide Cursor Coordinates:",              args.LABEL_W,               args.WIDGET_W);       //  1.6.        CURSOR POSITION.
                ImGui::CheckboxFlags("##H2_Grid_CursorCoordinates",     &m_plot_flags,              ImPlotFlags_NoMouseText );
                
                
                
                
                
                //      2.      AXES.
                args.Style.SmallNewLine();       ImGui::TextDisabled("Axes");
                //
                //
                this->S.labelf("X-Axis On Top:",                        args.LABEL_W,               args.WIDGET_W);       //  2.1.        X-AXIS POSITION.
                ImGui::CheckboxFlags( "##H2_Axes_XLocation",            &m_axes[0].flags,           ImPlotAxisFlags_Opposite );
                
                this->S.labelf("Y-Axis On Right:",                      args.LABEL_W,               args.WIDGET_W);       //  2.2.        Y-AXIS POSITION.
                ImGui::CheckboxFlags( "##H2_Axes_YLocation",            &m_axes[1].flags,           ImPlotAxisFlags_Opposite );
                
                
                this->S.labelf("Hide Axes Labels:",                     args.LABEL_W,               args.WIDGET_W);       //  2.3.       AXES LABELS.
                if ( ImGui::CheckboxFlags("##H2_Axes_Labels",           &m_axes[0].flags,           ImPlotAxisFlags_NoLabel ) )
                { m_axes[1].flags ^= ImPlotAxisFlags_NoLabel; }
                
                
                this->S.labelf("Hide Tick-Labels:",                     args.LABEL_W,               args.WIDGET_W);       //  2.4.        TICK LABELS.
                if ( ImGui::CheckboxFlags("##H2_Axes_TickLabels",       &m_axes[0].flags,           ImPlotAxisFlags_NoTickLabels ) )
                { m_axes[1].flags ^= ImPlotAxisFlags_NoTickLabels; }
                
                
                this->S.labelf("Hide Tick-Marks:",                      args.LABEL_W,               args.WIDGET_W);       //  2.5.        TICK MARKS.
                if ( ImGui::CheckboxFlags("##H2_Axes_TickMarks",        &m_axes[0].flags,           ImPlotAxisFlags_NoTickMarks ) )
                { m_axes[1].flags ^= ImPlotAxisFlags_NoTickMarks; }
                
            
                
                
                //  m_axes[1].flags
                //  ImGui::Checkbox("##H2_Grid_EqualGrid",                  &m_grid.visible);
                
                
                




                
            //  ImGui::CheckboxFlags(); //"io.BackendFlags: RendererHasViewports",   &io.BackendFlags, ImGuiBackendFlags_RendererHasViewports);
    
            //
            //
            ImGui::Unindent();      args.Style.PopSettingsWidgetW();
            //
            ImGui::NewLine();
            ImGui::TreePop();
        //
        }//  END "(1) Grid".



        //      2.      "Editor"
        ImGui::SetNextItemOpen(false, ImGuiCond_Once);
        if ( ImGui::TreeNode("Editor") )
        {
            ImGui::Indent();        args.Style.PushSettingsWidgetW(2);
            //
            //
                this->S.labelf("Disable Overlays:",  args.LABEL_W, args.WIDGET_W);                      //  2.1.        BLOCK OVERLAY MENUS.
                ImGui::Checkbox("##Editor_Settings_Mechanics_DisableOverlays",          &args.ES.m_block_overlays);
                //
                ImGui::BeginDisabled(args.ES.m_block_overlays);
                    this->S.labelf("Show Debugger Overlay:",  args.LABEL_W, args.WIDGET_W);             //  2.2.        SHOW DEBUG OVERLAY.
                    ImGui::Checkbox("##Editor_Settings_Mechanics_ShowDebugOverlay",         &args.ES.m_show_debug_overlay);
                    //
                    this->S.labelf("Show UI-Traits Overlay:",  args.LABEL_W, args.WIDGET_W);            //  2.3.        SHOW UI-TRAITS OVERLAY.
                    ImGui::Checkbox("##Editor_Settings_Mechanics_ShowUITraitsOverlay",      &args.ES.m_show_ui_traits_overlay);
                    //
                    this->S.labelf("Show UI-Objects Overlay:",  args.LABEL_W, args.WIDGET_W);           //  2.4.        SHOW UI-TRAITS OVERLAY.
                    ImGui::Checkbox("##Editor_Settings_Mechanics_ShowUIObjectsOverlay",     &args.ES.m_show_ui_objects_overlay);
                ImGui::EndDisabled();
            //
            //
            ImGui::Unindent();      args.Style.PopSettingsWidgetW();
            //
            ImGui::NewLine();
            ImGui::TreePop();
        //
        }//  END "(2) Editor".
    
            
        
        ImGui::Separator();
        ImGui::Unindent();      args.Style.PopSettingsWidgetW();
        //
        //
        //
        ImGui::NewLine();
        ImGui::TreePop();
    }
    //
    //
    //  END "Behaviors".
    
    
    
    return;
}


//  "_H2_mechanics"
//
inline void Editor::_H2_mechanics(SettingsData & args)
{
    //  BrowserState &                  BState              = this->m_browser_S;
    //
    constexpr ImGuiSliderFlags      SLIDER_FLAGS        = ImGuiSliderFlags_AlwaysClamp;
    //constexpr ImGuiColorEditFlags   COLOR_FLAGS         = ImGuiColorEditFlags_NoInputs;
    
    
    
    //      2.2.    INTERACTIVITY / "HUMAN INPUT" / "TOUCH AND FEEL".
    ImGui::SetNextItemOpen(false, ImGuiCond_Once);
    if ( ImGui::TreeNode("Human Input") )
    {
        ImGui::Indent();        args.Style.PushSettingsWidgetW(1);
        //
        //
        //
        //  //  2.1A.       HIT THRESHOLD.
            this->S.labelf("Vertex Hit Radius:", args.LABEL_W, args.WIDGET_W);
            ImGui::SliderFloat( "##H2_Editor_HitThreshold",             &m_style.HIT_THRESH_SQ,             4.0f,   81.0f,  "%.1f units-squared",  SLIDER_FLAGS);
            

            //  2.1C.       MOUSEWHEEL ZOOM RATE.
            this->S.labelf("Mousewheel Zoom Rate:", args.LABEL_W, args.WIDGET_W);
            ImGui::SliderFloat( "##H2_Canvas_ZoomRate", &args.ES.m_mousewheel_zoom_rate.value, args.ES.m_mousewheel_zoom_rate.Min(), args.ES.m_mousewheel_zoom_rate.Max(),  "%.2f",  SLIDER_FLAGS );
        //
        //
        //
        ImGui::Separator();
        ImGui::Unindent();        args.Style.PopSettingsWidgetW();
        //
        //
        //
        ImGui::TreePop();
    }
    
    
    
    return;
}








//
//
// *************************************************************************** //
// *************************************************************************** //   END "H2. BEHAVIORS AND MECHANICS".






// *************************************************************************** //
//
//          3.      "USER PREFERENCES"...
// *************************************************************************** //
// *************************************************************************** //

//  "_draw_settings_user_preferences"
//
inline void Editor::_settings_H3(SettingsData & args)
{
    constexpr ImGuiSliderFlags          SLIDER_FLAGS        = ImGuiSliderFlags_AlwaysClamp;
    constexpr ImGuiColorEditFlags       COLOR_FLAGS         = ImGuiColorEditFlags_NoInputs;
    //
    static Overlay &                    selection_resident  = *m_ov_manager.lookup_resident( m_residents[Resident::Selection].id );





    //      1.      SELECTION...
    ImGui::SetNextItemOpen(true, ImGuiCond_Once);
    if ( ImGui::TreeNode("Selection") )
    {
        ImVec4          lasso_line_color_f          = u32_to_f4(m_style.COL_LASSO_OUT);
        ImVec4          lasso_fill_color_f          = u32_to_f4(m_style.COL_LASSO_FILL);
        ImVec4          selection_bbox_color        = u32_to_f4(m_style.SELECTION_BBOX_COL);
        //
        const float     init_bbox_margin            = m_style.SELECTION_BBOX_MARGIN_PX;
    
    
        ImGui::Indent();        args.Style.PushSettingsWidgetW(1);
        //
        //
        //
            //      1.      SELECTION BOUNDING BOX...
            args.Style.SmallNewLine();       ImGui::TextDisabled("Selection Bounding Box");
            //
            //              1.1.    SELECTION BBOX MARGIN.
            this->S.labelf("Bounding-Box Margin:",  args.LABEL_W,        args.WIDGET_W);
            if ( ImGui::SliderFloat( "##H2_SelectionBBox_Margin",     &m_style.SELECTION_BBOX_MARGIN_PX,  1.0f,   100.0f,  "%.1f px",  SLIDER_FLAGS) )
                { selection_resident.cfg.anchor_px.y     += ( m_style.SELECTION_BBOX_MARGIN_PX - init_bbox_margin ); }
            //
            //              1.2.    SELECTION OVERLAY MARGIN.
            this->S.labelf("Overlay Margin:",       args.LABEL_W,        args.WIDGET_W);
            ImGui::SliderFloat( "##H2_SelectionOverlay_Offset",     &selection_resident.cfg.anchor_px.y,  m_style.SELECTION_BBOX_MARGIN_PX,   100.0f,  "%.1f px",  SLIDER_FLAGS );
            //
            //              1.3.    SELECTION BBOX COLOR.
            this->S.labelf("Bounding-Box Color:",   args.LABEL_W,        args.WIDGET_W);
            if ( ImGui::ColorEdit4( "##H2_SelectionBBox_Color",          (float*)&selection_bbox_color,  COLOR_FLAGS ) )
                { m_style.SELECTION_BBOX_COL = f4_to_u32(selection_bbox_color); }
                
                
                
            //      2.      LASSO TOOL.
            args.Style.SmallNewLine();       ImGui::TextDisabled("Lasso Tool");
            //
            //              2.1.    COL_LASSO_OUT.
            this->S.labelf("Line Color:",           args.LABEL_W,        args.WIDGET_W);
            if ( ImGui::ColorEdit4( "##Editor_Settings_Style_Selection_LassoLineColor",     (float*)&lasso_line_color_f,    COLOR_FLAGS ) )
                { m_style.COL_LASSO_OUT = f4_to_u32(lasso_line_color_f); }
            //
            //              2.2.    COL_LASSO_FILL.
            this->S.labelf("Fill Color:",           args.LABEL_W,        args.WIDGET_W);
            if ( ImGui::ColorEdit4( "##Editor_Settings_Style_Selection_LassoFillColor",     (float*)&lasso_fill_color_f,    COLOR_FLAGS ) )
                { m_style.COL_LASSO_FILL = f4_to_u32(lasso_fill_color_f); }
            
                  
        //
        //
        //
        ImGui::Separator();
        ImGui::Unindent();      args.Style.PopSettingsWidgetW();
        //
        //
        //
        ImGui::NewLine();
        ImGui::TreePop();
    }
    


    //      2.      INTERACTIVITY...
    ImGui::SetNextItemOpen(true, ImGuiCond_Once);
    if ( ImGui::TreeNode("Handles") )
    {
        ImVec4          handle_color_f              = u32_to_f4(m_style.ms_HANDLE_COLOR);
        ImVec4          handle_hover_color_f        = u32_to_f4(m_style.ms_HANDLE_HOVER_COLOR);
    
    
        ImGui::Indent();        args.Style.PushSettingsWidgetW(1);
        //
            this->S.labelf("Handle Size:", args.LABEL_W, args.WIDGET_W);            //  1.1.    HANDLE SIZE
            ImGui::SliderFloat( "##Editor_Settings_Style_Handle_Size", &m_style.ms_HANDLE_SIZE,         1.0f,   32.0f,  "%.2f px",  SLIDER_FLAGS);
            
            this->S.labelf("Handle Box-Size:", args.LABEL_W, args.WIDGET_W);        //  1.2.    HANDLE BOX-SIZE
            ImGui::SliderFloat( "##Editor_Settings_Style_Handle_BoxSize", &m_style.HANDLE_BOX_SIZE,     1.0f,   32.0f,  "%.2f px",  SLIDER_FLAGS);
            
        
        
            this->S.labelf("Handle Color:", args.LABEL_W, args.WIDGET_W);           //  1.3.    ms_HANDLE_COLOR
            if ( ImGui::ColorEdit4( "##Editor_Settings_Style_Handle_Color",          (float*)&handle_color_f,  COLOR_FLAGS ) )
            { m_style.ms_HANDLE_COLOR = f4_to_u32(handle_color_f); }
        
            this->S.labelf("Handle Hover Color:", args.LABEL_W, args.WIDGET_W);     //  1.4.    ms_HANDLE_HOVER_COLOR
            if ( ImGui::ColorEdit4( "##Editor_Settings_Style_Handle_HoverColor",     (float*)&handle_hover_color_f,  COLOR_FLAGS ) )
            { m_style.ms_HANDLE_HOVER_COLOR = f4_to_u32(handle_hover_color_f); }
        //
        ImGui::Separator();
        ImGui::Unindent();      args.Style.PopSettingsWidgetW();
        //
        //
        //
        ImGui::NewLine();
        ImGui::TreePop();
    }
    ImGui::NewLine();
    
    

    return;
}

//
//
// *************************************************************************** //
// *************************************************************************** //   END "H3 USER PREFERENCES".






// *************************************************************************** //
//
//          4.      "OPERATIONS"...
// *************************************************************************** //
// *************************************************************************** //

//  "_settings_H4"
//
inline void Editor::_settings_H4(SettingsData & args)
{
    namespace                   fs                      = std::filesystem;
    //
    const bool                  has_file                = this->has_file();
    [[maybe_unused]] bool       force_save_as           = false;
    bool                        requested_close         = false;


    
    //      1.      I/O OPERATIONS...
    ImGui::SetNextItemOpen(true, ImGuiCond_Once);
    if ( ImGui::TreeNode("I/O Operations") )
    {
        ImGui::Indent();        args.Style.PushSettingsWidgetW(1);
        
        
        //      2.1.    SAVE DIALOGUE...
        this->S.labelf("Save:",                         args.LABEL_W,    args.WIDGET_W);
        if ( ImGui::Button("Save", ms_SETTINGS_BUTTON_SIZE) )
        {
            
            if (has_file)       { this->save_async( args.ES.m_filepath );            }
            else                { force_save_as = true; requested_close = true;     }

        }
        
        
        //      2.2.    "SAVE AS..." DIALOGUE...
        this->S.labelf("Save As...:",                   args.LABEL_W,    args.WIDGET_W);
        if ( /*force_save_as || */ ImGui::Button("Save As...", ms_SETTINGS_BUTTON_SIZE) )
        {
            requested_close = true;
            this->save_as();
        }


        //      2.3.    "OPEN" DIALOGUE...
        this->S.labelf("Open File:",                    args.LABEL_W,    args.WIDGET_W);
        if ( ImGui::Button("Open", ms_SETTINGS_BUTTON_SIZE) )
        {
            CB_LOG( LogLevel::Info, "Editor | requesting file dialog to load from disk" );
            requested_close = true;
            args.ES.m_odialog_open.store( true, std::memory_order_release );
        }


        //      2.4.    "NEW" DIALOGUE...
        this->S.labelf("New File:",                     args.LABEL_W,    args.WIDGET_W);
        ImGui::BeginDisabled(true);
            if ( ImGui::Button("New", ms_SETTINGS_BUTTON_SIZE) )
            {
                args.ES.m_filepath       = fs::path{   };
                args.ES.m_project_name   .clear();
                requested_close     = true;
            }
        ImGui::EndDisabled();
        
        
        
        ImGui::Separator();
        ImGui::Unindent();      args.Style.PopSettingsWidgetW();
        //
        //
        //
        //  ImGui::NewLine();
        ImGui::TreePop();
    }
    //
    //
    ImGui::NewLine();
    
    
    if ( requested_close )      { ImGui::CloseCurrentPopup(); }
    
    

    return;
}

//
//
// *************************************************************************** //
// *************************************************************************** //   END "H4 OPERATIONS".






//
//
// *************************************************************************** //
// *************************************************************************** //   END "EDITOR SETTINGS".












// *************************************************************************** //
//
//
//
//      2.      SERIALIZATION ORCHESTRATOR FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

//  "_MECH_pump_main_tasks"
//
void Editor::_MECH_pump_main_tasks(void)
{
    std::vector<std::function<void()>> tasks;
    {
        std::lock_guard lk( this->m_editor_S.m_task_mtx );
        tasks.swap( this->m_editor_S.m_main_tasks );
    }
    for (auto & fn : tasks) fn();
}


//  "_draw_io_overlay"
//
void Editor::_draw_io_overlay(void)
{
    EditorState &       ES      = this->m_editor_S;
    
    if ( (!ES.m_io_busy)  &&  (ES.m_io_last == IOResult::Ok) )              { return; }   // nothing to show

    const char *        txt         = (ES.m_io_busy)    ? "Working..."  : ES.m_io_msg.c_str();
    ImVec2              pad         {8, 8};
    ImVec2              size        = ImGui::CalcTextSize(txt);
    ImVec2              pos         = ImGui::GetMainViewport()->Pos;
    pos.y                          += ImGui::GetMainViewport()->Size.y - size.y - pad.y*2;

    ImGui::SetNextWindowPos(pos, ImGuiCond_Always);
    ImGui::SetNextWindowBgAlpha(0.6f);
    //
    ImGui::Begin("##io_overlay", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_AlwaysAutoResize);
        ImGui::TextUnformatted(txt);
    ImGui::End();
    
    return;
}



//      2.1.        EDITOR SNAPSHOTS:
// *************************************************************************** //
// *************************************************************************** //

//  "make_snapshot"
//
EditorSnapshot Editor::make_snapshot(void) const
{
    //      1.      EXISTING...
    EditorSnapshot      s;
    s.vertices          = m_vertices;        // shallow copies fine
    s.paths             = m_paths;
    s.points            = m_points;
    s.selection         = m_sel;
    
    
    
    //      2.      NEW ENTRIES / SUB-OBJECTS...
    
    
    
    //      3.      TODO:   GRID, VIEW, MODE...
    //  _recompute_next_ids();
    
    return s;
}


//  "load_from_snapshot"
//
void Editor::load_from_snapshot(EditorSnapshot && snap)
{
    //      1.      EXISTING...
    m_vertices  = std::move(snap.vertices);
    m_paths     = std::move(snap.paths);
    m_points    = std::move(snap.points);
    m_sel       = std::move(snap.selection);
    
    
    //      2.      NEW ENTRIES / SUB-OBJECTS...
    
    
    //      3.      TODO:   GRID, VIEW, MODE...
    _recompute_next_ids();          // ← ensure unique IDs going forward
    
    return;
}

//
// *************************************************************************** //
// *************************************************************************** //   END "EDITOR SNAPSHOT".



//
//
// *************************************************************************** //
// *************************************************************************** //   END "SERIALIZATION ORCHESTRATOR".












// *************************************************************************** //
//
//
//
//      3.      SERIALIZATION IMPLEMENTATION FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //



// *************************************************************************** //
//
//              3.1.        SAVING:
// *************************************************************************** //
// *************************************************************************** //

//  "save_async"
//
bool Editor::save_async(std::filesystem::path path)
{
    namespace           fs          = std::filesystem;
    EditorState &       ES          = this->m_editor_S;
    bool                result      = true;
    //
    ES.m_io_busy                    = true;
    auto snap                       = make_snapshot();
    //
    //
    path                            = fs::absolute(path);   //  convert to absolute once, so worker sees a full path


    //      1.      PERFORM I/O ON SECONDARY THREAD...
    std::thread([this, snap = std::move(snap), path]{
        save_worker(snap, path);
    }).detach();
    
    
    
    //      2.      TAKE ACTIONS BASED ON SUCCESS/FAILURE OF LOADING PROCEDURE...
    if ( (ES.m_io_last == IOResult::Ok) || (ES.m_io_last == IOResult::VersionMismatch) )
    {
        result              = true;
    }
    else
    {
        result              = false;
    }
    
    
    return result;
}


//  "save_worker"
//
void Editor::save_worker(EditorSnapshot snap, std::filesystem::path path)
{
    EditorState &       ES      = this->m_editor_S;
    nlohmann::json      j;
    //
    //
    //  j["version"]                    = std::format("{}.{}", this->ms_MAJOR_VERSION, this->ms_MINOR_VERSION); //    kSaveFormatVersion;
    //
    //
        j["version"]                    = ms_EDITOR_SCHEMA;       // uses to_json(SchemaVersion)
        //
        j["state"]                      = snap;
    //
    //
    //  j["editor_state"]               = this->m_editor_S;
    //
    //
    std::ofstream       os          (path, std::ios::binary);
    IOResult            res         = ( os )    ? IOResult::Ok      : IOResult::IoError;
    
    
    if ( res == IOResult::Ok )      { os << j.dump(2); }
    
    
    // enqueue completion notification
    {
        std::lock_guard     lk      (ES.m_task_mtx);
        ES.m_main_tasks.push_back( [this, res, path]
        {
            EditorState &   ES_             = this->m_editor_S;
            std::string     message         = {   };

            switch ( res )
            {
                //  CASE 2A :   SAVING SUCCESS.
                case IOResult::Ok                   : {
                    message     = std::format( "Saved data to file \"{}\"", path.filename().string() );
                    break;
                }
                //
                //  CASE 2B :   SAVING FAILURE.
                default                             : {
                    message = std::format( "Failed to save file \"{}\", (I/O Result: {})", path.filename().string(), ms_IORESULT_NAMES[ res ] );
                    break;
                }
            }
            //
            //
            ES_.SetLastIOStatus( res, message );
            
        } );
    }
    
    
    //  2.  EVALUATE SUCCESS/FAILURE OF IO-OPERATION...
    {
        const char *    status      = this->ms_IORESULT_NAMES[ ES.m_io_last ];
    
        //  CASE 2A :   SAVE SUCCESS.
        if ( ES.m_io_last == IOResult::Ok ) {
            CB_LOG( LogLevel::Info, std::format("Editor | successfully saved data to \"{}\" [status: {}] ", path.filename().string(), status) );
        }
        //
        //  CASE 2A :   SAVE FAILURE.
        else {
            CB_LOG( LogLevel::Error, std::format("Editor | failed to save data to \"{}\" [status: {}] ", path.filename().string(), status) );
        }
    }
    
    
    
    return;
}

//
// *************************************************************************** //
// *************************************************************************** //   END "SAVING".



// *************************************************************************** //
//
//              3.2.        LOADING:
// *************************************************************************** //
// *************************************************************************** //

//  "load_async"
//
bool Editor::load_async(std::filesystem::path path)
{
    EditorState &       ES          = this->m_editor_S;
    ES.m_io_busy                    = true;
    path                            = std::filesystem::absolute(path);
    bool                result      = true;


    //      1.      PERFORM I/O ON SECONDARY THREAD...
    std::thread([this, path]{
        load_worker(path);
    }).detach();
    
    
    //      2.      TAKE ACTIONS BASED ON SUCCESS/FAILURE OF LOADING PROCEDURE...
    if ( (ES.m_io_last == IOResult::Ok) || (ES.m_io_last == IOResult::VersionMismatch) )
    {
        result              = true;
        ES.m_filepath       = path;
        ES.m_project_name   = path.filename().string();
    }
    else
    {
        result              = false;
        ES.m_filepath       = std::filesystem::path{   };
        ES.m_project_name   .clear();
        this->RESET_ALL();
    }
    
    
    return result;
}


//  "load_worker"
//
void Editor::load_worker(std::filesystem::path path)
{
    using                   Version     = cblib::SchemaVersion;
    EditorState &           ES          = this->m_editor_S;
    nlohmann::json          j;
    std::ifstream           is          (path, std::ios::binary);
    IOResult                res         = ( is )    ? IOResult::Ok      : IOResult::IoError;
    EditorSnapshot          snap;


    //      1.      LOAD FROM JSON-FILE...
    if ( res == IOResult::Ok )
    {
        try
        {
            is >> j;
            const Version     file_ver    = j.at("version").get<Version>();
            
            
            //      1A.     VERSION MIS-MATCH ERROR...
            if ( (file_ver != this->ms_EDITOR_SCHEMA)  ||  (file_ver > ms_EDITOR_SCHEMA) )
            {
                res = IOResult::VersionMismatch;
            }
            //
            //      1B.     MAIN LOADING BRANCH...
            else
            {
                snap = j.at("state").get<EditorSnapshot>();
            }
        }
        catch (...)     { res = IOResult::ParseError; }
    }


    //      2.      ASSESS I/O RESULT.      -- enqueue GUI-thread callback...
    {
        std::lock_guard     lk      (ES.m_task_mtx);
        
        ES.m_main_tasks.push_back( [this, res, snap = std::move(snap), path]() mutable
        {
            EditorState &   ES_             = this->m_editor_S;
            std::string     message         = {   };


            switch ( res )
            {
                //  CASE 2A :   LOADING SUCCESS.
                case IOResult::Ok                   : {
                    this->load_from_snapshot( std::move(snap) );
                    message     = std::format( "Loaded from file \"{}\"", path.filename().string() );
                    break;
                }
                //
                //  CASE 2B :   LOADING FAILURE.
                case IOResult::IoError              : { message = "load I/O error"              ; break;  }
                case IOResult::ParseError           : { message = "load parsing error"          ; break;  }
                case IOResult::VersionMismatch      : { message = "json version mismatch"       ; break;  }
                default                             : { message = "unknown load error"          ; break;  }
            }
            //
            //
            ES_.SetLastIOStatus( res, message );
            return;
        } );
    }
    
    
    return;
}

//
// *************************************************************************** //
// *************************************************************************** //   END "LOADING".



//
//
// *************************************************************************** //
// *************************************************************************** //   END "SERIALIZATION IMPLEMENTATION".
























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
