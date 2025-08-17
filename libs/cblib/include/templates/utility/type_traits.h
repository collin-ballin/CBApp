#ifndef _CBLIB_TYPE_TRAITS_H
#define _CBLIB_TYPE_TRAITS_H 1

#include <type_traits>
#include <array>
#include <vector>
#include <iomanip>
#if __cplusplus >= 201103L
# include <initializer_list>
#endif	// C++11.

#include "json.hpp"



// 	BEGIN NAMESPACE     "cblib".
// *************************************************************************** //
// *************************************************************************** //
namespace cblib 	{



// *************************************************************************** //
//	    Type-Trait Metafunctions.       [ NLOHMAN JSON ].
// *************************************************************************** //

//  "has_from_json"
//
/// @struct     has_from_json
/// @brief      Detect if a type has a valid "from_json" serializer function.
/// @note       Template type-trait style function to determine if a given type has a visible "from_json"
///             serializer function as specified within the "nlohmann JSON for Modern C++" library.
///             USAGE:  static_assert( has_from_json<MyType>::value, "Type \"MyType\" does NOT have a visible \"from_json\" serializer." );
///
/// @tparam     T, the type to query serializer status
///
///
template<typename T, typename = void>
struct has_from_json : std::false_type {};

//  "has_from_json"
//
template<typename T>
struct has_from_json<
    T,
    std::void_t< decltype(
        nlohmann::adl_serializer<T>::from_json(
            std::declval<const nlohmann::json &>(),
            std::declval<T &>() )
    ) >
    > : std::true_type
{/*  struct body  */};



//  "has_to_json"
//
/// @struct     has_to_json
/// @brief      Detect if a type has a valid "has_json" serializer function.
/// @note       Template type-trait style function to determine if a given type has a visible "to_json"
///             serializer function as specified within the "nlohmann JSON for Modern C++" library.
///             USAGE:  static_assert( has_to_json<MyType>::value, "Type \"MyType\" does NOT have a visible \"to_json\" serializer." );
///
/// @tparam     T, the type to query serializer status
///
template<typename T, typename = void>
struct has_to_json : std::false_type { };

//  "has_to_json"
//
template<typename T>
struct has_to_json<
    T,
    std::void_t<decltype(                       //  expression must be well‑formed
        nlohmann::adl_serializer<T>::to_json(
            std::declval<nlohmann::json &>(),           //  first arg        : json lvalue
            std::declval<const T&>() )                  //  second arg       : const T &
    ) >
    > : std::true_type
{/*  struct body  */};






//  "maximum_value_of_type"
//
template <class T>
constexpr auto maximum_value_of_type(void)
{
    if constexpr ( std::is_enum_v<T> ) {
        using U = std::underlying_type_t<T>;
        return std::numeric_limits<U>::max();
    }
    else {
        static_assert(std::is_integral_v<T>, "Template parameter is not an integral or enum type" );
        return std::numeric_limits<T>::max();
    }
}


//  "minimum_value_of_type"
//
template <class T>
constexpr auto minimum_value_of_type(void)
{
    if constexpr ( std::is_enum_v<T> ) {
        using U = std::underlying_type_t<T>;
        return std::numeric_limits<U>::min();
    }
    else {
        static_assert(std::is_integral_v<T>, "Template parameter is not an integral or enum type" );
        return std::numeric_limits<T>::min();
    }
}





//
// *************************************************************************** //
// *************************************************************************** //   END "JSON".






// *************************************************************************** //
//	    Type-Trait Metafunctions.       [ STANDARD ].
// *************************************************************************** //

//	1. Class-Member Defined Overloaded Operators.
// *************************************************************************** //

//	"has_assignment_operator_impl"
//
template<typename T>
struct has_assignment_operator_impl
{
	template<typename U>
	static auto test(U*) -> decltype(std::declval<U &>() = std::declval<const U &>());

	template<typename>
	static auto test(...) -> std::false_type;

	using type = typename std::is_same<T &, decltype(test<T>(0))>::type;
};

//	"has_assignment_operator"
//
template<typename T>
struct has_assignment_operator
		: public has_assignment_operator_impl<T>::type { };


//	"has_plus_equal_operator_impl"
//
template<typename T, typename U>
struct has_plus_equal_operator_impl
{
	template<typename V, typename W = V>
	static auto test(V*) -> decltype(std::declval<V &>() += std::declval<const W &>());

	template<typename, typename = void>
	static auto test(...) -> std::false_type;

