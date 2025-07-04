/***********************************************************************************
*
*       ********************************************************************
*       ****      F I L E _ D I A L O G U E . C P P  ____  F I L E      ****
*       ********************************************************************
*              AUTHOR:      Collin A. Bond
*               DATED:      July 1, 2025.
*
**************************************************************************************
**************************************************************************************/
#include "widgets/_file_dialogue.h"



namespace cb { //     BEGINNING NAMESPACE "cb"...
// *************************************************************************** //
// *************************************************************************** //



//      STATIC HELPERS...
// *************************************************************************** //
// *************************************************************************** //

//  "match_glob"
//
static bool match_glob(const char* pat, const char* txt)
{
    if (!pat) return true;
    while (*pat && *txt) {
        if (*pat == '*') {
            while (*pat == '*') ++pat;
            if (!*pat) return true;
            while (*txt)
                if (match_glob(pat, txt++)) return true;
            return false;
        }
        if (*pat != '?' && *pat != *txt) return false;
        ++pat; ++txt;
    }
    return (*pat == *txt) || (*pat == '*' && pat[1] == '\0');
}






// *************************************************************************** //
//
//
//
//      PUBLIC API...
// *************************************************************************** //
// *************************************************************************** //

//  "is_open"
//
bool FileDialog::is_open() const noexcept
{ return this->m_visible; }


//  "result"
//
std::optional<std::filesystem::path> FileDialog::result() const noexcept
{ return m_state->result; }


//  "set_filters"
//
void FileDialog::set_filters(std::vector<std::string> pat) noexcept
{
    m_state->filters = std::move(pat);
}


/// @fn         initialize(const Type mode, const Initializer & data) noexcept
/// @brief      Prime the dialog for this invocation. Must be called before Begin().
/// @param      mode                Open or Save behaviour.
/// @param      start_dir           Initial directory (defaults to cwd).
/// @param      default_filename    pre-filled filename (optional)
/// @param      force_ext           Mandatory extension such as ".json" (optional)
///
void FileDialog::initialize(const Type mode, const Initializer & data) noexcept
{
    State & s                   = *m_state;
    m_type                      = mode;
    m_prepared                  = (mode != Type::None);


    if (!m_prepared) return;    //  Return if INVALID state.  Begin() will fail ASSERTION.


    //  1.  CACHE THE VALUE OF "HOME" / "STARTING" DIRECTORY...
    s.home_dir                  = std::filesystem::exists(data.default_dir) ? data.default_dir : std::filesystem::current_path();
    s.history.clear();
    s.hist_index                = -1;
    push_history(s, s.home_dir);
    
    

    //  3.  EXISTING WORK...
    s.default_filename          = data.default_filename;                 //  NEW
    s.required_extension        = data.required_extension;
    s.valid_extensions          = data.valid_extensions;

    
    //  4.  PREPARE DIALOG MENU FOR OPENING...
    m_first_frame               = true;
    m_visible                   = true;

    return;
}






// *************************************************************************** //
//
//
//
//      MAIN MEMBER FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

//  Default Constructor.
//
FileDialog::FileDialog() noexcept
    : m_state{ std::make_unique<State>() }    { }


//  "Begin"
//
bool FileDialog::Begin(const char * popup_id)
{
    State &  s   = *m_state;
    
    //  0.  FIRST-FRAME INITIALIZATION  | Set size & position on the frame the popup appears...
    if (m_first_frame) {
        IM_ASSERT( this->m_prepared && "FileDialog::Begin() was invoked prior to being initialized with obj.initialize(mode, data)." );
        this->m_first_frame                 = false;
                            m_window_size   = this->get_window_size();
        ImGuiViewport *     vp              = ImGui::GetMainViewport();
        ImVec2              pos             = { vp->Pos.x + 0.5f * vp->Size.x,          vp->Pos.y + 0.5f * vp->Size.y };
        
        ImGui::SetNextWindowSize( m_window_size, ImGuiCond_Appearing );
        ImGui::SetNextWindowPos( pos, ImGuiCond_Appearing, FILE_DIALOG_REL_WINDOW_POSITION );
        ImGui::OpenPopup(popup_id);
    }
    
    //  CASE 0 :    EARLY-EXIT IF NO POPUP WINDOW...
    if ( !ImGui::BeginPopupModal(popup_id, nullptr, m_modal_flags) )
    { m_visible = false;    return false; }
    
    //  1.  AUTO-REFRESH IF EXTERNAL CHANGES...
    if ( std::filesystem::exists(s.cwd) &&
         std::filesystem::last_write_time(s.cwd) != s.cwd_timestamp )
        { refresh_listing(s); }



    //  2.  DRAWING MAIN FILE-DIALOG USER-INTERFACE...
    //
        //      2A.     DRAW TOP BAR...
        this->draw_top_bar(s);
        //
        //
        //      2B.     DRAW FILE NAVIGATOR TABLE...
        #ifdef _CB_FILEDIALOGUE_USE_FILE_LIST
            draw_file_list      (s,     "##FileDialog_List");
        #else
            draw_file_table     (s,     "##FileDialog_Table");
        #endif  //  _CB_FILEDIALOGUE_USE_FILE_LIST  //
        //
        //
        //      2C.     DRAW BOTTOM BAR...
        this->draw_bottom_bar(s);


    ImGui::EndPopup();
    return !m_visible;        // true on the frame it closes
}



// *************************************************************************** //
//
//
//
//      MEMBERS...
// *************************************************************************** //
// *************************************************************************** //

//  "draw_file_list"
//
inline void FileDialog::draw_file_list(State & s, const char* list_id)
{
    ImGui::BeginChild(list_id, ImVec2(0, 200), true, ImGuiWindowFlags_None); // ImGuiWindowFlags_NoScrollWithMouse);
    handle_keyboard_nav(s);


    if ( s.cwd.has_parent_path() && ImGui::Selectable(UPDIR_NAME, false) )      { push_history(s, s.cwd.parent_path()); }

    for ( auto & e : s.entries )
    {
        bool            is_dir      = e.is_directory();
        std::string     label       = is_dir ? DIRECTORY_PREFIX : FILENAME_PREFIX;
        label                      += e.path().filename().string();
        bool            sel         = s.current_selection && *s.current_selection == e.path();
        
        if ( ImGui::Selectable(label.c_str(), sel) )
        {
            if (is_dir)     { push_history(s, e.path()); }
            else {
                s.current_selection = e.path();
                s.default_filename = e.path().filename().string();
            }
        }
        if ( is_dir && ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0) )   { push_history(s, e.path()); }
    }
    
