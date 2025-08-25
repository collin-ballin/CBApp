/***********************************************************************************
*
*       *********************************************************************
*       ****      S E R I A L I Z A T I O N . C P P  ____  F I L E       ****
*       *********************************************************************
*
*              AUTHOR:      Collin A. Bond
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
    S.PushFont(Font::Small);


    //  1.  SAVE/LOAD SERIALIZATION...
    ImGui::SetNextItemOpen(true, ImGuiCond_Once);
    if ( ImGui::CollapsingHeader("Serialization") ) {
        this->_draw_settings_serialize();
    }
    
    
    //  2.  EDITOR SETTINGS...
    ImGui::SetNextItemOpen(true, ImGuiCond_Once);
    if ( ImGui::CollapsingHeader("Behaviors and Mechanics") ) {
        this->_draw_settings_mechanics();
    }


    //  3.  USER PREFERENCES...
    ImGui::SetNextItemOpen(false, ImGuiCond_Once);
    if ( ImGui::CollapsingHeader("Style and Preferences") ) {
        this->_draw_settings_style_and_preferences();
    }
    
    
    
    S.PopFont();
    return;
}



// *************************************************************************** //
//
//
//      HELPER FUNCTIONS FOR EDITOR SETTINGS...
// *************************************************************************** //
// *************************************************************************** //

//  "_draw_settings_serialize"
//
void Editor::_draw_settings_serialize(void)
{
    EditorState &               EState                  = this->m_editor_S;
    const bool                  has_file                = this->has_file();
    [[maybe_unused]] bool       force_save_as           = false;
    bool                        requested_close         = false;
    //
    //  const float &           LABEL_W             = m_style.ms_SETTINGS_LABEL_WIDTH;
    //  const float &           WIDGET_W            = m_style.ms_SETTINGS_WIDGET_WIDTH;
    
    
    
    
    
    
    
    //      1.      CURRENT FILE / PROJECT METADATA...
    //
    ImGui::SetNextItemOpen(true, ImGuiCond_Once);
    if ( ImGui::TreeNode("Session Metadata") )
    {
        ImGui::Indent();
        //
        //
        S.PushFont(Font::Main);
        
            this->S.labelf("Current File:",                 this->ms_SETTINGS_LABEL_WIDTH,      this->ms_SETTINGS_WIDGET_WIDTH);
            if ( has_file )     { ImGui::TextColored( app::DEF_APPLE_BLUE, "%s", EState.m_filepath.filename().string().c_str() ); }
            else                { ImGui::TextDisabled( "%s", ms_NO_ASSIGNED_FILE_STRING ); }
            
        S.PopFont();
        //
        //
        ImGui::Unindent();
        //
        //
        //
        ImGui::NewLine();
        ImGui::TreePop();
    }
    
    
    
    
    
    
    //      2.      I/O OPERATIONS...
    //
    ImGui::SetNextItemOpen(true, ImGuiCond_Once);
    if ( ImGui::TreeNode("I/O Operations") )
    {
        ImGui::Indent();
        
        
        //      2.1.    SAVE DIALOGUE...
        this->S.labelf("Save:",                         this->ms_SETTINGS_LABEL_WIDTH,      this->ms_SETTINGS_WIDGET_WIDTH);
        if ( ImGui::Button("Save", ms_SETTINGS_BUTTON_SIZE) )
        {
            
            if (has_file)       { this->save_async( EState.m_filepath );    }
            else                { force_save_as = true;                     }

        }
        
        
        //      3.      "SAVE AS..." DIALOGUE...
        this->S.labelf("Save As...:",                   this->ms_SETTINGS_LABEL_WIDTH,      this->ms_SETTINGS_WIDGET_WIDTH);
        if ( /*force_save_as || */ ImGui::Button("Save As...", ms_SETTINGS_BUTTON_SIZE) )
        {
            requested_close = true;
            this->save_as();
        }


        //      4.      "OPEN" DIALOGUE...
        this->S.labelf("Open File:",                    this->ms_SETTINGS_LABEL_WIDTH,      this->ms_SETTINGS_WIDGET_WIDTH);
        if ( ImGui::Button("Load", ms_SETTINGS_BUTTON_SIZE) )
        {
            CB_LOG( LogLevel::Info, "Editor | requesting file dialog to load from disk" );
            requested_close = true;
            EState.m_odialog_open.store( true, std::memory_order_release );
        }
        
        
        
        ImGui::Unindent();
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


