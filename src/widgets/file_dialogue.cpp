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



//      TYPES...
// *************************************************************************** //
// *************************************************************************** //

// ────────────────────────── internal state
struct FileDialog::state_t {
    std::filesystem::path                           cwd             = std::filesystem::current_path();
    std::string                                     name_buf;
    std::vector<std::filesystem::directory_entry>   entries;
    std::optional<std::filesystem::path>            selected;
    std::optional<std::filesystem::path>            result;
    std::vector<std::string>                        filters;
    std::vector<std::filesystem::path>              history;
    int                                             hist_index      = -1;
    std::filesystem::file_time_type                 dir_stamp{};
    bool                                            open_popup      = false;
    bool                                            visible         = false;
    //
    int                                             sort_column     = 0;        // 0-Name,1-Size,2-Type,3-Time
    bool                                            sort_asc        = true;     // toggle on header click
    //
    std::string                                     forced_ext;                 // ".json"  (empty => none)
    bool                                            enforce_ext     = false;
};






// *************************************************************************** //
//
//
//
//      PROTOTYPES...
// *************************************************************************** //
// *************************************************************************** //









// *************************************************************************** //
//
//
//
//      STATIC HELPERS...
// *************************************************************************** //
// *************************************************************************** //

// ────────────────────────── tiny glob (* ? )
// very small matcher good enough for "*.ext"
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
{ return m_state->visible; }


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