	using type = typename std::is_same<T &, decltype(test<T, U>(0))>::type;
};

//	"has_plus_equal_operator"
//
template<typename T, typename U = T>
struct has_plus_equal_operator
		: public has_plus_equal_operator_impl<T, U>::type { };


//	"has_minus_equal_operator_impl"
//
template<typename T, typename U>
struct has_minus_equal_operator_impl
{
	template<typename V, typename W = V>
	static auto test(V*) -> decltype(std::declval<V &>() -= std::declval<const W &>());

	template<typename, typename = void>
	static auto test(...) -> std::false_type;

	using type = typename std::is_same<T &, decltype(test<T, U>(0))>::type;
};

//	"has_minus_equal_operator"
//
template<typename T, typename U = T>
struct has_minus_equal_operator
		: public has_minus_equal_operator_impl<T, U>::type { };


//	"has_times_equal_operator_impl"
//
template<typename T, typename U>
struct has_times_equal_operator_impl
{
	template<typename V, typename W = V>
	static auto test(V*) -> decltype(std::declval<V &>() *= std::declval<const W &>());

	template<typename, typename = void>
	static auto test(...) -> std::false_type;

	using type = typename std::is_same<T &, decltype(test<T, U>(0))>::type;
};

//	"has_times_equal_operator"
//
template<typename T, typename U = T>
struct has_times_equal_operator
		: public has_times_equal_operator_impl<T, U>::type { };


//	"has_divide_equal_operator_impl"
//
template<typename T, typename U>
struct has_divide_equal_operator_impl
{
	template<typename V, typename W = V>
	static auto test(V*) -> decltype(std::declval<V &>() /= std::declval<const W &>());

	template<typename, typename = void>
	static auto test(...) -> std::false_type;

	using type = typename std::is_same<T &, decltype(test<T, U>(0))>::type;
};

//	"has_divide_equal_operator"
//
template<typename T, typename U = T>
struct has_divide_equal_operator
		: public has_divide_equal_operator_impl<T, U>::type { };


//	"has_mod_equal_operator_impl"
//
template<typename T, typename U>
struct has_mod_equal_operator_impl
{
	template<typename V, typename W = V>
	static auto test(V*) -> decltype(std::declval<V &>() /= std::declval<const W &>());

	template<typename, typename = void>
	static auto test(...) -> std::false_type;

	using type = typename std::is_same<T &, decltype(test<T, U>(0))>::type;
};

//	"has_mod_equal_operator"
//
template<typename T, typename U = T>
struct has_mod_equal_operator
		: public has_mod_equal_operator_impl<T, U>::type { };


//	"has_subscript_operator"
//
template<typename T, typename Index, typename = void>
struct has_subscript_operator : public std::false_type { };

//	"has_subscript_operator"
//
template<typename T, typename Index>
struct has_subscript_operator<T, Index, std::void_t<
		decltype(std::declval<T &>()[std::declval<Index>()])
		>> : public std::true_type { };

//	"has_subscript_operator_t"
//
template<typename T, typename Index>
using has_subscript_operator_t = typename has_subscript_operator
										  			<T, Index>::type;

//	"has_const_subscript_operator"
//
template<typename T, typename Index, typename = void>
struct has_const_subscript_operator : public std::false_type { };

//	"has_const_subscript_operator"
//
template<typename T, typename Index>
struct has_const_subscript_operator<T, Index, std::void_t<
		decltype(std::declval<const T &>()[std::declval<Index>()])
		>> : public std::true_type { };


//	"has_function_call_operator"
//
template<typename T, typename Index, typename = void>
struct has_function_call_operator : public std::false_type { };

//	"has_function_call_operator"
//
template<typename T, typename Index>
struct has_function_call_operator<T, Index, std::void_t<
		decltype(std::declval<T &>()(std::declval<Index>()))
		>> : public std::true_type { };


//	"has_2D_function_call_operator"
//
template<typename T, typename Index, typename = void>
struct has_2D_function_call_operator : public std::false_type { };

//	"has_2D_function_call_operator"
//
template<typename T, typename Index>
struct has_2D_function_call_operator<T, Index, std::void_t<
		decltype(std::declval<T &>()(std::declval<Index>(),
									 std::declval<Index>()))
		>> : public std::true_type { };


//	"has_prefix_increment_operator"
//
//	NOT ORIGINAL CODE.  
//	IMPLEMENTATION REFERENCED FROM STACK OVERFLOW.
template<typename T>
struct has_prefix_increment_operator
{
	template<typename U, typename = decltype( ++(std::declval<U &>()) )>
	static long test(const U &&);
	static char test(...);

