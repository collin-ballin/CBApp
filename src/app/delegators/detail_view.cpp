/***********************************************************************************
*
*       ********************************************************************
*       ****        D E T A I L _ V I E W . C P P  ____  F I L E        ****
*       ********************************************************************
*              AUTHOR:      Collin A. Bond.
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
inline void DetailView::init(void) {
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
    
    
    
    //      1.      IF NO UPDATE ON VISIBILITY STATUS, EXIT...
    if ( !update  &&  !S.m_show_detview_window )    { return; }

    
    
    //      2.      CREATE THE WINDOW AND BEGIN APPENDING WIDGETS INTO IT...
    ImGui::PushStyleColor( ImGuiCol_WindowBg, S.m_detview_bg );
    this->m_window_class.DockNodeFlagsOverrideSet   = S.m_detview_window_flags;
    ImGui::SetNextWindowClass( &this->m_window_class );
    //
    ImGui::Begin(uuid, p_open, flags);
    //
    //
        ImGui::PopStyleColor();
        ImGui::DockSpace(S.m_detview_dockspace_id, ImVec2(0,0), S.m_detview_dockspace_flags);
        
        if (update)
        {
        
            set_visibility_IMPL(S.m_show_detview_window);
        
        }
    //
    //
    ImGui::End();
    
    
    
    //      3.      RE-FOCUS LAST FOCUSED WINDOW AFTER CLOSE-AND_REOPEN...
    if ( this->m_queue_refocus )
    {
        this->m_queue_refocus   = false;
        
        if (this->m_top_win) {
            ImGui::SetWindowFocus( this->m_top_win->Name );
        }
    }
    
    
    
    return;
}

//
//
//
// *************************************************************************** //
// *************************************************************************** //






// *************************************************************************** //
//
//
//
//      2.      "MAIN UI FUNCTIONS"...
// *************************************************************************** //
// *************************************************************************** //


//  "set_visibility_IMPL"
//
inline void DetailView::set_visibility_IMPL(const bool p_open) {

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
inline void DetailView::close_all(void)
{
    this->cache_current_window();
    
    for (auto & win : S.m_detview_windows)
    {
        if (win) {
            win->open = false;
        }
    }
    this->m_is_open                         = false;
    this->m_queue_refocus                   = false;
    return;
}


//  "open_all"
//
inline void DetailView::open_all(void)
{
    S.m_detview_dockspace_flags     = (S.m_detview_windows.size() < 1)
        ? (S.m_detview_dockspace_flags | ImGuiDockNodeFlags_HiddenTabBar)  // set the bit
        : (S.m_detview_dockspace_flags & ~ImGuiDockNodeFlags_HiddenTabBar);  // clear the bit
        
    for (auto & win : S.m_detview_windows)
    {
        if (win) {
            win->open = true;   S.DockAtDetView( win->uuid.c_str() );
        }
    }
    
    this->m_is_open                         = true;
    this->m_queue_refocus                   = true;
    
    return;
}


//  void DetailView::open_all(void)
//  {
//      for (auto & win : S.m_detview_windows) {
//          if (win) {
//              win->open = true;
//              S.DockAtDetView( win->uuid.c_str() );
//          }
//      }
//      this->m_is_open                         = true;
//      //this->S.m_show_detview_window           = true;
//      return;
//  }




//  "cache_current_window"
//
inline void DetailView::cache_current_window(void) noexcept
{



    ImGuiDockNode &                                     dock        = *S.GetDetViewDockNodeID();
    [[maybe_unused]] const ImVector<ImGuiWindow*> &     windows     = dock.Windows;
    
    
    
    //  for ( const auto * win : windows )
    //  {
    //      const char *    name    = win->Name;
    //  
    //  
    //  
    //      std::cout << "\"" << win->Name << "\"\t" << std::endl;
    //  
    //  
    //          //  char*                   Name;                               // Window name, owned by the window.
    //          //  ImGuiID                 ID;
    //  
    //  }
    
    
    
    
    
    
    
    
    
    //this->m_top_win_ID          = S.GetDetViewDockedWindowID();

    //ImGuiContext &      g       = *GImGui;

    //ImGuiWindow *       win     = (ImGuiWindow*)g.WindowsById.GetVoidPtr( m_top_win_ID );;
    
    




    //  this->m_top_win_ID          = S.GetDetViewDockedWindowID();
    //  this->m_top_win             = (this->m_top_win_ID)
    //                                  ? ImGui::FindWindowByID(this->m_top_win_ID)
    //                                  : nullptr;
    












    //  ImGuiWindow * docked_window = ImGui::FindWindowByName("YourWindowName");
    //  if (docked_window && docked_window->DockNode && docked_window->DockNode->TabBar)
    //  {
    //      ImGuiTabBar* tab_bar = docked_window->DockNode->TabBar;
    //      if (tab_bar->VisibleTabId != 0) {
    //          ImGuiID active_window_id = tab_bar->VisibleTabId;
    //          // Now use this ID to lookup the window if needed:
    //          ImGuiWindow* active_window = ImGui::FindWindowByID(active_window_id);
    //          if (active_window) {
    //              // active_window->Name or active_window->ID gives you the currently visible docked window
    //          }
    //      }
    //  }

    
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
//  ?.      ???...
// *************************************************************************** //
// *************************************************************************** //

// Dummy data structure that we use for the Table demo.
// (pre-C++11 doesn't allow us to instantiate ImVector<MyItem> template if this structure is defined inside the demo function)
namespace
{
// We are passing our own identifier to TableSetupColumn() to facilitate identifying columns in the sorting code.
// This identifier will be passed down into ImGuiTableSortSpec::ColumnUserID.
// But it is possible to omit the user id parameter of TableSetupColumn() and just use the column index instead! (ImGuiTableSortSpec::ColumnIndex)
// If you don't use sorting, you will generally never care about giving column an ID!
enum MyItemColumnID
{
    MyItemColumnID_ID,
    MyItemColumnID_Name,
    MyItemColumnID_Action,
    MyItemColumnID_Quantity,
    MyItemColumnID_Description
};

struct MyItem
{
    int         ID;
    const char* Name;
    int         Quantity;

    // We have a problem which is affecting _only this demo_ and should not affect your code:
    // As we don't rely on std:: or other third-party library to compile dear imgui, we only have reliable access to qsort(),
    // however qsort doesn't allow passing user data to comparing function.
    // As a workaround, we are storing the sort specs in a static/global for the comparing function to access.
    // In your own use case you would probably pass the sort specs to your sorting/comparing functions directly and not use a global.
    // We could technically call ImGui::TableGetSortSpecs() in CompareWithSortSpecs(), but considering that this function is called
    // very often by the sorting algorithm it would be a little wasteful.
    static const ImGuiTableSortSpecs* s_current_sort_specs;

    static void SortWithSortSpecs(ImGuiTableSortSpecs* sort_specs, MyItem* items, int items_count)
    {
        s_current_sort_specs = sort_specs; // Store in variable accessible by the sort function.
        if (items_count > 1)
            qsort(items, (size_t)items_count, sizeof(items[0]), MyItem::CompareWithSortSpecs);
        s_current_sort_specs = NULL;
    }

    // Compare function to be used by qsort()
    static int IMGUI_CDECL CompareWithSortSpecs(const void* lhs, const void* rhs)
    {
        const MyItem* a = (const MyItem*)lhs;
        const MyItem* b = (const MyItem*)rhs;
        for (int n = 0; n < s_current_sort_specs->SpecsCount; n++)
        {
            // Here we identify columns using the ColumnUserID value that we ourselves passed to TableSetupColumn()
            // We could also choose to identify columns based on their index (sort_spec->ColumnIndex), which is simpler!
            const ImGuiTableColumnSortSpecs* sort_spec = &s_current_sort_specs->Specs[n];
            int delta = 0;
            switch (sort_spec->ColumnUserID)
            {
            case MyItemColumnID_ID:             delta = (a->ID - b->ID);                break;
            case MyItemColumnID_Name:           delta = (strcmp(a->Name, b->Name));     break;
            case MyItemColumnID_Quantity:       delta = (a->Quantity - b->Quantity);    break;
            case MyItemColumnID_Description:    delta = (strcmp(a->Name, b->Name));     break;
            default: IM_ASSERT(0); break;
            }
            if (delta > 0)
                return (sort_spec->SortDirection == ImGuiSortDirection_Ascending) ? +1 : -1;
            if (delta < 0)
                return (sort_spec->SortDirection == ImGuiSortDirection_Ascending) ? -1 : +1;
        }

        // qsort() is instable so always return a way to differentiate items.
        // Your own compare function may want to avoid fallback on implicit sort specs.
        // e.g. a Name compare if it wasn't already part of the sort specs.
        return (a->ID - b->ID);
    }
};
const ImGuiTableSortSpecs* MyItem::s_current_sort_specs = NULL;
}


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
//
// *************************************************************************** //
// *************************************************************************** //   END "???".












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
