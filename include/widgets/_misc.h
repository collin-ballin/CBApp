/***********************************************************************************
*
*       ********************************************************************
*       ****                _ M I S C . H  ____  F I L E                ****
*       ********************************************************************
*              AUTHOR:      Collin A. Bond.
*               DATED:      August 19, 2025.
*
**************************************************************************************
**************************************************************************************/
#ifndef _CBAPP_WIDGETS_MISC_H
#define _CBAPP_WIDGETS_MISC_H  1



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



namespace cb { namespace ui { //     BEGINNING NAMESPACE "cb::ui"...
// *************************************************************************** //
// *************************************************************************** //

// ImGuiColorEditFlags_None

//  "CustomColorEdit4"
//
inline bool CustomColorEdit4( const char * uuid, ImVec4 & color,
                              const ImGuiColorEditFlags flags = ImGuiColorEditFlags_AlphaPreviewHalf    | ImGuiColorEditFlags_AlphaBar  |
                                                                ImGuiColorEditFlags_PickerHueBar        | ImGuiColorEditFlags_Float     |
                                                                ImGuiColorEditFlags_NoInputs            | ImGuiColorEditFlags_NoLabel )
{
    bool    dirty       = false;
    dirty               = ImGui::ColorEdit4( uuid, reinterpret_cast<float*>( &color ), flags );
    
    
    
    return dirty;
}





//  "HexColor4"
//
inline bool HexColor4( const char * uuid, ImVec4 & color, const ImGuiColorEditFlags flags =
                            ImGuiColorEditFlags_Float                                                   |   //  DATA TYPE...
                            ImGuiColorEditFlags_DisplayHex                                              |   //  TEXT WIDGET DISPlAY...
                            ImGuiColorEditFlags_NoLabel             | ImGuiColorEditFlags_NoBorder      |   //  WIDGET APPEARANCE...
                            ImGuiColorEditFlags_AlphaPreviewHalf                                        |   //
                            ImGuiColorEditFlags_PickerHueBar        | ImGuiColorEditFlags_AlphaBar          //  PICKER POP-UP SETTINGS...
)
{
    bool    dirty       = false;
    dirty               = ImGui::ColorEdit4( uuid, reinterpret_cast<float*>( &color ), flags );
    
    
    
    return dirty;
}










/*
// Optional knobs for the popup
struct color_picker_opts {
    ImGuiColorEditFlags   flags             = ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreviewHalf;
    bool                  show_palette_btn  = true;          // show the "Palette" button next to the color button
    ImVec2                preview_size      = ImVec2(60, 40);
    int                   palette_cols      = 8;             // columns in the swatch grid
};

// Returns true if 'col' was modified.
inline bool color_button_with_picker(const char * label, ImVec4 & col, const color_picker_opts & opt = {})
{
    ImGui::PushID(label);                         // isolate all IDs under this control
    const ImGuiID picker_id = ImGui::GetID("picker");

    // One shared palette (like the demo). You can externalize this later if desired.
    static std::array<ImVec4, 32> palette{};
    static bool                   palette_init = false;
    if (!palette_init) {
        for (int n = 0; n < (int)palette.size(); ++n) {
            ImGui::ColorConvertHSVtoRGB(n / 31.0f, 0.8f, 0.8f, palette[n].x, palette[n].y, palette[n].z);
            palette[n].w = 1.0f;
        }
        palette_init = true;
    }

    // Per-control backup color (for the "Previous" swatch)
    static std::unordered_map<ImGuiID, ImVec4> backup_by_id;

    bool open_popup = ImGui::ColorButton("##btn", col, ImGuiColorEditFlags_NoTooltip);
    if (opt.show_palette_btn) {
        ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);
        open_popup |= ImGui::Button("Palette");
    }
    if (open_popup) {
        ImGui::OpenPopup("picker");
        backup_by_id[picker_id] = col;
    }

    bool changed = false;

    if (ImGui::BeginPopup("picker")) {
        ImGui::TextUnformatted("MY CUSTOM COLOR PICKER WITH AN AMAZING PALETTE!");
        ImGui::Separator();

        ImGuiColorEditFlags picker_flags =
            opt.flags | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview;

        changed |= ImGui::ColorPicker4("##picker", &col.x, picker_flags);
        ImGui::SameLine();

        ImGui::BeginGroup(); // right-side column: current/previous + palette
        {
            ImGui::TextUnformatted("Current");
            ImGui::ColorButton("##current", col,
                               ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_AlphaPreviewHalf,
                               opt.preview_size);

            ImGui::TextUnformatted("Previous");
            ImVec4 prev = col;
            if (auto it = backup_by_id.find(picker_id); it != backup_by_id.end())
                prev = it->second;

            if (ImGui::ColorButton("##previous", prev,
                                   ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_AlphaPreviewHalf,
                                   opt.preview_size))
            { col = prev; changed = true; }

            ImGui::Separator();
            ImGui::TextUnformatted("Palette");

            const float y_spacing = ImGui::GetStyle().ItemSpacing.y;
            for (int n = 0; n < (int)palette.size(); ++n) {
                ImGui::PushID(n);
                if ((n % opt.palette_cols) != 0)
                    ImGui::SameLine(0.0f, y_spacing);

                ImGuiColorEditFlags swatch_flags =
                    ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_NoTooltip;

                if (ImGui::ColorButton("##pal", palette[n], swatch_flags, ImVec2(20, 20))) {
                    col = ImVec4(palette[n].x, palette[n].y, palette[n].z, col.w); // preserve alpha
                    changed = true;
                }

                if (ImGui::BeginDragDropTarget()) {
                    if (const ImGuiPayload* p = ImGui::AcceptDragDropPayload(IMGUI_PAYLOAD_TYPE_COLOR_3F))
                        std::memcpy(&palette[n], p->Data, sizeof(float) * 3);
                    if (const ImGuiPayload* p = ImGui::AcceptDragDropPayload(IMGUI_PAYLOAD_TYPE_COLOR_4F))
                        std::memcpy(&palette[n], p->Data, sizeof(float) * 4);
                    ImGui::EndDragDropTarget();
                }
                ImGui::PopID();
            }
        }
        ImGui::EndGroup();
        ImGui::EndPopup();
    }

    ImGui::PopID();
    return changed;
}
*/















// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
} }//   END OF "cb::ui" NAMESPACE.






#endif      //  _CBAPP_WIDGETS_MISC_H  //
// *************************************************************************** //
// *************************************************************************** //
//
//  END.

