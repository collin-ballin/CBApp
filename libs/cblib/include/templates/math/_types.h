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
#include <algorithm>                //  std::clamp
#if __cpp_concepts >= 201907L
# include <concepts>
#endif  //  C++20.  //

#include <cmath>                    //  std::nextafter
#include <cstddef>
#include <iomanip>
#include <limits>

#include <vector>
#include <array>
#if __cplusplus >= 201103L
# include <initializer_list>
#endif	//  C++11.  //



namespace cblib { namespace math {   //     BEGINNING NAMESPACE "cblib" :: "math"...
// *************************************************************************** //
// *************************************************************************** //




namespace anon {   //     BEGINNING NAMESPACE "anon"...
// *************************************************************************** //
// *************************************************************************** //

template<class U>
concept ParamOrdered = std::totally_ordered<U>;

template<class U>
concept ParamNumeric = std::integral<U> || std::floating_point<U>;


// *************************************************************************** //
// *************************************************************************** //
}//   END OF "anon" NAMESPACE.












// *************************************************************************** //
//
//
//
//      1.      GENERAL MATH TYPES AND DATA ABSTRACTIONS...
// *************************************************************************** //
// *************************************************************************** //



// *************************************************************************** //
//      1A. GENERAL. |      AUXILIARY DEFINITIONS.
// *************************************************************************** //

//  "Range"
//
template<typename T>
struct Range {
    T               min,    max;
};



// *************************************************************************** //
//      1B. GENERAL. |      PRIMARY "Param" CLASS IMPLEMENTATION.
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
    //      QUERY FUNCTIONS.
    // *************************************************************************** //
    
    //  "CanDecrement"
    //
    [[nodiscard]] inline bool               CanDecrement                    (void) const noexcept
        requires (std::integral<T>)
    {
        return ( limits.min < value );      //  next value (=value+1) would still be ≤ max.
    }
    //
    [[nodiscard]] inline bool               CanDecrement                    (void) const noexcept
        requires (std::floating_point<T>)
    {
        const T prev = std::nextafter(value, -std::numeric_limits<T>::infinity());
        return !(prev < limits.min);
    }
    
    
    //  "CanIncrement"
    //
    [[nodiscard]] inline bool               CanIncrement                    (void) const noexcept
        requires (std::integral<T>)
    {
        return ( value < limits.max );      //  next value (=value+1) would still be ≤ max.
    }
    //
    [[nodiscard]] inline bool               CanIncrement                    (void) const noexcept
        requires (std::floating_point<T>)
    {
        const T next = std::nextafter(value, std::numeric_limits<T>::infinity());
        return !( limits.max < next );
    }



    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      UTILITY FUNCTIONS.
    // *************************************************************************** //
    
    [[nodiscard]] inline bool               within_range                    (const T & value_) const
    noexcept( noexcept(value_ < limits.min)  &&  noexcept(limits.max < value_) )
    {
        return ( !(value_ < limits.min)  &&  !(limits.max < value_) );
    }
    
    inline void                             Clamp                           (void) noexcept                     { this->value = std::clamp( this->value, limits.min, limits.max ); }
    
    // *************************************************************************** //



    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      GETTER FUNCTIONS.
    // *************************************************************************** //
    //                                  VALUE:
    inline void                             SetValue                        (const T & value_) noexcept
    {
        this->value = std::clamp( value_, limits.min, limits.max );
        return;
    }
    
    inline void                             SetValue                        (const Param<T> & obj) noexcept
    {
        this->value = std::clamp( obj.Value(), limits.min, limits.max );
        return;
    }
    //
    //
    //
    //                                  RANGE:
    inline void                             SetRange                        (const Range<T> & limits_) noexcept {
        this->limits    = limits_;
        this->value     = std::clamp( this->value, limits_.min, limits_.max );
    }
    
    // *************************************************************************** //
    
    
    
