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
//                 PRIMARY TEMPLATE DECLARATION:
//          "C B _ B R O W S E R" CLASS IMPLEMENTATION...
// *************************************************************************** //
// *************************************************************************** //

inline constexpr float FILE_DIALOG_WIDTH_FRAC  = 0.6f;   // 60 % of viewport
inline constexpr float FILE_DIALOG_HEIGHT_FRAC = 0.8f;   // 60 % of viewport


class FileDialog {
public:
    enum class Type { Open, Save };

    explicit FileDialog(Type type = Type::Open) noexcept;
    ~FileDialog();

    void open(const std::filesystem::path& start_dir =
              std::filesystem::current_path()) noexcept;

    bool draw(const char* popup_id = "FileDialog");          // call every frame
    [[nodiscard]] bool is_open() const noexcept;
    [[nodiscard]] std::optional<std::filesystem::path> result() const noexcept;

    void set_filters(std::vector<std::string> patterns) noexcept; // {"*.json", …}

private:
    struct state_t;

    /* helpers defined in .cpp */
    static void refresh_listing(state_t& s);
    static void push_history(state_t& s, const std::filesystem::path& new_dir);
    static void draw_breadcrumb_bar(state_t& s);
    static void draw_nav_buttons(state_t& s);
    static void handle_keyboard_nav(state_t& s);
    static void draw_file_list(state_t& s, const char* list_id);
    static void draw_file_table(state_t& s, const char* table_id);   // NEW

    Type                         m_type;
    std::unique_ptr<state_t>     m_state;
};















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

