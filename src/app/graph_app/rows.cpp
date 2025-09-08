/***********************************************************************************
*
*       ********************************************************************
*       ****               R O W S . C P P  ____  F I L E               ****
*       ********************************************************************
*              AUTHOR:      Collin A. Bond.
*               DATED:      May 24, 2025.
*
**************************************************************************************
**************************************************************************************/
#include "app/graph_app/graph_app.h"



namespace cb { //     BEGINNING NAMESPACE "cb"...
// *************************************************************************** //
// *************************************************************************** //




/*
// A simple record type we want to browse & edit
struct Person {
    std::string     name;
    int             age    = 30;
    bool            alive  = true;
};


//  "Test_Browser"
//
void App::Test_Browser(void)
{
    static std::vector<Person>      people                  = {       //  1.  In your application state (not every frame):
        {"Ada",         36,         true},
        {"Grace",       85,         false},
        {"Linus",       54,         true}
    };


    //  2.  Property-editor lambda (called once per frame for the selected item)
    static auto                     draw_person_properties  = [](Person& p){
        ImGui::InputText("Name", &p.name);
        ImGui::SliderInt("Age", &p.age, 0, 120);
        ImGui::Checkbox("Alive", &p.alive);
    };
    
    
    //  3.  Instantiate the widget (flat list selector in this example)
    static CB_Browser<Person>       browser(
        people,
        draw_person_properties,
        [](const Person& p){ return p.name; }
    );
    
    //  static cb::ui::CB_Browser<Person> browser{
    //      .items           = &people,                 // pass address of container
    //      .draw_properties = draw_person_properties,
    //      .label_provider  = [](const Person& p){ return p.name; }
    //  };


    // 4.  Draw the browser every frame
    browser.draw();


    // 5.  React to selection outside the widget if you like
    if (auto* person = browser.selected()) {
    }






    return;
}*/





//  1.      LOADING ROWS AND TABLE CONFIGURATIONS FOR APPLICATION WIDGETS...
// *************************************************************************** //
// *************************************************************************** //

//  "init_ctrl_rows"
//
void GraphApp::init_ctrl_rows(void)
{

    //      1.  FDTD...
    // *************************************************************************** //
    // *************************************************************************** //
    m_fdtd_table_CFG                = {
        /*  Table UUID  =   */      "FDTDControlsTable",
        /*  Table Flags =   */      ImGuiTableFlags_None | ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable | ImGuiTableFlags_NoKeepColumnsVisible | ImGuiTableFlags_SizingStretchProp,
        /*  Column CFGs =   */      {
                { "Label",          200.0f,         ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize },
                { "Widget",         -1.0f,          ImGuiTableColumnFlags_WidthStretch }
            },
        /*  Header Row  =   */      false
    };


    //  "FDTD_ROWS"...
    //
    ms_FDTD_ROWS    = {
    //
    //
    //
        {"FDTD",                        [this]
            {// BEGIN.
                //      CASE 1 :    SCRIPT IS  **NOT**  RUNNING...
                if (!m_running)
                {
                    if (ImGui::Button("Run Simulation")) // , ImVec2(ImGui::GetContentRegionAvail().x - pad, 0)) )
                    {
                        //
                        //  START THE MODEL...
                        //
                    }
                }
                else
                {
                    ImGui::PushStyleColor(ImGuiCol_Button,          ImVec4(0.800f, 0.216f, 0.180f, 1.00f) );
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered,   app::DEF_APPLE_RED );
                    if (ImGui::Button("Stop Simulation")) // , ImVec2(ImGui::GetContentRegionAvail().x - pad, 0)) )
                    {
                        //
                        //  STOP THE MODEL...
                        //
                    }
                    ImGui::PopStyleColor(2);
                }
            }// END.
        },
    //
        {"FDTD",                        []
            {// BEGIN.
            }// END.
        }
    //
    //
    //
    };//    END FDTD CTRLS.
    
    

    


