#ifndef _CBLIB_INIT_MATRIX_H 
#define    _CBLIB_INIT_MATRIX_H 1

#include <memory>
#include "templates/utility/type_traits.h"


namespace cblib        
{
//     BEGIN NAMESPACE.
// *************************************************************************** //

// *************************************************************************** //
//     Structs used for Template Meta-Programming.
// *************************************************************************** //

//     Type-Trait for Valid Matrix Types.
// *************************************************************************** //

//    "is_arithmetic_matrix"
//
//     PRIMARY TEMPLATE.  
//         This maps any type, T, that is passed to
//         "is_arithmetic_matrix<T>" to void by default.  
//         The types that meet the requirements of the 
//         "is_arithmetic_matrix" function will, instead, 
//         instantiate the EXPLICIT SPECIALIZATION of this metafunction.
template<typename T, typename = void>
struct is_arithmetic_matrix
        : public std::false_type { };


//    "is_arithmetic_matrix"
//
//     EXPLICIT TEMPLATE SPECIALIZATION.
//        Catches all of the types that satisfy the 
//        "is_arithmetic_matrix" requirements instead of 
//        having these types mapped to void (as per the primary template).
/*
template<typename T>
struct is_arithmetic_matrix
        < T, void_t
             < 
                 decltype
             >
        > : std::true_type { };
*/


/*template<typename T>
struct is_arithmetic_matrix
        : std::integral_constant
          <
           bool, 
           std::is_arithmetic<T>::value &&
           !std::is_same<bool, typename std::remove_cv<T>::type>::value
          >
{ };*/


//    "is_matrix_type"
//     1. Any arithmetic types EXCEPT boolean (e.g. int, float, etc).
//
template<typename T>
struct is_matrix_type
        : std::integral_constant
          <
           bool, 
           std::is_arithmetic<T>::value &&
           !std::is_same<bool, typename std::remove_cv<T>::type>::value
          > 
{ };


//    "is_matrix_allocator"
//
template<typename T, typename Allocator>
struct is_matrix_allocator
        : std::integral_constant
          <
           bool, 
           std::is_same<std::allocator<T>, typename std::remove_cv<Allocator>::type>::value
          > 
{ };


//    "valid_matrix_template"
//
//    WARNING:
//        OUT OF DATE. 
//        NEED TO UPDATE THIS METAFUNCTION.
template<typename T, typename Allocator>
struct valid_matrix_template
        : std::integral_constant
          <
           bool, 
           is_matrix_type<T>::value &&
           is_matrix_allocator<T, Allocator>::value
          > 
{ };


/*struct T
{
    enum { int_t, float_t } type;


    template<typename Integer,
             std::enable_if_t<std::is_integral<Integer>::value, bool> = true>
    T(Integer) : type(int_t) { std::cout << "\tType = int_t" << '\n'; }

    template<typename Floating,
             std::enable_if_t<std::is_floating_point<Floating>::value, bool> = true>
    T(Floating) : type(float_t) { std::cout << "\tType = float_t" << '\n'; }
};*/





// *************************************************************************** //
//     Namespace for Global Parameters and Constants for the Matrix Class.
// *************************************************************************** //

//     1. Placeholders for Default Matrix Template Parameters.
// *************************************************************************** //

//typedef     long double     DEF_MATRIX_VALUE_TYPE; 


// *************************************************************************** //
//     END NAMESPACE.
}    

#endif //  _CBLIB_INIT_MATRIX_H  //
