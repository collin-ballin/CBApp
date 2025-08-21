/***********************************************************************************
*
*       *********************************************************************
*       ****               I N I T . C P P  ____  F I L E                ****
*       *********************************************************************
*
*              AUTHOR:      Collin A. Bond
*               DATED:      August 20, 2025.
*
**************************************************************************************
**************************************************************************************/
#include "widgets/editor/editor.h"



namespace cb {  //     BEGINNING NAMESPACE "cb"...
// *************************************************************************** //
// *************************************************************************** //






// *************************************************************************** //
//
//
//  1.  INITIALIZATION METHODS...
// *************************************************************************** //
// *************************************************************************** //

//  Default Constructor.
//
Editor::Editor(app::AppState & src)
    : CBAPP_STATE_NAME(src)
    , m_it( std::make_unique<Interaction>() )
{
    namespace           fs                          = std::filesystem;
    this->m_window_class.DockNodeFlagsOverrideSet   = ImGuiDockNodeFlags_HiddenTabBar;
    
    
    //  INITIALIZE EACH RESIDENT OVERLAY-WINDOW...
    for (size_t i = 0; i < static_cast<size_t>( Resident::COUNT ); ++i)
    {
        Resident idx = static_cast<Resident>(i);
        this->_dispatch_resident_draw_fn( idx );
    }
    
    
    
    //  2.  LOAD DEFAULT TESTS FROM FILE...
    if ( !m_editor_S.m_filepath.empty() && fs::exists(m_editor_S.m_filepath) && fs::is_regular_file(m_editor_S.m_filepath) )
    {
        this->S.m_logger.debug( std::format("Editor | loading from default file, \"{}\"", m_editor_S.m_filepath.string()) );
        this->load_async(m_editor_S.m_filepath);
    }
    else {
        this->S.m_logger.warning( std::format("Editor | unable to load default file, \"{}\"", m_editor_S.m_filepath.string()) );
    }
    
    
    
    //  3.  INITIALIZE FUNCTIONS FOR DEBUGGER OVERLAY WINDOW...
    //  using               DebugItem                   = DebuggerState::DebugItem;
    this->m_debugger.windows    = {{
            {   "Hit Detection",    true,       DebuggerState::ms_FLAGS,        [this]{ this->_debugger_hit_detection   (); }       }
        ,   {   "Interaction",      false,      DebuggerState::ms_FLAGS,        [this]{ this->_debugger_interaction     (); }       }
        ,   {   "More Info",        false,      DebuggerState::ms_FLAGS,        [this]{ this->_debugger_more_info       (); }       }
    }};


    
    return;
}


//  Destructor.
//
Editor::~Editor(void)
{
    this->_clear_all();
}


//  "_dispatch_resident_draw_fn"
//
void Editor::_dispatch_resident_draw_fn(Resident idx)
{
    ResidentEntry &     entry   = m_residents[idx];

    //  Attatch the correct callback function for each RESIDENTIAL OVERLAY WINDOW...
    switch (idx)
    {
        case Resident::Debugger :                   //  1.  DEBUGGER-TOOL OVERLAY.
        { entry.cfg.draw_fn         = [this]{ _draw_debugger_resident(); };         break;      }
        
        case Resident::Selection :                  //  2.  SELECTION OVERLAY.
        { entry.cfg.draw_fn         = [this]{ _draw_selection_resident(); };        break;      }
    
        case Resident::Shape :                      //  3.  SHAPE-TOOL OVERLAY.
        { entry.cfg.draw_fn         = [this]{ _draw_shape_resident(); };            break;      }
    //
    //
    //
    //      4.      UI-TRAITS OVERLAY
        case Resident::UITraits :
        { entry.cfg.draw_fn         = [this]{ _draw_ui_traits_resident(); };        break;      }
    //
    //
    //
        default :                                   //  DEFAULT :  Failure.
        { IM_ASSERT(true && "Unknown resident index"); }
    }


    entry.id    = m_overlays.add_resident(entry.cfg, entry.style);
    entry.ptr   = m_overlays.get_resident(entry.id);   // now safe
    
    
    //  IF ANY ADDITIONAL FLAGS SET, FLIP THEIR VALUE IN THE OVERLAY...
        
        
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
