/***********************************************************************************
*
*       ********************************************************************
*       ****    T E M P L A T E _ H E A D E R . C P P  ____  F I L E    ****
*       ********************************************************************
*
*              AUTHOR:      Collin A. Bond.
*               DATED:      MONTH_DAY_YEAR.
*
*       ********************************************************************
*                FILE:      [include/YOUR_HEADER_PATH]
*
*
*
**************************************************************************************
**************************************************************************************/
#ifndef _CBAPP_YOUR_HEADER_GUARD_H
# define _CBAPP_YOUR_HEADER_GUARD_H      //   <=======|   DELETE THESE LINES.
#endif  //  _CBAPP_YOUR_HEADER_GUARD_H  //



#ifndef _CBAPP_YOUR_HEADER_GUARD_H
#define _CBAPP_YOUR_HEADER_GUARD_H  1

//  1.  INCLUDES    | Headers, Modules, etc...
// *************************************************************************** //
// *************************************************************************** //

//  1.1.        ** MY **  HEADERS...
#include "_config.h"
#include "cblib.h"
#include "YOUR_HEADER_PATH"
#include "utility/utility.h"
#include "widgets/widgets.h"
#include "app/_init.h"


//  1.2     STANDARD LIBRARY HEADERS...
#include <iostream>         //  <======| std::cout, std::cerr, std::endl, ...
#include <cstdlib>          // C-Headers...
#include <stdio.h>
#include <unistd.h>

#include <string>           //  <======| std::string, ...
#include <string_view>
#include <vector>           //  <======| std::vector, ...
#include <stdexcept>        //  <======| ...
#include <limits.h>
#include <math.h>


//  1.3     "DEAR IMGUI" HEADERS...
#include "imgui.h"
#include "implot.h"






namespace cb { //     BEGINNING NAMESPACE "cb"...
// *************************************************************************** //
// *************************************************************************** //

//  2.      UTILITY FUNCTIONS [NON-MEMBER FUNCTIONS]...
//                  ...






// *************************************************************************** //
// *************************************************************************** //
//          3.    PRIMARY CLASS INTERFACE.
//  @brief BRIEF_DESC_OF_CLASS
// *************************************************************************** //
// *************************************************************************** //

class YourClassName
{
// *************************************************************************** //
// *************************************************************************** //
public:
    //  1               PUBLIC MEMBER FUNCTIONS...
    // *************************************************************************** //
    //  1.1             Default Constructor, Destructor, etc.       [src/YOUR_IMPL_FILEPATH].
                        YourClassName                   (void);                                         //  Def. Constructor.
                        ~YourClassName                  (void);                                         //  Def. Destructor.
    
    //  1.2             Primary Class Interface.                    [src/YOUR_IMPL_FILEPATH].
    void                Begin                           (bool * p_open = nullptr);

    //  1.3             Deleted Operators, Functions, etc.
                        YourClassName                   (const YourClassName &    src)                = delete;   //  Copy. Constructor.
                        YourClassName                   (YourClassName &&         src)                = delete;   //  Move Constructor.
    YourClassName &     operator =                      (const YourClassName &    src)                = delete;   //  Assgn. Operator.
    YourClassName &     operator =                      (YourClassName &&         src)                = delete;   //  Move-Assgn. Operator.
    
    
    
// *************************************************************************** //
// *************************************************************************** //
protected:
    //  2.A             PROTECTED DATA-MEMBERS...
    // *************************************************************************** //
    
    //                  1.  BOOLEANS...
    //  bool                m_running                       = true;
    
    
    //                  2.  APPEARANCE...
    //                          Dimensions:
    //                              ...
    //
    //                          Colors:
    //                              ...
    //
    //                          Fonts:
    //                              ...
    
    
    //                  3.  WINDOW / GUI ITEMS...
    //                          ...
    
    
    //                  4.  MISC INFORMATION...
    //                          ...
    
    
    //                  5.  IMPORTANT VARIABLES...
    //                          ...
    
    
    //  2.B             PROTECTED MEMBER FUNCTIONS...
    // *************************************************************************** //
    
    //  2B.1            Class Initializations.          [src/YOUR_IMPL_FILEPATH].
    void                init                            (void);
    void                load                            (void);
    void                destroy                         (void);
    //
    //  2B.2            Secondary Class Methods.        [src/YOUR_IMPL_FILEPATH].
    //                          ...
    
    
    
    
    
// *************************************************************************** //
// *************************************************************************** //
private:
    //  3.              PRIVATE MEMBER FUNCTIONS...
    // *************************************************************************** //
    
    //
    //  ...
    //


// *************************************************************************** //
// *************************************************************************** //
};//	END "Menubar" INLINE CLASS DEFINITION.



// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
}//   END OF "cb" NAMESPACE.






#endif      //  _CBAPP_YOUR_HEADER_GUARD_H  //
// *************************************************************************** //
// *************************************************************************** //
//
//  END.