//  "open"
//
void FileDialog::open(const std::filesystem::path & start_dir,
                      std::string_view  default_name,
                      std::string_view  force_ext) noexcept
{
    auto& s = *m_state;
    /* existing dir/history init … */

    s.name_buf   = default_name;            // NEW
    s.forced_ext = std::string(force_ext);
    s.enforce_ext = !s.forced_ext.empty();

    s.open_popup = true;
    s.visible    = true;
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
FileDialog::FileDialog(Type type) noexcept
    :   m_type{ type }, m_state{ std::make_unique<state_t>() } { }


//  Default Destructor.
//
FileDialog::~FileDialog() = default;


//  "draw"
//
bool FileDialog::draw(const char * popup_id)
{
    auto &  s   = *m_state;
    
    //  0.  INITIALIZATION  | Set size & position on the frame the popup appears...
    if (s.open_popup) {
                            m_window_size   = this->get_window_size();
        ImGuiViewport *     vp              = ImGui::GetMainViewport();
        ImVec2              pos             = { vp->Pos.x + 0.5f * vp->Size.x,          vp->Pos.y + 0.5f * vp->Size.y };
        
        ImGui::SetNextWindowSize( m_window_size, ImGuiCond_Appearing );
        ImGui::SetNextWindowPos( pos, ImGuiCond_Appearing, FILE_DIALOG_REL_WINDOW_POSITION );
        ImGui::OpenPopup(popup_id);
        s.open_popup = false;
    }
    
    //  CASE 0 :    EARLY-EXIT IF NO POPUP WINDOW...
    if ( !ImGui::BeginPopupModal(popup_id, nullptr, m_modal_flags) )
    { s.visible = false;    return false; }

    //  1.  AUTO-REFRESH IF EXTERNAL CHANGES...
    if ( std::filesystem::exists(s.cwd) &&
         std::filesystem::last_write_time(s.cwd) != s.dir_stamp )
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
    return !s.visible;        // true on the frame it closes
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
inline void FileDialog::draw_file_list(state_t& s, const char* list_id)
{
    ImGui::BeginChild(list_id, ImVec2(0, 200), true, ImGuiWindowFlags_None); // ImGuiWindowFlags_NoScrollWithMouse);
    handle_keyboard_nav(s);


    if ( s.cwd.has_parent_path() && ImGui::Selectable(UPDIR_NAME, false) )      { push_history(s, s.cwd.parent_path()); }

    for ( auto & e : s.entries )
    {
        bool            is_dir      = e.is_directory();
        std::string     label       = is_dir ? DIRECTORY_PREFIX : FILENAME_PREFIX;
        label                      += e.path().filename().string();
        bool            sel         = s.selected && *s.selected == e.path();
        
        if ( ImGui::Selectable(label.c_str(), sel) )
        {
            if (is_dir)     { push_history(s, e.path()); }
            else {
                s.selected = e.path();
                s.name_buf = e.path().filename().string();
            }
        }
        if ( is_dir && ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0) )   { push_history(s, e.path()); }
    }
    
    ImGui::EndChild();
    return;
}


//  "draw_file_table"
//
inline void FileDialog::draw_file_table(state_t & s, const char * table_id)
{
    const float                 row_h           = ImGui::GetFrameHeightWithSpacing();
    const float                 bottom_h        = BOTTOM_HEIGHT * ( row_h ); // filename + buttons
    float                       avail_h         = m_window_size.y - bottom_h; //ImGui::GetContentRegionAvail().y - bottom_h;
    int                         row_idx         = 0;
    
    
    //  "fmt_size"
    auto                        fmt_size        = [](std::uintmax_t bytes) -> std::string {
        char            buf[32];
        const char *    units[]     = { "B", "KB", "MB", "GB", "TB" };
        double          value       = static_cast<double>(bytes);
        int             idx         = 0;
        while ( value >= 1024.0 && idx < 4 )    { value /= 1024.0; ++idx; }
        std::snprintf(buf, sizeof(buf), (idx == 0) ? "%.0f %s" : "%.1f %s", value, units[idx]);
        return buf;
    };
    
    
    
    //  CASE 0 :    EARLY-OUT IF TABLE IS NOT CREATED...
    if ( !ImGui::BeginTable(table_id, 4, m_table_flags, ImVec2(0, avail_h)) ) return;


    ImGui::TableSetupScrollFreeze(0, 1);          // header row always visible
    ImGui::TableSetupColumn("Name",     ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_DefaultSort, 0.0f);
    ImGui::TableSetupColumn("Type",     ImGuiTableColumnFlags_WidthStretch,                                     0.15f);
    ImGui::TableSetupColumn("Size",     ImGuiTableColumnFlags_WidthStretch,                                     0.15f);
    ImGui::TableSetupColumn("Modified", ImGuiTableColumnFlags_WidthStretch,                                     0.15f);
    ImGui::TableHeadersRow();
    

    //  1.  UPDATE SORTING POLICY IF USER CLICKS ON HEADER ROW...
    if ( ImGuiTableSortSpecs * specs = ImGui::TableGetSortSpecs() )
    {
        if ( specs->SpecsDirty && specs->SpecsCount == 1 ) {
            s.sort_column       = specs->Specs[0].ColumnIndex;
            s.sort_asc          = specs->Specs[0].SortDirection == ImGuiSortDirection_Ascending;
            specs->SpecsDirty   = false;

            auto key = [&](const std::filesystem::directory_entry & e)
            {
                switch (s.sort_column)
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
                    return s.sort_asc ? ka < kb : ka > kb;
                });
        }
    }
    
    
    //  2.  PRINTING EACH ROW...
    for ( auto & e : s.entries )
    {
    
        ImGui::TableNextRow();
        ImGui::TableNextColumn();

        // build label with selectable id
        bool            is_dir      = e.is_directory();
        std::string     disp_name   = is_dir ? DIRECTORY_PREFIX : FILENAME_PREFIX;
        disp_name                  += e.path().filename().string();
        bool            sel         = s.selected && *s.selected == e.path();
        
        
        //  1.  FILENAME...
        if ( ImGui::Selectable((disp_name + "##row" + std::to_string(row_idx++)).c_str(), sel) )
        {
            if ( is_dir)    { push_history(s, e.path() ); }
            else {
                s.selected = e.path();
                s.name_buf = e.path().filename().string();
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
            ImGui::TextUnformatted(ec ? EMPTY_STRING : fmt_size(bytes).c_str());
        }


        //  4.  LAST MODIFIED...
        ImGui::TableNextColumn();
        std::error_code         ec;
        auto                    ftime = std::filesystem::last_write_time(e.path(), ec);
        if (!ec)
        {
            auto secs = std::chrono::duration_cast<std::chrono::seconds>(
                            ftime.time_since_epoch()).count();
            ImGui::Text("%lld", static_cast<long long>(secs));
        }
        else { ImGui::TextUnformatted(EMPTY_STRING); }
        
    }

    ImGui::EndTable();
}



// *************************************************************************** //
//      TOP-BAR...
// *************************************************************************** //

//  "draw_top_bar"
//
inline void FileDialog::draw_top_bar(state_t & s)
{
    this->draw_nav_buttons(s);
    this->draw_breadcrumb_bar( s );
    ImGui::Separator();
    
    return;
}

//  "draw_breadcrumb_bar"
//
inline void FileDialog::draw_breadcrumb_bar(state_t & s)
{
    ImGuiStyle &            style           = ImGui::GetStyle();
    float                   avail_total     = 1.5 * this->m_window_size.x; // - 2 * style.WindowPadding.x;   // popup width

#ifdef _WIN32
    //  SHOW WINDOWS ("C:") DRIVE FIRST...
    if ( s.cwd.has_root_name() )
    {
        std::string     root_lbl    = s.cwd.root_name().string();
        if ( ImGui::SmallButton(root_lbl.c_str()) )     { push_history(s, s.cwd.root_name()); }
        ImGui::SameLine();      ImGui::TextUnformatted( BREADCRUMB_DELIM );     ImGui::SameLine();
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
        float btn_w = ImGui::CalcTextSize(part.c_str()).x + ImGui::GetStyle().FramePadding.x * 2.0f;


        //  3.  WRAP IMGUI COLUM WIDGET TO NEXT-LINE (IF NEEDED)...
        if (line_x + btn_w > x_start + avail_total) {
            ImGui::NewLine();
            line_x = x_start;
        }


        //  4.  DRAW BUTTON FOR THE DIRECTORY...
        accum              /= part_raw;
        bool    clicked     = ImGui::SmallButton(part.c_str());
        if (clicked)        { push_history(s, s.cwd.root_path() / accum); }

        line_x += btn_w;


        //  5.  DELIMETER CHARACTER BETWEEN DIRECTORY BUTTONS...
        ImGui::SameLine( 0.0f, 0.0f ); ImGui::TextUnformatted( BREADCRUMB_DELIM ); ImGui::SameLine( 0.0f, 0.0f );
        line_x += ImGui::CalcTextSize( BREADCRUMB_DELIM ).x
                + ImGui::GetStyle().ItemSpacing.x * 2.0f;
    }

    ImGui::NewLine();
}


//  "draw_nav_buttons"
//
inline void FileDialog::draw_nav_buttons(state_t & s)
{
    const bool      can_back        = s.hist_index > 0;
    const bool      can_fwd         = s.hist_index + 1 < (int)s.history.size();

    if ( !can_back )                                                    { ImGui::BeginDisabled(); }
    if ( ImGui::ArrowButton("##FileDialog_Back", ImGuiDir_Left) )       { push_history(s, s.history[--s.hist_index]); }
    if ( !can_back )                                                    { ImGui::EndDisabled(); }

    ImGui::SameLine(0.0f, 0.0f);

    if ( !can_fwd )                                                     { ImGui::BeginDisabled(); }
    if ( ImGui::ArrowButton("##FileDialog_Fwd", ImGuiDir_Right) )       { push_history(s, s.history[++s.hist_index]); }
    if ( !can_fwd )                                                     { ImGui::EndDisabled(); }
    
    return;
}



// *************************************************************************** //
//      BOTTOM-BAR...
// *************************************************************************** //

//  "draw_bottom_bar"
//
inline void FileDialog::draw_bottom_bar(state_t & s)
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
    ImGui::InputTextWithHint("##FileDialog_Filename", "filename", &s.name_buf);



    ImGui::Indent();
    ok          = ImGui::Button(m_type == Type::Save ? "Save" : "Open", BUTTON_SIZE);   ImGui::SameLine();
    cancel      = ImGui::Button("Cancel", BUTTON_SIZE);
    


    //  ACCEPTING / CANCELLING THE OPERATION...
    if ( ok && !s.name_buf.empty() )    { this->on_accept(); }
    if ( cancel )                       { this->on_cancel(); }
    
    
    
    return;
}

    
//  "on_accept"
//
inline void FileDialog::on_accept(void)
{
    auto &                  s           = *m_state;
    std::filesystem::path   fname       = s.name_buf;
    auto                    ieq         = [](std::string a, std::string b) {
        std::transform(a.begin(), a.end(), a.begin(), ::tolower);
        std::transform(b.begin(), b.end(), b.begin(), ::tolower);
        return a == b;
    };

    if ( s.enforce_ext ) {
        std::string     want        = s.forced_ext;
        std::string     ext         = fname.extension().string();
        if ( ext.empty() || !ieq(ext, want) )    { fname.replace_extension(want); }
    }

    s.result    = s.cwd / fname;
    s.visible   = false;
    ImGui::CloseCurrentPopup();
    return;
}


