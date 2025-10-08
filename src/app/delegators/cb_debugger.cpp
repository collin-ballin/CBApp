/***********************************************************************************
*
*       ********************************************************************
*       ****        C B _ D E B U G G E R . C P P  ____  F I L E        ****
*       ********************************************************************
*              AUTHOR:      Collin A. Bond.
*               DATED:      August 28, 2025.
*
**************************************************************************************
**************************************************************************************/
#include "app/app.h"
#include "app/delegators/_detail_view.h"
#include <random>
#include <algorithm>



namespace cb { //     BEGINNING NAMESPACE "cb"...
// *************************************************************************** //
// *************************************************************************** //






// *************************************************************************** //
//
//
//
//      1.      INITIALIZATION  | DEFAULT CONSTRUCTOR, DESTRUCTOR, ETC...
// *************************************************************************** //
// *************************************************************************** //

//  Default Constructor.
//
CBDebugger::CBDebugger(app::AppState & src)
    : S(src)                        {   }


//  "initialize"
//
void CBDebugger::initialize(void)
{
    if ( this->m_initialized )    { return; }
        
    this->init();
    return;
}


//  "init"          | protected
//
void CBDebugger::init(void) {
    this->m_window_class.DockNodeFlagsOverrideSet   = this-> m_docknode_override_flags;
    
    
    return;
}


//  Destructor.
//
CBDebugger::~CBDebugger(void)               { this->destroy(); }


//  "destroy"       | protected
//
void CBDebugger::destroy(void)              {   }



//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "INITIALIZATION".






// *************************************************************************** //
//
//
//
//      1B.     PUBLIC API FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

//  "Begin"
//
void CBDebugger::Begin([[maybe_unused]] const char *        uuid,
                       [[maybe_unused]] bool *              p_open,
                       [[maybe_unused]] ImGuiWindowFlags    flags)
{
    [[maybe_unused]] ImGuiIO &      io              = ImGui::GetIO();
    [[maybe_unused]] ImGuiStyle &   style           = ImGui::GetStyle();

    
    
    //      2.      CREATE THE WINDOW AND BEGIN APPENDING WIDGETS INTO IT...
    //  this->m_window_class.DockNodeFlagsOverrideSet   = S.m_detview_window_flags;
    ImGui::SetNextWindowClass( &this->m_window_class );
    //
    ImGui::Begin(uuid, p_open, flags);
    //
    //
        this->Begin_IMPL();
    //
    //
    ImGui::End();
    

    
    return;
}



//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "PUBLIC API".






// *************************************************************************** //
//
//
//
//      2A.     "MAIN UI FUNCTIONS"...
// *************************************************************************** //
// *************************************************************************** //
namespace {


using       namespace       cblib::containers;



//  "IncrementAction"
//      Dummy action: increments an int
//
struct IncrementAction : orchid::ABC
{
    IncrementAction             (int & ref, int delta = 1)
        : target(ref), delta(delta)
    {   }
        

    void                undo    (void)  noexcept override           { target -= delta;      }
    void                redo    (void)  noexcept override           { target += delta;      }
    std::string_view    label   (void)  const noexcept override     { return "Increment";   }

private:
    int &       target      ;
    int         delta       ;
};



//  "ToggleFlagAction"
//      flips a bool each time it is redone/undone
//
struct ToggleFlagAction : orchid::ABC
{
    ToggleFlagAction            (bool & ref)
        : target(ref)
    {   }
        

    void                undo    (void)  noexcept override           { target = !target;         }
    void                redo    (void)  noexcept override           { target = !target;         }
    std::string_view    label   (void)  const noexcept override     { return "Toggle flag";     }

private:
    bool &      target      ;
};








//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "2A.  Orchid".
}   //  END "anonymous" NAMESPACE.






// *************************************************************************** //
//
//
//
//      2B.     "ORCHID" TESTING...
// *************************************************************************** //
// *************************************************************************** //

//  "TestOrchid"
//
inline void CBDebugger::TestOrchid(void) noexcept
{
    static int              value               = 0;
    static bool             flag                = false;
    static Orchid           history             {   };
    //
    //
    //
    const bool              is_hovered          = ImGui::IsWindowHovered(ImGuiHoveredFlags_RootAndChildWindows      );
    bool                    invoke_undo         = false;
    bool                    invoke_redo         = false;
    
    
    
    //      CASE 0 :    ONLY FETCH INPUT IF WINDOW IS HOVERED...
    if ( is_hovered )
    {
        invoke_undo         = ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_Z                        );
        invoke_redo         = ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiMod_Shift | ImGuiKey_Z       );
    }
    
    
    
    //      1.          DATA---PRESENTATION...
    ImGui::SeparatorText("Values");
    ImGui::Indent();
    //
    //
        ImGui::Text( "Value:    %d", value);
        ImGui::Text( "Flag:     %s", (flag) ? "True" : "False");
    //
    //
    ImGui::Unindent();
    
    

    //      2.          DATA---PRESENTATION...
    ImGui::NewLine();
    ImGui::SeparatorText("Orchid Information");
    ImGui::Indent();
    //
    //
        ImGui::Text(
              "Undoable:\t%zu \nRedoable:\t%zu"
             , this->S.Orchid_undo_count()
             , this->S.Orchid_redo_count()
        );
    //
    //
    ImGui::Unindent();



    //      3.          USER---CONTROLS / ACTIONS...
    ImGui::NewLine();
    ImGui::SeparatorText("Actions");
    ImGui::Indent();
    //
    //
        if ( ImGui::Button("Add  +1") )
        {
            this->S.OrchidPush( std::make_unique<IncrementAction>(value, 1) );
        }

        ImGui::SameLine();
        if ( ImGui::Button("Add  +5") )
        {
            this->S.OrchidPush( std::make_unique<IncrementAction>(value, 5) );
        }

        ImGui::SameLine();
        if ( ImGui::Button("Toggle Flag") )
        {
            this->S.OrchidPush( std::make_unique<ToggleFlagAction>(flag) );
        }
    //
    //
    ImGui::Unindent();



    //      X.          QUERY USER INPUT...
    if ( invoke_undo )          { this->S.OrchidUndo(); }         //  UNDO.   [ CTRL Z ].
    if ( invoke_redo )          { this->S.OrchidRedo(); }         //  REDO.   [ CTRL SHIFT Z ].

    return;
}



//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "2B.  Orchid TESTING".












// *************************************************************************** //
//
//
//
//      2.      "MAIN UI FUNCTIONS"...
// *************************************************************************** //
// *************************************************************************** //

//  "Begin_IMPL"
//
inline void CBDebugger::Begin_IMPL(void)
{


    this->TestOrchid();


    return;
}



//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "MAIN UI FUNCTIONS".












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
