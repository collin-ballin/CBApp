/***********************************************************************************
*
*       ********************************************************************
*       ****            _ S E T T I N G S . h  ____  F I L E            ****
*       ********************************************************************
*
*              AUTHOR:      Collin A. Bond.
*               DATED:      July 6, 2025.
*
*       ********************************************************************
*                FILE:      [include/app/state/_state.h]
*
*
*
**************************************************************************************
**************************************************************************************/
#ifndef _CBAPP_APP_STATE_SETTINGS_H
#define _CBAPP_APP_STATE_SETTINGS_H  1



//  1.  INCLUDES    | Headers, Modules, etc...
// *************************************************************************** //
// *************************************************************************** //

//  1.1.        ** MY **  HEADERS...
#include CBAPP_USER_CONFIG
#include "cblib.h"
#include "utility/utility.h"
#include "widgets/widgets.h"
#include "app/state/_config.h"
//  #include "app/_init.h"



//  1.2     STANDARD LIBRARY HEADERS...
#include <iostream>         //  <======| std::cout, std::cerr, std::endl, ...
#include <cstdlib>          // C-Headers...
#include <stdio.h>
#include <unistd.h>
#include <cstdint>

#include <string>           //  <======| std::string, ...
#include <string_view>
#include <vector>           //  <======| std::vector, ...
#include <stdexcept>        //  <======| ...
#include <functional>
#include <atomic>



//  1.3     "DEAR IMGUI" HEADERS...
#include "imgui.h"
#include "implot.h"
#include "imgui_internal.h"