    // *************************************************************************** //
    //
    //
    //      2.  STEPSIZES...
    // *************************************************************************** //
    // *************************************************************************** //
    m_stepsize_table_CFG                = {
        /*  Table UUID  =   */      "StepsizesControlsTable",
        /*  Table Flags =   */      ImGuiTableFlags_None | ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable | ImGuiTableFlags_NoKeepColumnsVisible | ImGuiTableFlags_SizingStretchProp,
        /*  Column CFGs =   */      {
                { "Label",          200.0f,         ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize },
                { "Widget",         -1.0f,          ImGuiTableColumnFlags_WidthStretch }
            },
        /*  Header Row  =   */      false
    };
    
    
    //  STEPSIZES ROWS...
    //
    ms_STEPSIZE_ROWS                    = {
    //
        { "dx",                  [this]
            { // BEGIN LAMBDA.
                ImGui::SetNextItemWidth( ImGui::GetColumnWidth() );
                ImGui::SliderScalar("##FDTD_dx",    ImGuiDataType_Double,   &m_stepsizes.dx.value,      &m_stepsizes.dx.limits.min,     &m_stepsizes.dx.limits.max,     "%.1f spatial-units");
            } // END LAMBDA.
        }, // END ROW.
    //
        { "dy",                  [this]
            { // BEGIN LAMBDA.
                ImGui::BeginDisabled(true);
                ImGui::SetNextItemWidth( ImGui::GetColumnWidth() );
                ImGui::SliderScalar("##FDTD_dy",    ImGuiDataType_Double,   &m_stepsizes.dy.value,      &m_stepsizes.dy.limits.min,     &m_stepsizes.dy.limits.max,     "%.1f spatial-units");
                ImGui::EndDisabled();
            } // END LAMBDA.
        }, // END ROW.
    //
        { "dz",                  [this]
            { // BEGIN LAMBDA.
                ImGui::BeginDisabled(true);
                ImGui::SetNextItemWidth( ImGui::GetColumnWidth() );
                ImGui::SliderScalar("##FDTD_dz",    ImGuiDataType_Double,   &m_stepsizes.dz.value,      &m_stepsizes.dz.limits.min,     &m_stepsizes.dz.limits.max,     "%.1f spatial-units");
                ImGui::EndDisabled();
            } // END LAMBDA.
        }, // END ROW.
    //
        { "dt",                  [this]
            { // BEGIN LAMBDA.
                ImGui::SetNextItemWidth( ImGui::GetColumnWidth() );
                ImGui::SliderScalar("##FDTD_dt",    ImGuiDataType_Double,   &m_stepsizes.dt.value,      &m_stepsizes.dt.limits.min,     &m_stepsizes.dt.limits.max,     "%.1f temporal-units");
            } // END LAMBDA.
        }, // END ROW.
    //
        { "Sc",                  [this]
            { // BEGIN LAMBDA.
                ImGui::SetNextItemWidth( ImGui::GetColumnWidth() );
                ImGui::SliderScalar("##FDTD_Sc",    ImGuiDataType_Double,   &m_stepsizes.Sc.value,      &m_stepsizes.Sc.limits.min,     &m_stepsizes.Sc.limits.max,     "%.1f  Arb.");
            } // END LAMBDA.
        }, // END ROW.
    //
    //
    //
        { "",   []
            {
                ImGui::TextUnformatted("Simulated Region");
            }
        }, // END ROW.
    //
        { "NX",                 [this]
            { // BEGIN LAMBDA.
                ImGui::SetNextItemWidth( ImGui::GetColumnWidth() );
                ImGui::SliderScalar("##FDTD_NX",    ImGuiDataType_U64,      &m_steps.NX.value,      &m_steps.NX.limits.min,     &m_steps.NX.limits.max,  "%llu spatial-steps");
            } // END LAMBDA.
        }, // END ROW.
    //
        { "NY",                 [this]
            { // BEGIN LAMBDA.
                //ImGui::BeginDisabled(true);
                ImGui::SetNextItemWidth( ImGui::GetColumnWidth() );
                ImGui::SliderScalar("##FDTD_NY",    ImGuiDataType_U64,      &m_steps.NY.value,      &m_steps.NY.limits.min,     &m_steps.NY.limits.max,  "%llu spatial-steps");
                //ImGui::EndDisabled();
            } // END LAMBDA.
        }, // END ROW.
    //
        { "NZ",                 [this]
            { // BEGIN LAMBDA.
                ImGui::BeginDisabled(true);
                ImGui::SetNextItemWidth( ImGui::GetColumnWidth() );
                ImGui::SliderScalar("##FDTD_NZ",    ImGuiDataType_U64,      &m_steps.NZ.value,      &m_steps.NZ.limits.min,     &m_steps.NZ.limits.max,  "%llu spatial-steps");
                ImGui::EndDisabled();
            } // END LAMBDA.
        }, // END ROW.
    //
    //
    //
        { "NT",                 [this]
            { // BEGIN LAMBDA.
                ImGui::SetNextItemWidth( ImGui::GetColumnWidth() );
                ImGui::SliderScalar("##FDTD_NT",    ImGuiDataType_U64,      &m_steps.NT.value,      &m_steps.NT.limits.min,     &m_steps.NT.limits.max,  "%llu temporal-steps");
            } // END LAMBDA.
        } // END ROW.
    };
    
    
    
    
    
    
    // *************************************************************************** //
    //
    //
    //      3.  SOURCES...
    // *************************************************************************** //
    // *************************************************************************** //
    m_sources_table_CFG                 = {
        /*  Table UUID  =   */      "SourcesControlsTable",
        /*  Table Flags =   */      ImGuiTableFlags_None | ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable | ImGuiTableFlags_NoKeepColumnsVisible | ImGuiTableFlags_SizingStretchProp,
        /*  Column CFGs =   */      {
                { "Label",          200.0f,         ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize },
                { "Widget",         -1.0f,          ImGuiTableColumnFlags_WidthStretch }
            },
        /*  Header Row  =   */      false
    };
    
    
    //  SOURCES ROWS...
    //
    ms_SOURCES_ROWS                     = {
    //
        { "",   []
            {
                ImGui::TextUnformatted("Source");
            }
        }, // END ROW.
    //
        { "Type",   []
            {
                using                       Item            = std::pair<const char*, bool>;
                using                       Group           = std::pair<const char*, std::vector<Item>>;
                static std::vector<Group>   combo_groups    = {
                    {"Fundamental Sources", {
                        {"Gaussian Pulse",                              true},      {"Time-Harmonic / Monochromatic Plane Wave",    true},
                        {"Ricker Wavelet / Modulated Gaussian",         true},      {"Arbitrary Function Generator",                false}
                    }
                },
                    {"Temporal Profile Sources", {
                        {"Impulse / Dirac Delta",                       false},     {"Gated Sinusoid / Finite-Duration Tone",       false},
                        {"Chirped Pulse / Tunable-Band Tone",           false},     {"Time-Harmonic (Amplitude-Modulated)",         false},
                        {"Time-Harmonic (Frequency-Modulated)",         false},     {"Mathieu Beam",                                false}
                    }
                },
                    {"Spatial Distribution Sources", {
                        {"Point Dipole",                                false},     {"Line / Sheet",                                false},
                        {"Gaussian / Paraxial TEM_00",                  false},     {"Bessel / Vortex Beam",                        false},
                        {"Waveguide Port / TE_10 Computed Eigenmode",   false}
                    }
                },
                    {"Noise Sources", {
                        {"White Noise",                                 false},     {"Pink Noise",                                  false},
                        {"Shot / Poisson Noise",                        false},     {"Burst / Random Telegraph Noise",              false}
                    }
                },
                    {"Mode-Converter Sources", {
                        {"Ince-Gaussian (IG)",                          false},     {"Hermite-Gaussian (HG)",                       false},
                        {"Laguerre-Gaussian (LG)",                      false},     {"Bessel-Gaussian",                             false},
                        {"Airy Beam",                                   false},     {"Mathieu Beam",                                false}
                    }
                }
                //
                };  //  END.

                static ImGuiComboFlags      flags           = ImGuiComboFlags_PopupAlignLeft | ImGuiComboFlags_HeightLargest;
                static int                  item_current    = 0;
                const char *                current_label   = nullptr;
                int                         index           = 0;
                
                for (const auto & group : combo_groups)
                {
                    for (const auto & item : group.second) {
                        if (index == item_current) {
                            current_label = item.first;
                            break;
                        }
                        ++index;
                    }
                    if (current_label) break;
                }

                if (ImGui::BeginCombo("##FDTDSourceType", current_label, flags))
                {
                    int global_index = 0;
                    for (const auto& group : combo_groups) {
                        ImGui::TextDisabled("%s", group.first);
                        for (const auto& item : group.second) {
                            if (item.second) {
                                if (ImGui::Selectable(item.first, item_current == global_index)) {
                                    item_current = global_index;
                                }
                            }
                            else
                            {
                                ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyle().Colors[ImGuiCol_TextDisabled]);
                                ImGui::Selectable(item.first, false, ImGuiSelectableFlags_Disabled);
                                ImGui::PopStyleColor();
                            }
                            ++global_index;
                        }
                        ImGui::Separator();
                    }
                    ImGui::EndCombo();
                }
            
            }
        }, // END ROW.
    //
        { "Energy Propogation",   []
            {
                static const char *     items[]         = { "Soft", "Hard" };
                static int              selected        = 0;

                if (ImGui::BeginCombo("##FDTD_SourceEnergy Propogation", items[selected]))
                {
                    for (int i = 0; i < IM_ARRAYSIZE(items); ++i)
                    {
                        if (ImGui::Selectable(items[i], selected == i))
                            selected = i;
                    }
                    ImGui::EndCombo();
                }
            }
        }, // END ROW.
    //
    //
    //
        { "Wavelength",         [this]
            { // BEGIN LAMBDA.
                ImGui::SetNextItemWidth( ImGui::GetColumnWidth() );
                ImGui::SliderScalar("##FDTD_SourceWavelength",  ImGuiDataType_U64,                  &m_params.wavelength.value,
                                                                &m_params.wavelength.limits.min,    &m_params.wavelength.limits.max,  "%llu spatial-steps");
            } // END LAMBDA.
        }, // END ROW.
    //
        { "Duration",           [this]
            { // BEGIN LAMBDA.
                ImGui::SetNextItemWidth( ImGui::GetColumnWidth() );
                ImGui::SliderScalar("##FDTD_SourceDuration",    ImGuiDataType_U64,                  &m_params.duration.value,
                                                                &m_params.duration.limits.min,      &m_params.duration.limits.max,      "%llu # of periods");
            } // END LAMBDA.
        }, // END ROW.
        { "TF/SF Boundary",     []
            { // BEGIN LAMBDA.
                ImGui::SetNextItemWidth( ImGui::GetColumnWidth() );
                ImGui::Text("Boundary Conditions");
            } // END LAMBDA.
        }, // END ROW.
    //
    //
    //
        { "FDTD",                        []
            { // BEGIN LAMBDA.
                static bool temp = false;
                
                //      CASE 1 :    SCRIPT IS  **NOT**  RUNNING...
                if (!temp)
                {
                    if (ImGui::Button("Run Simulation")) // , ImVec2(ImGui::GetContentRegionAvail().x - pad, 0)) )
                    {
                        temp = true;
                        //
                        //  START THE MODEL...
                        //
                    }
                }
                else
                {
                    ImGui::PushStyleColor(ImGuiCol_Button,          ImVec4(0.800f, 0.216f, 0.180f, 1.00f) );
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered,   app::DEF_APPLE_RED );
                    if (ImGui::Button("Stop Simulation")) // , ImVec2(ImGui::GetContentRegionAvail().x - pad, 0)) )
                    {
                        temp = false;
                        //
                        //  STOP THE MODEL...
                        //
                    }
                    ImGui::PopStyleColor(2);
                }
            
            } // END LAMBDA.
        } // END ROW.
    };
    
    
    
    
    
    
    // *************************************************************************** //
    //
    //
    //      4.  EDITOR ROWS...
    // *************************************************************************** //
    // *************************************************************************** //

    //  INTERACTIVE VARIABLES...
    static ImGuiSliderFlags             SLIDER_FLAGS            = ImGuiSliderFlags_AlwaysClamp;

    constexpr float                     margin                  = 0.75f;
    constexpr float                     pad                     = 10.0f;
    
    
    m_editor_table_CFG                  = {
        /*  Table UUID  =   */      "EditorControlsTable",
        /*  Table Flags =   */      ImGuiTableFlags_None | ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable | ImGuiTableFlags_NoKeepColumnsVisible | ImGuiTableFlags_SizingStretchProp,
        /*  Column CFGs =   */      {
                { "Label",          200.0f,         ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize },
                { "Widget",         -1.0f,          ImGuiTableColumnFlags_WidthStretch }
            },
        /*  Header Row  =   */      false
    };
    
    
    //  EDITOR ROWS...
    //
    ms_EDITOR_ROWS    = {
    //  BEGIN  *ALL*  ROWS...
    //
    //
        { "Editor State",       [this]
            {// BEGIN LAMBDA.
                constexpr size_t    N           = 2;
                const float         T           = ImGui::GetColumnWidth() - N * pad;
                const float         w           = T / N;
                //
                //
                //      WIDGET 1.   EDITOR STATE SELECTOR...
                State &             state       = *this->m_editor.get_mode();
                int                 int_state   = static_cast<int>(state);
                //
                ImGui::SetNextItemWidth(w);
                if ( ImGui::Combo("##FDTDEditor_EditorState", &int_state, SketchWidget::STATE_LABELS.data(), static_cast<int>(State::Count) ) )
                {
                    this->m_editor.set_mode(int_state);
                }
                //
                //
                //      WIDGET 2.   CHANNEL SELECTOR...
                //
                ImGui::SetNextItemWidth(w);
                ImGui::SameLine();//0.0f, pad);
                int active = *this->m_editor.get_active_channel();  // get current index
                if (ImGui::BeginCombo("##FDTDEditor_ChannelSelector", m_editor.channel(active).map_title))
                {
                    for (int i = 0; i < m_editor.channel_count(); ++i)
                    {
                        bool selected = (active == i);
                        if (ImGui::Selectable( m_editor.channel(i).map_title, selected) ) {
                            m_editor.set_active_channel(i);
                            active = i; // update local copy so the combo label updates immediately
                        }
                        if (selected)
                            ImGui::SetItemDefaultFocus();
                    }
                    ImGui::EndCombo();
                }
                //
                //
                //
                //ImGui::Dummy( ImVec2(pad, 0.0f) );
                //
            }// END LAMBDA.
        },
    //
    //
    //
        { "New Browser",        [this]//    BEGIN ROW.
            {

                this->m_editor.draw_point_browser();

            }
        }, // END ROW.
    //
    //
    //
    //
    //
    //
        { "Paint Value Range",      [this]//    BEGIN ROW.
            {
                //float range [2]     = { *m_editor.get_vmin(),   *m_editor.get_vmax() };
                
                ImGui::SetNextItemWidth( ImGui::GetColumnWidth() );
                ImGui::SliderFloat2("##FDTDEditor_PaintValueRange",     m_editor.get_vmin(),   -100,      100,    "%.2f",      SLIDER_FLAGS);
            }
        }, // END ROW.
    //
        { "Paint Value",            [this]//    BEGIN ROW.
            {
                ImGui::SetNextItemWidth( ImGui::GetColumnWidth() );
                //  ImGui::SliderScalar("##FDTDEditor_PaintValue",  ImGuiDataType_Double,               &m_avg_window_sec.value,
                //      &m_avg_window_sec.limits.min,       &m_avg_window_sec.limits.max,      "%.2f seconds", SLIDER_FLAGS);
                ImGui::SliderScalar("##FDTDEditor_PaintValue",  ImGuiDataType_Float,               this->m_editor.get_paint_value(),
                    this->m_editor.get_vmin(),       this->m_editor.get_vmax(),      "%.2f ", SLIDER_FLAGS);
            }
        }, // END ROW.
    //
        {"Colormap",                                [this]
            {
                int     cmap    = *this->m_editor.get_cmap();
                float   w       = ImGui::GetColumnWidth();
                
                if (ImPlot::ColormapButton(ImPlot::GetColormapName(cmap), ImVec2(w, 0), cmap)) {
                    cmap = (cmap + 1) % ImPlot::GetColormapCount();
                    this->m_editor.set_cmap(cmap);
                }
            }
        }, // END ROW.
    //
        { "Brush Size",         [this]//    BEGIN ROW.
            {
                //this->m_editor
                ImGui::SetNextItemWidth( ImGui::GetColumnWidth() );
                ImGui::SliderInt( "##FDTDEditor_BrushSize", this->m_editor.get_brush_size(),      1,      10,     "%d px"     );
            }
        }, // END ROW.
    //
    //
        { "Brush Shape",        [this]//    BEGIN ROW.
            {
                BrushShape &    shape       = *m_editor.get_brush_shape();
                int             int_shape   = static_cast<int>( shape );
                
                if ( ImGui::Combo("##FDTDEditor_BrushShape", &int_shape, m_editor.BRUSH_SHAPE_LABELS.data(), m_editor.BRUSH_SHAPE_LABELS.size() ) ) {
                    m_editor.set_brush_shape(int_shape);
                }
            }
        } // END ROW.
    //
    //
    //
    //
    //
    //
        //  { "Browser",        [this]//    BEGIN ROW.
        //      {
        //          using                           Point                   = SketchWidget::CBDragPoint;
        //          using                           PointType               = SketchWidget::PointType;
        //
        //          static std::vector<Point>       data                    = {       //  1.  In your application state (not every frame):
        //          };
        //
        //
        //          //  2.  Property-editor lambda (called once per frame for the selected item)
        //          static auto                     draw_point_properties   = [this](Point & p)
        //          {
        //              int         point_type_index    = static_cast<int>(p.type);
        //              double      x_min               = static_cast<double>(m_steps.NX.limits.min),
        //                          x_max               = static_cast<double>(m_steps.NX.limits.max),
        //                          y_min               = static_cast<double>(m_steps.NY.limits.min),
        //                          y_max               = static_cast<double>(m_steps.NY.limits.max);
        //
        //              ImGui::SliderScalar("x-position", ImGuiDataType_Double,   &p.pos.x,      &x_min,        &x_max,     "%.1f");
        //              ImGui::SliderScalar("y-position", ImGuiDataType_Double,   &p.pos.y,      &y_min,        &y_max,     "%.1f");
        //              if ( ImGui::Combo( "Point Type", &point_type_index,
        //                            SketchWidget::POINT_TYPE_LABELS.data(),
        //                            static_cast<int>(SketchWidget::POINT_TYPE_LABELS.size()) )
        //              ) {
        //                  p.type = static_cast<PointType>(point_type_index);
        //              }
        //          };
    
    
        //          //  3.  Instantiate the widget (flat list selector in this example)
        //          static CB_Browser<Point>       browser(
        //              this->m_editor.get_points(),
        //              draw_point_properties,
        //              [](const Point & p){ return std::to_string(p.id); }
        //          );

        //          browser.draw();

        //      }
        //  } // END ROW.
    //
    //
    //
    //  END  *ALL*  ROWS...
    };
    
    
    
    
    
    
    
    

    


    // *************************************************************************** //
    //
    //
    //      5.  PLAYBACK CONTROLS...
    // *************************************************************************** //
    // *************************************************************************** //

    m_playback_table_CFG                = {
        /*  Table UUID  =   */      "PlaybackControlsTable",
        /*  Table Flags =   */      ImGuiTableFlags_None | ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable | ImGuiTableFlags_NoKeepColumnsVisible | ImGuiTableFlags_SizingStretchProp,
        /*  Column CFGs =   */      {
                { "Label",          200.0f,         ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize },
                { "Widget",         -1.0f,          ImGuiTableColumnFlags_WidthStretch }
            },
        /*  Header Row  =   */      false
    };
    
    
    //  PLAYBACK ROWS...
    //
    //  BEGIN  *ALL*  ROWS...
    //
    ms_PLAYBACK_ROWS                    = {
        {"Playback",                                [this]
        {
            Param<ImU64> & frame = this->m_playback.frame;
            
            //  WIDGET 1.  PLAYBACK SPEED SLIDER
            ImGui::BeginDisabled( !m_playback.ready );
            //
                ImGui::SetNextItemWidth( margin * ImGui::GetColumnWidth() );
                ImGui::SliderScalar("##FDTD_PlaybackProgressBar",    ImGuiDataType_U64,      &frame.value,
                        &frame.limits.min, &frame.limits.max,  "Frame #%llu", SLIDER_FLAGS);
                
                ImGui::SameLine(0.0f, pad);
                //
                //
                //  WIDGET 2.   PLAY/PAUSE BUTTON...
                if (!m_playback.playing) {
                    if (ImGui::Button("Play", ImVec2(ImGui::GetContentRegionAvail().x - pad, 0)) ) {
                        m_playback.playing = true;
                    }
                }
                else
                {
                    ImGui::PushStyleColor(ImGuiCol_Button,          ImVec4(0.800f, 0.216f, 0.180f, 1.00f) );
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered,   app::DEF_APPLE_RED );
                    if ( ImGui::Button("Pause", ImVec2(ImGui::GetContentRegionAvail().x - pad, 0)) ) {
                        m_playback.playing         = false;
                        m_playback.last_time     = ImGui::GetTime();
                    }
                    ImGui::PopStyleColor(2);
                }
            //
            ImGui::EndDisabled();
        }
        }, // END ROW.
    //
        {"Playback Speed",                  [this]
        {
            //  WIDGET 1.  PLAYBACK SPEED SLIDER
            ImGui::BeginDisabled( !m_playback.ready );
            //
                ImGui::SetNextItemWidth( margin * ImGui::GetColumnWidth() );
                ImGui::SliderFloat("##FDTD_PlaybackSpeed", &m_playback.fps, 0.1f, 240.0f, "%.1f  FPS");
                ImGui::SameLine(0.0f, pad);
            //
            ImGui::EndDisabled();
        }
        }, // END ROW.
    //
        {"Plots",                           [this]
        {
            //  WIDGET 1.  PLAYBACK SPEED SLIDER
            ImGui::BeginDisabled( !m_playback.ready );
            //
                ImGui::Checkbox("Time Domain",          &this->m_SHOW_PLOTS[0]);
                ImGui::SameLine();
                ImGui::Checkbox("Freqiency Domain",     &this->m_SHOW_PLOTS[1]);
            //
            ImGui::EndDisabled();
        }
        } // END ROW.
    //
    //
    //
    //  END  *ALL*  ROWS...
    };
    
    
    return;
}










// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //

//  "ShowEditor"
//
void GraphApp::ShowEditor(void)
{
    if ( this->m_editor.Begin(static_cast<int>(m_steps.NX.value), static_cast<int>(m_steps.NY.value)) )
    {
        this->m_editor.End();
    }
    
    return;
}



//  "ShowModelParameters"
//
void GraphApp::ShowModelParameters(void)
{
    //  OTHER CONSTANTS...
    
    //  CONSTANTS...



    //  0.  EDITOR TABLE...
    ImGui::SetNextItemOpen(true, ImGuiCond_Once);
    if ( ImGui::CollapsingHeader("Editor Controls") ) {
        utl::MakeCtrlTable(this->ms_EDITOR_ROWS,      m_editor_table_CFG);
    }



    //  1.  STEPSIZE TABLE...
    ImGui::SetNextItemOpen(false, ImGuiCond_Once);
    if ( ImGui::CollapsingHeader("Parameters") ) {
        utl::MakeCtrlTable(this->ms_STEPSIZE_ROWS,      m_stepsize_table_CFG);
    }
    
    
    //  2.  SOURCES TABLE...
    ImGui::SetNextItemOpen(false, ImGuiCond_Once);
    if ( ImGui::CollapsingHeader("Sources") ) {
        utl::MakeCtrlTable(this->ms_SOURCES_ROWS,   m_sources_table_CFG);
    }
    
    

    return;
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
