/***********************************************************************************
*
*       ********************************************************************
*       ****              W I D G E T S . H  ____  F I L E              ****
*       ********************************************************************
*              AUTHOR:      Collin A. Bond
*               DATED:      April 26, 2025.
*
**************************************************************************************
**************************************************************************************/
#ifndef _CBAPP_WIDGETS_H
#define _CBAPP_WIDGETS_H  1



//  1.  INCLUDES    | Headers, Modules, etc...
// *************************************************************************** //
// *************************************************************************** //

//  0.1.        ** MY **  HEADERS...
#include CBAPP_USER_CONFIG
#include "cblib.h"
#include "widgets/_popups.h"
#include "widgets/_heatmap.h"
#include "widgets/_file_dialogue.h"
#include "widgets/editor/editor.h"
#include "widgets/functional_testing/functional_testing.h"
#include "utility/utility.h"


//  0.2     STANDARD LIBRARY HEADERS...
#include <iostream>         //  <======| std::cout, std::cerr, std::endl, ...
#include <cstdlib>          // C-Headers...
#include <stdio.h>
#include <unistd.h>
#include <random>
#include <tuple>
#include <utility>
#include <algorithm>

#include <string>           //  <======| std::string, ...
#include <string_view>
#include <vector>           //  <======| std::vector, ...
#include <stdexcept>        //  <======| ...
#include <limits.h>
#include <math.h>


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

enum class BrowserMode : uint8_t
{
    Flat = 0,   // simple list
    // Tree      // (reserved for future hierarchical support)
};



//  "CB_Browser"
//
template <typename T, typename Container = std::vector<T>>
class CB_Browser
{
// *************************************************************************** //
//
//
//    LOCAL, CLASS-DEFINED PUBLIC ALIAS DEFINITIONS ...
// *************************************************************************** //
// *************************************************************************** //
public:
    using                       Item                            = T;
    using                       DrawPropsFn                     = std::function<void(Item&)>;               // inspector
    using                       LabelProviderFn                 = std::function<std::string(const Item&)>;  // list text


// *************************************************************************** //
//
//
//    PROTECTED DATA-MEMBERS ...
// *************************************************************************** //
// *************************************************************************** //
protected:
    ImGuiTextFilter             m_filter                        = ImGuiTextFilter();
    Container*                  m_items                         = nullptr;      // not owned
    Item*                       m_selection                     = nullptr;      // pointer into container
    std::size_t                 m_selection_index               = 0;            // index into container

    DrawPropsFn                 m_draw_properties               = DrawPropsFn();
    LabelProviderFn             m_label_provider                = [](const Item & )     { return "<item>"; };

