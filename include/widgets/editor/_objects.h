/***********************************************************************************
*
*       ********************************************************************
*       ****             _ O B J E C T S . H  ____  F I L E             ****
*       ********************************************************************
*
*              AUTHOR:      Collin A. Bond
*               DATED:      July 14, 2025.
*
*       ********************************************************************
*                FILE:      [./PyStream.h]
*
*
*
**************************************************************************************
**************************************************************************************/
#ifndef _CBWIDGETS_EDITOR_OBJECTS_H
#define _CBWIDGETS_EDITOR_OBJECTS_H  1



//  1.  INCLUDES    | Headers, Modules, etc...
// *************************************************************************** //
// *************************************************************************** //

//  0.1.        ** MY **  HEADERS...
#include CBAPP_USER_CONFIG
#include "cblib.h"
#include "utility/utility.h"
#include "widgets/editor/_constants.h"

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



//      1.      ENUMS / UTILITIES...
// *************************************************************************** //
// *************************************************************************** //

//  "AnchorType"
//
enum class AnchorType : uint8_t     {
    Corner, Smooth, Symmetric,
    COUNT
};
//
static constexpr cblib::EnumArray <AnchorType, const char *>
DEF_ANCHOR_TYPE_NAMES               = {
    "Corner",
    "Smooth",
    "Symmetric"
};
//
static constexpr std::array< ImColor, static_cast<size_t>(AnchorType::COUNT) >
ANCHOR_COLORS                       = { {
    /*  Corner          */      ImColor(255,       255,        0,      255     ),      //   yellow
    /*  Smooth          */      ImColor(  0,       255,        0,      255     ),      //   green
    /*  Symmetric       */      ImColor(  0,       200,        255,    255     )       //   cyan
} };


//  "EndpointInfo"
//
template<typename PID>
struct EndpointInfo_t { PID path_idx; bool prepend; };   // prepend==true ↔ first vertex



// *************************************************************************** //
// *************************************************************************** //   END "Enums".







// *************************************************************************** //
//
//
//
//      2.      [DATA LAYER]    VERTICES...
// *************************************************************************** //
// *************************************************************************** //

//  "Vertex_t" / "Pos" / "Anchor Point"
//
//      A pure geometry node. `in_handle` and `out_handle` are offsets from
//      `pos`; when they are both zero the segment attached to this vertex is
//      straight. Handles live in world‑space units.
//
template<typename VID>
struct Vertex_t {
    static constexpr const char *   ms_DEF_VERTEX_TITLE_FMT_STRING          = "Vertex V%03d (ID #%06u)";
    static constexpr const char *   ms_DEF_VERTEX_SELECTOR_FMT_STRING       = "V%03u";
    static constexpr size_t         ms_MAX_VERTEX_NAME_LENGTH               = 10ULL;
//
    VID         id              = 0;
    float       x               = 0.0f,
                y               = 0.0f;
    //
    ImVec2      in_handle       = ImVec2(0.0f, 0.0f);   // incoming Bézier handle (from previous vertex)
    ImVec2      out_handle      = ImVec2(0.0f, 0.0f);   // outgoing Bézier handle (to next vertex)
    AnchorType  kind            = AnchorType::Corner;
};
//
//  "to_json"
template <typename IdT>
inline void to_json(nlohmann::json& j, const Vertex_t<IdT>& v)
{
    j = {
        { "id",             v.id                                    },
        { "x",              v.x                                     },
        { "y",              v.y                                     },
        { "in_handle",      { v.in_handle.x,  v.in_handle.y    }    },
        { "out_handle",     { v.out_handle.x, v.out_handle.y   }    },
        { "kind",           v.kind                                  }
    };
}
//
//  "from_json"
template <typename IdT>
inline void from_json(const nlohmann::json& j, Vertex_t<IdT>& v)
{
    j.at("id")          .get_to(v.id);
    j.at("x")           .get_to(v.x);
    j.at("y")           .get_to(v.y);
    auto    ih          = j.at("in_handle");        v.in_handle  = { ih[0], ih[1] };
    auto    oh          = j.at("out_handle");       v.out_handle = { oh[0], oh[1] };
    j.at("kind")        .get_to(v.kind);
}

// *************************************************************************** //
// *************************************************************************** //   END "Vertices".






// *************************************************************************** //
//
//
//
//      3.      [DATA LAYER]    POINTS...
// *************************************************************************** //
// *************************************************************************** //

//  "PointStyle"
//
struct PointStyle   {
    ImU32       color       = IM_COL32(0,255,0,255);
    float       radius      = 4.0f;
    bool        visible     = true;
};
//
//  "to_json"
inline void to_json(nlohmann::json& j, const PointStyle& s)
{
    j = nlohmann::json{
        { "color",  s.color   },
        { "radius", s.radius  },
        { "visible",s.visible }
    };
}
//
//  "from_json"
inline void from_json(const nlohmann::json& j, PointStyle& s)
{
    j.at("color" ).get_to(s.color );
    j.at("radius").get_to(s.radius);
    j.at("visible").get_to(s.visible);
}




