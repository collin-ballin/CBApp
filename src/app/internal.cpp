/***********************************************************************************
*
*       *********************************************************************
*       ****            I N T E R N A L . C P P  ____  F I L E           ****
*       *********************************************************************
*
*              AUTHOR:      Collin A. Bond.
*               DATED:      August 11, 2025.
*
**************************************************************************************
**************************************************************************************/
#include "app/app.h"



namespace cb { //     BEGINNING NAMESPACE "cb"...
// *************************************************************************** //
// *************************************************************************** //









// *************************************************************************** //
//
//
//
//      3.3.        OVERSEER FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //



//  "QuerySignalStates"
//
void App::QuerySignalStates(void)
{
    using namespace     app;
    CBSignalFlags       bits        = S.m_pending.exchange(CBSignalFlags_None, std::memory_order_acquire);
    CBSignalFlags       bit         = bits & static_cast<CBSignalFlags>(-(int)bits);      //  Isolate LSB.
    
    
    while ( bits != CBSignalFlags_None )
    {
        switch (bit)
        {
            //  1.  SIGINT / SIGTERM / CTRL_C_EVENT.
            case CBSignalFlags_Shutdown :       {
                CB_LOG(LogLevel::Info, "Program recieved <CBSignalFlags_Shutdown>.");
                
                S.m_running.store(false, std::memory_order_relaxed);
                break;
            }

            //  2.  SIGHUP.
            case CBSignalFlags_ReloadCfg :      {
                CB_LOG(LogLevel::Info, "Program recieved <CBSignalFlags_ReloadCfg>.");
                //  reload_config();
                break;
            }

            //  3.  [OPERATOR NEW] / MALLOC FAILURE.
            case CBSignalFlags_NewFailure :      {
                CB_LOG(LogLevel::Critical, "Program recieved <CBSignalFlags_NewFailure>.");
                S.m_running.store(false, std::memory_order_relaxed);
                break;
            }

            //  3.  CUSTOM SIGNAL #1 [NOT IMPLEMENTED].
            case CBSignalFlags_Custom1 :        {
                CB_LOG(LogLevel::Info, "Program recieved <CBSignalFlags_Custom1> (How did you do this? This isn't even implemented yet?)");
                break;
            }

            //  ?.  DEFAULT.
            default:                            {
                break;
            }
        }
        bits &= ~bit;         // clear processed flag
    }



    return;
}



//  "DialogHandler"
//
inline void App::DialogHandler(void)
{
    static bool     initialized  = false;


    if (S.m_dialog_queued)
    {
    
        if (!initialized) {
            initialized  = true;
            this->S.m_file_dialog.initialize(this->S.m_dialog_settings );
        }

        if ( this->S.m_file_dialog.is_open() )
        {
            //m_file_dialog.store(false, std::memory_order_release);
            if ( this->S.m_file_dialog.Begin() ) {        // returns true when finished
            
                if ( auto path = this->S.m_file_dialog.result() ) {
                    //CB_LOG(LogLevel::Info, "DialogHandler recieved filepath: {}", path.value());
                }
            }
        }
        else {
            initialized         = false;
            S.m_dialog_queued   = false;
        }
    }
 
    return;
}


















// *************************************************************************** //
//
//
//
//      3.3.        APPLICATION HANDLER FUNCTIONS...        | PRIVATE.
// *************************************************************************** //
// *************************************************************************** //

//  "KeyboardShortcutHandler"
//
void App::KeyboardShortcutHandler(void)
{
    static constexpr ImGuiInputFlags        browser_key_flags       = ImGuiInputFlags_None; //   | ~ImGuiInputFlags_Repeat; // Merged flags
    static constexpr ImGuiInputFlags        detview_key_flags       = ImGuiInputFlags_None; //   | ~ImGuiInputFlags_Repeat; // Merged flags
        //  static constexpr ImGuiInputFlags        quit_key_flags          = ImGuiInputFlags_None;
    static constexpr ImGuiInputFlags        save_key_flags          = ImGuiInputFlags_None; //   | ~ImGuiInputFlags_Repeat; // Merged flags
    static constexpr ImGuiInputFlags        open_key_flags          = ImGuiInputFlags_None;
    static constexpr ImGuiInputFlags        undo_key_flags          = ImGuiInputFlags_None;
    static constexpr ImGuiInputFlags        redo_key_flags          = ImGuiInputFlags_None;
    //
    static constexpr ImGuiKeyChord          BROWSER_KEY             = ImGuiKey_GraveAccent;
    static constexpr ImGuiKeyChord          DETVIEW_KEY             = ImGuiMod_Shift | ImGuiKey_GraveAccent; //ImGuiMod_Shift | ImGuiKey_Apostrophe;
        //  static constexpr ImGuiKeyChord          QUIT_KEY                = ImGuiMod_Alt   | ImGuiKey_F4;
    static constexpr ImGuiKeyChord          SAVE_KEY                = ImGuiMod_Ctrl  | ImGuiKey_S;
    static constexpr ImGuiKeyChord          OPEN_KEY                = ImGuiMod_Ctrl  | ImGuiKey_O;
    static constexpr ImGuiKeyChord          UNDO_KEY                = ImGuiMod_Ctrl  | ImGuiKey_Z;
    static constexpr ImGuiKeyChord          REDO_KEY                = ImGuiMod_Ctrl  | ImGuiMod_Shift | ImGuiKey_Z;
    //
    [[maybe_unused]] ImGuiIO &              io                      = ImGui::GetIO();



    //  1.      ** MY HOTKEYS **    [ UNIVERSALLY AVAILABLE ]...
    //
    //          1.1.        OPEN/CLOSE BROWSER.
    if ( ImGui::IsKeyChordPressed(  BROWSER_KEY,    browser_key_flags) )        { this->m_controlbar.toggle_sidebar(); }
    //
    //          1.2.        OPEN/CLOSE DETAIL VIEW.
    if ( ImGui::IsKeyChordPressed(  DETVIEW_KEY,    detview_key_flags) )        { this->m_detview.toggle(); }
    //
    //          1.3.        QUIT PROGRAM.
    //  if ( ImGui::IsKeyChordPressed(  QUIT_KEY,       quit_key_flags) )           { this->enqueue_signal( app::CBSignalFlags_Shutdown ); }
    
    
    
    //  2.      CONDITIONALLY-AVAILABLE HOTKEYS...
    //
    //          2.3.        SAVE HANDLER.
    if ( ImGui::IsKeyChordPressed(  SAVE_KEY,       save_key_flags) )           { this->SaveHandler(); }
    //
    //          2.4.        OPEN HANDLER.
    if ( ImGui::IsKeyChordPressed(  OPEN_KEY,       open_key_flags) )           { this->OpenHandler(); }
    //
    //          2.5.        UNDO HANDLER.
    if ( ImGui::IsKeyChordPressed(  UNDO_KEY,       undo_key_flags) )           { this->UndoHandler(); }
    //
    //          2.6.        REDO HANDLER.
    if ( ImGui::IsKeyChordPressed(  REDO_KEY,       redo_key_flags) )           { this->RedoHandler(); }



    //  3.      FILE DIALOG HANDLER...
    this->DialogHandler();
    
    
    return;
}





//  "SaveHandler"
//
void App::SaveHandler(void)
{
    switch ( S.GetCurrentApplet() ) {
        case Applet::CCounterApp            : { this->m_counter_app     .save();        break; }
        case Applet::EditorApp              : { this->m_editor_app      .save();        break; }
        case Applet::GraphApp               : { this->m_graph_app       .save();        break; }
        case Applet::MimicApp               : { this->m_mimic_app       .save();        break; }
        default                             : { this->SaveHandler_Default();            break; }
    }
    
    return;
}


//  "OpenHandler"
//
void App::OpenHandler(void)
{
    switch ( S.GetCurrentApplet() ) {
        case Applet::CCounterApp            : { /* ... */                               break; }
        case Applet::EditorApp              : { this->m_editor_app.open();              break; }
        case Applet::GraphApp               : { /* ... */                               break; }
        case Applet::MimicApp               : { /* ... */                               break; }
        default                             : { /* this->OpenHandler_Default(); */      break; }
    }
    
    return;
}


//  "UndoHandler"
//
void App::UndoHandler(void)
{
    switch ( S.GetCurrentApplet() ) {
        case Applet::CCounterApp            : { this->m_counter_app     .undo();        break; }
        case Applet::EditorApp              : { this->m_editor_app      .undo();        break; }
        case Applet::GraphApp               : { this->m_graph_app       .undo();        break; }
        case Applet::MimicApp               : { /* ... */                               break; }
        default                             : { this->UndoHandler_Default();            break; }
    }
    
    return;
}


//  "RedoHandler"
//
void App::RedoHandler(void)
{
    switch ( S.GetCurrentApplet() ) {
        case Applet::CCounterApp            : { this->m_counter_app     .redo();        break; }
        case Applet::EditorApp              : { this->m_editor_app      .redo();        break; }
        case Applet::GraphApp               : { this->m_graph_app       .redo();        break; }
        case Applet::MimicApp               : { /* ... */                               break; }
        default                             : { this->RedoHandler_Default();            break; }
    }
    
    return;
}




// *************************************************************************** //
//
//
//
//  3.4     PLACEHOLDER HANDLERS...
// *************************************************************************** //
// *************************************************************************** //

//  "SaveHandler_Default"
//
void App::SaveHandler_Default(void)
{
    if (!S.m_dialog_queued)
    {
        S.m_dialog_queued = true;
        this->S.m_dialog_settings   = {
            /* type               = */  cb::FileDialog::Type::Save,
            /* window_name        = */  "Save File...",
            /* default_filename   = */  "",
            /* required_extension = */  "",
            /* valid_extensions   = */  {".json", ".cbjson", ".txt"},
            /* starting_dir       = */  std::filesystem::current_path()
        };

    }
    
    CB_LOG( LogLevel::Info, "Applet::HomeApp--save" );
    
    return;
}


//  "CopyHandler_Default"
//
void App::CopyHandler_Default(void)
{
    return;
}


//  "PasteHandler_Default"
//
void App::PasteHandler_Default(void)
{
    return;
}


//  "UndoHandler_Default"
//
void App::UndoHandler_Default(void) {
    S.m_logger.info( "Applet::HomeApp--undo" );
    return;
}


//  "RedoHandler_Default"
//
void App::RedoHandler_Default(void) {
    S.m_logger.info( "Applet::HomeApp--undo" );
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
// *************************************************************************** //  END.
