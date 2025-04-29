//
//  HeatMap.cpp
//  CBApp
//
//  Created by Collin Bond on 4/26/25.
//
// *************************************************************************** //
// *************************************************************************** //
#include "widgets/widgets.h"



namespace cb { //     BEGINNING NAMESPACE "cb"...
// *************************************************************************** //
// *************************************************************************** //


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

//  STATIC MEMBERS...
using dim_type      = HeatMap::dim_type;
using value_type    = HeatMap::value_type;

static constexpr dim_type                               ms_control_width    = dim_type(225);
static constexpr dim_type                               ms_scale_width      = dim_type(90);
static constexpr std::pair<dim_type, dim_type>          ms_width_range      = { dim_type(500),          dim_type(1200) };
static constexpr std::pair<dim_type, dim_type>          ms_height_range     = { dim_type(500),          dim_type(1200) };
static constexpr std::pair<value_type, value_type>      ms_vlimits           = { value_type(-10),        value_type(10) };






//  "Begin"
//
void HeatMap::Begin(void)
{
    COLORBAR_SIZE       = ImVec2(m_plot_dims[0],    0);
    SCALE_SIZE          = ImVec2(ms_scale_width,    m_plot_dims[1]);
    
    
    
    //ImGui::Begin("Plot Controls", nullptr);
    //    this->draw_controls();
    //ImGui::End();
    
    
    //ImGui::Begin("Demo", nullptr);
    this->test_controls();
    //ImGui::End();
    
    
    
    
    
    
    
    ImGui::Begin("Demo", nullptr);
        this->demo();
    ImGui::End();
    

    return;
}


//  "test_controls"
//
void HeatMap::test_controls(void)
{
    //  CONSTANTS...
    static ImGuiChildFlags      child_flags             = ImGuiWindowFlags_None | ImGuiChildFlags_Borders | ImGuiChildFlags_AutoResizeY;
    static ImGuiWindowFlags     window_flags            = ImGuiWindowFlags_None; //| ImGuiWindowFlags_MenuBar;
    static constexpr float      CORNER_RADIUS           = 5.0f;
    static const float          CONTROL_WIDTH           = ImGui::GetFontSize() * 8;
    
    
    //  WIDGET VARIABLES...
    static std::pair<int,int>   HEIGHT_LIMIT_IN_LINES   = {9, 15};  //  {y1, y2}    : "y1"  --- min height of child (no matter how many widgets are on it).     "y2"  --- max height the window will extend to WITHOUT adding scrollbar.
    static int                  draw_lines              = 3; 
      
    static ImVec2               win_dims_min, win_dims_max;
    static ImVec2               size_arg                = ImVec2(-FLT_MIN,      0.0f);
      
      
    win_dims_min    = ImVec2(0.0f,          ImGui::GetTextLineHeightWithSpacing() * HEIGHT_LIMIT_IN_LINES.first);
    win_dims_max    = ImVec2(FLT_MAX,       ImGui::GetTextLineHeightWithSpacing() * HEIGHT_LIMIT_IN_LINES.second);
    
      
    //  1.  DRAWING CHILD-WINDOW...
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding,        CORNER_RADIUS);
    ImGui::SetNextWindowSizeConstraints( win_dims_min,      win_dims_max );
    ImGui::BeginChild("ConstrainedChild", size_arg,         child_flags);
    
    
    //  2.  SET TITLE OF CHILD-WINDOW WITH COLLAPSING HEADER...
    ImGui::SetNextItemOpen(true, ImGuiCond_Once);
    if (ImGui::CollapsingHeader("Controls"))
    {
    
        //  1.  INITIAL WIDGETS...
        ImGui::SetNextItemWidth( CONTROL_WIDTH );
        ImGui::DragInt("Min Height (in Lines)", &HEIGHT_LIMIT_IN_LINES.first, 0.2f);
        
        ImGui::SetNextItemWidth( CONTROL_WIDTH );
        ImGui::DragInt("Max Height (in Lines)", &HEIGHT_LIMIT_IN_LINES.second, 0.2f);
        
        ImGui::SetNextItemWidth( CONTROL_WIDTH );
        ImGui::DragInt("Lines Count", &draw_lines, 0.2f);
    
    
    
        //  2.  OTHER WIDGETS...
        for (int n = 0; n < draw_lines; n++)
            ImGui::Text("Line %04d", n);
        
        
        
        //  3.  MY WIDGETS...
        ImGui::SeparatorText("this->draw_controls()");
        this->draw_controls();
        

        
    }
    //  END "Child Window".
    ImGui::EndChild();
    ImGui::PopStyleVar();





    return;
}





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