namespace cb { namespace app { //     BEGINNING NAMESPACE "cb" :: "app"...
// *************************************************************************** //
// *************************************************************************** //



// *************************************************************************** //
//
//
//
//      1.      UI-COLORS, UI-DIMENSIONS, ETC...
// *************************************************************************** //
// *************************************************************************** //

//  "UIColor"
//
enum class UIColor : uint8_t {
    //
    //      0.      "GLFW" WINDOW, DOCKSPACE, ETC...
      Viewport_BG = 0
    , Dockspace_BG
    //
    //      1.      "HOME" DELEGATOR...
    , Home_BG
    //
    //      2.      "CONTROLBAR" DELEGATOR...
    , ControlBar_BG
    //
    //      3.      "BROWSER" DELEGATOR...
    , Browser_BG
    , Browser_LeftBG
    , Browser_RightBG
    , Browser_ChildBG
    //
    //      4.      "DETAIL-VIEW" DELEGATOR...
    , DetView_BG
    //
    //
    //
    , COUNT
};



//  "cv_DEF_UI_COLORS"
//
static constexpr cblib::EnumArray< UIColor, ImVec4 >
cv_DEF_UI_COLORS     = {{
//
    //      0.      "GLFW" WINDOW, DOCKSPACE, ETC...
    /*  Viewport_BG         */    ImVec4(0.247f,    0.251f,     0.255f,     0.500f)
    /*  Dockspace_BG        */  , ImVec4(0.000f,    0.000f,     0.000f,     0.000f)
    //
    //
    //      1.      "HOME" DELEGATOR...
    /*  Home_BG             */  , ImVec4(0.125f,    0.133f,     0.141f,     1.000f)
    //
    //
    //      2.      "CONTROLBAR" DELEGATOR...
    /*  ControlBar_BG       */  , ImVec4(0.000f,    0.000f,     0.000f,     0.350f)
    //
    //
    //      3.      "BROWSER" DELEGATOR...
    /*  Browser_BG          */  , ImVec4(0.192f,    0.192f,     0.192f,     1.000f)
    /*  Browser_ChildBG     */  , ImVec4(0.141f,    0.141f,     0.141f,     1.000f)
    //
    //
    //      4.      "DETAIL-VIEW" DELEGATOR...
    /*  DetView_BG          */  , ImVec4(0.090f,    0.098f,     0.118f,     1.000f)
    //
//
}};



//  "cv_DEF_UI_COLOR_NAMES"
//
static constexpr cblib::EnumArray< UIColor, const char * >
cv_DEF_UI_COLOR_NAMES     = {{
//
    //      0.      "GLFW" WINDOW, DOCKSPACE, ETC...
    /*  Viewport_BG         */    "Viewport_BG"
    /*  Dockspace_BG        */  , "Dockspace_BG"
    //
    //
    //      1.      "HOME" DELEGATOR...
    /*  Home_BG             */  , "Home_BG"
    //
    //
    //      2.      "CONTROLBAR" DELEGATOR...
    /*  ControlBar_BG       */  , "ControlBar_BG"
    //
    //
    //      3.      "BROWSER" DELEGATOR...
    /*  Browser_BG          */  , "Browser_BG"
    /*  Browser_LeftBG      */  , "Browser_LeftBG"
    /*  Browser_RightBG     */  , "Browser_RightBG"
    //
    /*  Browser_ChildBG     */  , "Browser_ChildBG"
    //  /*  Browser_InfoLS  */  , "Browser_InfoLS"
    //
    //
    //      4.      "DETAIL-VIEW" DELEGATOR...
    /*  DetView_BG          */  , "DetView_BG"
    //
//
}};



//
//
//
// *************************************************************************** //
// *************************************************************************** //   END [[ 1.  "UI-COLORS-DIMENSIONS" ]].












// *************************************************************************** //
//
//
//
//      2.      APPLICATION SETTINGS...
// *************************************************************************** //
// *************************************************************************** //

//  "CBAppSettings"
//      PLAIN-OLD-DATA (POD) STRUCT.
//
struct CBAppSettings
{
    // *************************************************************************** //
    //      0. |    NESTED TYPENAME ALIASES.
    // *************************************************************************** //
    //  CBAPP_APPSTATE_ALIAS_API            //  *OR*    CBAPP_CBLIB_TYPES_API       //  FOR CBLIB...
    
    
    // *************************************************************************** //
    //
    // *************************************************************************** //
    //      0. |    STATIC CONSTEXPR CONSTANTS.
    // *************************************************************************** //
    static constexpr const char *           cv_TYPE_NAME                = "CBApp"       ;       //  exact section name in .ini
	static constexpr const char *           cv_SUBSECTION_SETTINGS      = "Settings"    ;
	static constexpr const char *           cv_SUBSECTION_COLORS        = "Colors"      ;
    
    
    // *************************************************************************** //
    //
    // *************************************************************************** //
    //      0. |    REFERENCES TO GLOBAL ARRAYS.
    // *************************************************************************** //
    inline static  auto                     ms_UI_COLORS                = cv_DEF_UI_COLORS          ;
    static constexpr auto                   ms_UI_COLOR_NAMES           = cv_DEF_UI_COLOR_NAMES     ;
    
//
// *************************************************************************** //
// *************************************************************************** //   END "0.  CONSTANTS AND ALIASES".



// *************************************************************************** //
//
//      1.          DATA-MEMBERS...
// *************************************************************************** //
// *************************************************************************** //
    
    // *************************************************************************** //
    //      1. |    STATE VARIABLES.
    // *************************************************************************** //
    //AppState &                          CBAPP_STATE_NAME;

    // *************************************************************************** //
    //      1. |    IMPORTANT DATA-MEMBERS.
    // *************************************************************************** //
    //  std::vector<ObjectType>             m_data                          {  };
    
    // *************************************************************************** //
    //      1. |    GENERIC DATA.
    // *************************************************************************** //
    //  bool                                m_initialized                   = false;
    //  bool                                m_first_frame                   = false;
    bool                                    m_show_debug_panel              = false;
    int32_t                                 m_selected_applet               = 0;
    float                                   m_master_volume                 = 1.0f;
    
//
// *************************************************************************** //
// *************************************************************************** //   END "1.  DATA-MEMBERS".



// *************************************************************************** //
//
//      2.A.        MEMBER FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //
    
