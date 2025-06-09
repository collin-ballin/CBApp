/***********************************************************************************
*
*       ********************************************************************
*       ****        D E T A I L _ V I E W . C P P  ____  F I L E        ****
*       ********************************************************************
*              AUTHOR:      Collin A. Bond
*               DATED:      June 05, 2025.
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




//  "DefaultTabRenderFunc"
//
static void DefaultTabRenderFunc([[maybe_unused]] const char * uuid, [[maybe_unused]] bool * p_open, [[maybe_unused]] ImGuiWindowFlags flags) {
    if (!p_open)
        return;
        
    ImGui::Text("Here is some text on a window with no render function.");
    return;
}


//----------------------------------
// Helper: hosting mode selector
//----------------------------------
enum class TabHostMode {
    StaticTabBar,      // Option 2 – fixed tab‑bar
    DockableWindows    // Option 3 – dockable windows
};






// *************************************************************************** //
//
//
//  1.      INITIALIZATION  | DEFAULT CONSTRUCTOR, DESTRUCTOR, ETC...
// *************************************************************************** //
// *************************************************************************** //

//  Default Constructor.
//
DetailView::DetailView(app::AppState & src)
    : S(src)                        { }


//  "initialize"
//
void DetailView::initialize(void)
{
    if (this->m_initialized)
        return;
        
    this->init();
    return;
}


//  "init"          | protected
//
void DetailView::init(void) {
    //  INITIALIZE THE DOCK NODE THAT IS HOSTED BY THE DETAIL-VIEW WINDOW...
    S.m_detview_dockspace_id                        = ImHashStr(S.m_detview_dockspace_uuid);
    
    this->m_window_class.DockNodeFlagsOverrideSet   = S.m_detview_window_flags;
    this->m_is_open                                 = !S.m_show_detview_window;
    
    return;
}


//  Destructor.
//
DetailView::~DetailView(void)               { this->destroy(); }


//  "destroy"       | protected
//
void DetailView::destroy(void)              { }






// *************************************************************************** //
//
//
//  1B.     PUBLIC API MEMBER FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

//  "open"
//
void DetailView::open(void) {
    this->S.m_show_detview_window           = true;
    return;
}

//  "close"
//
void DetailView::close(void) {
    this->S.m_show_detview_window           = false;
    return;
}

//  "toggle"
//
void DetailView::toggle(void) {
    this->S.m_show_detview_window = !this->S.m_show_detview_window;
}




// *************************************************************************** //
//
//
//  1C.     PUBLIC MEMBER FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

//  "Begin"
//
void DetailView::Begin([[maybe_unused]] const char *        uuid,
                       [[maybe_unused]] bool *              p_open,
                       [[maybe_unused]] ImGuiWindowFlags    flags)
{
    [[maybe_unused]] ImGuiIO &      io              = ImGui::GetIO(); (void)io;
    [[maybe_unused]] ImGuiStyle &   style           = ImGui::GetStyle();
    bool                            update          = (this->m_is_open != S.m_show_detview_window);
    
    
    if (!update && !S.m_show_detview_window)
        return;

    
    
    //  1.  CREATE THE WINDOW AND BEGIN APPENDING WIDGETS INTO IT...
    ImGui::PushStyleColor(ImGuiCol_WindowBg, S.m_detview_bg);   // Push before ImGui::Begin()
    this->m_window_class.DockNodeFlagsOverrideSet   = S.m_detview_window_flags;
    ImGui::SetNextWindowClass(&this->m_window_class);
    ImGui::Begin(uuid, p_open, flags);
        ImGui::PopStyleColor();
        
        ImGui::DockSpace(S.m_detview_dockspace_id, ImVec2(0,0), S.m_detview_dockspace_flags);
        
        if (update)
        {
        
            set_visibility_IMPL(S.m_show_detview_window);
        
        }
        

    ImGui::End();
    
    return;
}



// *************************************************************************** //
//
//
//  2.      PROTECTED MEMBER FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

//  "set_visibility_IMPL"
//
void DetailView::set_visibility_IMPL(const bool p_open) {

    //  CASE 1 :    CLOSE THE WINDOW...
    if (!p_open) {
        this->close_all();
    }
    //
    //  CASE 2 :    OPEN THE WINDOW...
    else {
        this->open_all();
    }


    return;
}


//  "close_all"
//
void DetailView::close_all(void)
{
    for (auto & win : S.m_detview_windows) {
        if (win) {
            win->open = false;
        }
    }
    this->m_is_open                         = false;
    //this->S.m_show_detview_window           = false;
    return;
}


//  "open_all"
//
void DetailView::open_all(void)
{
    for (auto & win : S.m_detview_windows) {
        if (win) {
            win->open = true;
            S.DockAtDetView( win->uuid.c_str() );
        }
    }
    this->m_is_open                         = true;
    //this->S.m_show_detview_window           = true;
    return;
}

















// *************************************************************************** //
//
//
//  ?.      ???...
// *************************************************************************** //
// *************************************************************************** //

//  "TestTabBar"
//
void DetailView::TestTabBar(void)
{
    //----------------------------------------------------------
    // 1. Persistent tab list
    //----------------------------------------------------------
    static std::vector<Tab_t> tabs = {
        Tab_t("Tab 1", true,  true, ImGuiTabItemFlags_None, nullptr),
        Tab_t("Tab 2", true,  true, ImGuiTabItemFlags_None, nullptr)
    };

    static TabHostMode host_mode  = TabHostMode::StaticTabBar;
    static bool        redock_all = false; // one‑shot flag

    //----------------------------------------------------------
    // 2. Mode switcher + redock button
    //----------------------------------------------------------
    ImGui::SeparatorText("Tab Host Mode");
    ImGui::RadioButton("TabBar + Tabs",      reinterpret_cast<int*>(&host_mode), static_cast<int>(TabHostMode::StaticTabBar));
    ImGui::SameLine();
    ImGui::RadioButton("DockSpace + Window", reinterpret_cast<int*>(&host_mode), static_cast<int>(TabHostMode::DockableWindows));

    if (host_mode == TabHostMode::DockableWindows) {
        ImGui::SameLine();
        if (ImGui::Button("Redock all"))
            redock_all = true;
    }
    ImGui::Separator();

    //----------------------------------------------------------
    // 3. Rendering path A: single tab‑bar (static)
    //----------------------------------------------------------
    if (host_mode == TabHostMode::StaticTabBar)
    {
        constexpr ImGuiTabBarFlags TABBAR_FLAGS =
              ImGuiTabBarFlags_AutoSelectNewTabs
            | ImGuiTabBarFlags_Reorderable
            | ImGuiTabBarFlags_FittingPolicyResizeDown
            | ImGuiTabBarFlags_NoCloseWithMiddleMouseButton
            | ImGuiTabBarFlags_TabListPopupButton;

        if (ImGui::BeginTabBar("MY_TAB_BAR", TABBAR_FLAGS))
        {
            //----------------------- Leading help tab ("?")
            if (ImGui::TabItemButton("?", ImGuiTabItemFlags_Leading | ImGuiTabItemFlags_NoTooltip))
                ImGui::OpenPopup("HelpMenu");
            if (ImGui::BeginPopup("HelpMenu"))
            {
                ImGui::TextUnformatted("Hello from the help tab!\nAdd instructions here.");
                ImGui::EndPopup();
            }

            //----------------------- User‑defined tabs
            for (Tab_t& tab : tabs)
            {
                if (!ImGui::BeginTabItem(tab.get_uuid(),
                                          tab.no_close ? nullptr : &tab.open,
                                          tab.flags))
                    continue;

                if (tab.render_fn)
                    tab.render_fn(tab.get_uuid(), &tab.open, ImGuiWindowFlags_None);
                else
                    DefaultTabRenderFunc(tab.get_uuid(), &tab.open, ImGuiWindowFlags_None);

                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }
        return; // early exit; nothing else to do in this mode
    }

    //----------------------------------------------------------
    // 4. Rendering path B: dockable windows
    //----------------------------------------------------------
    //
    //  const ImGuiID dockspace_id = ImGui::GetID("APP_DOCKSPACE");
    //  ImGui::DockSpace(dockspace_id, ImVec2(0, 0), ImGuiDockNodeFlags_None);
    //
    ImGui::DockSpace(S.m_detview_dockspace_id, ImVec2(0,0), S.m_detview_dockspace_flags);
    

    // Optional: class to tweak the appearance of windows once docked
    static ImGuiWindowClass docked_window_class;
    docked_window_class.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_None; // ImGuiDockNodeFlags_HiddenTabBar; // hide internal tab bar when docked

    for (Tab_t& tab : tabs)
    {
        if (!tab.open)
            continue;

        // 4‑A. Pre‑dock / redock into the root
        ImGuiCond cond = redock_all ? ImGuiCond_Always : ImGuiCond_FirstUseEver;
        ImGui::SetNextWindowDockID(S.m_detview_dockspace_id, cond);
        redock_all = false; // reset after first use

        // 4‑B. Apply window class (e.g., hide tab‑bar when docked)
        ImGui::SetNextWindowClass(&docked_window_class);

        // 4‑C. Translate TabItem flags to Window flags (unsaved marker)
        ImGuiWindowFlags win_flags = 0;
        if (tab.flags & ImGuiTabItemFlags_UnsavedDocument)
            win_flags |= ImGuiWindowFlags_UnsavedDocument;

        // 4‑D. Begin window (no close button if tab.no_close)
        if (ImGui::Begin(tab.get_uuid(),
                         tab.no_close ? nullptr : &tab.open,
                         win_flags))
        {
            if (tab.render_fn)
                tab.render_fn(tab.get_uuid(), &tab.open, win_flags);
            else
                DefaultTabRenderFunc(tab.get_uuid(), &tab.open, win_flags);
        }
        ImGui::End();
    }
    return;
}



// *************************************************************************** //
//
//
//  2.      PROTECTED MEMBER FUNCTIONS...
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
