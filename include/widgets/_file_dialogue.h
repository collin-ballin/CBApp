/***********************************************************************************
*
*       ********************************************************************
*       ****       _ F I L E _ D I A L O G U E . H  ____  F I L E       ****
*       ********************************************************************
*              AUTHOR:      Collin A. Bond
*               DATED:      July 1, 2025.
*
**************************************************************************************
**************************************************************************************/
#ifndef _CBAPP_WIDGETS_FILE_DIALOGUE_H
#define _CBAPP_WIDGETS_FILE_DIALOGUE_H  1



//  1.  INCLUDES    | Headers, Modules, etc...
// *************************************************************************** //
// *************************************************************************** //

//  0.1.        ** MY **  HEADERS...
#include CBAPP_USER_CONFIG
#include "cblib.h"
#include "utility/utility.h"


//  0.2     STANDARD LIBRARY HEADERS...
#include <iostream>         //  <======| std::cout, std::cerr, std::endl, ...
#include <cstdlib>          // C-Headers...
#include <cstring>
#include <stdio.h>
#include <tuple>

#include <string>           //  <======| std::string, ...
#include <string_view>
#include <vector>           //  <======| std::vector, ...
#include <stdexcept>        //  <======| ...
#include <limits.h>

#include <filesystem>
#include <optional>
#include <algorithm>
#include <memory>


//  0.3     "DEAR IMGUI" HEADERS...
#include "imgui.h"
#include "imgui_stdlib.h"
#include "imgui_internal.h"
#include "implot.h"