    // *************************************************************************** //
    //      SETTER FUNCTIONS.
    // *************************************************************************** //
    //                              VALUE:
    inline T &                          Value                               (void) noexcept                 { return value;         }       //  Get Value.
    inline const T &                    Value                               (void) const noexcept           { return value;         }       //  Get Value   [ CONST ].
    inline T &                          GetValue                            (void) noexcept                 { return value;         }       //  Get Value.
    inline T &                          GetValue                            (void) const noexcept           { return value;         }       //  Get Value   [ CONST ].
    //
    //                              MINIMUM RANGE:
    inline T &                          min                                 (void) noexcept                 { return limits.min;    }       //  Get Min.
    inline T &                          min                                 (void) const noexcept           { return limits.min;    }       //  Get Min     [ CONST ].
    inline T &                          Min                                 (void) noexcept                 { return limits.min;    }       //  Get Min.
    inline const T &                    Min                                 (void) const noexcept           { return limits.min;    }       //  Get Min     [ CONST ].
    inline T &                          GetMin                              (void) noexcept                 { return limits.min;    }       //  Get Min.
    inline const T &                    GetMin                              (void) const noexcept           { return limits.min;    }       //  Get Min     [ CONST ].
    inline T &                          RangeMin                            (void) noexcept                 { return limits.min;    }       //  Get Min.
    inline const T &                    RangeMin                            (void) const noexcept           { return limits.min;    }       //  Get Min     [ CONST ].
    //
    //                              MAXIMUM RANGE:
    inline T &                          max                                 (void) noexcept                 { return limits.max;    }       //  Get Max.
    inline const T &                    max                                 (void) const noexcept           { return limits.min;    }       //  Get Max     [ CONST ].
    inline T &                          Max                                 (void) noexcept                 { return limits.max;    }       //  Get Max.
    inline const T &                    Max                                 (void) const noexcept           { return limits.min;    }       //  Get Max     [ CONST ].
    inline T &                          GetMax                              (void) noexcept                 { return limits.max;    }       //  Get Max.
    inline const T &                    GetMax                              (void) const noexcept           { return limits.min;    }       //  Get Max     [ CONST ].
    inline T &                          RangeMax                            (void) noexcept                 { return limits.max;    }       //  Get Max.
    inline const T &                    RangeMax                            (void) const noexcept           { return limits.min;    }       //  Get Max     [ CONST ].
    //
    //
    //
    //                              ENTIRE RANGE:
    inline Range<T> &                   GetRange                            (void) noexcept                 { return limits;        }       //  Get Range.
    inline const Range<T> &             GetRange                            (void) const noexcept           { return limits;        }       //  Get Range   [ CONST ].
    
    // *************************************************************************** //
    
    
    
//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "INLINE" FUNCTIONS.






// *************************************************************************** //
// *************************************************************************** //
};//	END "Param" INLINE CLASS DEFINITION.



//
//
//
// *************************************************************************** //
// *************************************************************************** //   END [[ 1.  "GENERAL ABSTRACTIONS" ]].












// *************************************************************************** //
//
//
//
//      2.      GEOMETRY ABSTRACTIONS...
// *************************************************************************** //
// *************************************************************************** //



// *************************************************************************** //
//      2A. GEOMETRY. |     BOUNDING BOXES.
// *************************************************************************** //

//  "BBox2"
//      PLAIN-OLD-DATA (POD) STRUCT.
//
template <typename T>
struct BBox2 {
// *************************************************************************** //
//
//      1.          DATA-MEMBERS...
// *************************************************************************** //
// *************************************************************************** //

    // *************************************************************************** //
    //      1. |    IMPORTANT DATA-MEMBERS.
    // *************************************************************************** //
    T                                   min_x                           = T(0);         //  left
    T                                   min_y                           = T(0);         //  bottom
    T                                   max_x                           = T(0);         //  right
    T                                   max_y                           = T(0);         //  top
    
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
                                        BBox2                   (void) noexcept                     = default;
                                        BBox2                   (const T & min_x_, const T & min_y_, const T & max_x_, const T & max_y_) noexcept
                                                                    :  min_x(min_x_)     , min_y(min_y_)    , max_x(max_x_)     , max_y(max_y_)     {   }
                                        ~BBox2                  (void)                              = default;
                                        BBox2                   (const BBox2 & ) noexcept           = default;      //  Copy CTOR.
                                        BBox2                   (const BBox2 && ) noexcept          = default;      //  Move CTOR.
    
    //  "empty"
    //
    static constexpr BBox2              s_empty                 (void) noexcept {
        return {   std::numeric_limits<T>::max()        , std::numeric_limits<T>::max()
                 , std::numeric_limits<T>::lowest()     , std::numeric_limits<T>::lowest()  };
    }
    constexpr void                      clear_zero              (void)  noexcept    { min_x = min_y = max_x = max_y = T(0);     }
    constexpr void                      clear_empty             (void) noexcept     { *this = empty();                          }
    
    
    // *************************************************************************** //
    //      DELETED FUNCTIONS.              |   ...
    // *************************************************************************** //
    friend constexpr BBox2              operator +              (const BBox2 & , const BBox2 & )    = delete;
    friend constexpr BBox2              operator -              (const BBox2 & , const BBox2 & )    = delete;

