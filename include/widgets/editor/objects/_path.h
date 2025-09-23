/***********************************************************************************
*
*       ********************************************************************
*       ****                _ P A T H . H  ____  F I L E                ****
*       ********************************************************************
*
*              AUTHOR:      Collin A. Bond.
*               DATED:      September 7, 2025.
*              MODULE:      CBAPP > EDITOR > OBJECTS/       | _path.h
*
*       ********************************************************************
*                FILE:      [./PyStream.h]
*
*
*
**************************************************************************************
**************************************************************************************/
#ifndef _CBAPP_EDITOR_OBJECTS_PATH_H
#define _CBAPP_EDITOR_OBJECTS_PATH_H  1
#include CBAPP_USER_CONFIG



//  1.  INCLUDES    | Headers, Modules, etc...
// *************************************************************************** //
// *************************************************************************** //

//  0.1.        ** MY **  HEADERS...
#include "cblib.h"
#include "utility/utility.h"
#include "widgets/editor/_constants.h"
#include "widgets/editor/objects/_internal.h"

//  0.2     STANDARD LIBRARY HEADERS...
#include <iostream>         //  <======| std::cout, std::cerr, std::endl, ...
#include <cstdlib>          // C-Headers...
#include <stdio.h>
#include <unistd.h>

#include <algorithm>        //  SPECIAL...
#include <optional>
#include <variant>
#include <tuple>

#include <string>           //  DATA STRUCTURES...      //  <======| std::string, ...
#include <string_view>
#include <vector>           //                          //  <======| std::vector, ...
#include <array>
#include <unordered_set>

#include <stdexcept>        //                          //  <======| ...
#include <limits.h>
#include <math.h>
#include <complex>
#include <utility>


//  0.3     "DEAR IMGUI" HEADERS...
#include "json.hpp"
//
#include "imgui.h"
#include "imgui_stdlib.h"
#include "imgui_internal.h"
#include "implot.h"
#include "implot_internal.h"



namespace cb { //     BEGINNING NAMESPACE "cb"...
// *************************************************************************** //
// *************************************************************************** //






// *************************************************************************** //
//
//
//
//      5.      [DATA LAYER]    PATH EXTENSIONS...
// *************************************************************************** //
// *************************************************************************** //

namespace path { //     BEGINNING NAMESPACE "cb"...
// *************************************************************************** //
// *************************************************************************** //



// *************************************************************************** //
//      1.      DEFINE EACH "KIND" OF PATH...
// *************************************************************************** //

//  "PathKind"
//
enum class PathKind : uint8_t {
    None = 0,
    Generic,
//
    Source,
    Boundary,
    Dielectric,
//
    COUNT
};

//  "PATH_KIND_NAMES"
//
static constexpr std::array<const char *, static_cast<size_t>(PathKind::COUNT)>
DEF_PATH_KIND_NAMES                     = {
    "None",
    "Generic",
    "Source", "Boundary Condition", "Dielectric"
};






// *************************************************************************** //
//      2.      DEFINE DATA-PAYLOADS (FOR EACH PATH-KIND TO CARRY)...
// *************************************************************************** //

//  "GenericPayload"
struct GenericPayload {
// *************************************************************************** //
// *************************************************************************** //

    //  "ui_properties"
    inline void                     ui_properties                   (void)
    {
        static constexpr float          ms_LABEL_WIDTH          = 196.0f;
        static constexpr float          ms_WIDGET_WIDTH         = 300.0f;
        auto                            label                   = [&](const char * text) -> void
        { utl::LeftLabel(text, ms_LABEL_WIDTH, ms_WIDGET_WIDTH);   ImGui::SameLine(); };
        
        
        label("Double #1:");
        ImGui::InputDouble("##rel_perm", &x, 0.0, 0.0, "%.3f");     // x: relative permittivity

        label("Double #2:");
        ImGui::InputDouble("##rel_perm_mu", &y, 0.0, 0.0, "%.3f");  // y: relative permeability

        label("Persistent String Data:");
        ImGui::InputText("##payload_data", &data);                  // needs imgui_stdlib.h

        label("Metadata:");
        ImGui::InputText("##payload_meta", &meta);
        
        
        
        return;
    }
    
// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
    double                          x                               {1.0};              //  relative permittivity
    double                          y                               {1.0};              //  relative permeability
    std::string                     data                            {};
    std::string                     meta                            {};
// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "Path_t" CLASS DEFINITION.
};//    END "Path_t" CLASS DEFINITION.

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(GenericPayload, x, y, data, meta)






//  "SourcePayload"
struct SourcePayload {
// *************************************************************************** //
// *************************************************************************** //

    //  "ui_properties"
    inline void                     ui_properties                   (void)
    {
        static const char *     type_items[]    = { "Hard-E",   "Hard-H",           "Soft-E",   "Soft-H" };
        static const char *     wf_items[]      = { "Gaussian", "Time-Harmonic",    "Ricker",   "User" };
        constexpr float         LABEL_W         = 196.0f;
        constexpr float         WIDGET_W        = 300.0f;
        auto label_fn = [&](const char* txt) {
            utl::LeftLabel(txt, LABEL_W, WIDGET_W);
            ImGui::SameLine();
        };

        //      Type
        label_fn("Type:");
        int t = static_cast<int>(type);
        if (ImGui::Combo("##src_type", &t, type_items, IM_ARRAYSIZE(type_items)))
            type = static_cast<Type>(t);

        //      Waveform
        label_fn("Waveform:");
        int w = static_cast<int>(waveform);
        if (ImGui::Combo("##src_wave", &w, wf_items, IM_ARRAYSIZE(wf_items)))
            waveform = static_cast<Wave>(w);

        //      Amplitude / frequency / phase
        label_fn("Amplitude:");
        ImGui::DragScalar("##src_amp",  ImGuiDataType_Double, &amplitude, 1e-3, nullptr, nullptr, "%.6f");

        label_fn("Frequency [Hz]:");
        ImGui::DragScalar("##src_freq", ImGuiDataType_Double, &frequency, 1.0,  nullptr, nullptr, "%.4e");

        label_fn("Phase [rad]:");
        ImGui::DragScalar("##src_phase",ImGuiDataType_Double, &phase, 1e-3,   nullptr, nullptr, "%.4f");

        //      Pulse-specific parameters (always visible, disabled unless waveform is pulse-type)
        const bool pulse_ok = (waveform == Wave::Gaussian || waveform == Wave::Ricker);
        ImGui::BeginDisabled(!pulse_ok);
        {
            label_fn("Duration [s]:");
            ImGui::DragScalar("##src_tau", ImGuiDataType_Double, &tau, 1e-12, nullptr, nullptr, "%.3e");

            label_fn("T0 [s]:");
            ImGui::DragScalar("##src_t0",  ImGuiDataType_Double, &t0,  1e-12, nullptr, nullptr, "%.3e");
        }
        ImGui::EndDisabled();

        //      Direction vector (always active)
        label_fn("Direction:");
        ImGui::DragScalarN("##src_dir",
                           ImGuiDataType_Double,
                           direction.data(), 3,
                           1e-3, nullptr, nullptr, "%.4f");
    }
    
