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
    std::filesystem::path                         cwd = std::filesystem::current_path();
    std::string                                   name_buf;
    std::vector<std::filesystem::directory_entry> entries;
    std::optional<std::filesystem::path>          selected;
    std::optional<std::filesystem::path>          result;

    std::vector<std::string>                      filters;
    std::vector<std::filesystem::path>            history;
    int                                           hist_index = -1;

    std::filesystem::file_time_type               dir_stamp{};

    bool                                          open_popup = false;
    bool                                          visible    = false;
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
//      MAIN MEMBER FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

//  Default Constructor.
//
FileDialog::FileDialog(Type type) noexcept
:   m_type{type},
    m_state{std::make_unique<state_t>()}
{  }


//  Default Destructor.
//
FileDialog::~FileDialog() = default;


//  "draw"
//
bool FileDialog::draw(const char* popup_id)
{
    auto& s = *m_state;
    
    
    // Set size & position on the frame the popup appears
    if (s.open_popup) {
        ImGuiViewport* vp = ImGui::GetMainViewport();
        ImVec2 sz  = { vp->Size.x * FILE_DIALOG_WIDTH_FRAC,
                       vp->Size.y * FILE_DIALOG_HEIGHT_FRAC };
        ImGui::SetNextWindowSize(sz, ImGuiCond_Appearing);
        ImGui::SetNextWindowPos(
            ImVec2(vp->Pos.x + vp->Size.x * 0.5f,
                   vp->Pos.y + vp->Size.y * 0.5f),
            ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));     // pivot centre
        ImGui::OpenPopup(popup_id);
        s.open_popup = false;
    }
    
    
    
    if (!ImGui::BeginPopupModal(popup_id, nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        s.visible = false;
        return false;
    }

    // auto-refresh if external change
    if (std::filesystem::exists(s.cwd) &&
        std::filesystem::last_write_time(s.cwd) != s.dir_stamp)
        refresh_listing(s);

    draw_breadcrumb_bar(s);
    draw_nav_buttons(s);
    ImGui::Separator();


#ifdef _CB_FILEDIALOGUE_USE_FILE_LIST
    draw_file_list      (s,     "##FileDialog_List");
#else
    draw_file_table     (s,     "##FileDialog_Table");
#endif  //  _CB_FILEDIALOGUE_USE_FILE_LIST  //


    // left-aligned label helper assumed to exist in your utils
    utl::LeftLabelSimple("Filename:");
    ImGui::InputText("##FileDialog_Filename", &s.name_buf);

    bool ok     = ImGui::Button(m_type == Type::Save ? "Save" : "Open");
    ImGui::SameLine();
    bool cancel = ImGui::Button("Cancel");

    if (ok && !s.name_buf.empty()) {
        s.result  = s.cwd / s.name_buf;
        s.visible = false;
        ImGui::CloseCurrentPopup();
    }
    if (cancel) {
        s.result.reset();
        s.visible = false;
        ImGui::CloseCurrentPopup();
    }

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
void FileDialog::draw_file_list(state_t& s, const char* list_id)
{
    ImGui::BeginChild(list_id, ImVec2(0, 200), true, ImGuiWindowFlags_None); // ImGuiWindowFlags_NoScrollWithMouse);
    handle_keyboard_nav(s);

    if (s.cwd.has_parent_path() && ImGui::Selectable("..", false))
        push_history(s, s.cwd.parent_path());

    for (auto& e : s.entries) {
        bool is_dir = e.is_directory();
        std::string label = is_dir ? "[D] " : "    ";
        label += e.path().filename().string();

        bool sel = s.selected && *s.selected == e.path();
        if (ImGui::Selectable(label.c_str(), sel)) {
            if (is_dir)
                push_history(s, e.path());
            else {
                s.selected = e.path();
                s.name_buf = e.path().filename().string();
            }
        }
        if (is_dir && ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
            push_history(s, e.path());
    }
    ImGui::EndChild();
}


//  "draw_file_table"
//
void FileDialog::draw_file_table(state_t& s, const char* table_id)
{
    constexpr ImGuiTableFlags FLAGS =
        ImGuiTableFlags_RowBg | ImGuiTableFlags_Sortable |
        ImGuiTableFlags_Resizable | ImGuiTableFlags_ScrollY |
        ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV;

    if (!ImGui::BeginTable(table_id, 4, FLAGS, ImVec2(0, 200))) return;

    ImGui::TableSetupScrollFreeze(0, 1);          // header row always visible
    ImGui::TableSetupColumn("Name",     ImGuiTableColumnFlags_DefaultSort);
    ImGui::TableSetupColumn("Size");
    ImGui::TableSetupColumn("Type");
    ImGui::TableSetupColumn("Modified");
    ImGui::TableHeadersRow();

    /* ── update sort spec if user clicked header ───────────────────── */
    if (ImGuiTableSortSpecs* specs = ImGui::TableGetSortSpecs()) {
        if (specs->SpecsDirty && specs->SpecsCount == 1) {
            s.sort_column = specs->Specs[0].ColumnIndex;
            s.sort_asc    = specs->Specs[0].SortDirection == ImGuiSortDirection_Ascending;
            specs->SpecsDirty = false;

            auto key = [&](const std::filesystem::directory_entry& e) {
                switch (s.sort_column) {
                    case 0:  return e.path().filename().string();
                    case 1:  return std::to_string(e.is_directory() ? 0 :
                                 (uintmax_t)std::filesystem::file_size(e));
                    case 2:  return e.is_directory() ? std::string("[DIR]") :
                                 e.path().extension().string();
                    default: return std::to_string(e.is_directory() ? 0 :
                                 (uintmax_t)std::chrono::duration_cast<std::chrono::seconds>(
                                     std::filesystem::last_write_time(e).time_since_epoch()).count());
                }
            };
            std::stable_sort(s.entries.begin(), s.entries.end(),
                [&](auto& a, auto& b) {
                    auto ka = key(a), kb = key(b);
                    return s.sort_asc ? ka < kb : ka > kb;
                });
        }
    }

    /* ── rows ───────────────────────────────────────────────────────── */
    int row_idx = 0;
    for (auto& e : s.entries) {
        ImGui::TableNextRow();
        ImGui::TableNextColumn();

        // build label with selectable id
        bool is_dir = e.is_directory();
        std::string disp_name = is_dir ? "[D] " : "    ";
        disp_name += e.path().filename().string();
        bool sel = s.selected && *s.selected == e.path();
        if (ImGui::Selectable((disp_name + "##row" + std::to_string(row_idx++)).c_str(), sel))
        {
            if (is_dir) push_history(s, e.path());
            else {
                s.selected = e.path();
                s.name_buf = e.path().filename().string();
            }
        }

        ImGui::TableNextColumn();
        if (is_dir) ImGui::TextUnformatted("-");
        else ImGui::Text("%lld",
                static_cast<long long>(std::filesystem::file_size(e)));

        ImGui::TableNextColumn();
        ImGui::TextUnformatted(is_dir ? "[DIR]" : e.path().extension().string().c_str());

        ImGui::TableNextColumn();
        auto ftime = std::filesystem::last_write_time(e);
        auto secs  = std::chrono::time_point_cast<std::chrono::seconds>(ftime)
                       .time_since_epoch().count();
        ImGui::Text("%lld", static_cast<long long>(secs));
    }

    ImGui::EndTable();
}



//  "refresh_listing"
//
void FileDialog::refresh_listing(state_t& s)
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


//  "draw_breadcrumb_bar"
//
void FileDialog::draw_breadcrumb_bar(state_t& s)
{
#if defined(_WIN32)
    if (s.cwd.has_root_name()) {
        const auto root = s.cwd.root_name();
        if (ImGui::SmallButton(root.string().c_str()))
            push_history(s, root);
        ImGui::SameLine(); ImGui::TextUnformatted(">"); ImGui::SameLine();
    }
#endif
    std::filesystem::path accum;
    for (const auto& seg : s.cwd.relative_path()) {
        accum /= seg;
        bool click = ImGui::SmallButton(seg.string().c_str());
        if (click) push_history(s, s.cwd.root_path() / accum);
        ImGui::SameLine(); ImGui::TextUnformatted(">"); ImGui::SameLine();
    }
    ImGui::NewLine();
}


//  "handle_keyboard_nav"
//
void FileDialog::handle_keyboard_nav(state_t& s)
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


//  "draw_nav_buttons"
//
void FileDialog::draw_nav_buttons(state_t& s)
{
    bool can_back = s.hist_index > 0;
    bool can_fwd  = s.hist_index + 1 < (int)s.history.size();

    if (!can_back) ImGui::BeginDisabled();
    if (ImGui::ArrowButton("##FileDialog_Back", ImGuiDir_Left))
        push_history(s, s.history[--s.hist_index]);
    if (!can_back) ImGui::EndDisabled();

    ImGui::SameLine();

    if (!can_fwd) ImGui::BeginDisabled();
    if (ImGui::ArrowButton("##FileDialog_Fwd", ImGuiDir_Right))
        push_history(s, s.history[++s.hist_index]);
    if (!can_fwd) ImGui::EndDisabled();
}








// *************************************************************************** //
//
//
//
//      UTILITY MEMBERS...
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
void FileDialog::open(const std::filesystem::path& start_dir) noexcept
{
    auto& s = *m_state;
    s.cwd        = std::filesystem::exists(start_dir) ? start_dir
                                                      : std::filesystem::current_path();
    s.name_buf.clear();
    s.entries.clear();
    s.selected.reset();
    s.result.reset();

    s.history.clear();
    s.hist_index = -1;
    push_history(s, s.cwd);           // prime history with cwd

    s.open_popup = true;
    s.visible    = true;
}


//  "push_history"
//
void FileDialog::push_history(state_t& s, const std::filesystem::path& new_dir)
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