    // *************************************************************************** //
    //      INITIALIZATION METHODS.         |   "init.cpp" ...
    // *************************************************************************** //
    static inline CBAppSettings &       instance                            (void)                          //  Meyers-Style Singleton.  Created on first call, once.
        { static CBAppSettings single = CBAppSettings();     return single; }
    
    
    // *************************************************************************** //
    //      DELETED FUNCTIONS.              |   ...
    // *************************************************************************** //
                                        CBAppSettings                       (const CBAppSettings &   )          = delete;       //  Copy. Constructor.
                                        CBAppSettings                       (CBAppSettings &&        )          = delete;       //  Move Constructor.
    CBAppSettings &                     operator =                          (const CBAppSettings &   )          = delete;       //  Assgn. Operator.
    CBAppSettings &                     operator =                          (CBAppSettings &&        )          = delete;       //  Move-Assgn. Operator.
    
    
    // *************************************************************************** //
    //      PUBLIC API.                     |   ...
    // *************************************************************************** //
    void                                LoadFromIniLine                     (const char * );
    void                                SaveToIniLines                      (ImGuiTextBuffer & ) const;
    
    
    // *************************************************************************** //
    //      PRIVATE INITIALIZATIONS.        |   "init.cpp" ...
    // *************************************************************************** //
private:
                                        CBAppSettings                       (void) noexcept                     = default;
                                        ~CBAppSettings                      (void)                              = default;
                                        
                                        
//
// *************************************************************************** //
// *************************************************************************** //   END "2A.  MEMBER FUNCS".

    
   
// *************************************************************************** //
//
//      2.B.        INLINE FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //
public:

    // *************************************************************************** //
    //      2.B. |  QUERY FUNCTIONS.
    // *************************************************************************** //
    //  "_has_query"
    //  [[nodiscard]] inline bool           _has_query                          (void) const noexcept   { return; }


    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      2.B. |  CENTRALIZED STATE MANAGEMENT FUNCTIONS.
    // *************************************************************************** //
    
    //  "RegisterIniHandler"
    //
    static inline void                  RegisterIniHandler                  (void) noexcept
    {
        ImGuiSettingsHandler            handler         {   };
        
        
        handler.TypeName                = CBAppSettings::cv_TYPE_NAME;
        handler.TypeHash                = ImHashStr( CBAppSettings::cv_TYPE_NAME );
        handler.ReadOpenFn              =  CBAppSettings::_settings_read_open;
        handler.ReadLineFn              =  CBAppSettings::_settings_read_line;
        handler.WriteAllFn              =  CBAppSettings::_settings_write_all;
        //
        ImGui::AddSettingsHandler       (&handler);
        
        return;
    }


    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      2.B. |  HANDLER FUNCTIONS.
    // *************************************************************************** //
    static void *                       _settings_read_open                 (ImGuiContext * , ImGuiSettingsHandler * , const char * )           noexcept;
    static void                         _settings_read_line                 (ImGuiContext * , ImGuiSettingsHandler * , void * , const char * )  noexcept;
    static void                         _settings_write_all                 (ImGuiContext * , ImGuiSettingsHandler * , ImGuiTextBuffer * )      noexcept;
    //
    //
    void                                _load_from_ini_line                 (const char * )            noexcept;
    void                                _save_to_ini_lines                  (ImGuiTextBuffer & ) const noexcept;


    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      2.B. |  UTILITY FUNCTIONS.
    // *************************************************************************** //
    
    //  "_display_color_palette"
    //
    inline void                         _display_color_palette              (void) noexcept
    {
        static ImGuiColorEditFlags      color_flags     = ImGuiColorEditFlags_None | ImGuiColorEditFlags_Float;
        constexpr size_t                N               = static_cast<size_t>( UIColor::COUNT );
        size_t                          i               = 0ULL;
        UIColor                         idx             = static_cast<UIColor>( i );
            
            
        for (i = 0ULL; i < N; ++i, idx = static_cast<UIColor>(i) )
        {
            ImGui::ColorEdit4( CBAppSettings::ms_UI_COLOR_NAMES[idx], (float*)&CBAppSettings::ms_UI_COLORS[idx], color_flags );
        }
        
        return;
    }
    
    
    
    
    
    
    
//
// *************************************************************************** //
// *************************************************************************** //   END "2B.  INLINE" FUNCTIONS.



//
//
// *************************************************************************** //
// *************************************************************************** //
};//	END "CBAppSettings" INLINE STRUCT DEFINITION.






//
//
//
// *************************************************************************** //
// *************************************************************************** //   END [[ 2.  "APP SETTINGS" ]].















// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
} }//   END OF "cb" :: "app" NAMESPACE.












#endif      //  _CBAPP_APP_STATE_SETTINGS_H  //
// *************************************************************************** //
// *************************************************************************** //  END.