	static constexpr bool value = ( sizeof(long) == 
									sizeof(test(std::declval<T>())) );
};


//	"has_postfix_increment_operator"
//
template<typename T>
struct has_postfix_increment_operator
{
	template<typename U, typename = decltype( (std::declval<U &>())++ )>
	static long test(const U &&);
	static char test(...);

	static constexpr bool value = ( sizeof(long) == 
									sizeof(test(std::declval<T>())) );
};


//	"has_prefix_decrement_operator"
//
template<typename T>
struct has_prefix_decrement_operator
{
	template<typename U, typename = decltype( --(std::declval<U &>()) )>
	static long test(const U &&);
	static char test(...);

	static constexpr bool value = ( sizeof(long) == 
									sizeof(test(std::declval<T>())) );
};


//	"has_postfix_decrement_operator"
//
template<typename T>
struct has_postfix_decrement_operator
{
	template<typename U, typename = decltype( (std::declval<U &>())-- )>
	static long test(const U &&);
	static char test(...);

	static constexpr bool value = ( sizeof(long) == 
									sizeof(test(std::declval<T>())) );
};

//
// *************************************************************************** //
// *************************************************************************** //   END.



// *************************************************************************** //
//
//
//
//      2.      NON-MEMBER DEFINED OVERLOADED OPERATORS...
// *************************************************************************** //
// *************************************************************************** //

//	"has_addition_operator_impl"
//
template<typename T>
struct has_addition_operator_impl
{
	template<typename U>
	static auto test(U*) -> decltype(std::declval<const U &>() +
									 std::declval<const U &>());

	template<typename>
	static auto test(...) -> std::false_type;

	using type = typename std::is_same<T, decltype(test<T>(0))>::type;
};

//	"has_addition_operator"
//
template<typename T>
struct has_addition_operator
		: public has_addition_operator_impl<T>::type { };


//	"has_subtraction_operator_impl"
//
template<typename T>
struct has_subtraction_operator_impl
{
	template<typename U>
	static auto test(U*) -> decltype(std::declval<const U &>() -
									 std::declval<const U &>());

	template<typename>
	static auto test(...) -> std::false_type;

	using type = typename std::is_same<T, decltype(test<T>(0))>::type;
};

//	"has_subtraction_operator"
//
template<typename T>
struct has_subtraction_operator
		: public has_subtraction_operator_impl<T>::type { };


//	"has_multiplication_operator_impl"
//
template<typename T>
struct has_multiplication_operator_impl
{
	template<typename U>
	static auto test(U*) -> decltype(std::declval<const U &>() * 
									 std::declval<const U &>());

	template<typename>
	static auto test(...) -> std::false_type;

	using type = typename std::is_same<T, decltype(test<T>(0))>::type;
};

//	"has_multiplication_operator"
//
template<typename T>
struct has_multiplication_operator
		: public has_multiplication_operator_impl<T>::type { };


//	"has_division_operator_impl"
//
template<typename T>
struct has_division_operator_impl
{
	template<typename U>
	static auto test(U*) -> decltype(std::declval<const U &>() /
									 std::declval<const U &>());

	template<typename>
	static auto test(...) -> std::false_type;

	using type = typename std::is_same<T, decltype(test<T>(0))>::type;
};

//	"has_division_operator"
//
template<typename T>
struct has_division_operator
		: public has_division_operator_impl<T>::type { };


//	"has_modulo_operator_impl"
//
template<typename T>
struct has_modulo_operator_impl
{
	template<typename U>
	static auto test(U*) -> decltype(std::declval<const U &>() %
									 std::declval<const U &>());

	template<typename>
	static auto test(...) -> std::false_type;

	using type = typename std::is_same<T, decltype(test<T>(0))>::type;
};

//	"has_modulo_operator"
//
template<typename T>
struct has_modulo_operator
		: public has_modulo_operator_impl<T>::type { };






//	"has_ostream_operator_impl"
//
template<class Class>
struct has_ostream_operator_impl
{
	template<class V>
	static auto test(V*) -> decltype(std::declval<std::ostream &>() << std::declval<V>());

	template<typename>
	static auto test(...) -> std::false_type;

