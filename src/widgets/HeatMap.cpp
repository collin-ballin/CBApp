/***********************************************************************************
*
*       ********************************************************************
*       ****           _ H E A T M A P . C P P  ____  F I L E           ****
*       ********************************************************************
*              AUTHOR:      Collin A. Bond.
*               DATED:      April 26, 2025.
*
**************************************************************************************
**************************************************************************************/
#include "widgets/_heatmap.h"
#include "utility/utility.h"



namespace cb { //     BEGINNING NAMESPACE "cb"...
// *************************************************************************** //
// *************************************************************************** //



//  0.      STATIC MEMBERS...
// *************************************************************************** //
// *************************************************************************** //

//  STATIC MEMBERS...
using dim_type      = HeatMap::dim_type;
using value_type    = HeatMap::value_type;

[[maybe_unused]] static constexpr dim_type                               ms_control_width    = dim_type(225);
[[maybe_unused]] static constexpr dim_type                               ms_scale_width      = dim_type(90);
[[maybe_unused]] static constexpr std::pair<dim_type, dim_type>          ms_width_range      = { dim_type(500),          dim_type(1200) };
[[maybe_unused]] static constexpr std::pair<dim_type, dim_type>          ms_height_range     = { dim_type(500),          dim_type(1200) };
[[maybe_unused]] static constexpr std::pair<value_type, value_type>      ms_vlimits          = { value_type(-10),        value_type(10) };






// *************************************************************************** //
//
//
//  1.      INITIALIZATION  | DEFAULT CONSTRUCTOR, DESTRUCTOR, ETC...
// *************************************************************************** //
// *************************************************************************** //

//  Parametric Constructor 1.
//
HeatMap::HeatMap(const size_type rows_, const size_type cols_)
    : m_rows(rows_), m_cols(cols_)
{ this->init(); }


//  Parametric Constructor 2.
//
HeatMap::HeatMap(const char * id_, const size_type rows_, const size_type cols_)
    : m_id(id_), m_rows(rows_), m_cols(cols_)
{ this->init(); }


//  "init"
//
void HeatMap::init(void)
{
    this->m_data.resize(this->m_rows * this->m_cols);
    
    COLORBAR_SIZE   = ImVec2(m_plot_dims[0],    0);
    SCALE_SIZE      = ImVec2(ms_scale_width,    m_plot_dims[1]);
    
    return;
}



//  Destructor.
//
HeatMap::~HeatMap(void)     { this->destroy(); }


//  "destroy"
//
void HeatMap::destroy(void)
{
    this->m_data.clear();
    this->m_id      = nullptr;
    return;
}





// *************************************************************************** //
//
//
//  1B.     PUBLIC MEMBER FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

//
//  ...
//






// *************************************************************************** //
//
//
//  2.      PROTECTED MEMBER FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

//  "draw_plots"
//
void HeatMap::draw_plots(void)
{
    this->m_time += ImGui::GetIO().DeltaTime;
    utl::sinusoid_wave(this->m_data.data(), this->m_rows, this->m_cols, this->m_time, this->m_amp, this->m_freq);


    //  9.  DRAWING THE HEATMAP PLOT...
    //
    //ImPlot::SetupAxes(nullptr, nullptr, ImPlotAxisFlags_NoDecorations, ImPlotAxisFlags_NoDecorations);
    if (ImPlot::BeginPlot("##Heatmap1", this->m_plot_dims, m_plot_flags))
    {
        ImPlot::SetupAxes(nullptr, nullptr, m_axes_flags, m_axes_flags);
        
        ImPlot::SetupAxisTicks(ImAxis_X1,   0 + 1.0/14.0,      1 - 1.0/14.0,    static_cast<int>(m_rows),    nullptr);
        ImPlot::SetupAxisTicks(ImAxis_Y1,   1 - 1.0/14.0,      0 + 1.0/14.0,    static_cast<int>(m_cols),    nullptr);
        ImPlot::PlotHeatmap("heat",                     this->m_data.data(),
                            static_cast<int>(m_rows),   static_cast<int>(m_cols),
                            this->m_vmin,               this->m_vmax,
                            nullptr, ImPlotPoint(0,0), ImPlotPoint(1,1), m_hm_flags);
        ImPlot::EndPlot();
    }
    ImGui::SameLine();
    ImPlot::ColormapScale("##HeatScale", this->m_vmin, this->m_vmax, SCALE_SIZE);
    
    

    return;
}





//  "draw_controls"
//
void HeatMap::draw_controls(void)
{
// -----------------------------------------------------------------------------
//  Heat-map Controls — two-column table, now with Rows / Cols widgets
//  (Keeps all original names & style exactly the same)
// -----------------------------------------------------------------------------
    static constexpr std::pair<HeatMap::size_type, HeatMap::size_type>      ms_XY_range        = { HeatMap::size_type(1), HeatMap::size_type(256) };
    static constexpr std::pair<value_type, value_type>                      ms_amp_range        = { value_type(0.0f), value_type(10.0f)  };
    static constexpr std::pair<value_type, value_type>                      ms_freq_range       = { value_type(0.0f), value_type(5.0) };


    bool   resized = false;           // track whether the matrix size changed

    if (ImGui::BeginTable("heatmap_controls", 2,
                          ImGuiTableFlags_SizingStretchSame |
                          ImGuiTableFlags_NoBordersInBody))
    {
        ImGui::TableSetupColumn("Column0", ImGuiTableColumnFlags_WidthStretch, 1.0f);
        ImGui::TableSetupColumn("Column1", ImGuiTableColumnFlags_WidthStretch, 1.0f);

        // -----------------------------------------------------------------
        //  Row 0  |  Column-major checkbox (spans both columns)
        // -----------------------------------------------------------------
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted("Lock XY Axes");
        ImGui::SameLine();
        ImGui::CheckboxFlags("##LockXYAxes",
                             (unsigned int*)&this->m_plot_flags,
                             ImPlotFlags_Equal);

        ImGui::TableSetColumnIndex(1);
        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted("Column Major");
        ImGui::SameLine();
        ImGui::CheckboxFlags("##ColMajor",
                             (unsigned int*)&this->m_hm_flags,
                             ImPlotHeatmapFlags_ColMajor);

        // -----------------------------------------------------------------
        //  Row 1  |  Plot Width  |  Plot Height
        // -----------------------------------------------------------------
        ImGui::TableNextRow();

        ImGui::TableSetColumnIndex(0);
        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted("Plot Width");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(-FLT_MIN);
        ImGui::SliderFloat("##PlotWidth",
                           &this->m_plot_dims.x,
                           ms_width_range.first, ms_width_range.second,
                           "%.1f px");

        ImGui::TableSetColumnIndex(1);
        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted("Plot Height");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(-FLT_MIN);
        ImGui::SliderFloat("##PlotHeight",
                           &this->m_plot_dims.y,
                           ms_height_range.first, ms_height_range.second,
                           "%.1f px");

        // -----------------------------------------------------------------
        //  Row 2  |  VMin        |  VMax
        // -----------------------------------------------------------------
        ImGui::TableNextRow();

        ImGui::TableSetColumnIndex(0);
        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted("VMin");
        ImGui::SameLine();  ImGui::SetNextItemWidth(-FLT_MIN);
        ImGui::SliderFloat("##VMin", &this->m_vmin,
                           ms_vlimits.first, this->m_vmax,
                           "%.1f");

        ImGui::TableSetColumnIndex(1);
        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted("VMax");
        ImGui::SameLine();  ImGui::SetNextItemWidth(-FLT_MIN);
        ImGui::SliderFloat("##VMax", &this->m_vmax,
                           this->m_vmin, ms_vlimits.second,
                           "%.1f");

        // -----------------------------------------------------------------
        //  Row 3  |  Rows        |  Cols
        // -----------------------------------------------------------------
        ImGui::TableNextRow();

        // 2-0 : number of rows
        ImGui::TableSetColumnIndex(0);
        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted("Rows");
        ImGui::SameLine();  ImGui::SetNextItemWidth(-FLT_MIN);
        int tmp_rows = static_cast<int>(this->m_rows);
        if (ImGui::SliderInt("##Rows", &tmp_rows,
                             static_cast<int>(ms_XY_range.first), static_cast<int>(ms_XY_range.second),
                             "%d")) {
            this->m_rows = static_cast<size_type>(tmp_rows);
            resized = true;
        }

        // 2-1 : number of columns
        ImGui::TableSetColumnIndex(1);
        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted("Cols");
        ImGui::SameLine();  ImGui::SetNextItemWidth(-FLT_MIN);
        int tmp_cols = static_cast<int>(this->m_cols);
        if (ImGui::SliderInt("##Cols", &tmp_cols,
                             static_cast<int>(ms_XY_range.first), static_cast<int>(ms_XY_range.second),
                             "%d")) {
            this->m_cols = static_cast<size_type>(tmp_cols);
            resized = true;
        }

        // -----------------------------------------------------------------
        //  Row 4  |  Amplitude   |  Frequency
        // -----------------------------------------------------------------
        ImGui::TableNextRow();

        ImGui::TableSetColumnIndex(0);
        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted("Amplitude");
        ImGui::SameLine();  ImGui::SetNextItemWidth(-FLT_MIN);
        ImGui::SliderFloat("##Amp", &this->m_amp,
                           ms_amp_range.first, ms_amp_range.second,
                           "%.2f");

        ImGui::TableSetColumnIndex(1);
        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted("Frequency");
        ImGui::SameLine();  ImGui::SetNextItemWidth(-FLT_MIN);
        ImGui::SliderFloat("##Freq", &this->m_freq,
                           ms_freq_range.first, ms_freq_range.second,
                           "%.2f Hz");

        ImGui::EndTable();
    }



    /*  If either dimension changed, resize the data buffer to match
        the new matrix size. */
    if (resized) {
        this->m_data.resize(this->m_rows * this->m_cols);
    }
    
    
    //  4.  COLOR-MAP SELECTOR BUTTON...
    if (ImPlot::ColormapButton(ImPlot::GetColormapName(this->m_cmap), COLORBAR_SIZE, this->m_cmap)) {
        this->m_cmap = (this->m_cmap + 1) % ImPlot::GetColormapCount();       // We bust the color cache of our plots so that item colors will
        ImPlot::BustColorCache("##Heatmap1");               // resample the new colormap in the event that they have already
        ImPlot::BustColorCache("##Heatmap2");               // been created. See documentation in implot.h.
    }
    ImGui::SameLine();
    ImGui::LabelText("##Colormap Index", "%s", "Change Colormap");
    ImPlot::PushColormap(this->m_cmap);


    return;
}






// *************************************************************************** //
//
//
//  3.      PRIVATE MEMBER FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

















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
