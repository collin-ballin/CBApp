/******************************************************************************
 *	File:			"PCH.h"
 *	Type:			INTERNAL HEADER FILE.
******************************************************************************/
#ifndef _CBLIB_PCH_H
#define _CBLIB_PCH_H 1


// 1. Globally Included Headers and Libraries.
// *************************************************************************** //

// Headers for I/O.
// *************************************************************************** //

#ifndef _GLIBCXX_IOSTREAM
# include <iostream>
#endif	// _GLIBCXX_IOSTREAM

#ifndef _GLIBCXX_IOMANIP
# include <iomanip>
#endif	// _GLIBCXX_IOMANIP

#ifndef _GLIBCXX_FSTREAM
# include <fstream>
#endif	// _GLIBCXX_FSTREAM

#ifndef _GLIBCXX_MEMORY
# include <memory>
#endif	// _GLIBCXX_MEMORY


// Headers for Classes and other Data-Types.
// *************************************************************************** //

#ifndef _GLIBCXX_STRING
# include <string>
#endif	// _GLIBCXX_STRING

#ifndef _GLIBCXX_VECTOR
# include <vector>
#endif	// GLIBCXX_VECTOR

#ifndef _GLIBCXX_ITERATOR
# include <iterator>
#endif	// _GLIBCXX_ITERATOR

#ifndef _GLIBCXX_CSTDINT
# include <cstdint>
#endif	// _GLIBCXX_CSTDINT

#ifndef _GLIBCXX_STDEXCEPT
# include <stdexcept>
#endif	// _GLIBCXX_STDEXCEPT

#ifndef _GLIBCXX_CHRONO
# include <chrono>
#endif	// _GLIBCXX_CHRONO

#ifndef _GLIBCXX_CCTYPE
# include <cctype>
#endif	// _GDLIBCXX_CCTYPE


// Headers for Extended Functionality (e.g. Exception-Handling).
// *************************************************************************** //

#ifndef __EXCEPTION__
# include <exception>
#endif	// __EXCEPTION__

#ifndef _GLIBCXX_THREAD
# include <thread>
#endif	// _GLIBCXX_THREAD

#ifndef _GLIBCXX_UTILITY
# include <utility>
#endif	// _GLIBCXX_UTILITY

#ifndef _GLIBCXX_NUMERIC_LIMITS
# include <limits>
#endif	// _GLIBCXX_NUMERIC_LIMITS

#ifndef _GLIBCXX_TYPE_TRAITS
# include <type_traits>
#endif	// _GLIBCXX_TYPE_TRAITS

#ifndef _TYPEINFO
# include <typeinfo>
#endif	// _TYPEINFO


// *************************************************************************** //
#endif