//  "_draw_settings_mechanics"
//
void Editor::_draw_settings_mechanics(void)
{
    const float &                   LABEL_W             = m_style.ms_SETTINGS_LABEL_WIDTH;
    const float &                   WIDGET_W            = m_style.ms_SETTINGS_WIDGET_WIDTH;
    constexpr ImGuiSliderFlags      SLIDER_FLAGS        = ImGuiSliderFlags_AlwaysClamp;
    //constexpr ImGuiColorEditFlags   COLOR_FLAGS         = ImGuiColorEditFlags_NoInputs;
    //
    EditorState &                   EState              = this->m_editor_S;
    //  BrowserState &                  BState              = this->m_browser_S;
    
    
    //      1.      STATE...
    this->S.DisabledSeparatorText("State");
    ImGui::Indent();
    //
    //
        this->S.labelf("Show Grid:",              LABEL_W, WIDGET_W);           //  1.1.        SHOW GRID.
        ImGui::Checkbox("##Editor_Settings_Mechanics_ShowGrid",                 &m_grid.visible);
        
        this->S.labelf("Snap-To-Grid:",           LABEL_W, WIDGET_W);           //  1.2.        SNAP-TO-GRID.
        ImGui::Checkbox("##Editor_Settings_Mechanics_SnapToGrid",               &m_grid.snap_on);
        
        
        
        this->S.labelf("Show Debugger Overlay:",  LABEL_W, WIDGET_W);           //  2.1.        SHOW DEBUG OVERLAY.
        ImGui::Checkbox("##Editor_Settings_Mechanics_ShowDebugOverlay",         &EState.m_show_debug_overlay);
        //
        this->S.labelf("Show UI-Traits Overlay:",  LABEL_W, WIDGET_W);          //  2.2.        SHOW UI-TRAITS OVERLAY.
        ImGui::Checkbox("##Editor_Settings_Mechanics_ShowUITraitsOverlay",      &EState.m_show_ui_traits_overlay);
        //
        this->S.labelf("Show UI-Objects Overlay:",  LABEL_W, WIDGET_W);         //  2.3.        SHOW UI-TRAITS OVERLAY.
        ImGui::Checkbox("##Editor_Settings_Mechanics_ShowUIObjectsOverlay",     &EState.m_show_ui_objects_overlay);
    //
    //
    ImGui::Unindent();
        
    

    //      2.      MECHANICS...
    ImGui::NewLine();
    this->S.DisabledSeparatorText("Mechanics");
    //
    //              2.1.    INTERACTIVITY.
    ImGui::SetNextItemOpen(true, ImGuiCond_Once);
    if ( ImGui::TreeNode("Interactivity") )
    {
        ImGui::Indent();
        //
            this->S.labelf("Vertex Hit Radius:", LABEL_W, WIDGET_W);          //  2.1.        HIT THRESHOLD.
            ImGui::SliderFloat( "##Editor_Settings_Mechanics_HitThreshold",     &m_style.HIT_THRESH_SQ,       4.0f,   81.0f,  "%.1f units-squared",  SLIDER_FLAGS);
            //
            //
            this->S.labelf("Mousewheel Zoom Rate:", LABEL_W, WIDGET_W);            //  1.1.    HANDLE SIZE
            //
            {
                constexpr float     s_SCALE         = 100.0f;
                //
                float               zoom_rate       = s_SCALE * m_style.ms_ZOOM_RATE;
                const bool          dirty           = ImGui::SliderFloat( "##Editor_Settings_Style_ZoomRate", &zoom_rate,         0.0f,   100.0f,  "%.2f",  SLIDER_FLAGS);
                //
                if (dirty)
                {
                    m_style.ms_ZOOM_RATE = (zoom_rate / s_SCALE);
                }
                
                
            }

        //
        ImGui::Unindent();
        //
        //
        //
        ImGui::TreePop();
    }
    
    
    
    ImGui::NewLine();
    return;
}


