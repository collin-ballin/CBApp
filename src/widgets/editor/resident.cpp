/***********************************************************************************
*
*       *********************************************************************
*       ****           R E S I D E N T . C P P  ____  F I L E            ****
*       *********************************************************************
*
*              AUTHOR:      Collin A. Bond
*               DATED:      July 10, 2025.
*
**************************************************************************************
**************************************************************************************/
#include "widgets/editor/editor.h"



namespace cb { //     BEGINNING NAMESPACE "cb"...
// *************************************************************************** //
// *************************************************************************** //




// *************************************************************************** //
//
//
//
//      4.  RESIDENT OVERLAY WINDOWS...
// *************************************************************************** //
// *************************************************************************** //


// *************************************************************************** //
//          4.1.    DEBUGGER OVERLAY.
// *************************************************************************** //

//  "_draw_debugger_resident"
//
void Editor::_draw_debugger_resident(void)
{
    ImGui::TextUnformatted("Debugger");
    ImGui::Separator();
           
    return;
}




// *************************************************************************** //
//          4.2.    SELECTION OVERLAY.
// *************************************************************************** //

//  "_draw_selection_resident"
//
void Editor::_draw_selection_resident(void)
{
    ImGui::TextUnformatted("Selection");
    ImGui::Separator();
           
    return;
}



// *************************************************************************** //
//          4.3.    "SHAPE-TOOL" OVERLAY.
// *************************************************************************** //

//  "_draw_shape_resident"
//
void Editor::_draw_shape_resident(void)
{
    int kind_idx = static_cast<int>(m_shape.kind);



    ImGui::TextUnformatted("Shape Tool");
    //
    //
    //
    //  1.  DROP-DOWN FOR SHAPE TYPE.
    ImGui::Separator();
    if (ImGui::Combo( "##Editor_Shape_ShapeType",
                      &kind_idx,
                      this->ms_SHAPE_NAMES.data(),                          //  Names table
                      static_cast<int>(ShapeKind::COUNT)) )                 //  Item count
    {
        m_shape.kind = static_cast<ShapeKind>(kind_idx);
    }
    //
    //  2.  SLIDER FOR SHAPE RADIUS...
    switch (m_shape.kind)
    {
        //  2.1.    Shapes with only ONE Variable (Radius).
        case ShapeKind::Square      :
        case ShapeKind::Circle      :
        case ShapeKind::Oval        :
        case ShapeKind::Ellipse     : {
            this->_draw_shape_resident_default();
            break;
        }
        
        case ShapeKind::Rectangle   : {
            break;
        }
        
        default                     : {
            break;
        }
    
    }
    ImGui::SliderFloat("Radius", &m_shape.radius, 1.0f, 100.0f, "%.2f");
           

    return;
}


//  "_draw_shape_default_resident"
//
void Editor::_draw_shape_resident_default(void)
{

    ImGui::SliderFloat("Radius", &m_shape.radius, 1.0f, 100.0f, "%.2f");

    
    return;
}


//  "_draw_shape_resident_multi"
//      For drawing shapes that require more than one variable to describe.
//
void Editor::_draw_shape_resident_multi(void)
{
    ImGui::SliderFloat("R", &m_shape.radius, 1.0f, 100.0f, "%.2f");
    ImGui::SliderFloat("S", &m_shape.radius, 1.0f, 100.0f, "%.2f");
    return;
}



//  "_draw_shape_resident_custom"
//      For drawing shapes that require more than one variable to describe.
//
void Editor::_draw_shape_resident_custom(void)
{
    ImGui::SliderFloat("R", &m_shape.radius, 1.0f, 100.0f, "%.2f");
    ImGui::SliderFloat("S", &m_shape.radius, 1.0f, 100.0f, "%.2f");
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