    float                       m_left_width                    = 200.0f;       // px
    BrowserMode                 m_mode                          = BrowserMode::Flat;


// *************************************************************************** //
//
//
//  1.      PUBLIC MEMBER FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

public:
//
//          1.1         Initialization Functions.
// *************************************************************************** //
inline                              CB_Browser                      (void)                      = default;
inline                              ~CB_Browser                     (void)                      = default;

//  Default Constructor
//
inline CB_Browser( Container& items,
                   DrawPropsFn draw_props,
                   LabelProviderFn label_provider,
                   BrowserMode mode = BrowserMode::Flat )
{
    set_items(items);
    set_property_drawer(std::move(draw_props));
    set_label_provider(std::move(label_provider));
    m_mode = mode;
}

    
    
// *************************************************************************** //
//
//          1.2         Public Operation Functions.
// *************************************************************************** //
// *************************************************************************** //

//  "draw"
//
inline void draw()
{
    if (!m_items) return;

    draw_collections_column();
    ImGui::SameLine();
    draw_inspector_column();
}

inline CB_Browser& set_items(Container& items)
{
    m_items = &items;
    if (!items.empty())
    {
        m_selection_index = 0;
        m_selection       = &(*m_items)[0];
    }
    return *this;
}
    
    
// *************************************************************************** //
//
//          1.3         Public Utility Functions.
// *************************************************************************** //
// *************************************************************************** //

inline CB_Browser &                 set_property_drawer             (DrawPropsFn fn)            { m_draw_properties = std::move(fn);   return *this; }
inline CB_Browser &                 set_label_provider              (LabelProviderFn f)         { m_label_provider  = std::move(f);    return *this; }
inline CB_Browser &                 set_left_column_width           (float px)                  { m_left_width      = px;              return *this; }
inline CB_Browser &                 set_mode                        (BrowserMode m)             { m_mode            = m;               return *this; }

[[nodiscard]] inline Item *         selected                        (void) const noexcept       { return m_selection; }
[[nodiscard]] inline std::size_t    selected_index                  (void) const noexcept       { return m_selection_index; }
    
    
// *************************************************************************** //
//
//          1.4         Overloaded Operators.
// *************************************************************************** //
// *************************************************************************** //
inline CB_Browser                                                   (const CB_Browser & )       = delete;
inline CB_Browser                                                   (CB_Browser && )            = delete;
inline CB_Browser &                 operator =                      (const CB_Browser &)        = delete;
inline CB_Browser &                 operator =                      (CB_Browser && )            = delete;









    
// *************************************************************************** //
//
//
//  2.      PROTECTED MEMBER FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

protected:
//
//          2.1         Protected Utility Functions.
// *************************************************************************** //
// *************************************************************************** //

//  "draw_collections_column"
//
inline void draw_collections_column()
{
    ImGui::BeginChild("##cb_col", ImVec2(m_left_width, 0),
                      ImGuiChildFlags_Borders | ImGuiChildFlags_ResizeX);

    // Filter
    ImGui::SetNextItemWidth(-FLT_MIN);
    if (ImGui::InputTextWithHint("##cb_filter", "filter",
                                 m_filter.InputBuf, IM_ARRAYSIZE(m_filter.InputBuf)))
        m_filter.Build();

    // Item list
    for (std::size_t i = 0; i < m_items->size(); ++i)
    {
        Item&        item   = (*m_items)[i];
        std::string  label  = m_label_provider(item);
        if (!m_filter.PassFilter(label.c_str()))
            continue;

        const bool selected = (i == m_selection_index);
        if (ImGui::Selectable(label.c_str(), selected))
        {
            m_selection_index = i;
            m_selection       = &item;
        }
    }

    ImGui::EndChild();
}


//  "draw_inspector_column"
//
inline void draw_inspector_column()
{
    ImGui::BeginChild("##cb_inspector", ImVec2(0, 0), ImGuiChildFlags_Borders);
    if (m_selection && m_draw_properties)
        m_draw_properties(*m_selection);
    ImGui::EndChild();
}


//  "draw_tree_node"
/*
inline void draw_tree_node(Node * node)
{
    ImGuiTreeNodeFlags      flags       = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanFullWidth;
    
    ImGui::TableNextRow();
    ImGui::TableNextColumn();
    ImGui::PushID(node->uid());
    
    if (node == m_selection)
        flags |= ImGuiTreeNodeFlags_Selected;
    if (!node->has_children())
        flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_Bullet;

    const bool              open        = ImGui::TreeNodeEx("", flags, "%s", node->name().c_str());
    if ( ImGui::IsItemClicked() )
        m_selection = node;

    if ( open && node->has_children() )
    {
        for ( Node * child : node->children() )
            draw_tree_node(child);
        ImGui::TreePop();
    }
    ImGui::PopID();
    return;
}*/





// *************************************************************************** //
// *************************************************************************** //
};//	END "CB_Browser" CLASS PROTOTYPE.





















// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
}//   END OF "cb" NAMESPACE.






#endif      //  _CBAPP_WIDGETS_H  //
// *************************************************************************** //
// *************************************************************************** //
//
//  END.