    // *************************************************************************** //
    //      DATA MEMBERS...
    // *************************************************************************** //
    float                           value                           = 1.0f;
    enum class                      Type                            { HardE, HardH, SoftE, SoftH };
    enum class                      Wave                            { Gaussian, Sine, Ricker, User };
    Type                            type                            {Type::HardE};
    Wave                            waveform                        {Wave::Gaussian};
    //
    double                          amplitude                       {1.0};          // peak field
    double                          frequency                       {0.0};          // Hz (0 = DC / impulse)
    double                          phase                           {0.0};          // rad
    double                          tau                             {1.0e-9};       // pulse width for Gaussian/Ricker
    double                          t0                              {0.0};          // time offset
    std::array<double,3>            direction                       {1.0, 0.0, 0.0}; // unit vector (Ex,Hx etc)

// *************************************************************************** //
// *************************************************************************** //   END "SourcePayload" CLASS DEFINITION.
};//    END "SourcePayload" CLASS DEFINITION.

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(SourcePayload, value)






//  "BoundaryPayload"
struct BoundaryPayload {
// *************************************************************************** //
// *************************************************************************** //

    //  "ui_properties"
    inline void                     ui_properties                   (void)
    {
        const char *        kind_items[]    = { "PEC", "PMC", "PML", "Mur 1", "Mur 2", "TF/SF" };
        constexpr float     LABEL_W         = 196.0f;
        constexpr float     WIDGET_W        = 300.0f;
        auto label_fn = [&](const char* txt){
            utl::LeftLabel(txt, LABEL_W, WIDGET_W);
            ImGui::SameLine();
        };

        //------------------------------------------------------------------
        //  Kind
        label_fn("Kind:");
        int k = static_cast<int>(kind);
        if (ImGui::Combo("##b_kind", &k, kind_items, IM_ARRAYSIZE(kind_items)))
            kind = static_cast<Kind>(k);

        //------------------------------------------------------------------
        //  PML parameters (widgets always present, disabled if not PML)
        bool is_pml = (kind == Kind::PML);
        ImGui::BeginDisabled(!is_pml);
        {
            label_fn("Thickness [cells]:");
            ImGui::DragScalar("##b_thick", ImGuiDataType_U32, &thickness_cells, 1.0f);

            label_fn("PML order, m:");
            ImGui::DragScalar("##b_m",     ImGuiDataType_U8,  &pml_order,       1.0f);

            label_fn("Sigma, max:");
            ImGui::DragScalar("##b_sig",   ImGuiDataType_Double, &sigma_max, 1e-2, nullptr, nullptr, "%.3e");

            label_fn("Kappa, max:");
            ImGui::DragScalar("##b_kap",   ImGuiDataType_Double, &kappa_max, 1e-2, nullptr, nullptr, "%.3e");

            label_fn("Alpha, max");
            ImGui::DragScalar("##b_alp",   ImGuiDataType_Double, &alpha_max, 1e-3, nullptr, nullptr, "%.3e");
        }
        ImGui::EndDisabled();

        //------------------------------------------------------------------
        // TF/SF interface geometry (disabled unless TF/SF)
        bool is_tfsf = (kind == Kind::TFSF);
        ImGui::BeginDisabled(!is_tfsf);
        {
            label_fn("Pos. (x,y,z):");
            ImGui::DragScalarN("##b_refpos",
                               ImGuiDataType_Double,
                               ref_pos.data(), 3,
                               1e-3, nullptr, nullptr, "%.4f");

            label_fn("Normal (nx,ny,nz):");
            ImGui::DragScalarN("##b_norm",
                               ImGuiDataType_Double,
                               normal.data(), 3,
                               1e-3, nullptr, nullptr, "%.3f");
        }
        ImGui::EndDisabled();
        return;
    }
    
    // *************************************************************************** //
    //      DATA MEMBERS...
    // *************************************************************************** //
    float                           value                           = 1.0f;
    enum class                      Kind                            { PEC, PMC, PML, Mur1, Mur2, TFSF };
    Kind                            kind                            {Kind::PEC};

    //------------------------------------------------------------------ PML
    uint32_t                        thickness_cells                 {10};
    uint8_t                         pml_order                       {3};
    double                          sigma_max                       {1.0};
    double                          kappa_max                       {1.0};
    double                          alpha_max                       {0.0};

    //------------------------------------------------------------------ TF/SF
    std::array<double,3>            ref_pos                         {0.0, 0.0, 0.0};   // position of interface plane
    std::array<double,3>            normal                          {1.0, 0.0, 0.0};   // outward unit normal


// *************************************************************************** //
// *************************************************************************** //   END "BoundaryPayload" CLASS DEFINITION.
};//    END "BoundaryPayload" CLASS DEFINITION.

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(BoundaryPayload, value)






//  "DielectricPayload"
struct DielectricPayload {
// *************************************************************************** //
// *************************************************************************** //