//  "_draw_settings_user_preferences"
//
void Editor::_draw_settings_style_and_preferences(void)
{
    const float &                   LABEL_W             = m_style.ms_SETTINGS_LABEL_WIDTH;
    const float &                   WIDGET_W            = m_style.ms_SETTINGS_WIDGET_WIDTH;
    constexpr ImGuiSliderFlags      SLIDER_FLAGS        = ImGuiSliderFlags_AlwaysClamp;
    constexpr ImGuiColorEditFlags   COLOR_FLAGS         = ImGuiColorEditFlags_NoInputs;



    //      1.      CANVAS...
    ImGui::SetNextItemOpen(false, ImGuiCond_Once);
    if ( ImGui::TreeNode("Canvas") )
    {
        ImGui::Indent();
        //
            this->S.labelf("Mousewheel Zoom Rate:", LABEL_W, WIDGET_W);            //  1.1.    HANDLE SIZE
            ImGui::SliderFloat( "##Editor_Settings_Style_ZoomRate", &m_style.ms_ZOOM_RATE,         0.0f,   1.0f,  "%.2f",  SLIDER_FLAGS);

        //
        ImGui::Unindent();
        //
        //
        //
        ImGui::NewLine();
        ImGui::TreePop();
    }
    
    
    
    //      2.      SELECTION...
    ImGui::SetNextItemOpen(true, ImGuiCond_Once);
    if ( ImGui::TreeNode("Selection") )
    {
        ImVec4          lasso_line_color_f           = u32_to_f4(m_style.COL_LASSO_OUT);
        ImVec4          lasso_fill_color_f           = u32_to_f4(m_style.COL_LASSO_FILL);
        ImVec4          selection_bbox_color         = u32_to_f4(m_style.SELECTION_BBOX_COL);
    
    
        ImGui::Indent();
        //
            this->S.labelf("Lasso Line Color:", LABEL_W, WIDGET_W);               //  2.1.    COL_LASSO_OUT
            if ( ImGui::ColorEdit4( "##Editor_Settings_Style_Selection_LassoLineColor",     (float*)&lasso_line_color_f,    COLOR_FLAGS ) )
            { m_style.COL_LASSO_OUT = f4_to_u32(lasso_line_color_f); }
        
            this->S.labelf("Lasso Fill Color:", LABEL_W, WIDGET_W);               //  2.2.    COL_LASSO_FILL
            if ( ImGui::ColorEdit4( "##Editor_Settings_Style_Selection_LassoFillColor",     (float*)&lasso_fill_color_f,    COLOR_FLAGS ) )
            { m_style.COL_LASSO_FILL = f4_to_u32(lasso_fill_color_f); }
        
            this->S.labelf("Selection Bounding-Box Color:", LABEL_W, WIDGET_W);   //  2.3.    SELECTION_BBOX_COL
            if ( ImGui::ColorEdit4( "##Editor_Settings_Style_Selection_BBoxColor",          (float*)&selection_bbox_color,  COLOR_FLAGS ) )
            { m_style.SELECTION_BBOX_COL = f4_to_u32(selection_bbox_color); }
        //
        ImGui::Unindent();
        //
        //
        //
        ImGui::NewLine();
        ImGui::TreePop();
    }
    


    //      3.      INTERACTIVITY...
    ImGui::SetNextItemOpen(true, ImGuiCond_Once);
    if ( ImGui::TreeNode("Handles") )
    {
        ImVec4          handle_color_f              = u32_to_f4(m_style.ms_HANDLE_COLOR);
        ImVec4          handle_hover_color_f        = u32_to_f4(m_style.ms_HANDLE_HOVER_COLOR);
    
    
        ImGui::Indent();
        //
            this->S.labelf("Handle Size:", LABEL_W, WIDGET_W);            //  1.1.    HANDLE SIZE
            ImGui::SliderFloat( "##Editor_Settings_Style_Handle_Size", &m_style.ms_HANDLE_SIZE,         1.0f,   32.0f,  "%.2f px",  SLIDER_FLAGS);
            
            this->S.labelf("Handle Box-Size:", LABEL_W, WIDGET_W);        //  1.2.    HANDLE BOX-SIZE
            ImGui::SliderFloat( "##Editor_Settings_Style_Handle_BoxSize", &m_style.HANDLE_BOX_SIZE,     1.0f,   32.0f,  "%.2f px",  SLIDER_FLAGS);
            
        
        
            this->S.labelf("Handle Color:", LABEL_W, WIDGET_W);           //  1.3.    ms_HANDLE_COLOR
            if ( ImGui::ColorEdit4( "##Editor_Settings_Style_Handle_Color",          (float*)&handle_color_f,  COLOR_FLAGS ) )
            { m_style.ms_HANDLE_COLOR = f4_to_u32(handle_color_f); }
        
            this->S.labelf("Handle Hover Color:", LABEL_W, WIDGET_W);     //  1.4.    ms_HANDLE_HOVER_COLOR
            if ( ImGui::ColorEdit4( "##Editor_Settings_Style_Handle_HoverColor",     (float*)&handle_hover_color_f,  COLOR_FLAGS ) )
            { m_style.ms_HANDLE_HOVER_COLOR = f4_to_u32(handle_hover_color_f); }
        //
        ImGui::Unindent();
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
// *************************************************************************** //   END "EDITOR SETTINGS".












// *************************************************************************** //
//
//
//
//      2.      SERIALIZATION ORCHESTRATOR FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

//  "pump_main_tasks"
//
void Editor::pump_main_tasks(void)
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
    EditorState &       EState      = this->m_editor_S;
    
    if ( (!EState.m_io_busy)  &&  (EState.m_io_last == IOResult::Ok) )              { return; }   // nothing to show

    const char *        txt         = (EState.m_io_busy)    ? "Working..."  : EState.m_io_msg.c_str();
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
    EditorSnapshot      s;
    s.vertices          = m_vertices;        // shallow copies fine
    s.paths             = m_paths;
    s.points            = m_points;
    s.lines             = m_lines;
    s.selection         = m_sel;
    //
    //
    //  TODO:   grid, view, mode …
    return s;
}


//  "load_from_snapshot"
//
void Editor::load_from_snapshot(EditorSnapshot && snap)
{
    m_vertices  = std::move(snap.vertices);
    m_paths     = std::move(snap.paths);
    m_points    = std::move(snap.points);
    m_lines     = std::move(snap.lines);
    m_sel       = std::move(snap.selection);
    //
    //
    //
    //  TODO:   grid, view, mode …
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

//  "save_worker"
//
void Editor::save_worker(EditorSnapshot snap, std::filesystem::path path)
{
    EditorState &       EState      = this->m_editor_S;
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
        {
            std::lock_guard     lk      (EState.m_task_mtx);
            EState.m_main_tasks.push_back( [this, res, path]
            {
                this->m_editor_S.m_io_busy      = false;
                this->m_editor_S.m_io_last      = res;
                this->m_editor_S.m_io_msg       = (res == IOResult::Ok)
                                                    ? "Saved to " + path.generic_string()
                                                    : "Save failed";
            } );
        }
    }
    
    
    //  2.  EVALUATE SUCCESS/FAILURE OF IO-OPERATION...
    {
        const char *    status      = this->ms_IORESULT_NAMES[ static_cast<size_t>( EState.m_io_last ) ];
    
        //  CASE 2A :   SAVE SUCCESS.
        if ( EState.m_io_last == IOResult::Ok ) {
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

//
// *************************************************************************** //
// *************************************************************************** //   END "SAVING".



// *************************************************************************** //
//
//              3.2.        LOADING:
// *************************************************************************** //
// *************************************************************************** //

//  "load_worker"
//
void Editor::load_worker(std::filesystem::path path)
{
    using                   Version     = cblib::SchemaVersion;
    EditorState &           EState      = this->m_editor_S;
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
        std::lock_guard     lk      (EState.m_task_mtx);
        
        EState.m_main_tasks.push_back( [this, res, snap = std::move(snap), path]() mutable
        {
            this->m_editor_S.m_io_busy    = false;

            //  CASE 2A :   LOADING SUCCESS.
            if ( res == IOResult::Ok )
            {
                load_from_snapshot( std::move(snap) );
                this->m_editor_S.m_io_msg = std::format( "loaded file \"{}\"", path.generic_string() );
            }
            //
            //  CASE 2B :   LOADING FAILURE.
            else
            {
                switch (res)
                {
                    case IOResult::IoError              : {     this->m_editor_S.m_io_msg = "load I/O error";                break;     }
                    case IOResult::ParseError           : {     this->m_editor_S.m_io_msg = "load parsing error";            break;     }
                    case IOResult::VersionMismatch      : {     this->m_editor_S.m_io_msg = "json version mismatch";         break;     }
                    default                             : {     this->m_editor_S.m_io_msg = "unknown load error";            break;     }
                }
            }
            this->m_editor_S.m_io_last = res;
            
            return;
        } );
    }
    
    
    return;
}


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
    }
    else
    {
        result              = false;
        ES.m_filepath       = std::filesystem::path{   };
        this->RESET_ALL();
    }
    
    
    return result;
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