    friend constexpr BBox2              operator *              (const BBox2 & , T  )               = delete;
    friend constexpr BBox2              operator *              (T, const BBox2 &   )               = delete;
    friend constexpr BBox2              operator /              (const BBox2 & , T  )               = delete;

    friend constexpr bool               operator <              (const BBox2 & , const BBox2 & )    = delete;
    friend constexpr bool               operator <=             (const BBox2 & , const BBox2 & )    = delete;
    friend constexpr bool               operator >              (const BBox2 & , const BBox2 & )    = delete;
    friend constexpr bool               operator >=             (const BBox2 & , const BBox2 & )    = delete;

    friend constexpr BBox2              operator +              (const BBox2 & )                    = delete;   //  unary +
    friend constexpr BBox2              operator -              (const BBox2 & )                    = delete;   //  unary -
    
    
    // *************************************************************************** //
    //      OVERLOADED MEMBER OPERATORS.    |   ...
    // *************************************************************************** //
    //                              ASSIGNMENT OPERATORS.
    BBox2 &                             operator =              (const BBox2 &       ) noexcept     = default;   //  Assgn. Operator.
    BBox2 &                             operator =              (BBox2 &&            ) noexcept     = default;   //  Move-Assgn. Operator.
    //
    //                              COMPOUND-ASSIGNMENT OPERATORS.
    constexpr BBox2 &                   operator +=             (const std::pair<T,T>& d) noexcept {
        min_x += d.first;  max_x += d.first;
        min_y += d.second; max_y += d.second;
        return *this;
    }
    constexpr BBox2&                    operator -=             (const std::pair<T,T>& d) noexcept {
        min_x -= d.first;  max_x -= d.first;
        min_y -= d.second; max_y -= d.second;
        return *this;
    }
    
    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      OVERLOADED NON-MEMBER OPERATORS. |   ...
    // *************************************************************************** //
    //
    //                              COMPARISON OPERATORS.
    friend constexpr bool               operator ==             (const BBox2 & a, const BBox2 & b) noexcept
    {
        return ( ( a.min_x == b.min_x)  &&  ( a.min_y == b.min_y)  &&  ( a.max_x == b.max_x)  &&  ( a.max_y == b.max_y) );
    }
    friend constexpr bool               operator !=             (const BBox2 & a, const BBox2 & b) noexcept     { return !(a == b); }
    //
    //                              HIDDEN FRIEND OPERATORS.
    friend constexpr BBox2              operator +              (BBox2 b, const std::pair<T,T> & d) noexcept    { b += d;  return b;        }
    friend constexpr BBox2              operator -              (BBox2 b, const std::pair<T,T> & d) noexcept    { b -= d;  return b;        }
    friend constexpr BBox2              operator |              (const BBox2 & a, const BBox2 & b) noexcept     { return a.united(b);       }
    friend constexpr BBox2              operator &              (const BBox2 & a, const BBox2 & b) noexcept     { return a.intersected(b);  }

    
//
// *************************************************************************** //
// *************************************************************************** //   END "2A.  MEMBER FUNCS".

    
   
// *************************************************************************** //
//
//      2.B.        INLINE FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

    // *************************************************************************** //
    //      2.B. |  QUERY FUNCTIONS.
    // *************************************************************************** //
    
    //  "is_empty"
    [[nodiscard]] constexpr bool                is_empty                    (void) const noexcept       { return true; ( (min_x > max_x)  ||  (min_y > max_y) ); }
    
    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      2.B. |  OPERATION FUNCTIONS.
    // *************************************************************************** //
    
    //  "translate"
    constexpr BBox2 &                           translate                   (T dx, T dy) noexcept
    {
        this->min_x       += dx;
        this->max_x       += dx;
        this->min_y       += dy;
        this->max_y       += dy;
        return *this;
    }
    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      2.B. |  DIMENSIONS.
    // *************************************************************************** //
    
    //  "width"
    [[nodiscard]] constexpr T                   width                       (void) const noexcept       { return max_x - min_x;                                         }
    [[nodiscard]] constexpr T                   height                      (void) const noexcept       { return max_y - min_y;                                         }
    [[nodiscard]] constexpr std::pair<T,T>      dims                        (void) const noexcept       { return { this->width(), this->height() };                     };

    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      2.B. |  COORDINATES.
    // *************************************************************************** //