    //  "ui_properties"
    inline void ui_properties()
    {
        constexpr const char *      options[]           = { "None", "Debye", "Drude", "Lorentz" };
        static constexpr float      ms_LABEL_WIDTH      = 196.0f;
        static constexpr float      ms_WIDGET_WIDTH     = 300.0f;
        auto label = [&](const char* txt) {
            utl::LeftLabel(txt, ms_LABEL_WIDTH, ms_WIDGET_WIDTH);
            ImGui::SameLine();
        };


        // -----------------------------------------------------------------
        // εr
        double eps_vals[2] = { eps_r.real(), eps_r.imag() };
        label("Permittivity:");
        if (ImGui::DragScalarN("##eps",
                               ImGuiDataType_Double,
                               eps_vals,
                               2,               // two components
                               1e-3,            // step
                               nullptr, nullptr,
                               "%.6f Relative")) {
            eps_r = { eps_vals[0], eps_vals[1] };
        }

        // -----------------------------------------------------------------
        // μr
        double mu_vals[2] = { mu_r.real(), mu_r.imag() };
        label("Permeability:");
        if (ImGui::DragScalarN("##mu",
                               ImGuiDataType_Double,
                               mu_vals, 2, 1e-3, nullptr, nullptr, "%.6f Relative")) {
            mu_r = { mu_vals[0], mu_vals[1] };
        }

        // -----------------------------------------------------------------
        // Conductivities
        double sigma[2] = { sigma_e, sigma_m };
        label("Conductivity (E, B):");
        if (ImGui::DragScalarN("##sigma",
                               ImGuiDataType_Double,
                               sigma, 2, 1e-3, nullptr, nullptr, "%.6f [S/m]"))
        {
            sigma_e = sigma[0];     sigma_m = sigma[1];
        }
        
        
        

        // -----------------------------------------------------------------
        // Dispersion model
        label("Dispersion:");
        int choice = static_cast<int>(disp_model);
        if (ImGui::Combo("##disp_model", &choice, options, IM_ARRAYSIZE(options)))
            disp_model = static_cast<DispersionModel>(choice);

        ImGui::BeginDisabled( disp_model == DispersionModel::None );
        // Extra parameters only when a model is active
        {
            label("delta perm.:");
            ImGui::InputDouble("##d_eps", &disp_par[0], 0.0, 0.0, "%.5f");

            label("freq [Hz]:");
            ImGui::InputDouble("##f0", &disp_par[1], 0.0, 0.0, "%.4e");

            label("gamma [Hz]:");
            ImGui::InputDouble("##gamma", &disp_par[2], 0.0, 0.0, "%.4e");

            label("N / count:");
            ImGui::InputDouble("##order", &disp_par[3], 0.0, 0.0, "%.0f");
        }
        ImGui::EndDisabled();
        
        return;
    }
    
    // *************************************************************************** //
    //      DATA MEMBERS...
    // *************************************************************************** //
    float                           value                           = 1.0f;
    std::complex<double>            eps_r                           {1.0, 0.0};   // relative permittivity
    std::complex<double>            mu_r                            {1.0, 0.0};   // relative permeability
    double                          sigma_e                         {0.0};        // electric conductivity   [S/m]
    double                          sigma_m                         {0.0};        // magnetic conductivity   [S/m]

    enum class                      DispersionModel                 { None, Debye, Drude, Lorentz };
    DispersionModel                 disp_model                      {DispersionModel::None};
    std::array<double,4>            disp_par                        {};           // Δε, f₀, γ, N (or as you choose)


// *************************************************************************** //
// *************************************************************************** //   END "DielectricPayload" CLASS DEFINITION.
};//    END "DielectricPayload" CLASS DEFINITION.

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(DielectricPayload, value)






// *************************************************************************** //
//      3.      DEFINE "PAYLOAD" STD::VARIANT ALIAS TYPE...
// *************************************************************************** //
using       Payload         = std::variant<
    std::monostate,     GenericPayload,
    SourcePayload,      BoundaryPayload,    DielectricPayload
>;






// *************************************************************************** //
//      4.      DEFINE "JSON" HELPERS FOR PAYLOADS...
// *************************************************************************** //

//  "to_json"           | For GenericPayload...
//
inline void to_json(nlohmann::json & j, const Payload & v)
{
    //  1.  monostate  ⇒ store null  (omit field entirely if you prefer)
    if ( std::holds_alternative<std::monostate>(v) )                { j = nullptr; return; }

    //  2.  GenericPayload ⇒ store as plain object...
    if ( const auto* gp = std::get_if<GenericPayload>(&v) )         { j = *gp;  return; }    // uses GenericPayload serializer

    //  3.  SourcePayload...
    if ( const auto * gp = std::get_if<SourcePayload>(&v) )         { j = *gp;  return; }

    //  4.  BoundaryPayload...
    if ( const auto * gp = std::get_if<BoundaryPayload>(&v) )       { j = *gp;  return; }

    //  5.  DielectricPayload...
    if ( const auto * gp = std::get_if<DielectricPayload>(&v) )     { j = *gp;  return; }

    
    return;
}


//  "from_json"         | For GenericPayload...
//
inline void from_json(const nlohmann::json & j, Payload & v)
{
    //  1.  MONOSTATE...
    if ( j.is_null() || j.empty() )     { v = std::monostate{}; }
    
    //  2.  "GenericPayload"    | Until we add more Payload types, we assume either (1) Monostate, or (2) Generic...
    else                                { v = j.get<GenericPayload>(); }
    
    //  when adding more kinds:
    //      – inspect a “kind” key or structure to decide which get<>() to call...
    //
    
    return;
}



// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
}//   END OF "path" NAMESPACE.






// *************************************************************************** //
//
//
//
//      6.      [DATA LAYER]    CORE PATH OBJECTS...
// *************************************************************************** //
// *************************************************************************** //

//  "PathStyle"
//
struct PathStyle {
    ImU32       stroke_color        = IM_COL32(255,255,0,255);
    ImU32       fill_color          = IM_COL32(255,255,255,0);   // default: transparent white
    float       stroke_width        = 2.0f;
};


//  "to_json"
//
inline void to_json(nlohmann::json& j, const PathStyle& s)
{
    j = nlohmann::json{
        { "stroke_color"        , s.stroke_color    },
        { "fill_color"          , s.fill_color      },
        { "stroke_width"        , s.stroke_width    }
    };
    
    return;
}


//  "from_json"
//
inline void from_json(const nlohmann::json & j, PathStyle & s)
{
    j.at("stroke_color"     ).get_to(   s.stroke_color      );
    j.at("fill_color"       ).get_to(   s.fill_color        );
    j.at("stroke_width"     ).get_to(   s.stroke_width      );
    
    return;
}



//  "Path_t"  -- (polyline / spline / area).
//
//      A sequence of vertex IDs. `closed == true` means the last vertex connects
//      back to the first. Vertices carry their own Bézier handles; this struct
//      only stores ordering and style.
//
template< EditorCFGTraits CFG, typename V >
struct Path_t
{
//      0.          CONSTANTS AND ALIASES...
// *************************************************************************** //
// *************************************************************************** //
public:

    // *************************************************************************** //
    //      NESTED TYPENAME ALIASES.
    // *************************************************************************** //
    _EDITOR_CFG_PRIMATIVES_ALIASES
    //
    //                              LOCAL TYPES:
    using                               id_type                                 = path_id;
    using                               container_type                          = std::vector<vertex_id>;
    using                               size_type                               = container_type::size_type;
    using                               iterator                                = typename container_type::iterator;
    //
    using                               Vertex                                  = V;
    using                               Path                                    = Path_t<CFG, V>;
    using                               Payload                                 = path::Payload;
    using                               PathKind                                = path::PathKind;
    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      STATIC CONSTEXPR CONSTANTS.
    // *************************************************************************** //
    static constexpr float              ms_MIN_STROKE_WIDTH                     = 1e-6;
    static constexpr size_t             ms_MAX_PATH_LABEL_LENGTH                = 64;
    static constexpr const char *       ms_DEF_PATH_TITLE_FMT_STRING            = "%s (ID #%06u)";
    static constexpr ImU32              ms_DEF_PATH_FILL_COLOR                  = IM_COL32(0,110,255,45);
    
//
//
// *************************************************************************** //
// *************************************************************************** //   END "CONSTANTS AND ALIASES".



// *************************************************************************** //
//
//
//      1.          CLASS DATA-MEMBERS...
// *************************************************************************** //
// *************************************************************************** //

    // *************************************************************************** //
    //                          CORE:
    // *************************************************************************** //
    container_type                  verts;   // ordered anchor IDs
    path_id                         id                              = 0;
    bool                            closed                          = false;
    PathStyle                       style                           = PathStyle();
    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //                          NEW:
    // *************************************************************************** //
    z_id                            z_index                         = Z_FLOOR_USER;
    bool                            locked                          = false;
    bool                            visible                         = true;
    std::string                     label                           = "";
    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //                          NEW-ER:
    // *************************************************************************** //
    PathKind                        kind                            = PathKind::None;
    Payload                         payload                         {   };
    
//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "CLASS DATA-MEMBERS".



// *************************************************************************** //
//
//
//      2.A.        PUBLIC MEMBER FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //
    
//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "PUBLIC MEMBER FUNCS".

    
   
// *************************************************************************** //
//
//
//      2.C.        INLINE FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

    // *************************************************************************** //
    //      QUERY FUNCTIONS.
    // *************************************************************************** //

    //  "IsSelectable"
    [[nodiscard]] inline bool           IsSelectable                    (void) const noexcept           { return ( this->visible  &&  !this->locked ); }

    //  "IsMutable"
    [[nodiscard]] inline bool           IsMutable                       (void) const noexcept           { return ( this->visible  &&  !this->locked ); }
    
    
    
    //  "IsPath"
    [[nodiscard]] inline bool           IsPath                          (void) const noexcept           { return ( 1 < this->verts.size() ); }   //  { return ( (1 < this->verts.size())  &&  (!this->closed) ); }
    
    //  "IsArea"
    [[nodiscard]] inline bool           IsArea                          (void) const noexcept           { return ( (this->closed)  &&  (this->verts.size() >= 3) ); }
    
    //  "IsVisible"
    [[nodiscard]] inline bool           IsVisible                       (void) const noexcept           { return ( (this->visible) ); }
    
    //  "FillIsVisible"
    [[nodiscard]] inline bool           FillIsVisible                   (void) const noexcept           { return ( (this->style.fill_color & 0xFF000000) != 0 );    }
    
    //  "StrokeIsVisible"
    [[nodiscard]] inline bool           StrokeIsVisible                 (void) const noexcept           { return ( ((this->style.stroke_color & 0xFF000000) != 0)  &&  (this->ms_MIN_STROKE_WIDTH < this->style.stroke_width) );  }
    
    //  "IsTransparent"
    [[nodiscard]] inline bool           IsTransparent                   (void) const noexcept
    {
        const bool      is_visible                  = this->IsVisible();
        const bool      is_area                     = this->IsArea();
        const bool      stroke_is_transparent       = ( (this->style.stroke_color >> 24) == 0 );
        const bool      fill_is_transparent         = ( (this->style.fill_color >> 24) == 0 );
        
        return (is_area)
                    ? ( !is_visible  &&  (stroke_is_transparent || fill_is_transparent) )
                    : ( !is_visible  &&  stroke_is_transparent );
    }
    
    //  "SegmentIsCurved"
    //      Curvature rule: segment is curved   iff   a.Out NOT linear  *OR*  b.In NOT linear...
    //
    //  [[nodiscard]] inline bool           SegmentIsCurved                  (const V & a, const V & b) const noexcept   { return ( !a.IsOutLinear()  ||  !b.IsInLinear() );    }
    //  [[nodiscard]] inline bool           SegmentIsLinear                  (const V & a, const V & b) const noexcept   { return ( a.IsOutLinear()  &&  b.IsInLinear() );      }



    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      SETTER / GETTER FUNCTIONS.
    // *************************************************************************** //
    
    //  "size"
    [[nodiscard]] inline size_type      size                            (void) const noexcept           { return this->verts.size(); }
    
        
        
    //  "set_label"
    inline void                         set_label                       (const char * src) noexcept     { this->label = std::string(src); this->_truncate_label(); }
    //  "set_default_label"
    inline void                         set_default_label               (const path_id id_) noexcept        { this->id = id_;   this->label = std::format("Path {:03}", id_);   this->_truncate_label(); }


    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      CENTRALIZED STATE MANAGEMENT FUNCTIONS.
    // *************************************************************************** //
    
        
    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      PATH OPERATION FUNCTIONS.
    // *************************************************************************** //
    
    //  "remove_vertex"
    inline bool                         remove_vertex                       (vertex_id vid) noexcept {
        verts.erase(std::remove(verts.begin(), verts.end(), vid), verts.end());
        if ( closed && verts.size() < 3 )     { closed = false; }     // cannot stay a polygon

        return ( verts.size() >= 2 );
    }
    
    //  "insert_vertex_after"
    inline iterator                     insert_vertex_after                 (size_t seg_idx, vertex_id new_vid)
        { return verts.insert(verts.begin() + seg_idx + 1, new_vid); }
    
    //  "insert_vertex_between"
    //  inline iterator                     insert_vertex_between               (size_t seg_idx, vertex_id new_vid)
    //  { return verts.insert(verts.begin() + seg_idx + 1, new_vid); }
        
        
    //  "_bezier_eval_ws"
    //
    static inline ImVec2 _bezier_eval_ws(const ImVec2& P0, const ImVec2& P1,
                                         const ImVec2& P2, const ImVec2& P3, float t) noexcept
    {
        const float u   = 1.0f - t;
        const float uu  = u * u;
        const float tt  = t * t;
        const float uuu = uu * u;
        const float ttt = tt * t;

        ImVec2 r;
        r.x = uuu * P0.x + 3.0f * uu * t * P1.x + 3.0f * u * tt * P2.x + ttt * P3.x;
        r.y = uuu * P0.y + 3.0f * uu * t * P1.y + 3.0f * u * tt * P2.y + ttt * P3.y;
        return r;
    }