namespace cb { //     BEGINNING NAMESPACE "cb"...
// *************************************************************************** //
// *************************************************************************** //



// *************************************************************************** //
//      CONSTANTS...
// *************************************************************************** //

#if defined(_WIN32) || defined(MINGW)
    inline static constexpr const char *    BREADCRUMB_DELIM                    = "\\";
# else
    inline static constexpr const char *    BREADCRUMB_DELIM                    = "/";
#endif  //  defined(_WIN32) || defined(MINGW)  //



inline static constexpr ImVec2              OPENMODE_ABS_WINDOW_SIZE            = ImVec2(702.0f,    481.2f);       //    ImVec2(585.0f,    401.0f);        //  Pop-up Window Size          [ ABSOLUTE Dimensions ].
inline static constexpr ImVec2              SAVEMODE_ABS_WINDOW_SIZE            = ImVec2(969.6f,    549.6f);       //    ImVec2(808.0f,    458.0f);        //  Pop-up Window Size          [ ABSOLUTE Dimensions ].
inline static constexpr ImVec2              FILE_DIALOG_REL_WINDOW_SIZE         = ImVec2(0.5f,      0.5f);          //  Pop-up Window Size          [ Relative to main viewport ].
//
inline static constexpr ImVec2              FILE_DIALOG_REL_WINDOW_POSITION     = ImVec2(0.5f,      2.0f/3.0f);     //  Pop-up Window Postion       [ Relative to main viewport ].
//
inline static constexpr float               BOTTOM_HEIGHT                       = 7.0f;                             //  RESERVED SPACE AT BOTTOM    [ multiples of row height ].
//
//
//
inline static constexpr const char *        EMPTY_STRING                        = "--";                         //  Used for empty fields in File Dialog browser.
inline static constexpr const char *        ERROR_STRING                        = "???";                        //  Used if "std::error_code" for file operation.
inline static constexpr const char *        UPDIR_NAME                          = "..";                         //  Name for "cd .." (MOVE OUT OF CWD).
inline static constexpr const char *        TRUNCATION_CHAR                     = "...";                        //  Characters appended after TRUNCATION occurs.
inline static constexpr int                 TRUNCATION_CHAR_LENGTH              = 3;                            //  = strnlen( TRUNCATION_CHAR );

inline static constexpr const char *        DIRECTORY_PREFIX                    = "";                           //  "[D] "
inline static constexpr const char *        FILENAME_PREFIX                     = "    ";                       //  "    "
inline static constexpr const char *        DIR_TITLE                           = "Folder";
inline static constexpr const char *        DATETIME_FMT                        = "%Y-%m-%d %H:%M:%S";          //  Format of the "date modified" for each file.
//
inline static constexpr int                 BREADCRUMB_DIRNAME_LIMIT            = 10;



//  "FileDialogType"
//      Enum to define different TYPES/BEHAVIORS of File Dialog menus.
//
enum class FileDialogType : uint8_t {
    None, Open, Save, COUNT
};

//  "FileDialogSortingCriteria"
//      Enum to define which parameter we sort the file entries by.
//
enum class FileDialogSortingCriterion : uint8_t {
    Name, Size, Type, Time, COUNT
};

    
//  "Initializer_t"
//
struct Initializer_t {
    std::filesystem::path                           default_dir             = std::filesystem::current_path();
    std::string                                     default_filename;           // empty ⇒ none
    std::string                                     required_extension;         // ".json" etc.; empty ⇒ none
    std::vector<std::string>                        valid_extensions;           //  What file extensions the user is able to select.
};



//  "FileDialogState_t"
//
struct FileDialogState_t {
//
//
//  TARGET FILE DATA:
    std::string                                     default_filename;
    std::string                                     required_extension;                 // File-Extension that will be appended by FORCE (whether or not the user types it).
    std::optional<std::filesystem::path>            result;
//
//
//  NAVIGATOR / BROWSER DATA:
    std::filesystem::path                           cwd                     = std::filesystem::current_path();
    std::vector<std::filesystem::directory_entry>   entries;
    std::optional<std::filesystem::path>            current_selection;
//
//
//  FILE BROWSER SELECTION DATA:
    int                                             hist_index              = -1;
    std::vector<std::filesystem::path>              history;
    std::vector<std::string>                        filters;
    std::vector<std::string>                        valid_extensions;                   //  Which file extensions the user will be able to select.
    int                                             sort_criterion          = 0;        //  0-Name,      1-Size,     2-Type,     3-Time
    bool                                            sort_order              = true;     //  (T): Sort in ASCENDING Order.  (F): DESCENDING Order.
//
//
//  CACHED DATA:
    std::filesystem::path                           home_dir;                           //  Ref. to original default directory.
    std::filesystem::path                           desktop_dir;                        //  Ref. to user's Desktop folder.
    std::filesystem::file_time_type                 cwd_timestamp{};                    //  Track last modification of CWD to refresh if contents change while browsing.
//
//
//  MUTABLE OBJECT STATE / BEHAVIORS:
};



// *************************************************************************** //
//                 PRIMARY TEMPLATE DECLARATION:
//          "C B _ B R O W S E R" CLASS IMPLEMENTATION...
// *************************************************************************** //
// *************************************************************************** //

class FileDialog {
public:
    using                               Type                        = FileDialogType;
    using                               Initializer                 = Initializer_t;
    using                               Sort                        = FileDialogSortingCriterion;
    using                               State                       = FileDialogState_t;
//
//  CBAPP_APPSTATE_ALIAS_API        //  CLASS-DEFINED, NESTED TYPENAME ALIASES.
//
//
// *************************************************************************** //
// *************************************************************************** //
public:
    //  1.              INITIALIZATION METHODS...
    // *************************************************************************** //
                                        FileDialog                  (void) noexcept;
                                        ~FileDialog                 (void)  = default;
                                    
                                    
    //  2.              PUBLIC MEMBER FUNCTIONS...
    // *************************************************************************** //
    //
    //                                  MAIN PUBLIC API:
    bool                                Begin                       (const char* popup_id = "File Dialog");          // call every frame
    void                                initialize                  (const Type , const Initializer & ) noexcept;
    //
    //                                  PUBLIC UTILITIES:
    [[nodiscard]] bool                  is_open                     (void) const noexcept;
    void                                set_filters                 (std::vector<std::string> patterns) noexcept; // {"*.json", …}
    [[nodiscard]] std::optional<std::filesystem::path>
                                        result                      (void) const noexcept;



// *************************************************************************** //
// *************************************************************************** //
private:
    //
    //  2.A             PROTECTED OPERATION MEMBER FUNCTIONS...
    // *************************************************************************** //
    // *************************************************************************** //
    
    
    // *************************************************************************** //
    //      MAIN PROTECTED MEMBER FUNCTIONS...
    // *************************************************************************** //
    inline void                         draw_file_list              (State & s, const char * list_id);
    inline void                         draw_file_table             (State & s, const char * table_id);   // NEW
    //
    inline void                         draw_top_bar                (State & s);      //  TOP BAR.
    inline void                         draw_nav_buttons            (State & s);
    inline void                         draw_breadcrumb_bar         (State & s);
    //
    inline void                         draw_bottom_bar             (State & s);      //  BOTTOM BAR.
    inline void                         on_accept                   (void);
    inline void                         on_cancel                   (void);
    // *************************************************************************** //
    //
    //
    //
    // *************************************************************************** //
    //      TERTIARY MEMBER FUNCTIONS...
    // *************************************************************************** //
    inline void                         refresh_listing             (State & s);
    inline void                         push_history                (State & s, const std::filesystem::path & new_dir);
    inline void                         handle_keyboard_nav         (State & s);
    // *************************************************************************** //
    
    
    // *************************************************************************** //
    //      INLINE MEMBER FUNCTIONS...
    // *************************************************************************** //
    