    [[nodiscard]] constexpr std::pair<T,T>      center                      (void) const noexcept       { return { (min_x + max_x) / T(2), (min_y + max_y) / T(2) };    }
    //
    [[nodiscard]] constexpr std::pair<T,T>      bl                          (void) const noexcept       { return { min_x, min_y };                                      }
    [[nodiscard]] constexpr std::pair<T,T>      br                          (void) const noexcept       { return { max_x, min_y };                                      }
    [[nodiscard]] constexpr std::pair<T,T>      tl                          (void) const noexcept       { return { min_x, max_y };                                      }
    [[nodiscard]] constexpr std::pair<T,T>      tr                          (void) const noexcept       { return { max_x, max_y };                                      }

    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      2.B. |  PREDICATE FUNCTIONS.
    // *************************************************************************** //
    
    //  "contains"
    //
    [[nodiscard]] constexpr bool                contains                    (T x, T y) const noexcept {
        return (x >= min_x) && (x <= max_x) && (y >= min_y) && (y <= max_y);
    }
    
    //  "overlaps"
    //
    [[nodiscard]] constexpr bool                overlaps                    (const BBox2 & b) const noexcept {
        return !(b.max_x < min_x || b.min_x > max_x || b.max_y < min_y || b.min_y > max_y);
    }

    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      2.B. |  INTER-BBOX FUNCTIONS.
    // *************************************************************************** //
    
    //  "swap"
    //
    friend constexpr void                       swap                        (BBox2 & a, BBox2 & b) noexcept
    {
        std::swap(a.min_x, b.min_x);     std::swap(a.min_y, b.min_y);
        std::swap(a.max_x, b.max_x);     std::swap(a.max_y, b.max_y);
        return;
    }


    //  "united"
    //      --- pure functions returning new boxes (no mutation required) ---
    //
    [[nodiscard]] constexpr BBox2               united                      (const BBox2 & b) const noexcept
    {
        return {
            std::min(min_x, b.min_x), std::min(min_y, b.min_y),
            std::max(max_x, b.max_x), std::max(max_y, b.max_y)
        };
    }
    
    
    //  "intersected"
    //
    [[nodiscard]] constexpr BBox2               intersected                 (const BBox2 & b) const noexcept
    {
        BBox2 r{
            std::max(min_x, b.min_x), std::max(min_y, b.min_y),
            std::min(max_x, b.max_x), std::min(max_y, b.max_y)
        };
        return r; // may be empty (inverted); caller can test r.is_empty()
    }


    //  "include"
    //      --- small mutators (optional, still POD-safe) ---
    //
    constexpr void                              include                     (T x, T y) noexcept
    {
        if ( is_empty() )   { min_x = max_x = x; min_y = max_y = y; return; }
        min_x = std::min(min_x, x); min_y = std::min(min_y, y);
        max_x = std::max(max_x, x); max_y = std::max(max_y, y);
    }
    constexpr void                              include                     (const BBox2 & b) noexcept
    {
        if (b.is_empty()) return;
        if (is_empty()) { *this = b; return; }
        min_x = std::min(min_x, b.min_x); min_y = std::min(min_y, b.min_y);
        max_x = std::max(max_x, b.max_x); max_y = std::max(max_y, b.max_y);
    }


    //  "empty"
    //      --- factories (static, keep aggregate default construction intact) ---
    //
    [[nodiscard]] static constexpr BBox2        empty                       (void) noexcept
    {
        // Works for integral or floating T
        return { std::numeric_limits<T>::max(),  std::numeric_limits<T>::max(),
                 std::numeric_limits<T>::lowest(), std::numeric_limits<T>::lowest() };
    }
    
    
    //  "from_min_max"
    //
    [[nodiscard]] static constexpr BBox2        from_min_max                (T lx, T by, T rx, T ty) noexcept
    {
        return { lx, by, rx, ty }; // may be unnormalized; caller can pass normalized if desired
    }
    
    
    //  "from_center_size"
    //
    [[nodiscard]] static constexpr BBox2        from_center_size            (T cx, T cy, T w, T h) noexcept
    {
        const T hx = w / T(2), hy = h / T(2);
        return { cx - hx, cy - hy, cx + hx, cy + hy };
    }
    
//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "2B.  INLINE" FUNCTIONS.



//
//
// *************************************************************************** //
// *************************************************************************** //
};//	END "MyStruct" INLINE STRUCT DEFINITION.



//
//
//
// *************************************************************************** //
// *************************************************************************** //   END [[ 2.  "GEOMETRY ABSTRACTIONS" ]].












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
//      X.      JSON SERIALIZERS FOR CBLIB TYPES...
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
