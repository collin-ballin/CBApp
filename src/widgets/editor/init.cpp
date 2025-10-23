/***********************************************************************************
*
*       *********************************************************************
*       ****               I N I T . C P P  ____  F I L E                ****
*       *********************************************************************
*
*              AUTHOR:      Collin A. Bond.
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
    : CBAPP_STATE_NAME      ( src                                                               )
    , m_ov_manager          ( world_to_pixels                                                   )
    , m_it                  ( std::make_unique<Interaction>()                                   )
    , m_menu_state          ( std::make_unique<app::MenuState_t>()                              )
    , m_render_ctx          (
          world_to_pixels           , pixels_to_world
        , find_vertex               , find_vertex
        , m_vertices
    )
    , m_vertex_style        ( world_to_pixels, ms_VERTEX_STYLES[ VertexStyleType::Default ]     )
{
    using                   namespace               app;
    namespace               fs                      = std::filesystem;
    this->m_window_class.DockNodeFlagsOverrideSet   = ImGuiDockNodeFlags_HiddenTabBar;
    
    
    
    //      1.      INITIALIZE THE EDITOR'S MENU STATE...
    MenuState &             MS          = *this->m_menu_state;
    //
    //              1.1.    ASSIGN MENU-CALLBACKS.
    //  MS.m_callbacks.custom_menus         = {
    //      {
    //          /*  label       */    "Object"
    //          /*  render_fn   */  , [this] { _MENUBAR_object_menu(); }
    //      }
    //  };
    //  MS.set_capability                   (CBMenuCapabilityFlags_CustomMenus);
    MS.m_capabilities                   = app::CBMenuCapabilityFlags_None;
    
    
    
    
    
    
    //      INITIALIZE LABEL CALLBACKS...
    //  this->ms_default_label_fn   = [this](const char * t, float lw, float ww) {
    
    
    
    //      INITIALIZE EACH RESIDENT OVERLAY-WINDOW...
    for (size_t i = 0; i < static_cast<size_t>( Resident::COUNT ); ++i)
    {
        Resident idx = static_cast<Resident>(i);
        this->_dispatch_resident_draw_fn( idx );
    }
    
    
    
    //      2.      LOAD DEFAULT TESTS FROM FILE...
    if ( !m_editor_S.m_filepath.empty() && fs::exists(m_editor_S.m_filepath) && fs::is_regular_file(m_editor_S.m_filepath) )
    {
        this->S.m_logger.debug( std::format("Editor | loading from default file, \"{}\"", m_editor_S.m_filepath.string()) );
        this->load_async(m_editor_S.m_filepath);
    }
    else {
        this->S.m_logger.warning( std::format("Editor | unable to load default file, \"{}\"", m_editor_S.m_filepath.string()) );
    }
    
    
    
    //      3.      INITIALIZE FUNCTIONS FOR DEBUGGER OVERLAY WINDOW...
    //  using               DebugItem                   = DebuggerState::DebugItem;
    this->m_debugger.windows    = { {
          {   "State"         , true           , DebuggerState::ms_FLAGS       , [this]{ this->_DEBUGGER_state           (); }       }
        , {   "Canvas"        , false          , DebuggerState::ms_FLAGS       , [this]{ this->_DEBUGGER_canvas          (); }       }
        , {   "Misc."         , true           , DebuggerState::ms_FLAGS       , [this]{ this->_DEBUGGER_misc            (); }       }
    } };
    //
    //  this->m_debugger.windows    = {{
    //          {   "Hit Detection",    false,       DebuggerState::ms_FLAGS,        [this]{ this->_debugger_hit_detection   (); }       }
    //      ,   {   "Interaction",      true,      DebuggerState::ms_FLAGS,        [this]{ this->_debugger_interaction     (); }       }
    //      ,   {   "More Info",        false,      DebuggerState::ms_FLAGS,        [this]{ this->_debugger_more_info       (); }       }
    //  }};


    
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
    //      5.      UI-OBJECTS OVERLAY
        case Resident::UIObjects :
        { entry.cfg.draw_fn         = [this]{ _draw_ui_objects_resident(); };       break;      }
    //
    //
    //
        default :                                   //  DEFAULT :  Failure.
        { IM_ASSERT(true && "Unknown resident index"); }
    }


    entry.id            = m_ov_manager.add_resident(entry.cfg, entry.style);
    entry.ptr           = m_ov_manager.get_resident(entry.id);   // now safe
    //
    //  m_overlays [idx]    = std::make_unique( entry.ptr );
    
    
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
