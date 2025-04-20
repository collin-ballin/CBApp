/******************************************************************************
 *	
 *	File:			"cb_array.h"
 *	
 *	Type:			INTERNAL HEADER FILE.
 *
 *	Description:	
 *		This file is an INTERNAL header file - meaning this header is int-
 *	-ended to be included and used by other LIBRARY header files.  DO NOT
 *	include this header file directly.  Instead, include the associated 
 *	LIBRARY header file which utilizes this header file, "array.h".
 *
******************************************************************************/
#ifndef _CB_ARRAY_H
#define _CB_ARRAY_H 1

#include <iostream>
#include <stdexcept>
#if __cplusplus >= 201103L
# include <initializer_list>
#endif	// C++11.
#include <utility>
#include <cstring>


namespace cb /* _GLIBCXX_VISIBILITY(default) */
{ 
// 	BEGIN NAMESPACE "cb".
/*****************************************************************************/


/*****************************************************************************/
/*****************************************************************************/
// 				PRIMARY TEMPLATE DECLARATION:
// 		Fixed-Size Array Class Template Definition
/*****************************************************************************/
/*****************************************************************************/

//	"array"
//
template< typename 		T, 	
		  std::size_t 	N, 
		  typename 		Allocator 	= std::allocator<T> >
class array
{
	public:

// 	Class-Defined Nested Public Alias Definitions.
/*****************************************************************************/

	using 	allocator_type		= Allocator;
	using	Alloc				= allocator_type;
	using 	value_type 			= T;
	using 	size_type			= std::size_t;
	using 	difference_type 	= std::ptrdiff_t;
	using 	pointer 			= std::allocator_traits<Allocator>::pointer;
	using 	const_pointer 		= std::allocator_traits<Allocator>::const_pointer;
	using 	reference 			= value_type &;
	using 	const_reference 	= const value_type &;
	using 	iterator 			= T *;


//	Declaration of Friend Classes and Functions.
/*****************************************************************************/
//	
//	...
//


//	"Concept" / "Requires" clauses to prevent imporper template-type arguments.
/*****************************************************************************/

#ifdef _GLIBCXX_CONCEPT_CHECKS
# if __cplusplus >= 201103L
	//	Value-Type.
	static_assert(std::is_same<typename std::remove_cv<T>::type, 
							   T>::value, 
				  "cb::array value-type requirements not met,\n"
				  "array must have a non-const, non-volatile value_type.");

# if __cplusplus >= 201703L || defined __STRICT_ANSI__
	static_assert(std::is_same<Alloc::value_type, T>::value,
			  	  "cb::array allocator-type requirements not met,\n"
			  	  "array must have the same value_type as its allocator.\n");
# endif	// C++ 2017,03L

	//	Number of Rows (Parameter 'N').
	static_assert(N > 0, 
				  "cb::array size requirements not met.\nSize (Number of "
				  "elements) of array, N, must be greater-than 0.\n");
# endif	// C++ 2011,03L.
#endif 	// _GLIBCXX_CONCEPT_CHECKS.


/*****************************************************************************/

	protected:

// 	Protected Data-Members.
/*****************************************************************************/

	pointer		m_array		= nullptr;


/*****************************************************************************/

	private:

// 	Private Member Functions.
//
/*****************************************************************************/
//
//	...
//


/*****************************************************************************/

   protected:

// 	Protected Operation Member Functions.
/*****************************************************************************/
//
//	...
//


// 	Protected Utility Member Functions.
/*****************************************************************************/
//
//	...
//


/*****************************************************************************/

	public:

// 	Constructors, Destructor.
/*****************************************************************************/
//
//	...
//


//	Overloaded Member Operators.
/*****************************************************************************/
//
//	...
//


// 	Overloaded Non-Member Operators.
/*****************************************************************************/
//
//	...
//


// 	Public Operation Member Functions.
/*****************************************************************************/
//
//	...
//


// 	Public Utility Member Functions.
/*****************************************************************************/

	// 	"size"
	//
	[[nodiscard]] inline size_type size(void) const noexcept
	{ return N; }


	//	"empty"
	//
	[[nodiscard]] inline bool empty(void) const noexcept
	{ return (this->m_array == nullptr); }


/*****************************************************************************/
/*****************************************************************************/
};




/*****************************************************************************/
// 	END NAMESPACE "cb".
}

#endif /* _CBLIB_ARRAY_H */
