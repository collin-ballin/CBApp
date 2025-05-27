//
//  _cb_demo.h
//  CBApp
//
//  Created by Collin Bond on 4/16/25.
//
// *************************************************************************** //
// *************************************************************************** //
#ifndef _CBAPP_DEMO_H
#define _CBAPP_DEMO_H   1



//  1.  INCLUDES    | Headers, Modules, etc...
// *************************************************************************** //
// *************************************************************************** //

//  0.1.        ** MY **  HEADERS...
#include CBAPP_USER_CONFIG
#include "cblib.h"
#include "utility/utility.h"

//  0.2     STANDARD LIBRARY HEADERS...
#include <iostream>         //  <======| std::cout, std::cerr, std::endl, ...
#include <cstdlib>          // C-Headers...
#include <stdio.h>
#include <unistd.h>
#include <random>
#include <algorithm>

#include <string>           //  <======| std::string, ...
#include <string_view>
#include <vector>           //  <======| std::vector, ...
#include <stdexcept>        //  <======| ...
#include <limits.h>
#include <math.h>

//  0.3     "DEAR IMGUI" HEADERS...
#include "imgui.h"
#include "implot.h"



namespace cb { //     BEGINNING NAMESPACE "cb"...
// *************************************************************************** //
// *************************************************************************** //



// *************************************************************************** //
// *************************************************************************** //
//                         CBDemo:
//      Class for Demo-ing and Testing Dear ImGui Stuff...
// *************************************************************************** //
// *************************************************************************** //

struct CBDemo
{
// *************************************************************************** //
// *************************************************************************** //
    //
    //  0.               PUBLIC CLASS-NESTED ALIASES...
    // *************************************************************************** //
    using   value_type              = float;
    using   size_type               = std::size_t;
    using   dim_type                = float;
    
    
    //  1.               INITIALIZATION MEMBER FUNCTIONS...
    // *************************************************************************** //
                        CBDemo                  (void);         //  Def. Constructor.
                        ~CBDemo                 (void);         //  Destructor.
    void                init                    (void);
    void                destroy                 (void);
    
    
    //  2A.             PRIMARY MEMBER FUNCTIONS...
    // *************************************************************************** //
    void                Begin                   (bool * p_open = nullptr);
    
    
    //  2B.             SECONDARY MEMBER FUNCTIONS...
    // *************************************************************************** //
    void                begin_IMPL              (void);
    void                placeholder_1           (void);
    void                child_tab_bar           (void);
    void                child_windows           (void);
    void                tab_widgets             (void);
    
    void                placeholder_2           (void);
    void                placeholder_21          (void);
    
    void                placeholder_3           (void);
    void                placeholder_31          (void);
    
    
    //  3.             PRIVATE / UTILITY FUNCTIONS...
    // *************************************************************************** //
    void                _draw_menu_1            (void);



// *************************************************************************** //
// *************************************************************************** //
    //
    //  2.A             DATA-MEMBERS...
    // *************************************************************************** //
    
    //  Constants.
    //      ...
        
    //  Appearance.
    //      ...
    
    
    //  Data.
    //      ...
        
        
    //  Interactive Variables.
    //      ...
    


// *************************************************************************** //
// *************************************************************************** //
};//	END "HeatMap" CLASS PROTOTYPE.







// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
}//   END OF "cb" NAMESPACE.






#endif      //  _CBAPP_DEMO_H  //
// *************************************************************************** //
// *************************************************************************** //
//
//  END.