    //  "_quad_unit_roots"
    //      Solve quadratic a t^2 + b t + c = 0 in (0,1); append valid roots to out[]
    //
    static inline void _quad_unit_roots(float a, float b, float c, float out[2], int& count) noexcept
    {
        const float     eps     = 1e-8f;
        count                   = 0;

        if ( fabsf(a) < eps )
        {
            const float t = -c / b;
            
            if ( fabsf(b) < eps )               { return; }
            if ( (t > 0.0f)  &&  (t < 1.0f) )   { out[count++] = t; }
            return;
        }

        const float D = b*b - 4.0f*a*c;
        if (D < 0.0f)   { return; }

        const float     sD      = sqrtf(std::max(D, 0.0f));
        const float     inv2    = 0.5f / a;
        float           t1      = (-b - sD) * inv2;
        float           t2      = (-b + sD) * inv2;

        if ( (t1 > 0.0f)  &&  (t1 < 1.0f) )     { out[count++] = t1; }
        if ( (t2 > 0.0f)  &&  (t2 < 1.0f)  &&  (count == 0 || fabsf(t2 - out[0]) > 1e-6f) )
            { out[count++] = t2; }
            
        return;
    }


    //  "_aabb_cubic_tight"
    //      Axis-aligned tight AABB for one cubic segment by analytic extrema (endpoints + interior roots)
    //
    static inline void _aabb_cubic_tight(const ImVec2 & P0, const ImVec2 & P1,
                                         const ImVec2 & P2, const ImVec2 & P3,
                                         ImVec2 & tl_ws, ImVec2 & br_ws, bool & first) noexcept
    {
        auto add = [&](const ImVec2 & q)
        {
            if (first)  { tl_ws = br_ws = q; first = false; }
            else
            {
                tl_ws.x = std::min(tl_ws.x, q.x);  tl_ws.y = std::min(tl_ws.y, q.y);
                br_ws.x = std::max(br_ws.x, q.x);  br_ws.y = std::max(br_ws.y, q.y);
            }
        };

        add(P0); add(P3);   //  Always include endpoints

        //  For cubic B(t), the derivative component coefficients are quadratic:
        //
        //      B'(t)   = 3 * ( a t^2 + b t + c ) ; with,
        //            a = -P0 + 3P1 - 3P2 + P3
        //            b = 2*( P0 - 2P1 + P2 )
        //            c = -P0 + P1
        //
        auto deriv_coeffs = [](float p0, float p1, float p2, float p3, float & a, float & b, float & c)
        {
            a = (-p0 + 3.0f*p1 - 3.0f*p2 + p3);
            b = 2.0f*( p0 - 2.0f*p1 + p2 );
            c = (-p0 + p1);
        };

        float ax, bx, cx, ay, by, cy;
        deriv_coeffs( P0.x, P1.x, P2.x, P3.x, ax, bx, cx );
        deriv_coeffs( P0.y, P1.y, P2.y, P3.y, ay, by, cy );

        float tx[2]; int nx = 0; _quad_unit_roots(ax, bx, cx, tx, nx);
        float ty[2]; int ny = 0; _quad_unit_roots(ay, by, cy, ty, ny);

        for (int i = 0; i < nx; ++i)    { add( _bezier_eval_ws(P0, P1, P2, P3, tx[i]) ); }
        for (int i = 0; i < ny; ++i)    { add( _bezier_eval_ws(P0, P1, P2, P3, ty[i]) ); }
        
        return;
    }
    /*
    static inline void _aabb_cubic_tight(const ImVec2 & P0, const ImVec2 & P1,
                                         const ImVec2 & P2, const ImVec2 & P3,
                                         ImVec2 & tl_ws, ImVec2 & br_ws, bool & first) noexcept
    {
        auto add = [&](const ImVec2 & q)
        {
            if (first) { tl_ws = br_ws = q; first = false; }
            else
            {
                tl_ws.x = std::min(tl_ws.x, q.x);  tl_ws.y = std::min(tl_ws.y, q.y);
                br_ws.x = std::max(br_ws.x, q.x);  br_ws.y = std::max(br_ws.y, q.y);
            }
        };

        //  Endpoints...
        add(P0); add(P3);

        //  Derivative coefficients per component:
        //
        //      B'(t)   = 3 * ( a t^2 + b t + c ) ; with,
        //            a = -P0 + 3P1 - 3P2 + P3
        //            b = 2*( P0 - 2P1 + P2 )
        //            c = -P0 + P1
        //
        const auto deriv_coeffs = [](float P0, float P1, float P2, float P3, float & a, float & b, float & c) {
            a = (-P0 + 3.0f*P1 - 3.0f*P2 + P3);
            b = 2.0f*( P0 - 2.0f*P1 + P2 );
            c = (-P0 + P1);
        };

        float ax, bx, cx, ay, by, cy;
        deriv_coeffs( P0.x, P1.x, P2.x, P3.x, ax, bx, cx );
        deriv_coeffs( P0.y, P1.y, P2.y, P3.y, ay, by, cy );

        // Roots in (0,1) where x'(t)=0 and y'(t)=0
        float tx[2]; int nx = 0; _quad_unit_roots(ax, bx, cx, tx, nx);
        float ty[2]; int ny = 0; _quad_unit_roots(ay, by, cy, ty, ny);

        // Evaluate candidates
        for (int i = 0; i < nx; ++i)    { add( _bezier_eval_ws(P0, P1, P2, P3, tx[i]) ); }
        for (int i = 0; i < ny; ++i)    { add( _bezier_eval_ws(P0, P1, P2, P3, ty[i]) ); }
        
        return;
    }*/
    
    
    //  "aabb_geometry_tight"
    //
    template <class CTX>
    inline bool aabb_geometry_tight(const CTX & ctx, ImVec2 & tl_ws, ImVec2 & br_ws) const noexcept
    {
        constexpr auto &    cb              = ctx.callbacks; // cb.get_vertex(cb.vertices, id)
        const size_t        N               = this->size();
        bool                first           = true;
        const bool          is_area         = this->IsArea();
        const size_t        seg_cnt         = N - (is_area ? 0 : 1);
        
        
        if ( N < 2 )        { return false; }


        for (size_t si = 0; si < seg_cnt; ++si)
        {
            const vertex_id     a_id    = static_cast<vertex_id>( this->verts[si]           );
            const vertex_id     b_id    = static_cast<vertex_id>( this->verts[(si + 1) % N] );

            const auto* a = cb.get_vertex(cb.vertices, a_id);
            const auto* b = cb.get_vertex(cb.vertices, b_id);
            if (!a || !b) continue;

            const ImVec2 P0{ a->x, a->y };
            const ImVec2 P3{ b->x, b->y };
            const ImVec2 P1{ a->x + a->m_bezier.out_handle.x, a->y + a->m_bezier.out_handle.y };
            const ImVec2 P2{ b->x + b->m_bezier.in_handle.x,  b->y + b->m_bezier.in_handle.y  };

            _aabb_cubic_tight(P0, P1, P2, P3, tl_ws, br_ws, first);
        }
        return !first;
    }
    
    
    //  "segment_control_points"
    //
    template<class CTX>
    inline bool	segment_control_points( size_t si, ImVec2 & P0, ImVec2 & P1, ImVec2 & P2, ImVec2 & P3, const CTX & ctx ) const noexcept
    {
        const size_t        N           = this->size();
        if ( N < 2 )                    { return false; }

        const auto &        cb          = ctx.callbacks;
        const vertex_id	    a_id        = static_cast<vertex_id>( this->verts[si]               );
        const vertex_id	    b_id        = static_cast<vertex_id>( this->verts[(si + 1) % N]     );

        const auto *	    a           = cb.get_vertex(cb.vertices, a_id);
        const auto *	    b           = cb.get_vertex(cb.vertices, b_id);
        if ( !a  ||  !b )               { return false; }


        P0                              = ImVec2{ a->x, a->y };
        P3                              = ImVec2{ b->x, b->y };

        // Model-aware handles: use effective_* so "None" (linear) doesn't bloat bounds.
        const ImVec2        out_eff     = a->EffectiveOutHandle();
        const ImVec2        in_eff      = b->EffectiveInHandle();

        if ( a->IsOutLinear()  &&  b->IsInLinear() )
        {
            P1 = P0;              // linear segment
            P2 = P3;
        }
        else
        {
            P1 = ImVec2{ P0.x + out_eff.x, P0.y + out_eff.y };
            P2 = ImVec2{ P3.x + in_eff.x,  P3.y + in_eff.y  };
        }
        return true;
    }
        
        
        
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      UI HELPER FUNCTIONS.
    // *************************************************************************** //
    