    //  "is_valid_extension"
    [[nodiscard]] static inline bool    is_valid_extension          (const std::vector<std::string> & wl, const std::filesystem::path & p) {
        if (wl.empty() || !p.has_extension()) return true;
        std::string ext = p.extension().string();
        std::transform(ext.begin(), ext.end(), ext.begin(),     ::tolower);
        for (auto a : wl) {
            std::transform(a.begin(), a.end(), a.begin(),       ::tolower);
            if (ext == a) return true;
        }
        return false;
    }
    
    //  "get_window_size"
    [[nodiscard]] inline ImVec2         get_window_size             (void)
    {
        ImGuiViewport *     vp          = ImGui::GetMainViewport();
        ImVec2              size        = vp->Size * FILE_DIALOG_REL_WINDOW_SIZE;
        
        switch (this->m_type) {
            case Type::Open :   { size = OPENMODE_ABS_WINDOW_SIZE; break; }     //  1.  OPEN-MODE Dimensions...
            case Type::Save :   { size = SAVEMODE_ABS_WINDOW_SIZE; break; }     //  2.  SAVE-MODE Dimensions...
            default :           { break; }                                      //  DEFAULT: Use RELATIVE Dimensions...
        }
        
        return size;
    }
    
    //  "fmt_file_size"
    [[nodiscard]] inline std::string    fmt_file_size               (const std::uintmax_t bytes) {
        static constexpr const char *   units[]         = { "B", "KB", "MB", "GB", "TB" };
        static constexpr size_t         SIZE            = 64;
        //
        char                            buf[SIZE]       = {};
        double                          value           = static_cast<double>(bytes);
        int                             idx             = 0;
        
        while ( value >= 1024.0 && idx < 4 )            { value /= 1024.0; ++idx; }
        std::snprintf(buf, sizeof(buf), (idx == 0) ? "%.0f %s" : "%.1f %s", value, units[idx]);
        return buf;
    };
    
    
    // *************************************************************************** //
    // *************************************************************************** //
    


    // *************************************************************************** //
    //
    //
    //  3.              PROTECTED DATA-MEMBERS...
    // *************************************************************************** //
    // *************************************************************************** //
    std::unique_ptr<State>          m_state                 = {};
    Type                            m_type                  = Type::None;
//
    bool                            m_prepared              = false;
    bool                            m_visible               = false;
    bool                            m_first_frame           = false;
    bool                            m_show_hidden           = false;     //  (T): Show Hidden-Files that have the form ".filename.txt".  (F): Hide these.
//
    ImVec2                          m_window_size           = {};
    ImGuiWindowFlags                m_modal_flags           = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoSavedSettings;
    ImGuiTableFlags                 m_table_flags           = ImGuiTableFlags_RowBg | ImGuiTableFlags_Sortable | ImGuiTableFlags_Resizable | ImGuiTableFlags_ScrollY | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV;



    // *************************************************************************** //
    //
    //
    //  4.              STATIC CONSTEXPR DATA...
    // *************************************************************************** //
    // *************************************************************************** //
    
    
    
// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
};//    END "FileDialog" CLASS DEFINITION.















// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
}//   END OF "cb" NAMESPACE.






#endif      //  _CBAPP_WIDGETS_FILE_DIALOGUE_H  //
// *************************************************************************** //
// *************************************************************************** //
//
//  END.