//  "on_cancel"
//
inline void FileDialog::on_cancel(void)
{
    auto &                  s       = *m_state;
    s.result.reset();
    s.visible = false;
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
inline void FileDialog::refresh_listing(state_t& s)
{
    s.entries.clear();
    auto accept = [&](const std::filesystem::directory_entry& e)
    {
        if (e.is_directory() || s.filters.empty()) { s.entries.push_back(e); return; }

        const auto name = e.path().filename().string();
        for (auto& f : s.filters)
            if (match_glob(f.c_str(), name.c_str())) { s.entries.push_back(e); break; }
    };

    for (auto& e : std::filesystem::directory_iterator(s.cwd))
        accept(e);

    std::sort(s.entries.begin(), s.entries.end(),
              [](auto& a, auto& b) {
                  bool ad = a.is_directory(), bd = b.is_directory();
                  if (ad != bd) return ad;                       // dirs first
                  return a.path().filename() < b.path().filename();
              });

    s.dir_stamp = std::filesystem::last_write_time(s.cwd);
}


//  "handle_keyboard_nav"
//
inline void FileDialog::handle_keyboard_nav(state_t& s)
{
    if (!ImGui::IsWindowFocused()) return;

    int move = (ImGui::IsKeyPressed(ImGuiKey_UpArrow)   ? -1 :
               (ImGui::IsKeyPressed(ImGuiKey_DownArrow) ? +1 : 0));

    if (move && !s.entries.empty()) {
        int idx = -1;
        if (s.selected) {
            auto it = std::find_if(s.entries.begin(), s.entries.end(),
                [&](auto& e){ return e.path()==*s.selected; });
            idx = int(it - s.entries.begin());
        }
        idx = std::clamp(idx + move, 0, int(s.entries.size()) - 1);
        s.selected = s.entries[idx].path();
        s.name_buf = s.selected->filename().string();
    }

    if (ImGui::IsKeyPressed(ImGuiKey_Enter) && s.selected) {
        if (std::filesystem::is_directory(*s.selected))
            push_history(s, *s.selected);
        else {
            s.result  = *s.selected;
            s.visible = false;
            ImGui::CloseCurrentPopup();
        }
    }
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
inline void FileDialog::push_history(state_t& s, const std::filesystem::path& new_dir)
{
    if (new_dir == s.cwd) return;

    if (s.hist_index + 1 < (int)s.history.size())
        s.history.erase(s.history.begin() + s.hist_index + 1, s.history.end());

    s.history.push_back(new_dir);
    ++s.hist_index;

    s.cwd = new_dir;
    FileDialog::refresh_listing(s);
    s.selected.reset();
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
