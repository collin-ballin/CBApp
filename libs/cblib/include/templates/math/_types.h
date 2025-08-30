/***********************************************************************************
*
*       ********************************************************************
*       ****               _ T Y P E S . H  ____  F I L E               ****
*       ********************************************************************
*
*              AUTHOR:      Collin A. Bond
*               DATED:      June 15, 2025.
*
*       ********************************************************************
*                FILE:      [./PyStream.h]
*
*
*
**************************************************************************************
**************************************************************************************/
#ifndef _CBLIB_MATH_TYPES_H
#define _CBLIB_MATH_TYPES_H 1

#include <iostream>
#include <type_traits>
#include <algorithm>
#if __cpp_concepts >= 201907L
# include <concepts>
#endif  //  C++20.  //

#include <cmath>
#include <cstddef>
#include <iomanip>

#include <vector>
#include <array>
#if __cplusplus >= 201103L
# include <initializer_list>
#endif	//  C++11.  //



namespace cblib { namespace math {   //     BEGINNING NAMESPACE "cblib" :: "math"...
// *************************************************************************** //
// *************************************************************************** //



//  1.  GENERAL MATH TYPES AND DATA ABSTRACTIONS...
// *************************************************************************** //
// *************************************************************************** //

//  "Range"
//      - 1.
template<typename T>
struct Range {
    T               min,    max;
};












// *************************************************************************** //
// *************************************************************************** //
//                PRIMARY CLASS INTERFACE:
// 		Class-Interface for the "Param" Abstraction.
// *************************************************************************** //
// *************************************************************************** //

//  "Param"
//      - 2.    A parameter that carries both a value and its valid range
//
template<typename T>
struct Param
{
//      0.          CONSTANTS AND ALIASES...
// *************************************************************************** //
// *************************************************************************** //
public:

    // *************************************************************************** //
    //      NESTED TYPENAME ALIASES.
    // *************************************************************************** //
    
    // *************************************************************************** //
    //      STATIC CONSTEXPR CONSTANTS.
    // *************************************************************************** //
    
//
//
// *************************************************************************** //
// *************************************************************************** //   END "CONSTANTS AND ALIASES".



// *************************************************************************** //
//
//
//      1.          CLASS DATA-MEMBERS...
// *************************************************************************** //
// *************************************************************************** //

    // *************************************************************************** //
    //      IMPORTANT DATA-MEMBERS.
    // *************************************************************************** //
    T                                   value;
    Range<T>                            limits;
    
    // *************************************************************************** //
    
    
    
//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "CLASS DATA-MEMBERS".



// *************************************************************************** //
//
//
//      2.C.        INLINE FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

    // *************************************************************************** //
    //      "RULE OF ..." FUNCTIONS.
    // *************************************************************************** //
    //                              INITIALIZATIONS:
        //  inline                              Param                               (void) noexcept                 = default;
        //  inline                              Param                               (const Param<T> &) noexcept     = default;
        //  inline virtual                      ~Param                              (void)                          = default;
    //
    //
    //                              OVERLOADED OPERATORS:
        //  inline Param<T> &                       operator =                          (const Param<T> & src) {
        //
        //      this->value = std::clamp( src.value, this->limits.min, this->limits.max );
        //      return (*this);
        //  }
    //
    //
    //                              VALUE:
    //inline void                         SetValue                            (const T & value_) noexcept     { this->value = std::clamp( value_, limits.min, limits.max ); }
    
    // *************************************************************************** //



    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      UTILITY FUNCTIONS.
    // *************************************************************************** //
    
    [[nodiscard]] inline bool               within_range                        (const T & value_) const
    noexcept( noexcept(value_ < limits.min)  &&  noexcept(limits.max < value_) )
    {
        return !(value_ < limits.min) && !(limits.max < value_);
    }
    