    ImGui::EndChild();
    return;
}


//  "draw_file_table"
//
inline void FileDialog::draw_file_table(State & s, const char * table_id)
{
    const float                 row_h           = ImGui::GetFrameHeightWithSpacing();
    const float                 bottom_h        = BOTTOM_HEIGHT * ( row_h ); // filename + buttons
    float                       avail_h         = m_window_size.y - bottom_h; //ImGui::GetContentRegionAvail().y - bottom_h;
    int                         row_idx         = 0;
    
    
    //  CASE 0 :    EARLY-OUT IF TABLE IS NOT CREATED...
    if ( !ImGui::BeginTable(table_id, 4, m_table_flags, ImVec2(0, avail_h)) ) return;


    ImGui::TableSetupScrollFreeze(0, 1);          // header row always visible
    ImGui::TableSetupColumn("Name",     ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_DefaultSort, 0.0f); //    , 0.55f);
    ImGui::TableSetupColumn("Type",     ImGuiTableColumnFlags_WidthFixed);  // ImGuiTableColumnFlags_WidthStretch,                                     0.15f);
    ImGui::TableSetupColumn("Size",     ImGuiTableColumnFlags_WidthFixed);  // ImGuiTableColumnFlags_WidthStretch,                                     0.15f);
    ImGui::TableSetupColumn("Modified", ImGuiTableColumnFlags_WidthFixed);  // ImGuiTableColumnFlags_WidthStretch,                                     0.15f);
    ImGui::TableHeadersRow();
    

    //  1.  UPDATE SORTING POLICY IF USER CLICKS ON HEADER ROW...
    if ( ImGuiTableSortSpecs * specs = ImGui::TableGetSortSpecs() )
    {
        if ( specs->SpecsDirty && specs->SpecsCount == 1 ) {
            s.sort_criterion       = specs->Specs[0].ColumnIndex;
            s.sort_order          = specs->Specs[0].SortDirection == ImGuiSortDirection_Ascending;
            specs->SpecsDirty   = false;

            auto key = [&](const std::filesystem::directory_entry & e)
            {
                switch (s.sort_criterion)
                {
                    case 0:     { return e.path().filename().string(); }
                    case 1:     { return std::to_string(e.is_directory() ? 0 : (uintmax_t)std::filesystem::file_size(e)); }
                    case 2:     { return e.is_directory() ? std::string( DIR_TITLE ) : e.path().extension().string(); }
                    default: {
                        return std::to_string(e.is_directory() ? 0 :
                                 (uintmax_t)std::chrono::duration_cast<std::chrono::seconds>(
                                     std::filesystem::last_write_time(e).time_since_epoch()).count());
                    }
                }
            };
            std::stable_sort(s.entries.begin(), s.entries.end(),
                [&](auto& a, auto& b) {
                    auto ka = key(a), kb = key(b);
                    return s.sort_order ? ka < kb : ka > kb;
                });
        }
    }
    
    
    //  2.  PRINTING EACH FILE IN THE NAVIGATOR...
    //
    //          2.1.    DISPLAY THE “..” PARENT DIRECTOR ACCESSOR.
    if ( s.cwd.has_parent_path() )
    {
        ImGui::TableNextRow(); ImGui::TableNextColumn();
        bool                sel_up          = false;            // never highlighted
        const std::string   parent_name     = std::string(UPDIR_NAME) + "##row" + std::to_string(row_idx++);
        if ( ImGui::Selectable(parent_name.c_str(), sel_up) )       { push_history(s, s.cwd.parent_path()); }   // jump up one level

        ImGui::TableNextColumn(); ImGui::TextUnformatted(""); // empty cells for the remaining columns
        ImGui::TableNextColumn(); ImGui::TextUnformatted("");
        ImGui::TableNextColumn(); ImGui::TextUnformatted("");
        ++row_idx;   // keep row IDs unique
    }
    //
    //          2.2.    PRINTING EACH ROW...
    for ( auto & e : s.entries )
    {
    
        ImGui::TableNextRow();
        ImGui::TableNextColumn();

        // build label with selectable id
        bool            is_dir          = e.is_directory();
        bool            clickable       = is_dir || is_valid_extension( s.valid_extensions, e.path() );
        std::string     disp_name       = is_dir ? DIRECTORY_PREFIX : FILENAME_PREFIX;
        disp_name                      += e.path().filename().string();
        bool            sel             = s.current_selection && *s.current_selection == e.path();
        
        //  0.  IF FILENAME IS *NOT* SELECTABLE, DISABLE THE FOLLOWING ROW...
        if ( !clickable )       { ImGui::BeginDisabled(); }
        //
        //
        //
            //  1.  FILENAME...
            if ( ImGui::Selectable((disp_name + "##row" + std::to_string(row_idx++)).c_str(), sel) )
            {
                if ( is_dir)    { push_history(s, e.path() ); }
                else {
                    s.current_selection = e.path();
                    s.default_filename = e.path().filename().string();
                }
            }


            //  2.  KIND...
            ImGui::TableNextColumn();
            ImGui::TextUnformatted(is_dir ? DIR_TITLE : e.path().extension().string().c_str());
            

            // 3.   FILE SIZE...
            ImGui::TableNextColumn();
            if (is_dir) {
                ImGui::TextUnformatted( EMPTY_STRING );
            }
            else {
                std::error_code     ec;
                std::uintmax_t      bytes = std::filesystem::file_size(e.path(), ec);
                ImGui::TextUnformatted(ec ? EMPTY_STRING : fmt_file_size(bytes).c_str());
            }


            //  4.  LAST MODIFIED...
            ImGui::TableNextColumn();
            std::error_code         ec;
            auto                    ftime   = std::filesystem::last_write_time(e.path(), ec);
            ImGui::TextUnformatted( ec ? ERROR_STRING : cblib::utl::format_file_time(ftime).c_str() );
        //
        //
        //
        if ( !clickable )       { ImGui::EndDisabled(); }   //  END DISABLED ROW...

    }

    ImGui::EndTable();
}