	using type = typename std::is_same<std::ostream &, decltype(test<Class>(0))>::type;
};

//	"has_ostream_operator"
//	
//	NOT ORIGINAL CODE.
//		This code is based off of an implementation referenced from
//		'GitHub Gist' that is written by user "szymek156" (GitHub Username).
template<class Class>
struct has_ostream_operator
		: public has_ostream_operator_impl<Class>::type { };
  
  

//  "is_convertible_impl"
//
template<typename From, typename To>
struct is_convertible_impl
{
    template<typename F>
    static auto test(F*) -> decltype(static_cast<To>(std::declval<F>()), std::true_type{});

    template<typename>
    static auto test(...) -> std::false_type;

    using type = decltype(test<From>(0));
};

//  "is_convertible_impl"
//
template<typename From, typename To>
struct is_convertible
        : public is_convertible_impl<From, To>::type { };



// *************************************************************************** //
//
//
//
// *************************************************************************** //

// *************************************************************************** //
//	2.1.    GENERAL / MISC. METAFUNCTIONS...
// *************************************************************************** //

//	"is_floating_point"
//
template<typename T>
struct is_floating_point 
		: std::integral_constant
		  <
		   bool, 
		   std::is_same<float, typename std::remove_cv<T>::type>::value
		  > 
{ };


// *************************************************************************** //
// *************************************************************************** //

/*
template<class Class>
struct has_clear_function_impl
{
	template<class V>
	static auto test(V*) -> decltype(std::declval<V>() += std::declval<V>());

	template<typename>
	static auto test(...) -> std::false_type;

	using type = typename std::is_same<Class &, decltype(test<Class>(0))>::type;
};

template<class Class>
struct has_clear_function
		: public has_clear_function_impl<Class>::type { };
*/

//
// *************************************************************************** //
// *************************************************************************** //   END.












// *************************************************************************** //
//
//
//
//	    4.0.    "AUDITOR" CLASS...
// *************************************************************************** //
// *************************************************************************** //


#ifdef DO_NOT_DEFINE_THIS

template<class T>
struct auditor {
// *************************************************************************** //
// *************************************************************************** //
    using                       size_type                           = std::size_t;

    //  Group 1.
    static constexpr bool       has_assignment_operator             = cb::has_assignment_operator<T>::value;
    static constexpr bool       has_plus_equal_operator             = cb::has_plus_equal_operator<T>::value;
    static constexpr bool       has_minus_equal_operator            = cb::has_minus_equal_operator<T>::value;
    static constexpr bool       has_times_equal_operator            = cb::has_times_equal_operator<T>::value;
    static constexpr bool       has_divide_equal_operator           = cb::has_divide_equal_operator<T>::value;
    static constexpr bool       has_mod_equal_operator              = cb::has_mod_equal_operator<T>::value;
    
    //  Group 2.
    static constexpr bool       has_subscript_operator              = cb::has_subscript_operator<T, size_type>::value;
    static constexpr bool       has_const_subscript_operator        = cb::has_const_subscript_operator<T, size_type>::value;
    static constexpr bool       has_function_call_operator          = cb::has_function_call_operator<T, size_type>::value;
    static constexpr bool       has_2D_function_call_operator       = cb::has_2D_function_call_operator<T, size_type>::value;
    static constexpr bool       has_ostream_operator                = cb::has_ostream_operator<T>::value;
    
