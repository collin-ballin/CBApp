//
//  cb_demo.cpp
//  CBApp
//
//  Created by Collin Bond on 4/26/25.
//
// *************************************************************************** //
// *************************************************************************** //
#include "widgets/_cb_demo.h"



namespace cb { //     BEGINNING NAMESPACE "cb"...
// *************************************************************************** //
// *************************************************************************** //



//  1.      STATIC DATA MEMBERS (GLOBAL TO THIS FILE)...
// *************************************************************************** //
// *************************************************************************** //
// using dim_type      = CBDemo::dim_type;
// using value_type    = CBDemo::value_type;

// static constexpr dim_type                               ms_control_width    = dim_type(225);
// static constexpr dim_type                               ms_scale_width      = dim_type(90);
// static constexpr std::pair<dim_type, dim_type>          ms_width_range      = { dim_type(500),          dim_type(1200) };
// static constexpr std::pair<dim_type, dim_type>          ms_height_range     = { dim_type(500),          dim_type(1200) };
// static constexpr std::pair<value_type, value_type>      ms_vlimits           = { value_type(-10),        value_type(10) };






// *************************************************************************** //
//
//
//  1.      INITIALIZATION  | DEFAULT CONSTRUCTOR, DESTRUCTOR, ETC...
// *************************************************************************** //
// *************************************************************************** //

//  Default Constructor.
//
CBDemo::CBDemo(void)
{
    this->init();
}

//  "init"
//
void CBDemo::init(void) {
    return;
}




//  Destructor.
//
CBDemo::~CBDemo(void)
{
    this->destroy();
}


//  "destroy"
//
void CBDemo::destroy(void) {
    return;
}





// *************************************************************************** //
//
//
//  2A.     PRIMARY MEMBER FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

//  "Begin"
//
void CBDemo::Begin(bool * p_open)
{
    //  1.  CREATE A MAIN-WINDOW FOR THE DEMO...
    ImGui::Begin("CBDemo Window", p_open);
    {

    
        //  2.  INVOKE THE SUBSIDIARY MANAGER FUNCTION...
        this->begin_IMPL();



    }
    ImGui::End();
    return;
}


//  "begin_IMPL"
//
void CBDemo::begin_IMPL(void)
{
    //  1.  SUB-ITEM #1...
    ImGui::SetNextItemOpen(true, ImGuiCond_Once);
    if (ImGui::CollapsingHeader("Placeholder 1"))
    {
        this->placeholder_1();
    }


    //  2.  SUB-ITEM #2...
    ImGui::SetNextItemOpen(true, ImGuiCond_Once);
    if (ImGui::CollapsingHeader("Placeholder 2"))
    {
        this->placeholder_2();
    }


    //  3.  SUB-ITEM #3...
    ImGui::SetNextItemOpen(true, ImGuiCond_Once);
    if (ImGui::CollapsingHeader("Placeholder 3"))
    {
        this->placeholder_3();
    }



    return;
}


// *************************************************************************** //
//
//
//  2B.     SECONDARY MEMBER FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //



//  "placeholder_1"
//
void CBDemo::placeholder_1(void)
{


    return;
}



//  "placeholder_2"
//
void CBDemo::placeholder_2(void)
{


    return;
}



//  "placeholder_3"
//
void CBDemo::placeholder_3(void)
{


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