    //  "ui_all"
    inline void                         ui_all                              (void)
    {
        using namespace path;
        if ( this->ui_kind() )        { /*    "kind" was changes.     */ }
        
        //  2.  CALL THE "PROPERTIES UI" FOR THE
        std::visit([&](auto & pl)
        {
            using T = std::decay_t<decltype(pl)>;
            //  skip monostate (has no draw_ui)
            if constexpr (!std::is_same_v<T, std::monostate>)
                { pl.ui_properties(); }          // every real payload implements this
        }, payload);
        
        return;
    }
    
    //  "ui_properties"
    inline void                         ui_properties                       (void)
    {
        using namespace path;
        
        //  2.  CALL THE "PROPERTIES UI" FOR THE
        std::visit([&](auto & pl)
        {
            using T = std::decay_t<decltype(pl)>;
            //  skip monostate (has no draw_ui)
            if constexpr (!std::is_same_v<T, std::monostate>)
                { pl.ui_properties(); }          // every real payload implements this
        }, payload);
        
        return;
    }
    
    //  "ui_kind"
    inline bool                         ui_kind                             (void)
    {
        bool    modified    = false;
        int     kind_idx    = static_cast<int>(this->kind);
        if (( modified = ImGui::Combo("##Path_UIInternal_Kind",             &kind_idx,
                                      path::DEF_PATH_KIND_NAMES.data(),     static_cast<int>(PathKind::COUNT)) ))
        {
            this->kind          = static_cast<PathKind>(kind_idx);
            this->payload       = make_default_payload(this->kind);     // << reset payload
        }
        
        return modified;
    }
    
    
    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      UTILITY FUNCTIONS.
    // *************************************************************************** //
    
    //  "_truncate_label"
    inline void                         _truncate_label                     (void)
    { if (this->label.size() > ms_MAX_PATH_LABEL_LENGTH) { this->label.resize( ms_MAX_PATH_LABEL_LENGTH ); } }
    
    
    //  inline void                         _segment_is_curved                  (const V * a, const V * b) noexcept
    //  {
    //      constexpr float eps2 = 1e-8f; // or your ms_BEZIER_NUMERICAL_EPSILON_SQ
    //      const float o2 = a->m_bezier.out_handle.x * a->m_bezier.out_handle.x
    //                     + a->m_bezier.out_handle.y * a->m_bezier.out_handle.y;
    //      const float i2 = b->m_bezier.in_handle.x  * b->m_bezier.in_handle.x
    //                     + b->m_bezier.in_handle.y  * b->m_bezier.in_handle.y;
    //      return (o2 > eps2) || (i2 > eps2);
    //  };
    
    
    //  "make_default_payload"
    static inline Payload               make_default_payload                (const PathKind k)
    {
        switch (k)
        {
            case PathKind::Generic :        { return path::GenericPayload       {   };          }   //  default-constructed
            //
            case PathKind::Boundary :       { return path::BoundaryPayload      {   };          }
            case PathKind::Source :         { return path::SourcePayload        {   };          }
            case PathKind::Dielectric :     { return path::DielectricPayload    {   };          }
            default:                        { break;                                            }   //  PathKind::None or unknown
        }
        
        return ( std::monostate{   } );     //     no extra data
    }
    
    
    //  "aabb_control_hull"
    template <class CTX>
    inline bool                         aabb_control_hull                   (ImVec2 & tl_ws, ImVec2 & br_ws, const CTX & ctx) const noexcept
    {
        const auto &        cb              = ctx.callbacks; // cb.get_vertex(cb.vertices, id)
        const size_t        N               = this->size();
        auto                add_pt          = [&](const ImVec2& p) {
            tl_ws.x = std::min(tl_ws.x, p.x);  tl_ws.y = std::min(tl_ws.y, p.y);
            br_ws.x = std::max(br_ws.x, p.x);  br_ws.y = std::max(br_ws.y, p.y);
        };
        bool                first           = true;
        auto                init            = [&](const ImVec2& p){ tl_ws = br_ws = p; first = false; };

        const bool          is_area         = this->IsArea();
        const size_t        seg_cnt         = N - (is_area ? 0 : 1);
        
        
        if ( N < 2 )        { return false; }


        for (size_t si = 0; si < seg_cnt; ++si)
        {
            const vertex_id     a_id    = static_cast<vertex_id>( this->verts[si]           );
            const vertex_id     b_id    = static_cast<vertex_id>( this->verts[(si + 1) % N] );

            const auto *        a       = cb.get_vertex(cb.vertices, a_id);
            const auto *        b       = cb.get_vertex(cb.vertices, b_id);
            if ( !a || !b )     { continue; }

            const ImVec2 P0{ a->x, a->y };
            const ImVec2 P3{ b->x, b->y };
            const ImVec2 P1{ a->x + a->m_bezier.out_handle.x, a->y + a->m_bezier.out_handle.y };
            const ImVec2 P2{ b->x + b->m_bezier.in_handle.x,  b->y + b->m_bezier.in_handle.y  };

            if (first) { init(P0); }
            add_pt(P0); add_pt(P1); add_pt(P2); add_pt(P3);
        }
        return !first;
    }
    
    
    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      RENDER FUNCTIONS.
    // *************************************************************************** //
    