// *************************************************************************** //
//      TOP-BAR...
// *************************************************************************** //

//  "draw_top_bar"
//
inline void FileDialog::draw_top_bar(State & s)
{
    this->draw_breadcrumb_bar( s );
    ImGui::NewLine();
    this->draw_nav_buttons(s);
    ImGui::Separator();
    
    return;
}

//  "draw_breadcrumb_bar"
//
inline void FileDialog::draw_breadcrumb_bar(State & s)
{
    ImGuiStyle &            style           = ImGui::GetStyle();
    float                   avail_total     = 1.8 * this->m_window_size.x; // - 2 * style.WindowPadding.x;   // popup width

#ifdef _WIN32
    //  SHOW WINDOWS ("C:") DRIVE FIRST...
    if ( s.cwd.has_root_name() )
    {
        std::string     root_lbl    = s.cwd.root_name().string();
        if ( ImGui::SmallButton(root_lbl.c_str()) )     { push_history(s, s.cwd.root_name()); }
        ImGui::SameLine( 0.0f, 0.0f );      ImGui::TextUnformatted( BREADCRUMB_DELIM );     ImGui::SameLine( 0.0f, 0.0f );
    }
#endif  //  _WIN32  //


    std::filesystem::path   accum;
    float                   x_start     = ImGui::GetCursorPosX();
    float                   line_x      = x_start;

    for ( const auto & part_raw : s.cwd.relative_path() )
    {
        //  1.  TRUNCATE DIRECTORY-NAME IF TOO LONG...
        std::string     part    = part_raw.string();
        if ( (int)part.length() > BREADCRUMB_DIRNAME_LIMIT )
            { part = part.substr(0, BREADCRUMB_DIRNAME_LIMIT - TRUNCATION_CHAR_LENGTH) + TRUNCATION_CHAR; }


        //  2.  ESTIMATE WIDTH...
        float btn_w = ImGui::CalcTextSize(part.c_str()).x + 2.0f * ImGui::GetStyle().FramePadding.x;


        //  3.  WRAP IMGUI COLUM WIDGET TO NEXT-LINE (IF NEEDED)...
        if (line_x + btn_w > x_start + avail_total) {
            ImGui::NewLine();
            line_x = x_start;
        }


        //  4.  DRAW BUTTON FOR THE DIRECTORY...
        accum              /= part_raw;
        ImGui::PushID(part.c_str());
            bool    clicked     = ImGui::SmallButton( part.c_str() );
            if (clicked)        { push_history(s, s.cwd.root_path() / accum); }
        ImGui::PopID();

        line_x += btn_w;


        //  5.  DELIMETER CHARACTER BETWEEN DIRECTORY BUTTONS...
        ImGui::SameLine( 0.0f, 0.0f ); ImGui::TextUnformatted( BREADCRUMB_DELIM ); ImGui::SameLine( 0.0f, 0.0f );
        line_x += ImGui::CalcTextSize( BREADCRUMB_DELIM ).x + 2.0f * ImGui::GetStyle().ItemSpacing.x;
    }

    ImGui::NewLine();
}