//  "Point_t"
//
template <typename PtID>
struct Point_t        {
    PtID            v;
    PointStyle      sty{};
};
//
//  "to_json"
template <typename PtID>
inline void to_json(nlohmann::json & j, const Point_t<PtID> & p)
{
    j = {
        { "v",          p.v         },
        { "sty",        p.sty       }
    };
    
    return;
}
//
//  "from_json"
template <typename PtID>
inline void from_json(const nlohmann::json & j, Point_t<PtID> & p)
{
    j.at("v"  )         .get_to(p.v     );
    j.at("sty")         .get_to(p.sty   );
    
    return;
}

// *************************************************************************** //
// *************************************************************************** //   END "POINTS".






// *************************************************************************** //
//
//
//
//      4.      [DATA LAYER]    LINES...
// *************************************************************************** //
// *************************************************************************** //

//  "Line_t"
//
template <typename LID, typename ZID>
struct Line_t         {
    [[nodiscard]] inline bool is_mutable(void) const noexcept
    { return visible && !locked; }
//
//
    LID         a, b;
    ImU32       color       = IM_COL32(255,255,0,255);
    float       thickness   = 2.0f;
//
    ZID         z_index     = Z_FLOOR_USER;
    bool        locked      = false;
    bool        visible     = true;
};
//
//  "to_json"
template <typename LID, typename ZID>
inline void to_json(nlohmann::json  j, const Line_t<LID, ZID> & l)
{
    j = { { "a",            l.a             },
          { "b",            l.b             },
          { "color",        l.color         },
          { "thickness",    l.thickness     },
          { "z_index",      l.z_index       },
          { "locked",       l.locked        },
          { "visible",      l.visible       }
    };
}
//
//  "from_json"
template <typename LID, typename ZID>
inline void from_json(const nlohmann::json & j, Line_t<LID, ZID>  & l)
{
    j.at("a"                ).get_to(l.a);
    j.at("b"                ).get_to(l.b);
    j.at("color"            ).get_to(l.color);
    j.at("z_index"          ).get_to(l.z_index);
    j.at("locked"           ).get_to(l.locked);
    j.at("visible"          ).get_to(l.visible);
}

// *************************************************************************** //
// *************************************************************************** //   END "LINES".






// *************************************************************************** //
//
//
//
//      5.      [DATA LAYER]    PATH EXTENSIONS...
//
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
        { "stroke_color", s.stroke_color },
        { "fill_color",   s.fill_color   },
        { "stroke_width", s.stroke_width }
    };
}


//  "from_json"
//
inline void from_json(const nlohmann::json& j, PathStyle& s)
{
    j.at("stroke_color").get_to(s.stroke_color);
    j.at("fill_color"  ).get_to(s.fill_color  );
    j.at("stroke_width").get_to(s.stroke_width);
}



//  "Path_t"  -- (polyline / spline / area).
//
//      A sequence of vertex IDs. `closed == true` means the last vertex connects
//      back to the first. Vertices carry their own Bézier handles; this struct
//      only stores ordering and style.
//
template<typename PID, typename VID, typename ZID>
struct Path_t {
// *************************************************************************** //
// *************************************************************************** //
//
//
//
//      0.  GLOBAL CONSTANTS, TYPENAME ALIASES, ETC...
// *************************************************************************** //
// *************************************************************************** //
    //                          ALIASES:
    using                           Path                            = Path_t<PID, VID, ZID>;
    using                           Payload                         = path::Payload;
    using                           PathKind                        = path::PathKind;
    //
    //                          CONSTANTS:
    static constexpr size_t         ms_MAX_PATH_LABEL_LENGTH        = 64;
    static constexpr const char *   ms_DEF_PATH_TITLE_FMT_STRING    = "%s (ID #%06u)";
    static constexpr ImU32          ms_DEF_PATH_FILL_COLOR          = IM_COL32(0,110,255,45); 
    
// *************************************************************************** //
//
//
//
//      1.  MEMBER FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

    // *************************************************************************** //
    //                          OPERATION FUNCTIONS:
    // *************************************************************************** //
    
                                    //
                                    //  ...
                                    //
                                    
    // *************************************************************************** //
    //
    //
    //
    // *************************************************************************** //
    //                          UI HELPER FUNCTIONS:
    // *************************************************************************** //
    
    //  "ui_all"
    inline void                     ui_all                          (void)
    {
        using namespace path;
        if ( this->ui_kind() )        { /*    "kind" was changes.     */ }
        
        //  2.  CALL THE "PROPERTIES UI" FOR THE
        std::visit([&](auto & pl) {
            using T = std::decay_t<decltype(pl)>;
            //  skip monostate (has no draw_ui)
            if constexpr (!std::is_same_v<T, std::monostate>)
                { pl.ui_properties(); }          // every real payload implements this
        }, payload);
        
        return;
    }
    
