/***********************************************************************************
*
*       ********************************************************************
*       ****            _ T I T L E B A R . H  ____  F I L E            ****
*       ********************************************************************
*              AUTHOR:      Collin A. Bond
*               DATED:      May 07, 2025.
*
**************************************************************************************
**************************************************************************************/
#include "app/app.h"
#include "app/titlebar/_titlebar.h"
#include <random>
#include <algorithm>



namespace cb { //     BEGINNING NAMESPACE "cb"...
// *************************************************************************** //
// *************************************************************************** //



//  1.      INITIALIZATION  | DEFAULT CONSTRUCTOR, DESTRUCTOR, ETC...
// *************************************************************************** //
// *************************************************************************** //

//  Default Constructor.
//
TitleBar::TitleBar(app::AppState & src)
    : S(src)                        { this->init(); }


//  "init"          | private
//
void TitleBar::init(void)           { }


//  Destructor.
//
TitleBar::~TitleBar(void)           { this->destroy(); }


//  "destroy"       | protected
//
void TitleBar::destroy(void)        { }




// *************************************************************************** //
//
//
//  1B.     PUBLIC MEMBER FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

//  "Begin"
//
void TitleBar::Begin([[maybe_unused]] const char *       uuid,
                     [[maybe_unused]] bool *             p_open,
                     [[maybe_unused]] ImGuiWindowFlags   flags)
{
    ImGuiIO &               io                      = ImGui::GetIO(); (void)io;
    ImGuiStyle &            style                   = ImGui::GetStyle();
    
    
    //  DRAW THE FLOATING, TITLEBAR MENU...
    //
    ImGui::SetNextWindowViewport(this->S.m_main_viewport->ID);
    ImGui::SetNextWindowPos( m_win_pos, ImGuiCond_Always );
    ImGui::PushStyleColor( ImGuiCol_WindowBg, S.m_titlebar_bg );
    //ImGui::PushStyleColor( ImGuiCol_Text, ImGuiCond_Always );
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize,     app::DEF_SB_TWIN_WINDOWBORDERSIZE);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding,       app::DEF_SB_TWIN_WINDOWROUNDING);
    //
    ImGui::Begin(uuid, nullptr, flags);
    {
        ImGui::PopStyleColor();
        
        this->compute_win_pos();
        this->ShoWTitleBarWindow();
 
    
    
        if (this->m_invalid_cache) [[unlikely]]     //  VALIDATE CACHE...
            { this->ValidateCache(); }
    
    }// END OF "TitleBar" WIN.
    ImGui::End();
    ImGui::PopStyleVar(2);
    
    
    return;
}


//  "ValidateCache"
//
void TitleBar::ValidateCache(void)
{
    ImGuiStyle &            style               = ImGui::GetStyle();
    this->m_invalid_cache                       = false;
    
    ImVec2                  window_padding      = style.WindowPadding;    // padding between window border and contents
    //ImVec2                  frame_padding       = style.FramePadding;     // padding within a framed widget (e.g., Button)
    //ImVec2                  item_spacing        = style.ItemSpacing;      // spacing between widgets
    //ImVec2                  item_inner_space    = style.ItemInnerSpacing; // spacing within compound widgets (e.g., between label and checkbox)
    //float                 indent_spacing      = style.IndentSpacing;    // horizontal indent per tree level
    //float                 scrollbar_size      = style.ScrollbarSize;    // width of vertical scrollbar
    //float                 grab_min_size       = style.GrabMinSize;
    
    
    const float             padx                = window_padding.x;
    const float             pady                = window_padding.y;
    this->m_win_size_cache                      = ImGui::GetItemRectSize();
    this->m_win_size_cache.x                   += 2.0 * padx;
    //this->m_win_size_cache.y                   += 2.0 * pady;
    
    
    return;
}


// *************************************************************************** //
//
//
//  2.      PROTECTED MEMBER FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

//  "ShoWTitleBarWindow"
//
void TitleBar::ShoWTitleBarWindow(void)
{
    //  Draw the SIDEBAR COLLAPSE Button...
    if ( ImGui::Button(this->S.m_show_sidebar_window ? ms_CLOSE_TEXT : ms_OPEN_TEXT) ) {
        S.m_windows[ Window::SideBar ].open = !S.m_windows[ Window::SideBar ].open;
        this->S.m_show_sidebar_window       = !this->S.m_show_sidebar_window;
        this->S.m_sidebar_ratio             = this->S.m_show_sidebar_window ? app::DEF_SB_OPEN_WIDTH : 0.0f;
    }

    return;
}



//  "compute_win_pos"
//
void TitleBar::compute_win_pos(void)
{
    
    if (S.m_show_sidebar_window)
    {
        this->m_win_pos         = utl::GetImGuiWindowCoords( S.m_windows[Window::SideBar].get_uuid(), utl::Anchor::NorthEast );
        this->m_win_pos.x      -= this->m_win_size_cache.x;
        //this->m_win_pos.y      += this->m_win_size_cache.y;
    }
    else
    {
        //this->m_button_size                         = ImGui::CalcTextSize( this->S.m_show_sidebar_window ? "-" : "+" );
        this->m_win_pos.x           = this->S.m_main_viewport->WorkPos.x;   // .m_main_viewport->WorkPos.x + S.m_main_viewport->WorkSize.x * this->S.m_sidebar_ratio;
        this->m_win_pos.y           = this->S.m_main_viewport->WorkPos.y;
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