//  "draw_nav_buttons"
//
inline void FileDialog::draw_nav_buttons(State & s)
{
    const bool      can_back        = s.hist_index > 0;
    const bool      can_fwd         = s.hist_index + 1 < (int)s.history.size();
    const bool      can_home        = (s.cwd != s.home_dir);
    const bool      can_desktop     = (!s.desktop_dir.empty() && s.cwd != s.desktop_dir);


    //  1.  BACK BUTTON...
    if ( !can_back )                                                    { ImGui::BeginDisabled(); }
    if ( ImGui::ArrowButton("##FileDialog_Back", ImGuiDir_Left) )       { push_history(s, s.history[--s.hist_index]); }
    if ( !can_back )                                                    { ImGui::EndDisabled(); }

    ImGui::SameLine(0.0f, 0.0f);

    //  2.  BACK BUTTON...
    if ( !can_fwd )                                                     { ImGui::BeginDisabled(); }
    if ( ImGui::ArrowButton("##FileDialog_Fwd", ImGuiDir_Right) )       { push_history(s, s.history[++s.hist_index]); }
    if ( !can_fwd )                                                     { ImGui::EndDisabled(); }
    
    
    
    ImGui::SameLine();
    
    
    
    //  3.  HOME BUTTON...
    if ( !can_home )                                                    { ImGui::BeginDisabled(); }
    if ( ImGui::Button("Home") )                                        { push_history(s, s.home_dir); }
    if ( !can_home )                                                    { ImGui::EndDisabled(); }

    ImGui::SameLine();

    //  4.  DESKTOP BUTTON...
    if ( !can_desktop )                                                 { ImGui::BeginDisabled(); }
    if ( ImGui::Button("Desktop") )                                     { push_history(s, s.desktop_dir); }
    if ( !can_desktop )                                                 { ImGui::EndDisabled(); }

    ImGui::SameLine();
    
    //  5.  SHOW HIDDEN FILES..
    if ( ImGui::Checkbox("Hidden Files", &m_show_hidden) )              { this->refresh_listing(s); } 
    
    
    
    return;
}



