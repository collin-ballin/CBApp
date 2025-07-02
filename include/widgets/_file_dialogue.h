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



inline static constexpr ImVec2              OPENMODE_ABS_WINDOW_SIZE            = ImVec2(585.0f,    401.0f);        //  Pop-up Window Size          [ ABSOLUTE Dimensions ].
inline static constexpr ImVec2              SAVEMODE_ABS_WINDOW_SIZE            = ImVec2(808.0f,    458.0f);        //  Pop-up Window Size          [ ABSOLUTE Dimensions ].
inline static constexpr ImVec2              FILE_DIALOG_REL_WINDOW_SIZE         = ImVec2(0.5f,      0.5f);          //  Pop-up Window Size          [ Relative to main viewport ].
//
inline static constexpr ImVec2              FILE_DIALOG_REL_WINDOW_POSITION     = ImVec2(0.5f,      2.0f/3.0f);     //  Pop-up Window Postion       [ Relative to main viewport ].
//
inline static constexpr float               BOTTOM_HEIGHT                       = 7.0f;                             //  RESERVED SPACE AT BOTTOM    [ multiples of row height ].
//
//
//
inline static constexpr const char *        EMPTY_STRING                        = "--";                         //  Used for empty fields in File Dialog browser.
inline static constexpr const char *        UPDIR_NAME                          = "..";                         //  Name for "cd .." (MOVE OUT OF CWD).
inline static constexpr const char *        TRUNCATION_CHAR                     = "...";                        //  Characters appended after TRUNCATION occurs.
inline static constexpr int                 TRUNCATION_CHAR_LENGTH              = 3;                            //  = strnlen( TRUNCATION_CHAR );

inline static constexpr const char *        DIRECTORY_PREFIX                    = "";                           //  "[D] "
inline static constexpr const char *        FILENAME_PREFIX                     = "    ";                       //  "    "
inline static constexpr const char *        DIR_TITLE                           = "Folder";
//
inline static constexpr int                 BREADCRUMB_DIRNAME_LIMIT            = 10;


//  "FileDialogType"
//      Enum to define different TYPES/BEHAVIORS of File Dialog menus.
//
enum class FileDialogType {
    Open, Save, COUNT
};



// *************************************************************************** //
//                 PRIMARY TEMPLATE DECLARATION:
//          "C B _ B R O W S E R" CLASS IMPLEMENTATION...
// *************************************************************************** //
// *************************************************************************** //

class FileDialog {
public:
    using                           Type                        = FileDialogType;
//
//  CBAPP_APPSTATE_ALIAS_API        //  CLASS-DEFINED, NESTED TYPENAME ALIASES.
//
//
// *************************************************************************** //
// *************************************************************************** //
public:
    //  1.              INITIALIZATION METHODS...
    // *************************************************************************** //
    explicit                        FileDialog                  (Type type = Type::Open) noexcept;
                                    ~FileDialog                 (void);
                                    
                                    
    //  2.              PUBLIC MEMBER FUNCTIONS...
    // *************************************************************************** //
    //
    //                              MAIN PUBLIC API:
    bool                            draw                        (const char* popup_id = "File Dialog");          // call every frame
    void                            open                        (
           const std::filesystem::path & start_dir       = std::filesystem::current_path(),
                        std::string_view default_name    = {},
                        std::string_view force_ext       = {}   ) noexcept;
    //
    //                              PUBLIC UTILITIES:
    [[nodiscard]] bool              is_open                     (void) const noexcept;
    void                            set_filters                 (std::vector<std::string> patterns) noexcept; // {"*.json", …}
    [[nodiscard]] std::optional<std::filesystem::path>
                                    result                      (void) const noexcept;



// *************************************************************************** //
// *************************************************************************** //
private:
    //
    //  2.A             PROTECTED OPERATION MEMBER FUNCTIONS...
    // *************************************************************************** //
    // *************************************************************************** //
    struct state_t;
    
    
    // *************************************************************************** //
    //      MAIN PROTECTED MEMBER FUNCTIONS...
    // *************************************************************************** //
    inline static void              draw_file_list              (state_t & s, const char * list_id);
    inline void                     draw_file_table             (state_t & s, const char * table_id);   // NEW
    //
    inline void                     draw_top_bar                (state_t & s);      //  TOP BAR.
    inline static void              draw_nav_buttons            (state_t & s);
    inline void                     draw_breadcrumb_bar         (state_t & s);
    //
    inline void                     draw_bottom_bar             (state_t & s);      //  BOTTOM BAR.
    inline void                     on_accept                   (void);
    inline void                     on_cancel                   (void);
    // *************************************************************************** //
    //
    //
    //
    // *************************************************************************** //
    //      TERTIARY MEMBER FUNCTIONS...
    // *************************************************************************** //
    inline static void              refresh_listing             (state_t & s);
    inline static void              push_history                (state_t & s, const std::filesystem::path & new_dir);
    inline static void              handle_keyboard_nav         (state_t & s);
    // *************************************************************************** //
    
    
    // *************************************************************************** //
    //      INLINE MEMBER FUNCTIONS...
    // *************************************************************************** //
    
    //  "get_window_size"
    [[nodiscard]] inline ImVec2     get_window_size             (void)
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
    
    
    // *************************************************************************** //
    // *************************************************************************** //
    


    // *************************************************************************** //
    //
    //
    //  3.              PROTECTED DATA-MEMBERS...
    // *************************************************************************** //
    // *************************************************************************** //
    Type                            m_type;
    std::unique_ptr<state_t>        m_state;
    //
    ImVec2                          m_window_size;
    ImGuiWindowFlags                m_modal_flags           = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoSavedSettings;
    ImGuiTableFlags                 m_table_flags           = ImGuiTableFlags_RowBg | ImGuiTableFlags_Sortable | ImGuiTableFlags_Resizable | ImGuiTableFlags_ScrollY | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV;


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