    //  Group 3.
    static constexpr bool       has_prefix_increment_operator       = cb::has_prefix_increment_operator<T>::value;
    static constexpr bool       has_postfix_increment_operator      = cb::has_postfix_increment_operator<T>::value;
    static constexpr bool       has_prefix_decrement_operator       = cb::has_prefix_decrement_operator<T>::value;
    static constexpr bool       has_postfix_decrement_operator      = cb::has_postfix_decrement_operator<T>::value;
    static constexpr bool       has_addition_operator               = cb::has_addition_operator<T>::value;
    static constexpr bool       has_subtraction_operator            = cb::has_subtraction_operator<T>::value;
    static constexpr bool       has_multiplication_operator         = cb::has_multiplication_operator<T>::value;
    static constexpr bool       has_division_operator               = cb::has_division_operator<T>::value;
    static constexpr bool       has_modulo_operator                 = cb::has_modulo_operator<T>::value;
    
    
//  Default Constructor.
//
auditor(void)
{
    static constexpr unsigned int   num_groups      = 3U;
    const size_type                 width           = 35ULL;
    using                           names_t         = std::array< std::string, num_groups >;
    using                           tests_t         = std::vector< std::pair<std::string, bool> >;
    const std::string               type            = utl::type_name<T>();
    unsigned int		            t	            = 0U;
    unsigned int		            g	            = 0U;
    
    
    const std::array< std::string, 2 >  TF_fmt      = { std::string(RED_BB) + "FALSE" + RESET + "\n",
                                                        std::string(GREEN_BB) + "TRUE" + RESET + "\n" };

    auto print_test = [&width, &TF_fmt](const std::string & test, const bool result) {
        std::cout << "\t"         << YELLOW_BB    << std::setw(width)
                  << std::right   << test         << RESET
                  << "\t: "       << (result ? TF_fmt[1] : TF_fmt[0]);
    };
    
    
    const names_t                   groups          = {
        "ASSIGNMENT / COMPOUNT-ASSIGNMENT OPERATORS",   "DATA ACCESS OPERATORS",    "ARETHMETIC OPERATORS"
    };
    const tests_t                   tests_1         = {
        std::make_pair( "has_assignment_operator",          has_assignment_operator         ),      std::make_pair( "has_plus_equal_operator",          has_plus_equal_operator         ),
        std::make_pair( "has_plus_equal_operator",          has_plus_equal_operator         ),      std::make_pair( "has_minus_equal_operator",         has_minus_equal_operator        ),
        std::make_pair( "has_times_equal_operator",         has_times_equal_operator        ),      std::make_pair( "has_divide_equal_operator",        has_divide_equal_operator       ),
        std::make_pair( "has_mod_equal_operator",           has_mod_equal_operator          )
    };
    const tests_t                   tests_2         = {
        std::make_pair( "has_subscript_operator",           has_subscript_operator          ),      std::make_pair( "has_const_subscript_operator",     has_const_subscript_operator    ),
        std::make_pair( "has_function_call_operator",       has_function_call_operator      ),      std::make_pair( "has_2D_function_call_operator",    has_2D_function_call_operator   ),
        std::make_pair( "has_ostream_operator",             has_ostream_operator            ),
    };
    const tests_t                   tests_3         = {
        std::make_pair( "has_prefix_increment_operator",    has_prefix_increment_operator   ),      std::make_pair( "has_postfix_increment_operator",   has_postfix_increment_operator  ),
        std::make_pair( "has_prefix_decrement_operator",    has_prefix_decrement_operator   ),      std::make_pair( "has_postfix_decrement_operator",   has_postfix_decrement_operator  ),
        std::make_pair( "has_addition_operator",            has_addition_operator           ),      std::make_pair( "has_subtraction_operator",         has_subtraction_operator        ),
        std::make_pair( "has_multiplication_operator",      has_multiplication_operator     ),      std::make_pair( "has_division_operator",            has_division_operator           ),
        std::make_pair( "has_modulo_operator",              has_modulo_operator             )
    };
    
    const std::vector< tests_t >    tests           = { tests_1, tests_2, tests_3 };
    
    
    
    //  0.  HEADER OUTPUT FOR AUDITOR CLASS.
    std::cout << WHITE_BB   << "\nAUDIT OF TYPE \""
              << RED_BB     << UNDERLINE            << type
              << RESET      << WHITE_BB             << "\"..."
              << DIM        << utl::println         << "\n"     << RESET;
              
           
    //  1.  ITERATE THROUGH EACH GROUP OF TESTS...
    for (g=0U; g < num_groups; ++g)
    {
        //  1.1.    Display header for the group.
        std::cout << MAGENTA_BOLD   << "GROUP #"    << g        << ".\t"
                  << CYAN_BRIGHT    << groups[g]
                  << MAGENTA_BOLD   << "..."        << RESET    << std::endl;
    
    
        //  1.2.    Use Range-Based 'for'-Loop through each set of tests.
        for (const auto & test : tests[g])
        {
            const auto &    [name, result]     = test;//   Structured Binding.
            
            std::cout << "\t" << WHITE_BOLD << g << "." << ++t << ".\t" << RESET;
            print_test(name, result);
        }
    
        t=0U;
        std::cout << char(0x0A * (g != (num_groups-1ULL)));
    }

    std::cout << DIM << cb::utl::println << RESET << "\n" << std::endl;
    //return;
}
    

//  Default Destructor.
//
~auditor(void) = default;
    

//	END "AUDITOR" INLINE CLASS DEFINITION.
// *************************************************************************** //
// *************************************************************************** //
};

#endif




// *************************************************************************** //
// *************************************************************************** //
}// END NAMESPACE   "cblib".






// *************************************************************************** //
// *************************************************************************** //
#endif	//  _CBLIB_MATRIX_DRIVER_H  //