// *************************************************************************** //
//      BOTTOM-BAR...
// *************************************************************************** //

//  "draw_bottom_bar"
//
inline void FileDialog::draw_bottom_bar(State & s)
{
    const float     ROW_HEIGHT      = ImGui::GetFrameHeightWithSpacing();
    const ImVec2    BUTTON_SIZE     = ImVec2(1.8f, 0.0f) * ImGui::CalcTextSize("Cancel");
    bool            ok              = false;
    bool            cancel          = false;
    
    
    //  left-aligned label helper assumed to exist in your utils
    //
    //ImGui::Dummy( ImGui::GetContentRegionAvail() * ImVec2(0.0f, 0.25f) );
    ImGui::NewLine();
    
    
    utl::LeftLabelSimple("Filename:");
    ImGui::SetNextItemWidth(-1.0f);
    ImGui::InputTextWithHint("##FileDialog_Filename", "filename", &s.default_filename);



    ImGui::Indent();
    ok          = ImGui::Button(m_type == Type::Save ? "Save" : "Open", BUTTON_SIZE);   ImGui::SameLine();
    cancel      = ImGui::Button("Cancel", BUTTON_SIZE);
    


    //  ACCEPTING / CANCELLING THE OPERATION...
    if ( ok && !s.default_filename.empty() )    { this->on_accept(); }
    if ( cancel )                       { this->on_cancel(); }
    
    
    
    return;
}

    
//  "on_accept"
//
inline void FileDialog::on_accept(void)
{
    auto &                  s           = *m_state;
    std::filesystem::path   fname       = s.default_filename;
    auto                    ieq         = [](std::string a, std::string b) {
        std::transform(a.begin(), a.end(), a.begin(), ::tolower);
        std::transform(b.begin(), b.end(), b.begin(), ::tolower);
        return a == b;
    };

    if ( s.required_extension.empty() ) {
        std::string     want        = s.required_extension;
        std::string     ext         = fname.extension().string();
        if ( ext.empty() || !ieq(ext, want) )    { fname.replace_extension(want); }
    }

    s.result    = s.cwd / fname;
    m_visible   = false;
    ImGui::CloseCurrentPopup();
    return;
}