    //  "render_fill_area"
    template<class CTX>
    inline void                         render_fill_area                    (const CTX & ctx) const noexcept
    {
        const auto &        callbacks           = ctx.callbacks;
        const auto &        args                = ctx.args;
        ImDrawList *        dl                  = args.dl;
        const size_t        N                   = this->size();


        //  Fast-outs: visibility, topology, alpha
        const bool          dont_render         = ( !this->IsVisible()  &&  !this->IsArea()  &&  !this->FillIsVisible() );
        
        
        if ( dont_render )                      { return; }

        dl->PathClear();


        //  Build the pixel-space outline by walking each segment (a -> b)
        for (size_t i = 0; i < N; ++i)
        {
            const V *       a           = callbacks.get_vertex(     callbacks.vertices,      static_cast<vertex_id>( this->verts[i]            )   );
            const V *       b           = callbacks.get_vertex(     callbacks.vertices,      static_cast<vertex_id>( this->verts[(i + 1) % N]  )   );
            if ( !a || !b )             { continue; }


            //  Segment curvature: depends on a's OUT and b's IN handles
            const bool      curved      = !( a->IsOutLinear() && b->IsInLinear() );

            if ( !curved )
            {
                //  Straight segment: append anchor 'a' in pixel space
                dl->PathLineTo( callbacks.ws_to_px(ImVec2{ a->x, a->y }) );
            }
            else
            {
                const int   steps   = ctx.args.bezier_fill_steps;
                if ( steps <= 0 )
                {
                    // Degenerate sampling: fall back to anchor 'a'
                    dl->PathLineTo( callbacks.ws_to_px(ImVec2{ a->x, a->y }) );
                }
                else
                {
                    for (int s = 0; s <= steps; ++s)
                    {
                        const float     t       = static_cast<float>(s) / static_cast<float>(steps);
                        const ImVec2    wp      = cubic_eval<vertex_id>(a, b, t);             //  world-space point
                        dl->PathLineTo( ctx.callbacks.ws_to_px(wp) );                   //  append in pixel space
                    }
                }
            }
        }

        //  Fill the constructed outline
        dl->PathFillConvex(this->style.fill_color);
    
        return;
    }
    
    
    
    
    
    
    //  "_render_segment"
    //
    /*template<class CTX>
    inline void	                        _render_segment                     (const V & a, const V & b, const CTX & ctx) const noexcept
    {
        const auto &        callbacks	    = ctx.callbacks;
        ImDrawList *        dl			    = ctx.args.dl;
        
        
        //  Curvature rule: segment is curved iff a.Out NOT linear OR b.In NOT linear
        const bool          curved          = this->SegmentIsCurved( a, b );

        if (!curved)
        {
            const ImVec2        A       = callbacks.ws_to_px({ a->x, a->y });
            const ImVec2        B       = callbacks.ws_to_px({ b->x, b->y });
            dl->AddLine(A, B, this->style.stroke_color, this->style.stroke_width);
            return;
        }

        // Cubic Bezier: P0=a, P1=a+out, P2=b+in, P3=b — all mapped to pixels
        const ImVec2 P0 = callbacks.ws_to_px({ a->x,                             a->y });
        const ImVec2 P1 = callbacks.ws_to_px({ a->x + a->m_bezier.out_handle.x,  a->y + a->m_bezier.out_handle.y });
        const ImVec2 P2 = callbacks.ws_to_px({ b->x + b->m_bezier.in_handle.x,   b->y + b->m_bezier.in_handle.y  });
        const ImVec2 P3 = callbacks.ws_to_px({ b->x,                             b->y });

        dl->AddBezierCubic( P0, P1, P2, P3,
                            this->style.stroke_color,
                            this->style.stroke_width,
                            segs );
        return;
    }*/
    
    
    //  "render_stroke"
    //
    template<class CTX>
    inline void	                        render_stroke                       (const CTX & ctx) const noexcept
    {
        const auto &        callbacks	    = ctx.callbacks;
        ImDrawList *        dl			    = ctx.args.dl;
        const size_t        N			    = this->verts.size();
        const int           segs	        = (ctx.args.bezier_segments > 0) ? ctx.args.bezier_segments : 0; // 0 = ImGui auto


        // Local helper: draw one segment a -> b (straight or cubic)
        auto                draw_seg_si     = [&](size_t si)
        {
            ImVec2 P0, P1, P2, P3;
            if ( !this->segment_control_points(si, P0, P1, P2, P3, ctx) )   { return; }

            // Linear iff effective control points coincide with endpoints
            const bool linear = (P1.x == P0.x && P1.y == P0.y &&
                                 P2.x == P3.x && P2.y == P3.y);

            const ImVec2 A = ctx.callbacks.ws_to_px(P0);
            const ImVec2 B = ctx.callbacks.ws_to_px(P3);

            if (linear) {
                dl->AddLine(A, B, this->style.stroke_color, this->style.stroke_width);
            }
            else {
                const ImVec2 Q1 = ctx.callbacks.ws_to_px(P1);
                const ImVec2 Q2 = ctx.callbacks.ws_to_px(P2);
                dl->AddBezierCubic(A, Q1, Q2, B,
                                   this->style.stroke_color,
                                   this->style.stroke_width,
                                   segs);
            }
        };


        //  Open chain
        for (size_t i = 0; i + 1 < N; ++i)
            draw_seg_si(i);

        //  Close the loop if needed
        if (this->closed)
            draw_seg_si(N - 1);
        
        return;
    }
    
    
    