    // *************************************************************************** //



    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      GETTER FUNCTIONS.
    // *************************************************************************** //
    //                              VALUE:
    inline void                         SetValue                            (const T & value_) noexcept         { this->value = std::clamp( value_, limits.min, limits.max ); }
    inline void                         SetValue                            (const Param<T> & obj) noexcept     { this->value = std::clamp( obj.Value(), limits.min, limits.max ); }
    //
    //
    //
    //                              RANGE:
    inline void                         SetRange                            (const Range<T> & limits_) noexcept {
        this->limits    = limits_;
        this->value     = std::clamp( this->value, limits_.min, limits_.max );
    }
    
    // *************************************************************************** //
    
    
    
    // *************************************************************************** //
    //      SETTER FUNCTIONS.
    // *************************************************************************** //
    //                              VALUE:
    inline T &                          Value                               (void) noexcept                 { return value;         }       //  Get Value.
    inline T &                          Value                               (void) const noexcept           { return value;         }       //  Get Value   [ CONST ].
    inline T &                          GetValue                            (void) noexcept                 { return value;         }       //  Get Value.
    inline T &                          GetValue                            (void) const noexcept           { return value;         }       //  Get Value   [ CONST ].
    //
    //                              MINIMUM RANGE:
    inline T &                          min                                 (void) noexcept                 { return limits.min;    }       //  Get Min.
    inline T &                          min                                 (void) const noexcept           { return limits.min;    }       //  Get Min     [ CONST ].
    inline T &                          Min                                 (void) noexcept                 { return limits.min;    }       //  Get Min.
    inline T &                          Min                                 (void) const noexcept           { return limits.min;    }       //  Get Min     [ CONST ].
    inline T &                          GetMin                              (void) noexcept                 { return limits.min;    }       //  Get Min.
    inline T &                          GetMin                              (void) const noexcept           { return limits.min;    }       //  Get Min     [ CONST ].
    inline T &                          RangeMin                            (void) noexcept                 { return limits.min;    }       //  Get Min.
    inline T &                          RangeMin                            (void) const noexcept           { return limits.min;    }       //  Get Min     [ CONST ].
    //
    //                              MAXIMUM RANGE:
    inline T &                          max                                 (void) noexcept                 { return limits.max;    }       //  Get Max.
    inline T &                          max                                 (void) const noexcept           { return limits.min;    }       //  Get Max     [ CONST ].
    inline T &                          Max                                 (void) noexcept                 { return limits.max;    }       //  Get Max.
    inline T &                          Max                                 (void) const noexcept           { return limits.min;    }       //  Get Max     [ CONST ].
    inline T &                          GetMax                              (void) noexcept                 { return limits.max;    }       //  Get Max.
    inline T &                          GetMax                              (void) const noexcept           { return limits.min;    }       //  Get Max     [ CONST ].
    inline T &                          RangeMax                            (void) noexcept                 { return limits.max;    }       //  Get Max.
    inline T &                          RangeMax                            (void) const noexcept           { return limits.min;    }       //  Get Max     [ CONST ].
    //
    //
    //
    //                              ENTIRE RANGE:
    inline Range<T> &                   GetRange                            (void) noexcept                 { return limits;        }       //  Get Range.
    inline Range<T> &                   GetRange                            (void) const noexcept           { return limits;        }       //  Get Range   [ CONST ].
    
    // *************************************************************************** //
    
    
    
//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "INLINE" FUNCTIONS.






// *************************************************************************** //
// *************************************************************************** //
};//	END "Param" INLINE CLASS DEFINITION.






//  "Param"
//      - 2.    A parameter that carries both a value and its valid range
/*
template<typename T>
struct Param {
//
//                          INITIALIZATION FUNCTIONS...
//
//
//                          OVERLOADED OPERATORS...
    inline T &                  Value                   (void)      { return value;         }
    inline T &                  Min                     (void)      { return limits.min;    }
    inline T &                  Max                     (void)      { return limits.max;    }
//
    inline T &                  RangeMin                (void)      { return limits.min;    }
    inline T &                  RangeMax                (void)      { return limits.max;    }
//
//
//                          DATA MEMBERS...
    T                           value;
    Range<T>                    limits;
};
*/







// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
} }//   END OF "cblib" :: "math" NAMESPACE.







// *************************************************************************** //
// *************************************************************************** //
#endif  //  _CBLIB_MATH_TYPES_H  //