//  "on_cancel"
//
inline void FileDialog::on_cancel(void)
{
    auto &                  s       = *m_state;
    s.result.reset();
    m_visible = false;
    ImGui::CloseCurrentPopup();
    return;
}





// *************************************************************************** //
//
//
//      TERTIARY FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

//  "refresh_listing"
//
inline void FileDialog::refresh_listing(State & s)
{
    s.entries.clear();
    
    auto        accept      = [&](const std::filesystem::directory_entry & e) {
        const std::string name = e.path().filename().string();

        if (!m_show_hidden && !name.empty() && name[0] == '.')  { return; }     //  1.  Skip Hidden-Files if toggle is OFF...

        if ( e.is_directory() )     {  s.entries.push_back(e); return; }        //  2.  Directory rows always accepted...
    

        if ( s.filters.empty() )    { s.entries.push_back(e); return; }         //  3.  Apply extension filters if any...
        for (auto & f : s.filters) {
            if ( match_glob(f.c_str(), name.c_str()) )      { s.entries.push_back(e); break; }
        }
        return;
    };


    for ( auto & e : std::filesystem::directory_iterator(s.cwd) )   { accept(e); }

    std::sort(s.entries.begin(), s.entries.end(), [](auto& a, auto& b) {
                  bool ad = a.is_directory(),     bd = b.is_directory();
                  if (ad != bd)     { return ad; }                       // dirs first
                  return a.path().filename() < b.path().filename();
              });

    s.cwd_timestamp             = std::filesystem::last_write_time(s.cwd);
    return;
}


//  "handle_keyboard_nav"
//
inline void FileDialog::handle_keyboard_nav(State & s)
{
    if ( !ImGui::IsWindowFocused() )    { return; }

    int         move                    = (ImGui::IsKeyPressed(ImGuiKey_UpArrow)   ? -1 : (ImGui::IsKeyPressed(ImGuiKey_DownArrow) ? +1 : 0));
    int         idx                     = -1;
    auto        is_row_selectable       = [&](size_t i) {
        if (i >= s.entries.size()) return false;
        const auto & ent = s.entries[i];
        return ent.is_directory() || is_valid_extension(s.valid_extensions, ent.path());
    };

    //  1.  NAVIGATE WITH ARROW KEYS...
    while ( move ) {
        int next = std::clamp(idx + move, 0, int(s.entries.size()) - 1);
        if (is_row_selectable(next)) { idx = next; break; }
        if (next == idx) break;      // no selectable rows in that direction
        idx = next;
    }

    //  2.  ACCEPT WITH ENTER KEY...
    if (ImGui::IsKeyPressed(ImGuiKey_Enter) && s.current_selection) {
        if (std::filesystem::is_directory(*s.current_selection))
            push_history(s, *s.current_selection);
        else {
            s.result  = *s.current_selection;
            m_visible = false;
            ImGui::CloseCurrentPopup();
        }
    }

    //  3.  CLOSE DIALOG WITH ESCAPE KEY...
    if ( ImGui::IsKeyPressed(ImGuiKey_Escape ) ) {
        this->on_cancel();
    }
    
    
    return;
}






// *************************************************************************** //
//
//
//
//      UTILITY MEMBERS...
// *************************************************************************** //
// *************************************************************************** //

//  "push_history"
//
inline void FileDialog::push_history(State& s, const std::filesystem::path& new_dir)
{
    if (new_dir == s.cwd) return;

    if (s.hist_index + 1 < (int)s.history.size())
        s.history.erase(s.history.begin() + s.hist_index + 1, s.history.end());

    s.history.push_back(new_dir);
    ++s.hist_index;

    s.cwd = new_dir;
    FileDialog::refresh_listing(s);
    s.current_selection.reset();
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