//  "demo"
//
void HeatMap::demo(void)
{
    if (!ImGui::CollapsingHeader("Layout & Scrolling"))
        return;

    if (ImGui::TreeNode("Child windows"))
    {
        ImGui::SeparatorText("Child windows");

        static bool disable_mouse_wheel = false;
        static bool disable_menu = false;
        ImGui::Checkbox("Disable Mouse Wheel", &disable_mouse_wheel);
        ImGui::Checkbox("Disable Menu", &disable_menu);

        // Child 1: no border, enable horizontal scrollbar
        {
            ImGuiWindowFlags window_flags = ImGuiWindowFlags_HorizontalScrollbar;
            if (disable_mouse_wheel)
                window_flags |= ImGuiWindowFlags_NoScrollWithMouse;
            ImGui::BeginChild("ChildL", ImVec2(ImGui::GetContentRegionAvail().x * 0.5f, 260), ImGuiChildFlags_None, window_flags);
            for (int i = 0; i < 100; i++)
                ImGui::Text("%04d: scrollable region", i);
            ImGui::EndChild();
        }

        ImGui::SameLine();

        // Child 2: rounded border
        {
            ImGuiWindowFlags window_flags = ImGuiWindowFlags_None;
            if (disable_mouse_wheel)
                window_flags |= ImGuiWindowFlags_NoScrollWithMouse;
            if (!disable_menu)
                window_flags |= ImGuiWindowFlags_MenuBar;
            ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
            ImGui::BeginChild("ChildR", ImVec2(0, 260), ImGuiChildFlags_Borders, window_flags);
            if (!disable_menu && ImGui::BeginMenuBar())
            {
                if (ImGui::BeginMenu("Menu"))
                {
                    ImGui::EndMenu();
                }
                ImGui::EndMenuBar();
            }
            if (ImGui::BeginTable("split", 2, ImGuiTableFlags_Resizable | ImGuiTableFlags_NoSavedSettings))
            {
                for (int i = 0; i < 100; i++)
                {
                    char buf[32];
                    ImGui::TableNextColumn();
                    ImGui::Button(buf, ImVec2(-FLT_MIN, 0.0f));
                }
                ImGui::EndTable();
            }
            ImGui::EndChild();
            ImGui::PopStyleVar();
        }

        // Child 3: manual-resize
        ImGui::SeparatorText("Manual-resize");
        {
        

            ImGui::PushStyleColor(ImGuiCol_ChildBg, ImGui::GetStyleColorVec4(ImGuiCol_FrameBg));
            if (ImGui::BeginChild("ResizableChild", ImVec2(-FLT_MIN, ImGui::GetTextLineHeightWithSpacing() * 8), ImGuiChildFlags_Borders | ImGuiChildFlags_ResizeY))
                for (int n = 0; n < 10; n++)
                    ImGui::Text("Line %04d", n);
            ImGui::PopStyleColor();
            ImGui::EndChild();
        }

        // Child 4: auto-resizing height with a limit
        ImGui::SeparatorText("Auto-resize with constraints");
        {
            static int draw_lines = 3;
            static int max_height_in_lines = 10;
            ImGui::SetNextItemWidth(ImGui::GetFontSize() * 8);
            ImGui::DragInt("Lines Count", &draw_lines, 0.2f);
            ImGui::SetNextItemWidth(ImGui::GetFontSize() * 8);
            ImGui::DragInt("Max Height (in Lines)", &max_height_in_lines, 0.2f);

            ImGui::SetNextWindowSizeConstraints(ImVec2(0.0f, ImGui::GetTextLineHeightWithSpacing() * 1), ImVec2(FLT_MAX, ImGui::GetTextLineHeightWithSpacing() * max_height_in_lines));
            if (ImGui::BeginChild("ConstrainedChild", ImVec2(-FLT_MIN, 0.0f), ImGuiChildFlags_Borders | ImGuiChildFlags_AutoResizeY))
                for (int n = 0; n < draw_lines; n++)
                    ImGui::Text("Line %04d", n);
            ImGui::EndChild();
        }

        ImGui::SeparatorText("Misc/Advanced");

        // Demonstrate a few extra things
        // - Changing ImGuiCol_ChildBg (which is transparent black in default styles)
        // - Using SetCursorPos() to position child window (the child window is an item from the POV of parent window)
        //   You can also call SetNextWindowPos() to position the child window. The parent window will effectively
        //   layout from this position.
        // - Using ImGui::GetItemRectMin/Max() to query the "item" state (because the child window is an item from
        //   the POV of the parent window). See 'Demo->Querying Status (Edited/Active/Hovered etc.)' for details.
        {
            static int offset_x = 0;
            static bool override_bg_color = true;
            static ImGuiChildFlags child_flags = ImGuiChildFlags_Borders | ImGuiChildFlags_ResizeX | ImGuiChildFlags_ResizeY;
            ImGui::SetNextItemWidth(ImGui::GetFontSize() * 8);
            ImGui::DragInt("Offset X", &offset_x, 1.0f, -1000, 1000);
            ImGui::Checkbox("Override ChildBg color", &override_bg_color);
            ImGui::CheckboxFlags("ImGuiChildFlags_Borders", &child_flags, ImGuiChildFlags_Borders);
            ImGui::CheckboxFlags("ImGuiChildFlags_AlwaysUseWindowPadding", &child_flags, ImGuiChildFlags_AlwaysUseWindowPadding);
            ImGui::CheckboxFlags("ImGuiChildFlags_ResizeX", &child_flags, ImGuiChildFlags_ResizeX);
            ImGui::CheckboxFlags("ImGuiChildFlags_ResizeY", &child_flags, ImGuiChildFlags_ResizeY);
            ImGui::CheckboxFlags("ImGuiChildFlags_FrameStyle", &child_flags, ImGuiChildFlags_FrameStyle);
            if (child_flags & ImGuiChildFlags_FrameStyle)
                override_bg_color = false;

            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (float)offset_x);
            if (override_bg_color)
                ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(255, 0, 0, 100));
            ImGui::BeginChild("Red", ImVec2(200, 100), child_flags, ImGuiWindowFlags_None);
            if (override_bg_color)
                ImGui::PopStyleColor();

            for (int n = 0; n < 50; n++)
                ImGui::Text("Some test %d", n);
            ImGui::EndChild();
            bool child_is_hovered = ImGui::IsItemHovered();
            ImVec2 child_rect_min = ImGui::GetItemRectMin();
            ImVec2 child_rect_max = ImGui::GetItemRectMax();
            ImGui::Text("Hovered: %d", child_is_hovered);
            ImGui::Text("Rect of child window is: (%.0f,%.0f) (%.0f,%.0f)", child_rect_min.x, child_rect_min.y, child_rect_max.x, child_rect_max.y);
        }

        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Widgets Width"))
    {
        static float f = 0.0f;
        static bool show_indented_items = true;
        ImGui::Checkbox("Show indented items", &show_indented_items);

        // Use SetNextItemWidth() to set the width of a single upcoming item.
        // Use PushItemWidth()/PopItemWidth() to set the width of a group of items.
        // In real code use you'll probably want to choose width values that are proportional to your font size
        // e.g. Using '20.0f * GetFontSize()' as width instead of '200.0f', etc.

        ImGui::Text("SetNextItemWidth/PushItemWidth(100)");
        ImGui::PushItemWidth(100);
        ImGui::DragFloat("float##1b", &f);
        if (show_indented_items)
        {
            ImGui::Indent();
            ImGui::DragFloat("float (indented)##1b", &f);
            ImGui::Unindent();
        }
        ImGui::PopItemWidth();

        ImGui::Text("SetNextItemWidth/PushItemWidth(-100)");
        ImGui::PushItemWidth(-100);
        ImGui::DragFloat("float##2a", &f);
        if (show_indented_items)
        {
            ImGui::Indent();
            ImGui::DragFloat("float (indented)##2b", &f);
            ImGui::Unindent();
        }
        ImGui::PopItemWidth();

        ImGui::Text("SetNextItemWidth/PushItemWidth(GetContentRegionAvail().x * 0.5f)");
        ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x * 0.5f);
        ImGui::DragFloat("float##3a", &f);
        if (show_indented_items)
        {
            ImGui::Indent();
            ImGui::DragFloat("float (indented)##3b", &f);
            ImGui::Unindent();
        }
        ImGui::PopItemWidth();

        ImGui::Text("SetNextItemWidth/PushItemWidth(-GetContentRegionAvail().x * 0.5f)");
        ImGui::PushItemWidth(-ImGui::GetContentRegionAvail().x * 0.5f);
        ImGui::DragFloat("float##4a", &f);
        if (show_indented_items)
        {
            ImGui::Indent();
            ImGui::DragFloat("float (indented)##4b", &f);
            ImGui::Unindent();
        }
        ImGui::PopItemWidth();

        // Demonstrate using PushItemWidth to surround three items.
        // Calling SetNextItemWidth() before each of them would have the same effect.
        ImGui::Text("SetNextItemWidth/PushItemWidth(-FLT_MIN)");
        ImGui::PushItemWidth(-FLT_MIN);
        ImGui::DragFloat("##float5a", &f);
        if (show_indented_items)
        {
            ImGui::Indent();
            ImGui::DragFloat("float (indented)##5b", &f);
            ImGui::Unindent();
        }
        ImGui::PopItemWidth();

        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Basic Horizontal Layout"))
    {
        ImGui::TextWrapped("(Use ImGui::SameLine() to keep adding items to the right of the preceding item)");

        // Text
        ImGui::Text("Two items: Hello"); ImGui::SameLine();
        ImGui::TextColored(ImVec4(1, 1, 0, 1), "Sailor");

        // Adjust spacing
        ImGui::Text("More spacing: Hello"); ImGui::SameLine(0, 20);
        ImGui::TextColored(ImVec4(1, 1, 0, 1), "Sailor");

        // Button
        ImGui::AlignTextToFramePadding();
        ImGui::Text("Normal buttons"); ImGui::SameLine();
        ImGui::Button("Banana"); ImGui::SameLine();
        ImGui::Button("Apple"); ImGui::SameLine();
        ImGui::Button("Corniflower");

        // Button
        ImGui::Text("Small buttons"); ImGui::SameLine();
        ImGui::SmallButton("Like this one"); ImGui::SameLine();
        ImGui::Text("can fit within a text block.");

        // Aligned to arbitrary position. Easy/cheap column.
        ImGui::Text("Aligned");
        ImGui::SameLine(150); ImGui::Text("x=150");
        ImGui::SameLine(300); ImGui::Text("x=300");
        ImGui::Text("Aligned");
        ImGui::SameLine(150); ImGui::SmallButton("x=150");
        ImGui::SameLine(300); ImGui::SmallButton("x=300");

        // Checkbox
        static bool c1 = false, c2 = false, c3 = false, c4 = false;
        ImGui::Checkbox("My", &c1); ImGui::SameLine();
        ImGui::Checkbox("Tailor", &c2); ImGui::SameLine();
        ImGui::Checkbox("Is", &c3); ImGui::SameLine();
        ImGui::Checkbox("Rich", &c4);

        // Various
        static float f0 = 1.0f, f1 = 2.0f, f2 = 3.0f;
        ImGui::PushItemWidth(80);
        const char* items[] = { "AAAA", "BBBB", "CCCC", "DDDD" };
        static int item = -1;
        ImGui::Combo("Combo", &item, items, IM_ARRAYSIZE(items)); ImGui::SameLine();
        ImGui::SliderFloat("X", &f0, 0.0f, 5.0f); ImGui::SameLine();
        ImGui::SliderFloat("Y", &f1, 0.0f, 5.0f); ImGui::SameLine();
        ImGui::SliderFloat("Z", &f2, 0.0f, 5.0f);
        ImGui::PopItemWidth();

        ImGui::PushItemWidth(80);
        ImGui::Text("Lists:");
        static int selection[4] = { 0, 1, 2, 3 };
        for (int i = 0; i < 4; i++)
        {
            if (i > 0) ImGui::SameLine();
            ImGui::PushID(i);
            ImGui::ListBox("", &selection[i], items, IM_ARRAYSIZE(items));
            ImGui::PopID();
            //ImGui::SetItemTooltip("ListBox %d hovered", i);
        }
        ImGui::PopItemWidth();

        // Dummy
        ImVec2 button_sz(40, 40);
        ImGui::Button("A", button_sz); ImGui::SameLine();
        ImGui::Dummy(button_sz); ImGui::SameLine();
        ImGui::Button("B", button_sz);

        // Manually wrapping
        // (we should eventually provide this as an automatic layout feature, but for now you can do it manually)
        ImGui::Text("Manual wrapping:");
        ImGuiStyle& style = ImGui::GetStyle();
        int buttons_count = 20;
        float window_visible_x2 = ImGui::GetCursorScreenPos().x + ImGui::GetContentRegionAvail().x;
        for (int n = 0; n < buttons_count; n++)
        {
            ImGui::PushID(n);
            ImGui::Button("Box", button_sz);
            float last_button_x2 = ImGui::GetItemRectMax().x;
            float next_button_x2 = last_button_x2 + style.ItemSpacing.x + button_sz.x; // Expected position if next button was on same line
            if (n + 1 < buttons_count && next_button_x2 < window_visible_x2)
                ImGui::SameLine();
            ImGui::PopID();
        }

        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Groups"))
    {
        ImGui::BeginGroup();
        {
            ImGui::BeginGroup();
            ImGui::Button("AAA");
            ImGui::SameLine();
            ImGui::Button("BBB");
            ImGui::SameLine();
            ImGui::BeginGroup();
            ImGui::Button("CCC");
            ImGui::Button("DDD");
            ImGui::EndGroup();
            ImGui::SameLine();
            ImGui::Button("EEE");
            ImGui::EndGroup();
            ImGui::SetItemTooltip("First group hovered");
        }
        // Capture the group size and create widgets using the same size
        ImVec2 size = ImGui::GetItemRectSize();
        const float values[5] = { 0.5f, 0.20f, 0.80f, 0.60f, 0.25f };
        ImGui::PlotHistogram("##values", values, IM_ARRAYSIZE(values), 0, NULL, 0.0f, 1.0f, size);

        ImGui::Button("ACTION", ImVec2((size.x - ImGui::GetStyle().ItemSpacing.x) * 0.5f, size.y));
        ImGui::SameLine();
        ImGui::Button("REACTION", ImVec2((size.x - ImGui::GetStyle().ItemSpacing.x) * 0.5f, size.y));
        ImGui::EndGroup();
        ImGui::SameLine();

        ImGui::Button("LEVERAGE\nBUZZWORD", size);
        ImGui::SameLine();

        if (ImGui::BeginListBox("List", size))
        {
            ImGui::Selectable("Selected", true);
            ImGui::Selectable("Not Selected", false);
            ImGui::EndListBox();
        }

        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Text Baseline Alignment"))
    {
        {
            ImGui::BulletText("Text baseline:");
            ImGui::Indent();

            ImGui::Text("KO Blahblah"); ImGui::SameLine();
            ImGui::Button("Some framed item"); ImGui::SameLine();

            // If your line starts with text, call AlignTextToFramePadding() to align text to upcoming widgets.
            // (because we don't know what's coming after the Text() statement, we need to move the text baseline
            // down by FramePadding.y ahead of time)
            ImGui::AlignTextToFramePadding();
            ImGui::Text("OK Blahblah"); ImGui::SameLine();
            ImGui::Button("Some framed item##2"); ImGui::SameLine();
            
            // SmallButton() uses the same vertical padding as Text
            ImGui::Button("TEST##1"); ImGui::SameLine();
            ImGui::Text("TEST"); ImGui::SameLine();
            ImGui::SmallButton("TEST##2");

            // If your line starts with text, call AlignTextToFramePadding() to align text to upcoming widgets.
            ImGui::AlignTextToFramePadding();
            ImGui::Text("Text aligned to framed item"); ImGui::SameLine();
            ImGui::Button("Item##1"); ImGui::SameLine();
            ImGui::Text("Item"); ImGui::SameLine();
            ImGui::SmallButton("Item##2"); ImGui::SameLine();
            ImGui::Button("Item##3");

            ImGui::Unindent();
        }

        ImGui::Spacing();

        {
            ImGui::BulletText("Multi-line text:");
            ImGui::Indent();
            ImGui::Text("One\nTwo\nThree"); ImGui::SameLine();
            ImGui::Text("Hello\nWorld"); ImGui::SameLine();
            ImGui::Text("Banana");

            ImGui::Text("Banana"); ImGui::SameLine();
            ImGui::Text("Hello\nWorld"); ImGui::SameLine();
            ImGui::Text("One\nTwo\nThree");

            ImGui::Button("HOP##1"); ImGui::SameLine();
            ImGui::Text("Banana"); ImGui::SameLine();
            ImGui::Text("Hello\nWorld"); ImGui::SameLine();
            ImGui::Text("Banana");

            ImGui::Button("HOP##2"); ImGui::SameLine();
            ImGui::Text("Hello\nWorld"); ImGui::SameLine();
            ImGui::Text("Banana");
            ImGui::Unindent();
        }

        ImGui::Spacing();

        {
            ImGui::BulletText("Misc items:");
            ImGui::Indent();

            // SmallButton() sets FramePadding to zero. Text baseline is aligned to match baseline of previous Button.
            ImGui::Button("80x80", ImVec2(80, 80));
            ImGui::SameLine();
            ImGui::Button("50x50", ImVec2(50, 50));
            ImGui::SameLine();
            ImGui::Button("Button()");
            ImGui::SameLine();
            ImGui::SmallButton("SmallButton()");

            // Tree
            // (here the node appears after a button and has odd intent, so we use ImGuiTreeNodeFlags_DrawLinesNone to disable hierarchy oappine)
            const float spacing = ImGui::GetStyle().ItemInnerSpacing.x;
            ImGui::Button("Button##1");
            ImGui::SameLine(0.0f, spacing);
            if (ImGui::TreeNodeEx("Node##1", ImGuiTreeNodeFlags_DrawLinesNone))
            {
                // Placeholder tree data
                for (int i = 0; i < 6; i++)
                    ImGui::BulletText("Item %d..", i);
                ImGui::TreePop();
            }

            // Vertically align text node a bit lower so it'll be vertically centered with upcoming widget.
            // Otherwise you can use SmallButton() (smaller fit).
            ImGui::AlignTextToFramePadding();

            // Common mistake to avoid: if we want to SameLine after TreeNode we need to do it before we add
            // other contents below the node.
            bool node_open = ImGui::TreeNode("Node##2");
            ImGui::SameLine(0.0f, spacing); ImGui::Button("Button##2");
            if (node_open)
            {
                // Placeholder tree data
                for (int i = 0; i < 6; i++)
                    ImGui::BulletText("Item %d..", i);
                ImGui::TreePop();
            }

            // Bullet
            ImGui::Button("Button##3");
            ImGui::SameLine(0.0f, spacing);
            ImGui::BulletText("Bullet text");

            ImGui::AlignTextToFramePadding();
            ImGui::BulletText("Node");
            ImGui::SameLine(0.0f, spacing); ImGui::Button("Button##4");
            ImGui::Unindent();
        }

        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Scrolling"))
    {
        // Vertical scroll functions

        static int track_item = 50;
        static bool enable_track = true;
        static bool enable_extra_decorations = false;
        static float scroll_to_off_px = 0.0f;
        static float scroll_to_pos_px = 200.0f;

        ImGui::Checkbox("Decoration", &enable_extra_decorations);

        ImGui::Checkbox("Track", &enable_track);
        ImGui::PushItemWidth(100);
        ImGui::SameLine(140); enable_track |= ImGui::DragInt("##item", &track_item, 0.25f, 0, 99, "Item = %d");

        bool scroll_to_off = ImGui::Button("Scroll Offset");
        ImGui::SameLine(140); scroll_to_off |= ImGui::DragFloat("##off", &scroll_to_off_px, 1.00f, 0, FLT_MAX, "+%.0f px");

        bool scroll_to_pos = ImGui::Button("Scroll To Pos");
        ImGui::SameLine(140); scroll_to_pos |= ImGui::DragFloat("##pos", &scroll_to_pos_px, 1.00f, -10, FLT_MAX, "X/Y = %.0f px");
        ImGui::PopItemWidth();

        if (scroll_to_off || scroll_to_pos)
            enable_track = false;

        ImGuiStyle& style = ImGui::GetStyle();
        float child_w = (ImGui::GetContentRegionAvail().x - 4 * style.ItemSpacing.x) / 5;
        if (child_w < 1.0f)
            child_w = 1.0f;
        ImGui::PushID("##VerticalScrolling");
        for (int i = 0; i < 5; i++)
        {
            if (i > 0) ImGui::SameLine();
            ImGui::BeginGroup();
            const char* names[] = { "Top", "25%", "Center", "75%", "Bottom" };
            ImGui::TextUnformatted(names[i]);

            const ImGuiWindowFlags child_flags = enable_extra_decorations ? ImGuiWindowFlags_MenuBar : 0;
            const ImGuiID child_id = ImGui::GetID((void*)(intptr_t)i);
            const bool child_is_visible = ImGui::BeginChild(child_id, ImVec2(child_w, 200.0f), ImGuiChildFlags_Borders, child_flags);
            if (ImGui::BeginMenuBar())
            {
                ImGui::TextUnformatted("abc");
                ImGui::EndMenuBar();
            }
            if (scroll_to_off)
                ImGui::SetScrollY(scroll_to_off_px);
            if (scroll_to_pos)
                ImGui::SetScrollFromPosY(ImGui::GetCursorStartPos().y + scroll_to_pos_px, i * 0.25f);
            if (child_is_visible) // Avoid calling SetScrollHereY when running with culled items
            {
                for (int item = 0; item < 100; item++)
                {
                    if (enable_track && item == track_item)
                    {
                        ImGui::TextColored(ImVec4(1, 1, 0, 1), "Item %d", item);
                        ImGui::SetScrollHereY(i * 0.25f); // 0.0f:top, 0.5f:center, 1.0f:bottom
                    }
                    else
                    {
                        ImGui::Text("Item %d", item);
                    }
                }
            }
            float scroll_y = ImGui::GetScrollY();
            float scroll_max_y = ImGui::GetScrollMaxY();
            ImGui::EndChild();
            ImGui::Text("%.0f/%.0f", scroll_y, scroll_max_y);
            ImGui::EndGroup();
        }
        ImGui::PopID();

        // Horizontal scroll functions
        ImGui::Spacing();
        ImGui::PushID("##HorizontalScrolling");
        for (int i = 0; i < 5; i++)
        {
            float child_height = ImGui::GetTextLineHeight() + style.ScrollbarSize + style.WindowPadding.y * 2.0f;
            ImGuiWindowFlags child_flags = ImGuiWindowFlags_HorizontalScrollbar | (enable_extra_decorations ? ImGuiWindowFlags_AlwaysVerticalScrollbar : 0);
            ImGuiID child_id = ImGui::GetID((void*)(intptr_t)i);
            bool child_is_visible = ImGui::BeginChild(child_id, ImVec2(-100, child_height), ImGuiChildFlags_Borders, child_flags);
            if (scroll_to_off)
                ImGui::SetScrollX(scroll_to_off_px);
            if (scroll_to_pos)
                ImGui::SetScrollFromPosX(ImGui::GetCursorStartPos().x + scroll_to_pos_px, i * 0.25f);
            if (child_is_visible) // Avoid calling SetScrollHereY when running with culled items
            {
                for (int item = 0; item < 100; item++)
                {
                    if (item > 0)
                        ImGui::SameLine();
                    if (enable_track && item == track_item)
                    {
                        ImGui::TextColored(ImVec4(1, 1, 0, 1), "Item %d", item);
                        ImGui::SetScrollHereX(i * 0.25f); // 0.0f:left, 0.5f:center, 1.0f:right
                    }
                    else
                    {
                        ImGui::Text("Item %d", item);
                    }
                }
            }
            float scroll_x = ImGui::GetScrollX();
            float scroll_max_x = ImGui::GetScrollMaxX();
            ImGui::EndChild();
            ImGui::SameLine();
            const char* names[] = { "Left", "25%", "Center", "75%", "Right" };
            ImGui::Text("%s\n%.0f/%.0f", names[i], scroll_x, scroll_max_x);
            ImGui::Spacing();
        }
        ImGui::PopID();

        // Miscellaneous Horizontal Scrolling Demo
        static int lines = 7;
        ImGui::SliderInt("Lines", &lines, 1, 15);
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2.0f, 1.0f));
        ImVec2 scrolling_child_size = ImVec2(0, ImGui::GetFrameHeightWithSpacing() * 7 + 30);
        ImGui::BeginChild("scrolling", scrolling_child_size, ImGuiChildFlags_Borders, ImGuiWindowFlags_HorizontalScrollbar);
        for (int line = 0; line < lines; line++)
        {
            // Display random stuff. For the sake of this trivial demo we are using basic Button() + SameLine()
            // If you want to create your own time line for a real application you may be better off manipulating
            // the cursor position yourself, aka using SetCursorPos/SetCursorScreenPos to position the widgets
            // yourself. You may also want to use the lower-level ImDrawList API.
            int num_buttons = 10 + ((line & 1) ? line * 9 : line * 3);
            for (int n = 0; n < num_buttons; n++)
            {
                if (n > 0) ImGui::SameLine();
                ImGui::PushID(n + line * 1000);
                char num_buf[16];
                sprintf(num_buf, "%d", n);
                const char* label = (!(n % 15)) ? "FizzBuzz" : (!(n % 3)) ? "Fizz" : (!(n % 5)) ? "Buzz" : num_buf;
                float hue = n * 0.05f;
                ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(hue, 0.6f, 0.6f));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(hue, 0.7f, 0.7f));
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(hue, 0.8f, 0.8f));
                ImGui::Button(label, ImVec2(40.0f + sinf((float)(line + n)) * 20.0f, 0.0f));
                ImGui::PopStyleColor(3);
                ImGui::PopID();
            }
        }
        float scroll_x = ImGui::GetScrollX();
        float scroll_max_x = ImGui::GetScrollMaxX();
        ImGui::EndChild();
        ImGui::PopStyleVar(2);
        float scroll_x_delta = 0.0f;
        ImGui::SmallButton("<<");
        if (ImGui::IsItemActive())
            scroll_x_delta = -ImGui::GetIO().DeltaTime * 1000.0f;
        ImGui::SameLine();
        ImGui::Text("Scroll from code"); ImGui::SameLine();
        ImGui::SmallButton(">>");
        if (ImGui::IsItemActive())
            scroll_x_delta = +ImGui::GetIO().DeltaTime * 1000.0f;
        ImGui::SameLine();
        ImGui::Text("%.0f/%.0f", scroll_x, scroll_max_x);
        if (scroll_x_delta != 0.0f)
        {
            // Demonstrate a trick: you can use Begin to set yourself in the context of another window
            // (here we are already out of your child window)
            ImGui::BeginChild("scrolling");
            ImGui::SetScrollX(ImGui::GetScrollX() + scroll_x_delta);
            ImGui::EndChild();
        }
        ImGui::Spacing();

        static bool show_horizontal_contents_size_demo_window = false;
        ImGui::Checkbox("Show Horizontal contents size demo window", &show_horizontal_contents_size_demo_window);

        if (show_horizontal_contents_size_demo_window)
        {
            static bool show_h_scrollbar = true;
            static bool show_button = true;
            static bool show_tree_nodes = true;
            static bool show_text_wrapped = false;
            static bool show_columns = true;
            static bool show_tab_bar = true;
            static bool show_child = false;
            static bool explicit_content_size = false;
            static float contents_size_x = 300.0f;
            if (explicit_content_size)
                ImGui::SetNextWindowContentSize(ImVec2(contents_size_x, 0.0f));
            ImGui::Begin("Horizontal contents size demo window", &show_horizontal_contents_size_demo_window, show_h_scrollbar ? ImGuiWindowFlags_HorizontalScrollbar : 0);
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(2, 0));
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 0));
            
            ImGui::Checkbox("H-scrollbar", &show_h_scrollbar);
            ImGui::Checkbox("Button", &show_button);            // Will grow contents size (unless explicitly overwritten)
            ImGui::Checkbox("Tree nodes", &show_tree_nodes);    // Will grow contents size and display highlight over full width
            ImGui::Checkbox("Text wrapped", &show_text_wrapped);// Will grow and use contents size
            ImGui::Checkbox("Columns", &show_columns);          // Will use contents size
            ImGui::Checkbox("Tab bar", &show_tab_bar);          // Will use contents size
            ImGui::Checkbox("Child", &show_child);              // Will grow and use contents size
            ImGui::Checkbox("Explicit content size", &explicit_content_size);
            ImGui::Text("Scroll %.1f/%.1f %.1f/%.1f", ImGui::GetScrollX(), ImGui::GetScrollMaxX(), ImGui::GetScrollY(), ImGui::GetScrollMaxY());
            if (explicit_content_size)
            {
                ImGui::SameLine();
                ImGui::SetNextItemWidth(100);
                ImGui::DragFloat("##csx", &contents_size_x);
                ImVec2 p = ImGui::GetCursorScreenPos();
                ImGui::GetWindowDrawList()->AddRectFilled(p, ImVec2(p.x + 10, p.y + 10), IM_COL32_WHITE);
                ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(p.x + contents_size_x - 10, p.y), ImVec2(p.x + contents_size_x, p.y + 10), IM_COL32_WHITE);
                ImGui::Dummy(ImVec2(0, 10));
            }
            ImGui::PopStyleVar(2);
            ImGui::Separator();
            if (show_button)
            {
                ImGui::Button("this is a 300-wide button", ImVec2(300, 0));
            }
            if (show_tree_nodes)
            {
                bool open = true;
                if (ImGui::TreeNode("this is a tree node"))
                {
                    if (ImGui::TreeNode("another one of those tree node..."))
                    {
                        ImGui::Text("Some tree contents");
                        ImGui::TreePop();
                    }
                    ImGui::TreePop();
                }
                ImGui::CollapsingHeader("CollapsingHeader", &open);
            }
            if (show_text_wrapped)
            {
                ImGui::TextWrapped("This text should automatically wrap on the edge of the work rectangle.");
            }
            if (show_columns)
            {
                ImGui::Text("Tables:");
                if (ImGui::BeginTable("table", 4, ImGuiTableFlags_Borders))
                {
                    for (int n = 0; n < 4; n++)
                    {
                        ImGui::TableNextColumn();
                        ImGui::Text("Width %.2f", ImGui::GetContentRegionAvail().x);
                    }
                    ImGui::EndTable();
                }
                ImGui::Text("Columns:");
                ImGui::Columns(4);
                for (int n = 0; n < 4; n++)
                {
                    ImGui::Text("Width %.2f", ImGui::GetColumnWidth());
                    ImGui::NextColumn();
                }
                ImGui::Columns(1);
            }
            if (show_tab_bar && ImGui::BeginTabBar("Hello"))
            {
                if (ImGui::BeginTabItem("OneOneOne")) { ImGui::EndTabItem(); }
                if (ImGui::BeginTabItem("TwoTwoTwo")) { ImGui::EndTabItem(); }
                if (ImGui::BeginTabItem("ThreeThreeThree")) { ImGui::EndTabItem(); }
                if (ImGui::BeginTabItem("FourFourFour")) { ImGui::EndTabItem(); }
                ImGui::EndTabBar();
            }
            if (show_child)
            {
                ImGui::BeginChild("child", ImVec2(0, 0), ImGuiChildFlags_Borders);
                ImGui::EndChild();
            }
            ImGui::End();
        }

        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Text Clipping"))
    {
        static ImVec2 size(100.0f, 100.0f);
        static ImVec2 offset(30.0f, 30.0f);
        ImGui::DragFloat2("size", (float*)&size, 0.5f, 1.0f, 200.0f, "%.0f");
        ImGui::TextWrapped("(Click and drag to scroll)");


        for (int n = 0; n < 3; n++)
        {
            if (n > 0)
                ImGui::SameLine();

            ImGui::PushID(n);
            ImGui::InvisibleButton("##canvas", size);
            if (ImGui::IsItemActive() && ImGui::IsMouseDragging(ImGuiMouseButton_Left))
            {
                offset.x += ImGui::GetIO().MouseDelta.x;
                offset.y += ImGui::GetIO().MouseDelta.y;
            }
            ImGui::PopID();
            if (!ImGui::IsItemVisible()) // Skip rendering as ImDrawList elements are not clipped.
                continue;

            const ImVec2 p0 = ImGui::GetItemRectMin();
            const ImVec2 p1 = ImGui::GetItemRectMax();
            const char* text_str = "Line 1 hello\nLine 2 clip me!";
            const ImVec2 text_pos = ImVec2(p0.x + offset.x, p0.y + offset.y);
            ImDrawList* draw_list = ImGui::GetWindowDrawList();
            switch (n)
            {
            case 0:
                ImGui::PushClipRect(p0, p1, true);
                draw_list->AddRectFilled(p0, p1, IM_COL32(90, 90, 120, 255));
                draw_list->AddText(text_pos, IM_COL32_WHITE, text_str);
                ImGui::PopClipRect();
                break;
            case 1:
                draw_list->PushClipRect(p0, p1, true);
                draw_list->AddRectFilled(p0, p1, IM_COL32(90, 90, 120, 255));
                draw_list->AddText(text_pos, IM_COL32_WHITE, text_str);
                draw_list->PopClipRect();
                break;
            case 2:
                ImVec4 clip_rect(p0.x, p0.y, p1.x, p1.y); // AddText() takes a ImVec4* here so let's convert.
                draw_list->AddRectFilled(p0, p1, IM_COL32(90, 90, 120, 255));
                draw_list->AddText(ImGui::GetFont(), ImGui::GetFontSize(), text_pos, IM_COL32_WHITE, text_str, NULL, 0.0f, &clip_rect);
                break;
            }
        }

        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Overlap Mode"))
    {
        static bool enable_allow_overlap = true;

        
        ImGui::Checkbox("Enable AllowOverlap", &enable_allow_overlap);

        ImVec2 button1_pos = ImGui::GetCursorScreenPos();
        ImVec2 button2_pos = ImVec2(button1_pos.x + 50.0f, button1_pos.y + 50.0f);
        if (enable_allow_overlap)
            ImGui::SetNextItemAllowOverlap();
        ImGui::Button("Button 1", ImVec2(80, 80));
        ImGui::SetCursorScreenPos(button2_pos);
        ImGui::Button("Button 2", ImVec2(80, 80));

        // This is typically used with width-spanning items.
        // (note that Selectable() has a dedicated flag ImGuiSelectableFlags_AllowOverlap, which is a shortcut
        // for using SetNextItemAllowOverlap(). For demo purpose we use SetNextItemAllowOverlap() here.)
        if (enable_allow_overlap)
            ImGui::SetNextItemAllowOverlap();
        ImGui::Selectable("Some Selectable", false);
        ImGui::SameLine();
        ImGui::SmallButton("++");

        ImGui::TreePop();
    }
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
