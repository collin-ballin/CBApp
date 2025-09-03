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



//      1.  GENERAL MATH TYPES AND DATA ABSTRACTIONS...
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
    
    inline void                             Clamp                               (void) noexcept                     { this->value = std::clamp( this->value, limits.min, limits.max ); }
    
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
//
//
//
//      3.      JSON SERIALIZERS FOR CBLIB TYPES...
// *************************************************************************** //
// *************************************************************************** //

namespace nlohmann { //     BEGINNING NAMESPACE "nlohmann"...
// *************************************************************************** //
// *************************************************************************** //

//      "Range<T>"              SERIALIZER...
//
template<typename T>
struct adl_serializer<cblib::math::Range<T>>
{
    template<typename T_>   using Range = cblib::math::Range<T_>;
    template<typename T_>   using Param = cblib::math::Param<T_>;
    
    
    static void                 to_json                 (json & j, Range<T> const & r)
    {
        j = json{{"min", r.min}, {"max", r.max}};        // canonical
        return;
    }

    static void                 from_json               (json const & j, Range<T> & r)
    {
        if ( j.is_array()  &&  j.size() == 2 )
        {
            r.min = j[0].get<T>();
            r.max = j[1].get<T>();
        }
        else if ( j.is_object()  &&  j.contains("min")  &&  j.contains("max") )
        {
            r.min = j.at("min").get<T>();
            r.max = j.at("max").get<T>();
        }
        else {
            throw std::runtime_error("Range<T> expects [min,max] or {min,max}");
        }
        if constexpr ( std::is_arithmetic_v<T> )
        {
            if (r.min > r.max) std::swap(r.min, r.max);
        }
        
        return;
    }
};


//      "Param<T>"              SERIALIZER...
//
template<typename T>
struct adl_serializer<cblib::math::Param<T>>
{
    template<typename T_>   using Range = cblib::math::Range<T_>;
    template<typename T_>   using Param = cblib::math::Param<T_>;
        

    static void                 to_json                 (json & j, cblib::math::Param<T> const & p)
    {
        j = json{{"value", p.value}, {"limits", p.limits}};   // canonical
    }

    static void                 from_json               (json const & j, Range<T> & p)
    {
        
        cblib::math::Range<T>    lims        {   };
        T           val         {   };

        // Accept [value, min, max]
        if ( j.is_array()  &&  j.size() == 3 )
        {
            val      = j[0].get<T>();
            lims.min = j[1].get<T>();
            lims.max = j[2].get<T>();
        }
        else
        {
            // Limits: prefer nested object; allow flattened keys
            if (j.contains("limits")) {
                lims = j.at("limits").get<Range<T>>();
            } else if (j.contains("min") && j.contains("max")) {
                lims.min = j.at("min").get<T>();
                lims.max = j.at("max").get<T>();
            } else {
                throw std::runtime_error("Param<T> missing limits");
            }
            // Value: required; if absent, default to min
            if (j.contains("value")) {
                val = j.at("value").get<T>();
            } else if (j.is_array() && !j.empty()) {
                val = j[0].get<T>();
            } else {
                val = lims.min;
            }
        }

        //  Sanitize & clamp where meaningful
        if constexpr ( std::is_floating_point_v<T> )
        {
            if ( !std::isfinite(lims.min)   )   { lims.min  = T(0);      }
            if ( !std::isfinite(lims.max)   )   { lims.max  = lims.min;  }
            if ( !std::isfinite(val)        )   { val       = lims.min;  }
        }
        if constexpr (std::is_arithmetic_v<T>)
        {
            if ( lims.min > lims.max )      { std::swap(lims.min, lims.max); }
            val = std::clamp(val, lims.min, lims.max);
        }

        p.limits = lims;
        p.value  = val;
        
        return;
    }
};


// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
}//   END OF "nlohmann" NAMESPACE.


















// *************************************************************************** //
// *************************************************************************** //
#endif  //  _CBLIB_MATH_TYPES_H  //