    //  "ui_properties"
    inline void                     ui_properties                   (void)
    {
        using namespace path;
        
        //  2.  CALL THE "PROPERTIES UI" FOR THE
        std::visit([&](auto & pl) {
            using T = std::decay_t<decltype(pl)>;
            //  skip monostate (has no draw_ui)
            if constexpr (!std::is_same_v<T, std::monostate>)
                { pl.ui_properties(); }          // every real payload implements this
        }, payload);
        
        return;
    }
    
    //  "ui_kind"
    inline bool                     ui_kind                         (void)
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
    //
    // *************************************************************************** //
    //                          PATH OPERATION FUNCTIONS:
    // *************************************************************************** //
    
    //  "remove_vertex"
    inline bool                     remove_vertex                   (VID vid) noexcept {
        verts.erase(std::remove(verts.begin(), verts.end(), vid), verts.end());

        if (closed && verts.size() < 3)     { closed = false; }     // cannot stay a polygon

        return verts.size() >= 2;
    }
    
    inline typename std::vector<VID>::iterator
    insert_vertex_after(size_t seg_idx, VID new_vid)
    { return verts.insert(verts.begin() + seg_idx + 1, new_vid); }
                                    
                                    
                                    
    // *************************************************************************** //
    //
    //
    //
    // *************************************************************************** //
    //                          UTILITY FUNCTIONS:
    // *************************************************************************** //
    
    //  "make_default_payload"
    static inline Payload           make_default_payload(const PathKind k)
    {
        switch (k)
        {
            case PathKind::Generic :        { return path::GenericPayload{}; }          // default-constructed
            //
            //
            case PathKind::Boundary :       { return path::BoundaryPayload{}; }
            case PathKind::Source :         { return path::SourcePayload{}; }
            case PathKind::Dielectric :     { return path::DielectricPayload{}; }
            default:                        { break; }                   // PathKind::None or unknown
        }
        
        return std::monostate{};                  // no extra data
    }


    //  "is_area"
    [[nodiscard]] inline bool       is_area                         (void) const noexcept
    { return this->closed && this->verts.size() >= 3; }
    
    //  "is_mutable"
    [[nodiscard]] inline bool       is_mutable                      (void) const noexcept
    { return visible && !locked; }
        
    //  "set_label"
    inline void                     set_label                       (const char * src) noexcept
    { this->label = std::string(src); this->_truncate_label(); }
    
    //  "set_default_label"
    inline void                     set_default_label               (const PID id_) noexcept
    { this->id = id_;   this->label = std::format("Path {:03}", id_);   this->_truncate_label(); }
    
    //  "_truncate_label"
    inline void                     _truncate_label                 (void)
    { if (this->label.size() > ms_MAX_PATH_LABEL_LENGTH) { this->label.resize( ms_MAX_PATH_LABEL_LENGTH ); } }
    
// *************************************************************************** //
//
//
//
//      2.  DATA MEMBERS...
// *************************************************************************** //
// *************************************************************************** //

    // *************************************************************************** //
    //                          CORE:
    // *************************************************************************** //
    std::vector<VID>                verts;   // ordered anchor IDs
    PID                             id                              = 0;
    bool                            closed                          = false;
    PathStyle                       style                           = PathStyle();
    // *************************************************************************** //
    //
    //
    //
    // *************************************************************************** //
    //                          NEW:
    // *************************************************************************** //
    ZID                             z_index                         = Z_FLOOR_USER;
    bool                            locked                          = false;
    bool                            visible                         = true;
    std::string                     label                           = "";
    // *************************************************************************** //
    //
    //
    //
    // *************************************************************************** //
    //                          NEW-ER:
    // *************************************************************************** //
    PathKind                        kind                            = PathKind::None;
    Payload                         payload                         {  };
    

// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "Path_t" CLASS DEFINITION.
};//    END "Path_t" CLASS DEFINITION.




//  "to_json"           | For Path_t...
//
template<typename PID, typename VID, typename ZID>
inline void to_json(nlohmann::json & j, const Path_t<PID, VID, ZID> & p)
{
    j={
        { "verts",        p.verts                             },
        { "id",           p.id                                },
        { "closed",       p.closed                            },
        { "style",        p.style                             },
        { "z_index",      p.z_index                           },
        { "locked",       p.locked                            },
        { "visible",      p.visible                           },
        { "label",        p.label                             },
        { "kind",         static_cast<uint8_t>( p.kind )      }
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
template<typename PID, typename VID, typename ZID>
inline void from_json(const nlohmann::json & j, Path_t<PID, VID, ZID> & p)
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
    if ( has_payload && !invalid)
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




// *************************************************************************** //
// *************************************************************************** //   END "PATH".

  















// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
}//   END OF "cb" NAMESPACE.












#endif      //  _CBWIDGETS_EDITOR_OBJECTS_H  //
// *************************************************************************** //
// *************************************************************************** //   END FILE.