    /*template<class CTX>
    inline void	                        render_stroke                       (const CTX & ctx) const noexcept
    {
        const auto &        callbacks	    = ctx.callbacks;
        ImDrawList *        dl			    = ctx.args.dl;
        const size_t        N			    = this->verts.size();

        const int           segs	        = (ctx.args.bezier_segments > 0) ? ctx.args.bezier_segments : 0; // 0 = ImGui auto


        // Local helper: draw one segment a -> b (straight or cubic)
        auto draw_seg = [&](const V *a, const V *b)
        {
            if ( !a || !b )     { return; }

            // Curvature rule: segment is curved iff a.Out NOT linear OR b.In NOT linear
            const bool curved = !( a->IsOutLinear() && b->IsInLinear() );

            if (!curved)
            {
                const ImVec2        A       = callbacks.ws_to_px({ a->x, a->y });
                const ImVec2        B       = callbacks.ws_to_px({ b->x, b->y });
                dl->AddLine(A, B, this->style.stroke_color, this->style.stroke_width);
                return;
            }

            // Cubic Bezier: P0=a, P1=a+out, P2=b+in, P3=b — all mapped to pixels
            const ImVec2 P0 = callbacks.ws_to_px({ a->x,                             a->y });
            const ImVec2 P1 = callbacks.ws_to_px({ a->x + a->m_bezier.out_handle.x,  a->y + a->m_bezier.out_handle.y });
            const ImVec2 P2 = callbacks.ws_to_px({ b->x + b->m_bezier.in_handle.x,   b->y + b->m_bezier.in_handle.y  });
            const ImVec2 P3 = callbacks.ws_to_px({ b->x,                             b->y });

            dl->AddBezierCubic( P0, P1, P2, P3,
                                this->style.stroke_color,
                                this->style.stroke_width,
                                segs );
        };


        //  Open chain
        for (size_t i = 0; i + 1 < N; ++i)
        {
            const V *   a   = callbacks.get_vertex( callbacks.vertices, static_cast<vertex_id>( this->verts[i])      );
            const V *   b   = callbacks.get_vertex( callbacks.vertices, static_cast<vertex_id>( this->verts[i + 1])  );
            IM_ASSERT( (a != nullptr) && (b != nullptr)  && "Cannot have NULL Vertices" );
            //  this->_render_segment( *a, *b );
            draw_seg(a, b);
        }

        //  Close the loop if needed
        if (this->closed)
        {
            const V *   a   = callbacks.get_vertex( callbacks.vertices, static_cast<vertex_id>( this->verts[N - 1])  );
            const V *   b   = callbacks.get_vertex( callbacks.vertices, static_cast<vertex_id>( this->verts[0])      );
            IM_ASSERT( (a != nullptr) && (b != nullptr)  && "Cannot have NULL Vertices" );
            //  this->_render_segment( *a, *b );
            draw_seg(a, b);
        }
        
        return;
    }*/
    
    
    
    // *************************************************************************** //
    
//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "INLINE" FUNCTIONS.
    


//
//
//
// *************************************************************************** //
// *************************************************************************** //
};//	END "Path_t" INLINE CLASS DEFINITION.






//  "to_json"           | For Path_t...
//
template< EditorCFGTraits CFG, typename V >
inline void to_json(nlohmann::json & j, const Path_t<CFG, V> & p)
{
    j={
        { "verts"               , p.verts                           },
        { "id"                  , p.id                              },
        { "closed"              , p.closed                          },
        { "style"               , p.style                           },
        { "z_index"             , p.z_index                         },
        { "locked"              , p.locked                          },
        { "visible"             , p.visible                         },
        { "label"               , p.label                           },
        { "kind"                , static_cast<uint8_t>( p.kind )    }
    };
    
    //if ( !std::holds_alternative<std::monostate>(p.payload) )   { j["payload"] = p.payload; }   // Serialises only non-empty payload.
    if ( !std::holds_alternative<std::monostate>(p.payload) ) {
        j["payload"] = std::visit(
            [](const auto& pl) -> nlohmann::json
            {
                using T = std::decay_t<decltype(pl)>;
                
                if constexpr (std::is_same_v<T, std::monostate>)
                        { return nlohmann::json{};      }           //  Never reaches here at run-time...
                else    { return nlohmann::json(pl);    }           //  Uses the payload’s serializer...
            },
            p.payload);
    }
    
    return;
}


//  "from_json"         | For Path_t...
//
template< EditorCFGTraits CFG, typename V >
inline void from_json(const nlohmann::json & j, Path_t<CFG, V> & p)
{
    using   namespace       path;
    bool    has_kind        = j.contains("kind");
    bool    has_payload     = j.contains("payload");
    bool    invalid         = has_kind != has_payload;

    //  1.      ID...
    if ( j.contains("id") )         { j.at("id").get_to(p.id);              }
    else                            { p.id = 0;                             }
    
    //  2.      LABEL...
    if ( j.contains("label") )      { j.at("label").get_to(p.label);        }
    else                            { p.set_label("?");                                                 }
    
    //  3.      Z-INDEX...
    if ( j.contains("z_index") )    { j.at("z_index").get_to(p.z_index);                                }
    else                            { p.z_index = Z_FLOOR_USER;                                         }
    
    //  4.      LOCKED...
    if ( j.contains("locked") )     { j.at("locked").get_to(p.locked);                                  }
    else                            { p.locked = false;                                                 }
    
    //  5.      VISIBLE...
    if ( j.contains("visible") )    { j.at("visible").get_to(p.visible);                                }
    else                            { p.visible = true;                                                 }
    
    
    //  6A.     INVALID FORM OF OBJECT (Must have either BOTH a kind AND payload -- OR NEITHER)...
    if ( invalid )                  { IM_ASSERT(true && "JSON loading error for typename<Path_t>:  object must have either (1) BOTH 'kind' AND 'payload', (2) NEITHER 'kind' NOR 'payload'."); }
    //
    //  6B.     GET "kind"...
    if ( has_kind && !invalid)      { p.kind = static_cast<PathKind>( j.at("kind").get<uint8_t>() );    }
    else                            { p.kind = PathKind::None;                                          }
    //
    //  6C.     GET "payload"...
    if ( has_payload  &&  !invalid )
    {
        const auto & jp = j.at("payload");

        switch (p.kind) {
            case PathKind::Generic:
                p.payload = jp.get<GenericPayload>();
                break;

            case PathKind::Source:
                p.payload = jp.get<SourcePayload>();
                break;

            case PathKind::Boundary:
                p.payload = jp.get<BoundaryPayload>();
                break;

            case PathKind::Dielectric:
                p.payload = jp.get<DielectricPayload>();
                break;

            default:      // Default or unknown kind
                break;
        }
    }
    
    
    //  99.     REMAINING DATA-MEMBERS...
    j.at("verts"    ).get_to(p.verts    );
    j.at("closed"   ).get_to(p.closed   );
    j.at("style"    ).get_to(p.style    );
            
            
            
    return;
}

//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "PATH".

  















// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
}//   END OF "cb" NAMESPACE.












#endif      //  _CBAPP_EDITOR_OBJECTS_PATH_H  //
// *************************************************************************** //
// *************************************************************************** //   END FILE.
