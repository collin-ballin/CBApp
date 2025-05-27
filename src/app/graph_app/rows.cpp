/***********************************************************************************
*
*       ********************************************************************
*       ****               R O W S . C P P  ____  F I L E               ****
*       ********************************************************************
*              AUTHOR:      Collin A. Bond
*               DATED:      May 24, 2025.
*
**************************************************************************************
**************************************************************************************/
#include "app/graph_app/graph_app.h"



namespace cb { //     BEGINNING NAMESPACE "cb"...
// *************************************************************************** //
// *************************************************************************** //



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
                        {"Shot / Poisson Noise",                        false},     {"Burst / Random Telegraph Noise",              false},
                        { "Schottky / Poisson Noise",                   false}
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

                if (ImGui::BeginCombo("Combo", items[selected]))
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
        { "Brush Size",         [this]//    BEGIN ROW.
            {
                //this->m_editor
                ImGui::SetNextItemWidth( ImGui::GetColumnWidth() );
                ImGui::SliderInt( "##FDTDEditor_BrushSize", this->m_editor.get_brush_size(),      1,      10,     "%d px"     );
            }
        }, // END ROW.
    //
        { "Editor State",       [this]
            {
                State &state = *this->m_editor.get_mode();
                int int_state = static_cast<int>(state);
                
                if ( ImGui::Combo("##FDTDEditor_EditorState", &int_state, SketchWidget::STATE_LABELS.data(), static_cast<int>(State::Count) ) )
                {
                        
                }
        
                //ImGui::Combo("##FDTDEditor_EditorState", this->m_editor.get_mode(), s_modes, 2);
            }
        } //,
    //
        //{ "Brush Shape",        [this]//    BEGIN ROW.
        //    {
        //        //  ImGui::Combo("##FDTDEditor_BrushShape",         &sketch::brush_shape, sketch::brush_shapes, IM_ARRAYSIZE(sketch::brush_shapes));
        //        //this->m_editor
        //        //  ImGui::SetNextItemWidth( ImGui::GetColumnWidth() );
        //        //  ImGui::SliderInt( "##FDTDEditor_BrushSize", this->m_editor.get_brush_size(), sketch::brush_shapes, IM_ARRAYSIZE(sketch::brush_shapes)    );
        //    }
        //} // END ROW.
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
    static const ImVec2                 SPACING                     = ImVec2( 0.0f, 0.25*ImGui::GetTextLineHeightWithSpacing() );
    
    //  CONSTANTS...



    //  0.  EDITOR TABLE...
    //ImGui::SetNextItemOpen(true, ImGuiCond_Once);
    //if ( ImGui::CollapsingHeader("Editor Controls") ) {
    //    utl::MakeCtrlTable(this->ms_EDITOR_ROWS,      m_editor_table_CFG);
    //}



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
