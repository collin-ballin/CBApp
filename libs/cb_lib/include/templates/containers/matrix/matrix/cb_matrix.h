/******************************************************************************
 *	
 *	File:			"cb_matrix.h"
 *	
 *	Type:			INTERNAL HEADER FILE.
 *
 *	Description:	
 *		This file is an INTERNAL header file - meaning this header is int-
 *	-ended to be included and used by other LIBRARY header files.  DO NOT
 *	include this header file directly.  Instead, include the associated 
 *	LIBRARY header file which utilizes this header file, "matrix.h".
 *
******************************************************************************/
#ifndef _CB_MATRIX_H
#define _CB_MATRIX_H 1


//#ifndef	_CB_ARRAY_H
//# include "../../array/array.h"
//#endif	/* _CB_ARRAY_H 


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
//***************************************************************************//

//***************************************************************************//
// 	Forward-Declarations for Fixed-Size Matrix Class Template.
//***************************************************************************//

/*template< typename 		T, 	
		  std::size_t 	N, 
		  std::size_t 	M, 	
		  class 		Allocator >
class matrix;*/


//***************************************************************************//
//***************************************************************************//
// 				PRIMARY TEMPLATE DECLARATION:
// 		Fixed-Size Matrix Class Template Definition
//***************************************************************************//
//***************************************************************************//

//	"matrix"
//
template< typename 		T, 	
		  std::size_t 	N, 
		  std::size_t 	M, 	
		  typename 		Allocator 	= std::allocator<T> >
class matrix
{
	public:

// 	Class-Defined Nested Public Alias Definitions.
//***************************************************************************//

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
//***************************************************************************//

	template<typename, std::size_t, std::size_t, typename>
	friend class matrix;


//	"Concept" / "Requires" clauses to prevent imporper template-type arguments.
//***************************************************************************//

#ifdef _GLIBCXX_CONCEPT_CHECKS
# if __cplusplus >= 201103L
	//	Value-Type.
	static_assert(is_matrix_type<T>::value, 
			  	  "cb::matrix value-type requirements not met.\n");

	static_assert(std::is_same<typename std::remove_cv<T>::type, 
							   T>::value, 
				  "cb::matrix value-type requirements not met,\n"
				  "matrix must have a non-const, non-volatile value_type.");

	//	Allocator-Type.
	static_assert(is_matrix_allocator<T, Allocator>::value,
			  	  "cb::matrix allocator-type requirements not met.\n");

# if __cplusplus >= 201703L || defined __STRICT_ANSI__
	static_assert(std::is_same<Alloc::value_type, T>::value,
			  	  "cb::matrix allocator-type requirements not met,\n"
			  	  "matrix must have the same value_type as its allocator.\n");
# endif	// C++ 2017,03L

	//	Number of Rows (Parameter 'N').
	static_assert(N > 0, 
				  "cb::matrix dimension requirements not met.\nNumber of "
				  "rows in the matrix, N, must be greater-than 0.\n");

	//	Number of Columns (Parameter 'M').
	static_assert(M > 0, 
				  "cb::matrix dimension requirements not met.\nNumber of "
			  	  "columns in the matrix, M, must be greater-than 0.\n");
# endif	// C++ 2011,03L.
#endif 	// _GLIBCXX_CONCEPT_CHECKS.


//***************************************************************************//

	protected:

// 	Protected Data-Members.
//***************************************************************************//

	pointer		m_matrix	= nullptr;


//***************************************************************************//

   private:

// 	Private Member Functions.
//
//***************************************************************************//

	//	"new_handler"
	//
	inline void new_handler(const char * location = nullptr) noexcept
	{
		std::cerr << "Matrix Class-Template's \"new_handler\" has been called after "
				  << "exception was thrown by \"new\" operator due to memory allocat"
				  << "ion failure in location: \"";

		if (location) 	std::cerr << location << "\"." << '\n';			
		else 			std::cerr << "UNKNOWN\"." << '\n';			

		if (this->m_matrix)
			delete [] this->m_matrix;

		this->m_matrix 	= nullptr;

		return;
	}


//***************************************************************************//

   protected:

// 	Protected Operation Member Functions.
//***************************************************************************//


// 	Protected Utility Member Functions.
//***************************************************************************//

	// 	"display"
	//
	inline std::ostream & display(std::ostream & output) const noexcept
	{
   		char 				space 		= 0x0;
   		const_pointer 		current 	= this->m_matrix;

		// 	CASE 0 : This Matrix is EMPTY.
		if (!current)
		{
			output << "[ ]";
			return output;
		}

   		output << "[\t";
   		// 	Looping through each row in the matrix.
		for(size_type i = 0ULL; i < M; ++i)
		{
			space = char(0x09 * (i != 0ULL));
			output << char(0x0a * (i != 0ULL))
		       	   << space << "[";

   			// 	Looping through each column in the matrix.
			for(size_type j = 0ULL; j < N; ++j)
			{
				output << (*current) 
			       	   << char(0x2c * (j != (N-1ULL))) 
			       	   << char(0x09 * (j != (N-1ULL)));
				++current;
			}
			output << "]" << char(0x2c * (i != (M-1ULL)));
		}
   		output << "\t]";

   		return output;
	}


//***************************************************************************//

	public:

// 	Constructors, Destructor.
//***************************************************************************//

/* 
NOTE: USE THIS WHEN DESIGNING A CLASS THAT NEEDS THE FOLLOWING...
		=>	"class_member = default;"
THIS CODE IMPLEMENTS THE "member = default;" FEATURE THAT WAS PROVIDED IN 
C++11, HOWEVER - IF COMPILED WITH A DIFFERENT VERSION OF C++, IT MANUALLY 
SPECIFIES AN EMPTY CONSTRUCTOR TO SERVE AS THE DEFAULT.

#if __cplusplus >= 201103L	
	matrix() = default;
#else
	matrix() { }
#endif	// __cplusplus >= 201103L. 
*/


	//	Default Constructor.
	//	throw(std::bad_alloc)
	//
	inline matrix(void) 
	{
		const size_type	size		= N * M;
		pointer			current		= nullptr;
		pointer			end			= nullptr;	

		try
		{
			this->m_matrix 	= new value_type [size];
			current			= this->m_matrix;
			end				= this->m_matrix + size;

			for (; current != end; *current++ = T());
		}
		catch(std::bad_alloc & new_error)
		{
			this->new_handler("DEFAULT CONSTRUCTOR");
			throw;
		}
	}


	// 	Destructor.
	//
	inline virtual ~matrix(void) noexcept 
	{
		if (this->m_matrix) 
		{
			delete [] this->m_matrix;
			this->m_matrix = nullptr;
		}
	}


	//	Copy Constructor.
	// 	throw(std::bad_alloc)
	//
	inline matrix(const matrix & source)
	{
		const size_type		size			= N * M;
		const_pointer 		src_current		= source.m_matrix;
		pointer				dst_current		= nullptr;

		// 	CASE 1 : "Source" matrix is NOT EMPTY.
		if (source.m_matrix)
		{
			try
			{
				this->m_matrix 	= new value_type [size];
				dst_current 	= this->m_matrix;

				for (size_type i = 0ULL; i < size; ++i)
					 *dst_current++ = *src_current++;
			}
			catch (std::bad_alloc & error)
			{
				this->new_handler("COPY CONSTRUCTOR");
				throw;
			}
		}
	}


	//	"Move" Constructor.
	//
	inline matrix(matrix && source) noexcept
				  : m_matrix(std::move(source.m_matrix))
	{ 
		static_assert(std::is_move_constructible<T>::value,
			  	  	  "cb::matrix value-type is not move-constructible.\n");
		source.m_matrix = nullptr; 
	}


	// 	Nested Initialization List Constructor.
	// 	throw(std::invalid_argument, std::bad_alloc)
	//
	inline matrix(const std::initializer_list<std::initializer_list<T>> & list)
	{
		using 	iter_t	= std::initializer_list<std::initializer_list<T>>::iterator;
		using	error_t	= std::invalid_argument;
		iter_t	it		= list.begin(), 
				src_end	= list.end();
		pointer				current		= nullptr;
		size_type			rows		= list.size();
		const size_type		size		= N * M;
		bool				bad_dims	= false;

		// 	CASE 0 : The initializer list is EMPTY.
		if (it == src_end)
		{
			error_t	empty_list("Empty Initializer-List.\nCannot perform the am"
							   "biguous operation of assigning the Matrix's va"
							   "lue from an empty initializer list.");
			throw empty_list;
		}

		try
		{
			bad_dims = (rows != N && it->size() != M);

			// 	1. Check that each row in the initializer list of is equal size.
			for (; (it != src_end) && (!bad_dims); ++it)
				bad_dims = (it->size() != rows);
	
			// 	CASE 1.1 : INVALID LIST DIMENSIONS. 
			if (bad_dims)
			{
				error_t	bad_list("Invalid Initializer-List Dimensions.\nThe nu"
								 "mber of elements in each nested-initializer "
								 "list must all be equal.");
				throw bad_list;
			}

			// 	2. Dynamically Allocating memory for the Matix and copy-
			// 	   -ing the values from the initialization list into the Matrix.
			this->m_matrix 	= new value_type [size];

			current 		= this->m_matrix;
			for (it = list.begin(); it != src_end; ++it)
			{
				for (value_type elem : (*it))
					*current++ = elem;
			}
		}
		catch (std::bad_alloc & new_error)
		{
			this->new_handler("NESTED-INITIALIZATION LIST CONSTRUCTOR");
			throw;
		}
	}


//	Overloaded Member Operators.
//***************************************************************************//

	//	Overloaded Assignment Operator = (Matrix).
	//
	//	FIX THIS:
	//		Fix this function so that, if a memory allocation error occurs 
	//		(a call to "new" throws std::bad_alloc), DON'T delete the existing
	//		memory.  
	//	INSTEAD:
	//		Return the matrix to its previous state before the assignment
	//		operation was invoked.
	//
	inline matrix & operator = (const matrix & source) 
	{
		const size_type		size			= N * M;
		const_pointer		src_current		= source.m_matrix;
		pointer 			dst_current 	= this->m_matrix;

		// 	CASE 0 : Check for Self-Assignment 
		// 			 OR - The "Source" Matrix is EMPTY.
		if (&source == this || !src_current)
			return (*this);


		// 	1. Performing a 'deep-copy' of all the data-members.
		try
		{
			this->m_matrix 	= new value_type [size];

			// 	2. Check for any previously existing dynamically allocated
			// 	   memory prior to performing deep-copy assignment operation.
			if (dst_current)
				delete [] dst_current;

			dst_current 	= this->m_matrix;
			for (size_type i = 0ULL; i < size; ++i)
				 *dst_current++ = *src_current++;
		}
		catch(std::bad_alloc & error)
		{
			this->m_matrix = dst_current;
			throw;
		}

		return (*this);
	}


	// 	Overloaded Assignment Operator = (std::initialization_list).
	//	throw(std::invalid_argument)
	//
	inline matrix & operator = (const std::initializer_list<std::initializer_list<T>> 
								& list)
	{
		using 	iter_t	= std::initializer_list<std::initializer_list<T>>::iterator;
		using	error_t	= std::invalid_argument;
		iter_t	it		= list.begin(), 
				src_end	= list.end();
		pointer				current		= this->m_matrix;
		size_type			rows		= list.size();
		const size_type		size		= N * M;
		bool				bad_dims	= false;

		// 	CASE 0 : The initializer list is EMPTY.
		if (it == src_end)
		{
			error_t	empty_list("Empty Initializer-List.\nCannot perform the am"
							   "biguous operation of assigning the Matrix's va"
							   "lue from an empty initializer list.");
			throw empty_list;
		}

		try
		{
			bad_dims = (rows != N && it->size() != M);

			// 	1. Check that each row in the initializer list of is equal size.
			for (; (it != src_end) && (!bad_dims); ++it)
				bad_dims = (it->size() != rows);
	
			// 	CASE 1.1 : INVALID LIST DIMENSIONS. 
			if (bad_dims)
			{
				error_t	bad_list("Invalid Initializer-List Dimensions.\nThe nu"
								 "mber of elements in each nested-initializer "
								 "list must all be equal.");
				throw bad_list;
			}

			// 	2. Dynamically Allocating memory for the Matix and copy-
			// 	   -ing the values from the initialization list into the Matrix.
			this->m_matrix 	= new value_type [size];

			//	2.1. Check for previously-allocated dynamic memory.
			if (current)
				delete [] current;

			current 		= this->m_matrix;
			for (it = list.begin(); it != src_end; ++it)
			{
				for (value_type elem : (*it))
					*current++ = elem;
			}
		}
		catch (std::bad_alloc & new_error)
		{
			this->m_matrix = current;
			throw;
		}

		return (*this);
	}


	//	Overloaded "Move" Assignment Operator = (Matrix).
	//
	inline matrix & operator = (matrix && source) noexcept
	{
		static_assert(std::is_move_assignable<T>::value,
			  	  	  "cb::matrix value-type is not move-assignable.\n");


		//	CASE 0 : Check for Self-Assignment.
		if (&source == this)
			return (*this);

		//	1. Check for any preexisting dynamically-allocated 
		//	memory prior to performing the assignment operation.
		if (this->m_matrix)
		{
			delete [] this->m_matrix;
			this->m_matrix = nullptr;
		}

		// 	2. Transfering Ownership (Moving) the data-members.
		this->m_matrix 	= std::move(source.m_matrix);
		source.m_matrix = nullptr;

		return (*this);
	}


	// 	Overloaded Compound Assignment Operator += (matrix and matrix).
	//	throw(std::invalid_argument)
	//
	inline matrix & operator += (const matrix & matrix_B) 
	{
		pointer					a_current	= this->m_matrix;
		pointer					end			= this->m_matrix + N*M;
		const_pointer			b_current	= matrix_B.m_matrix;
		std::invalid_argument	empty("One or both of the matrices are emp"
									  "ty (have no allocated memory).\n");

		//	CASE 0 : One or both of the provided matrices are EMPTY.
		if (!a_current || !b_current)
			throw empty;

		for (; a_current != end; *a_current++ += *b_current++);

		return (*this);
	}


	// 	Overloaded Compound Assignment Operator -= (matrix and matrix).
	//	throw(std::invalid_argument)
	//
	inline matrix & operator -= (const matrix & matrix_B) 
	{
		pointer					a_current	= this->m_matrix;
		pointer					end			= this->m_matrix + N*M;
		const_pointer			b_current	= matrix_B.m_matrix;
		std::invalid_argument	empty("One or both of the matrices are emp"
									  "ty (have no allocated memory).\n");

		//	CASE 0 : One or both of the provided matrices are EMPTY.
		if (!a_current || !b_current)
			throw empty;

		for (; a_current != end; *a_current++ -= *b_current++);

		return (*this);
	}


	// 	Overloaded Compound Assignment Operator *= (matrix and matrix).
	//	throw(std::invalid_argument)
	//
	inline matrix & operator *= (const matrix & matrix_B) 
	{
   		pointer 		a_current 		= nullptr;
   		const_pointer 	b_current 		= nullptr;
		value_type		products [M]	= { 0 };

		//	CASE 0 : One or both of the provided matrices are EMPTY.
		if (!this->m_matrix || !matrix_B.m_matrix)
		{
			std::invalid_argument	empty("One or both of the matrices are emp"
										  "ty (have no allocated memory).\n");
			throw empty;
		}

   		// 	Outermost For-Loop.
   		for(size_type c = 0ULL; c < M; ++c)
		{
			// 	1. Iterating through each of the N-columns in matrix B.
			for(size_type i = 0ULL; i < N; ++i)
			{
				a_current 	= (this->m_matrix + (c*M));
				b_current 	= (matrix_B.m_matrix + i);

				// 	2. Iterating through each element in this
				//     respective row within matrix A (*this matrix).
				products[i] = 0;
				for(size_type j = 0ULL; j < M; ++j)
				{
					products[i] += (*a_current) * (*b_current);
					++a_current;
					b_current   += N;
				}
			}

			// 	3. Finally, assigning the values to the row.
			a_current = (this->m_matrix + (c*M));
			for(size_type j = 0ULL; j < M; ++j)
				*a_current++ = *(products + j);
		}

   		return (*this);
	}


	//	Overloaded Subscript Operator [] (Non-Const).
	//
	inline reference operator [] (size_type pos) 
	{ return *(this->m_matrix + pos); }


	//	Overloaded Subscript Operator [] (Const).
	//
	inline const_reference operator [] (size_type pos) const
	{ return *(this->m_matrix + pos); }


	// 	Overloaded Function Call Operator () (Non-Const).
	//
	[[nodiscard]] inline reference operator () (size_type pos_N, size_type pos_M)
	{ return *(this->m_matrix + (this->m_rows * pos_M) + pos_N); }


	// 	Overloaded Function Call Operator () (Const).
	//
	[[nodiscard]] inline const_reference operator () (size_type pos_N, 
													  size_type pos_M) const
	{ return *(this->m_matrix + (this->m_rows * pos_M) + pos_N); }


// 	Overloaded Non-Member Operators.
//***************************************************************************//

	// 	Overloaded std::ostream Extraction Operator.
	//
	inline friend std::ostream & operator << (std::ostream & output, 
					  						  const matrix & arg1) noexcept
	{ return arg1.display(output); }


	// 	Overloaded Equality-Comparison Operator (==, Matrix).
	//
	inline friend bool operator == (const matrix & LHS, 
									const matrix & RHS) noexcept
	{
		bool				equal			= true;
		const size_type		size			= N * M;
		const_pointer 		LHS_current 	= LHS.m_matrix,
							RHS_current 	= RHS.m_matrix;

			//	CASE 0 : Both matrices are EMPTY.
			if (!LHS.m_matrix && !RHS.m_matrix) 
					return true;

			for ( size_type i = 0ULL; 
				  i < size && (equal = (*LHS_current++ == *RHS_current++));
				  ++i );

		return equal;
	}


	// 	Overloaded Inequality-Comparison Operator (!=, Matrix).
	//
	inline friend bool operator != (const matrix & LHS, 
									const matrix & RHS) noexcept
	{
		bool				equal			= true;
		const size_type		size			= N * M;
		const_pointer 		LHS_current 	= LHS.m_matrix,
							RHS_current 	= RHS.m_matrix;

			// 	CASE 0 : Both matrices are EMPTY.
			if (!LHS.m_matrix && !RHS.m_matrix) 
					return false;

			for ( size_type i = 0ULL; 
				  i < size && (equal = (*LHS_current++ == *RHS_current++));
				  ++i );

		return (!equal);
	}


	//	Overloaded Addition Operator (matrix and matrix).
	//	throw(std::invalid_argument)
	//
	[[nodiscard]] inline friend matrix operator + (const matrix & matrix_a,
												   const matrix & matrix_b)
	{
		const_pointer			a_current	= matrix_a.m_matrix,
								b_current	= matrix_b.m_matrix;
		matrix 					sum;
		pointer					current		= sum.m_matrix;
		const_pointer			end			= sum.m_matrix + N*M;
		std::invalid_argument	empty("One or both of the matrices are emp"
									  "ty (have no allocated memory).\n");
	
		//	CASE 0 : One or both of the provided matrices are EMPTY.
		if (!matrix_a.m_matrix || !matrix_b.m_matrix)
			throw empty;

		for (; current != end; 
			   *current++ = *a_current++ + *b_current++);

		return sum;
	}


	//	Overloaded Subtraction Operator (matrix and matrix).
	//	throw(std::invalid_argument)
	//
	[[nodiscard]] inline friend matrix operator - (const matrix & matrix_a,
												   const matrix & matrix_b)
	{
		const_pointer			a_current	= matrix_a.m_matrix,
								b_current	= matrix_b.m_matrix;
		matrix 					diff;
		pointer					current		= diff.m_matrix;
		const_pointer			end			= diff.m_matrix + N*M;
		std::invalid_argument	empty("One or both of the matrices are emp"
									  "ty (have no allocated memory).\n");
	
		//	CASE 0 : One or both of the provided matrices are EMPTY.
		if (!matrix_a.m_matrix || !matrix_b.m_matrix)
			throw empty;

		for (; current != end; 
			   *current++ = *a_current++ - *b_current++);

		return diff;
	}


	// 	Overloaded Multiplication Operator (matrix and matrix).
	// 	throw(std::invalid_argument)
	//
	[[nodiscard]] inline friend matrix operator * (const matrix & matrix_a, 
												   const matrix & matrix_b) 
	{
		value_type				sum			= 0;
   		matrix 					product;
   		pointer					current		= product.m_matrix;
		std::invalid_argument	empty("One or both of the matrices are emp"
									  "ty (have no allocated memory).\n");
	
		//	CASE 0 : One or both of the provided matrices are EMPTY.
		if (!matrix_a.m_matrix || !matrix_b.m_matrix)
			throw empty;


   		// 	Outer Loop for each element in the matrix.
   		for(size_type n = 0ULL; n < M; ++n)
		{
			const_pointer 	a_current 	= (matrix_a.m_matrix + (n*M));
   			const_pointer 	b_current 	= matrix_b.m_matrix;
	
			// 	Iterating across each element in this row.
			for(size_type i = 0ULL; i < M; ++i)
			{
				sum 						= 0;
				const_pointer 	a_temp 	= (a_current);
				const_pointer	b_temp 	= (b_current + (i%M));

				// 	Traversing through every column in the matrix.
				for(size_type j = 0ULL; j < N; ++j)
				{
					sum 		+= (*a_temp) * (*b_temp);
					++a_temp;
					b_temp 	 = (b_temp + N);
				}
				(*current) = sum;
				++current;
			}
		}

		return product;
	}


// 	Public Operation Member Functions.
//***************************************************************************//

	//	"det"
	//	throw(std::out_of_range, std::bad_alloc)
	//
	[[nodiscard]] inline value_type det(void) const
	{
		static_assert(std::is_arithmetic<T>::value, 
					  "determinant is only defined for arithmetic value types");
		static_assert(N == M, "determinant is only defined for square "
							  "matrices (dimension: M x M)");
		std::out_of_range 		empty("Cannot take determinant of empty"
									  " matrix (no allocated memory).\n");
		
		//	CASE 0 : This matrix is EMPTY.
		if (!this->m_matrix)
			throw empty;

		return this->determinant();
	}
	

	//	"determinant"
	//
	inline value_type determinant(void) const noexcept
	{
		value_type 		det			= T();

		//	RECURSIVE BASE-CASE : Dimension of the minor 
		//						  matrices are [2 x 2].
		if constexpr (N == 2ULL)
		{
			det  = (*this)[0] * (*this)[3];
			det -= (*this)[1] * (*this)[2];
		}
		//	CASE 2 : Dimensions are NOT [2 x 2].  
		//			 Continue Decomposing into minor matrices.
		else
		{
			const size_type			size			= N * M;
			int						sign			= 0;
			value_type				current_det		= T();
			pointer 				current 		= nullptr;
			pointer 				min_current 	= nullptr;
			matrix<T, N-1, M-1> 	minor [M];

			//	Decomposing Matrix into Minor Matrices...
			for (std::size_t idx = 0ULL; idx < M; ++idx)
			{
				current 	= (this->m_matrix + M);
				min_current = minor[idx].m_matrix;
	
				for (std::size_t i = M; i < size; ++i)
				{
					*min_current 	= *current++;
					min_current    += ( 1ULL * (i%M != idx) );
				}
			}

			current = this->m_matrix;
			for (std::size_t i = 0ULL; i < M; ++i)
			{
				sign 	 		= ( (1 * (i%2 == 0)) + (-1 * (i%2 == 1)) );
				current_det		= minor[i].determinant() * (*current++);
				det			   += sign * current_det;
			}
		}

		return det;
	}


	//	"swap_rows"
	//	throw(std::out_of_range)
	//
	inline void swap_rows(size_type n1, size_type n2)
	{
		value_type			temp		= 0;
		pointer				n1_ptr		= this->m_matrix + n1*N;
		pointer				n2_ptr		= this->m_matrix + n2*N;
		std::out_of_range 	bad_rows("The provided indices are outside the"
									 " range of the rows in this matrix.\n");

		//	CASE 0 : The matrix is EMPTY OR the rows that have
		//			 been indicated to swap are invalid / out-of-range.
		if (!this->m_matrix	|| n1 == n2	||
				n1 >= N		|| n2 >= N)
			throw bad_rows;

		for (size_type i = 0ULL; i < M;
			 	++i, ++n1_ptr, ++n2_ptr)
		{
			temp 		= std::move(*n1_ptr);
			(*n1_ptr) 	= std::move(*n2_ptr);
			(*n2_ptr)	= temp;
		}

		return;
	}


	//	"swap_columns"
	//	throw(std::out_of_range)
	//
	inline void swap_columns(size_type m1, size_type m2)
	{
		value_type			temp		= 0;
		pointer				m1_ptr		= this->m_matrix + m1;
		pointer				m2_ptr		= this->m_matrix + m2;
		std::out_of_range 	bad_cols("The provided indices are outside the "
									 "range of the columns in this matrix.\n");

			//	CASE 0 : The matrix is EMPTY OR the rows that have
			//			 been indicated to swap are invalid / out-of-range.
			if (!this->m_matrix	|| m1 == m2	||
					m1 >= M		|| m2 >= M)
				throw bad_cols;

			for (size_type i = 0ULL; i < N; 
			 		++i, m1_ptr += N, m2_ptr += N)
			{
				temp 		= std::move(*m1_ptr);
				(*m1_ptr) 	= std::move(*m2_ptr);
				(*m2_ptr)	= std::move(temp);
			}

		return;
	}

/*
	//	"set_row"
	//	throw(std::out_of_range)
	//
	inline void set_row(size_type n1)
	{
		const size_type		N			= this->m_rows,
			  				M			= this->columns;
		//auto				[N, M] 		= this->dimensions();
		value_type			temp		= 0;
		pointer				n1_ptr		= this->m_matrix + n1*N;
		pointer				n2_ptr		= this->m_matrix + n2*N;
		std::out_of_range 	bad_row("The provided indices are outside the"
									 " range of the rows in this matrix.\n");

			//	CASE 0 : The matrix is EMPTY OR the rows that have
			//			 been indicated to swap are invalid / out-of-range.
			if (!this->m_matrix	|| n1 == n2	||
					n1 >= N		|| n2 >= N)
				throw bad_rows;

			for (size_type i = 0ULL; i < M;
			 		++i, ++n1_ptr, ++n2_ptr)
			{
				temp 		= std::move(*n1_ptr);
				(*n1_ptr) 	= std::move(*n2_ptr);
				(*n2_ptr)	= temp;
			}

		return;
	}
*/


	//	"begin"
	//
	[[nodiscard]] inline iterator begin(void) noexcept
	{ return iterator(this->m_matrix); }


	//	"end"
	//
	[[nodiscard]] inline iterator end(void) noexcept
	{ return iterator(this->m_matrix + (this->m_rows * this->m_columns)); }


	//	"clear"
	//
	inline void clear(void) noexcept
	{
			if (this->m_matrix)
			{
					delete [] this->m_matrix;
					this->m_matrix = nullptr;
			}
	
		return;
	}


	//	"at" (Non-Const)
	//	throw(std::out_of_range)
	//
	[[nodiscard]] inline reference at(size_type pos_N, size_type pos_M)
	{
		const size_type		index	= (this->m_rows * pos_M) + pos_N;
		std::out_of_range 	bad_index("Provided index is outside the range of"
									  " allocated memory for the Matrix.\n");

			// CASE 0 : Provided coordinate indices are 
			// 			out-of-range of the Matrix's Memory.
			if (!this->m_matrix || 
				pos_N >= N 		|| 	pos_M >= M)
				throw bad_index;

		return *(this->m_matrix + index);
	}


	//	"at" (Const)
	//	throw(std::out_of_range)
	//
	[[nodiscard]] inline const_reference at(size_type pos_N, 
											size_type pos_M) const
	{
		const size_type		index	= (this->m_rows * pos_M) + pos_N;
		std::out_of_range 	bad_index("Provided index is outside the range of"
									  " allocated memory for the Matrix.\n");

			// CASE 0 : Provided coordinate indices are 
			// 			out-of-range of the Matrix's Memory.
			if (!this->m_matrix || 
				pos_N >= N 		|| 	pos_M >= M)
				throw bad_index;

		return *(this->m_matrix + index);
	}


// 	Public Utility Member Functions.
//***************************************************************************//

	// 	"size"
	//
	[[nodiscard]] inline size_type size(void) const noexcept
	{ return (N * M); }


	// 	"dimensions"
	//
	[[nodiscard]] inline std::tuple<size_type, size_type> dimensions(void) 
															const noexcept
	{ return { N, M }; }


	//	"empty"
	//
	[[nodiscard]] inline bool empty(void) const noexcept
	{ return (this->m_matrix == nullptr); }


//***************************************************************************//

//	END MATRIX INLINE CLASS DEFINITION.
//***************************************************************************//

};




















namespace DEBUG
{
//	BEGIN NAMESPACE.

//***************************************************************************//
// 	Matrix Class.
//***************************************************************************//

template<typename T, class Allocator>
class matrix;


//***************************************************************************//
// 	Matrix Class-Template Primary Inline Definition.
//***************************************************************************//

//	"matrix"
//
template<typename T, typename Allocator = std::allocator<T>>
class matrix
{ 
//	"Concept" / "Requires" clauses to prevent imporper template-type arguments.
//***************************************************************************//

/*static_assert(valid_matrix_template<T, Allocator>::value,
			  "\nOne or more of the types provided to the \"matrix\" "
			  "class-template is invalid for use in the \"matrix\" class.");*/


//***************************************************************************//

	public:

// 	Class-Defined Nested Public Alias Definitions.
//***************************************************************************//

	using allocator_type	= Allocator;
	using value_type 		= T;
	using size_type			= std::size_t;
	using difference_type 	= std::ptrdiff_t;
	using pointer 			= std::allocator_traits<Allocator>::pointer;
	using const_pointer 	= std::allocator_traits<Allocator>::const_pointer;
	using reference 		= value_type &;
	using const_reference 	= const value_type &;
	using iterator 			= T *;
	/*using iterator 			= std::iterator< std::random_access_iterator_tag, 
	      					  				 value_type, difference_type,
											 pointer, reference >;*/


//***************************************************************************//

	protected:

// 	Protected Data-Members.
//***************************************************************************//

	pointer		m_matrix	= nullptr;
	//size_type	m_capacity	= 0ULL;
	size_type	m_rows		= 0ULL;
	size_type	m_columns	= 0ULL;


//***************************************************************************//

   private:

// 	Private Member Functions.
//
//***************************************************************************//

	//	"new_handler"
	//
	inline void new_handler(const char * location = nullptr) noexcept
	{
		std::cerr << "Matrix Class-Template's \"new_handler\" has been called after "
				  << "exception was thrown by \"new\" operator due to memory allocat"
				  << "ion failure in location: \"";

			if (location) 	std::cerr << location << "\"." << '\n';			
			else 			std::cerr << "UNKNOWN\"." << '\n';			

			if (this->m_matrix)
			{
					delete [] this->m_matrix;
					this->m_matrix 					= nullptr;
					this->m_rows = this->m_columns 	= 0ULL;
			}

		return;
	}


//***************************************************************************//

   protected:

// 	Protected Operation Member Functions.
//***************************************************************************//

	//	"determinant"
	//
	inline void determinant(matrix * & minors) const noexcept
	{
		const size_type		N 			= this->m_rows, 
			  				M 			= this->m_columns;
		size_type			counter 	= 0ULL;
		const_pointer		current		= this->m_matrix;
		matrix *			min_current	= minors;

		//	1. Iterate through "min" number of minor-matrices in the Matrix.
		for (size_type min = 0ULL; min < M; ++min)
		{
				//	2. Iterate through each row in the Matrix.	
				for (size_type i = 1ULL; i < N; ++i)
				{
					current = this->m_matrix + i*N;

					//	3. Iterate through each column in the "i-th" row.
					for (size_type j = 0ULL; j < M; ++j, ++current)
					{
						if (j != min)
							min_current->operator[](counter++) = (*current);
					}
				}

			++min_current;
			counter = 0ULL;
		}

		return;
	}
	

// 	Protected Utility Member Functions.
//***************************************************************************//

	// 	"display"
	//
	inline std::ostream & display(std::ostream & output) const noexcept
	{
   		char 				space 		= 0x0;
   		const size_type		M			= this->m_rows;
   		const size_type		N			= this->m_columns;
   		const_pointer 		current 	= this->m_matrix;

			// 	CASE 0 : This Matrix is EMPTY.
			if (!current)
			{
				output << "[ ]";
				return output;
			}

   		output << "[\t";
   			// 	Looping through each row in the matrix.
			for(size_type i = 0ULL; i < M; ++i)
			{
				space = char(0x09 * (i != 0ULL));
				output << char(0x0a * (i != 0ULL))
		       		   << space << "[";

   					// 	Looping through each column in the matrix.
					for(size_type j = 0ULL; j < N; ++j)
					{
						output << (*current) 
			       			   << char(0x2c * (j != (N-1ULL))) 
			       			   << char(0x09 * (j != (N-1ULL)));
						++current;
					}
				output << "]" << char(0x2c * (i != (M-1ULL)));
			}
   		output << "\t]";

   		return output;
	}


//***************************************************************************//

	public:

// 	Constructors, Destructor.
//***************************************************************************//

	//	Default Constructor.
	//
	inline matrix(void) noexcept = default;


	// 	Destructor.
	//
	inline virtual ~matrix(void) noexcept 	
	{
			if (this->m_matrix)
			{
					delete [] this->m_matrix;
					this->m_matrix = nullptr;
			}
	
		this->m_rows = this->m_columns 	= 0ULL;
	}


	//	Copy Constructor.
	// 	throw(std::bad_alloc)
	//
	inline matrix(const matrix & source)
				  : m_rows(source.m_rows), 
				  	m_columns(source.m_columns)
	{
		const size_type		src_size		= source.m_rows * source.m_columns;
		const_pointer 		src_current		= source.m_matrix;
		pointer				dst_current		= nullptr;

			// 	CASE 1 : "Source" matrix is NOT EMPTY.
			if (source.m_matrix)
			{
					try
					{
						this->m_matrix 	= new value_type [src_size];
						dst_current 	= this->m_matrix;
							for (size_type i = 0ULL; i < src_size; ++i)
								*dst_current++ = *src_current++;
					}
					catch (std::bad_alloc & error)
					{
							this->new_handler("COPY CONSTRUCTOR");
							throw;
					}
			}
	}


	//	"Move" Constructor.
	//
	inline matrix(matrix && source) noexcept
				  : m_matrix(std::move(source.m_matrix)),
					m_rows(std::move(source.m_rows)),
				  	m_columns(std::move(source.m_columns))
	{ 
		source.m_matrix 					= nullptr; 
		source.m_rows = source.m_columns 	= 0ULL;
	}


	// 	Nested Initialization List Constructor.
	// 	throw(std::invalid_argument, std::bad_alloc)
	//
	inline matrix(const std::initializer_list<std::initializer_list<T>> & list)
	{
		using 			iter_t			= std::initializer_list<std::initializer_list<T>>::iterator;
		using			error_t			= std::invalid_argument;
		bool			bad_dims		= false;
		pointer			current			= nullptr;
		iter_t			it				= list.begin();
		iter_t			end				= list.end();
		size_type		N				= list.size();
		size_type		M				= it->size();
		error_t			bad_list("ERROR\nThe dimensions of the prov"
								 "ided initializer list are invalid.\n");

			// 	CASE 0 : The initializer list is EMPTY.
			if (it == end)
				throw bad_list;	

			// 	1. Check that each row in the initializer list of is equal size.
			for (; (it != end) && (!bad_dims); ++it)
				bad_dims = (it->size() != N);

			// 	CASE 1 : The dimensions of each row in the 
			// 			 list are NOT equal (bad dimensions).
			if (bad_dims)
				throw bad_list;


			// 2. Dynamically Allocating memory for the Matix and copy-
			// 	  -ing the values from the initialization list into the Matrix.
			try
			{
				this->m_matrix 	= new value_type [M * N];
				this->m_rows 	= N;
				this->m_columns	= M;

				current 		= this->m_matrix;
				for (it = list.begin(); it != end; ++it)
				{
						for (value_type elem : (*it))
						{
							(*current) = elem;
							++current;
						}
				}
			}
			catch (std::bad_alloc & error)
			{
				this->new_handler("NESTED-INITIALIZATION LIST CONSTRUCTOR");
				throw;
			}
	}


	//	Assignment Constructor 1.
	// 	throw(std::invalid_argument, std::bad_alloc)
	//
	inline matrix(size_type N, size_type M) 
				  : m_rows(N), m_columns(M)
	{
		using 				error_t		= std::invalid_argument;
		pointer				current		= nullptr;
		const size_type		size		= N * M;
		error_t				bad_dims("ERROR\nThe provided matrix "
								 	 "dimensions are invalid.\n");

			//	CASE 0 : The provided dimensions are invalid.
			if (N == 0ULL || M == 0ULL)
				throw bad_dims;

			try
			{
				this->m_matrix 	= new value_type [N * M];
				current			= this->m_matrix;
					for (size_type i = 0ULL; i < size; ++i)
							*current++ = 0;
			}
			catch (std::bad_alloc & error)
			{
				this->new_handler("ASSIGNMENT CONSTRUCTOR");
				throw;
			}
	}


//	Overloaded Member Operators.
//***************************************************************************//

	//	Overloaded Assignment Operator = (Matrix).
	//
	//	FIX THIS:
	//		Fix this function so that, if a memory allocation error occurs 
	//		(a call to "new" throws std::bad_alloc), DON'T delete the existing
	//		memory.  
	//	INSTEAD:
	//		Return the matrix to its previous state before the assignment
	//		operation was invoked.
	//
	inline matrix & operator = (const matrix & source) 
	{
		const size_type		N				= source.m_rows,
			  				M				= source.m_columns;
		const size_type		src_size		= N * M;
		const_pointer		src_current		= source.m_matrix;
		pointer 			dst_current 	= this->m_matrix;

			// 	CASE 0 : Check for Self-Assignment 
			// 			 OR - The "Source" Matrix is EMPTY.
			if (&source == this || !src_current)
				return (*this);


			// 1. Performing a 'deep-copy' of all the data-members.
			try
			{
				this->m_matrix 	= new value_type [src_size];

					// 2. Check for any previously existing dynamically allocated
					// 	  memory prior to performing deep-copy assignment operation.
					if (dst_current)
					{
						delete [] dst_current;
						dst_current = nullptr;
					}

				this->m_rows	= N;
				this->m_columns	= M;
				dst_current 	= this->m_matrix;
					for (size_type i = 0ULL; i < src_size; ++i)
						*dst_current++ = *src_current++;
			}
			catch(std::bad_alloc & error)
			{
				this->m_matrix = dst_current;
				throw;
			}

		return (*this);
	}


	// 	Overloaded Assignment Operator = (std::initialization_list).
	//
	inline matrix & operator = (const std::initializer_list<std::initializer_list<T>> 
								& list)
	{
		using 			iter_t			= std::initializer_list<std::initializer_list<T>>::iterator;
		using			error_t			= std::invalid_argument;
		bool			bad_dims		= false;
		pointer			dst_current		= this->m_matrix;
		iter_t			it 		= list.begin(), 	end  = list.end();
		size_type		N 		= list.size(), 		M 	 = it->size();
		error_t			bad_list("ERROR\nThe dimensions of the prov"
								 "ided initializer list are invalid.\n");

			// 	CASE 0 : The initializer list is EMPTY.
			if (it == end)
			{
					if (dst_current)
					{
						delete [] this->m_matrix;
						dst_current = this->m_matrix = nullptr;
					}
				this->m_rows = this->m_columns = 0ULL;
				return (*this);
			}

			// 	1. Check that each row in the initializer list of is equal size.
			for (; (it != end) && (!bad_dims); 
				 	++it, bad_dims = (it->size() != N));

			try
			{
				this->m_matrix 	= new value_type [M * N];

					if (dst_current)
					{
						delete [] dst_current;
						dst_current = nullptr;
					}

				this->m_rows 	= N;
				this->m_columns	= M;
				dst_current 	= this->m_matrix;
					for (it = list.begin(); it != end; ++it)
					{
							for (value_type elem : (*it))
								*dst_current++ = elem;
					}
			}
			catch(std::bad_alloc & error)
			{
				this->m_matrix 	= dst_current;
				throw;
			}

		return (*this);
	}


	//	Overloaded "Move" Assignment Operator = (Matrix).
	//
	inline matrix & operator = (matrix && source) noexcept
	{
			//	CASE 0 : Check for Self-Assignment.
			if (&source == this)
				return (*this);

			//	1. Check for any preexisting dynamically-allocated 
			//	memory prior to performing the assignment operation.
			if (this->m_matrix)
			{
				delete [] this->m_matrix;
				this->m_matrix = nullptr;
			}


		// 	2. Transfering Ownership (Moving) the data-members.
		this->m_matrix 	= std::move(source.m_matrix);
		this->m_rows	= std::move(source.m_rows);
		this->m_columns	= std::move(source.m_columns);

		source.m_matrix = nullptr;
		source.m_rows 	= source.m_columns 	= 0ULL;

		return (*this);
	}


	// Overloaded Compound Assignment Operator *= (matrix and matrix).
	//
	inline matrix & operator *= (const matrix & matrix_B) noexcept 
	{
		const size_type		N			= this->m_rows,
			  				M			= this->m_columns;
   		pointer 			a_current 	= nullptr;
   		const_pointer 		b_current 	= nullptr;
   		value_type * 		products 	= nullptr;


			// CASE 0 : One or both of the matrices are EMPTY
			// 			- OR - Matrices are NOT OF VALID DIMENSIONS.
			if (!this->m_matrix 		||	!matrix_B.m_matrix		||
				N != matrix_B.m_columns ||	M != matrix_B.m_rows)
			{
				return (*this);
			}

		products = new value_type [M];
   			// Outermost For-Loop.
   			for(size_type c = 0ULL; c < M; ++c)
			{
				// 1. Iterating through each of the N-columns in matrix B.
					for(size_type i = 0ULL; i < N; ++i)
					{
						a_current 	= (this->m_matrix + (c*M));
						b_current 	= (matrix_B.m_matrix + i);

						// 2. Iterating through each element in this
						//    respective row within matrix A (*this matrix).
						products[i] = 0;
							for(size_type j = 0ULL; j < M; ++j)
							{
								products[i] += (*a_current) * (*b_current);
								++a_current;
								b_current   += N;
							}
					}

				// 3. Finally, assigning the values to the row.
				a_current = (this->m_matrix + (c*M));
					for(size_type j = 0; j < M; ++j)
					{
						(*a_current) = *(products + j);
						++a_current;
					}
			}
		delete [] products;
		products = nullptr;

   		return (*this);
	}


	//	Overloaded Subscript Operator [] (Non-Const).
	//
	inline reference operator [] (size_type pos) 
	{ return *(this->m_matrix + pos); }


	//	Overloaded Subscript Operator [] (Const).
	//
	inline const_reference operator [] (size_type pos) const
	{ return *(this->m_matrix + pos); }


	// Overloaded Function Call Operator () (Non-Const).
	//
	[[nodiscard]] inline reference operator () (size_type pos_N, size_type pos_M)
	{ return *(this->m_matrix + (this->m_rows * pos_M) + pos_N); }


	// Overloaded Function Call Operator () (Const).
	//
	[[nodiscard]] inline const_reference operator () (size_type pos_N, 
													  size_type pos_M) const
	{ return *(this->m_matrix + (this->m_rows * pos_M) + pos_N); }


// 	Overloaded Non-Member Operators.
//***************************************************************************//

	// 	Overloaded std::ostream Extraction Operator.
	//
	inline friend std::ostream & operator << (std::ostream & output, 
					  						  const matrix & arg1) noexcept
	{ return arg1.display(output); }


	// Overloaded Equality-Comparison Operator (==, Matrix).
	//
	inline friend bool operator == (const matrix & LHS, 
									const matrix & RHS) noexcept
	{
		bool				equal			= true;
		size_type			size			= 0ULL;
		const_pointer 		LHS_current 	= LHS.m_matrix,
							RHS_current 	= RHS.m_matrix;
		const size_type		LHS_N = LHS.m_rows, LHS_M = LHS.m_columns,
							RHS_N = RHS.m_rows, RHS_M = RHS.m_columns;


			// CASE 0 : Both matrices are EMPTY.
			if (!LHS.m_matrix && !RHS.m_matrix) 
					return true;

			// CASE 1 : Matrices are NOT OF EQUAL DIMENSIONS - OR -
			// 			of the two matrices is EMPTY (but not the other).
			if (LHS_N != RHS_N 		|| 	   LHS_M != RHS_M 	||
				!LHS_current	  	|| 	   !RHS_current)
					return false;

		size = LHS_N * LHS_M;
			for ( size_type i = 0ULL; 
				  i < size && (equal = (*LHS_current++ == *RHS_current++));
				  ++i);

		return equal;
	}


	// Overloaded Inequality-Comparison Operator (!=, Matrix).
	//
	inline friend bool operator != (const matrix & LHS, 
									const matrix & RHS) noexcept
	{
		bool				equal			= true;
		size_type			size			= 0ULL;
		const_pointer 		LHS_current 	= LHS.m_matrix,
							RHS_current 	= RHS.m_matrix;
		const size_type		LHS_N = LHS.m_rows, LHS_M = LHS.m_columns,
							RHS_N = RHS.m_rows, RHS_M = RHS.m_columns;

			// CASE 0 : Both matrices are EMPTY.
			if (!LHS.m_matrix && !RHS.m_matrix) 
					return false;

			// CASE 1 : Matrices are NOT OF EQUAL DIMENSIONS - OR -
			// 			of the two matrices is EMPTY (but not the other).
			if (LHS_N != RHS_N 		|| 	   LHS_M != RHS_M 	||
				!LHS_current	  	|| 	   !RHS_current)
					return true;

		size = LHS_N * LHS_M;
			for ( size_type i = 0ULL; 
				  i < size && (equal = (*LHS_current++ == *RHS_current++));
				  ++i);

		return (!equal);
	}


	// 	Overloaded Multiplication Operator (matrix and matrix).
	// 	throw(std::out_of_range).
	//
	[[nodiscard]] inline friend matrix operator * (const matrix & lhs, const matrix & rhs) 
	{
		const size_type		N			= lhs.m_rows,
			  				M			= lhs.m_columns;
		value_type			sum			= 0;
   		matrix 				product(N, M);
   		pointer				current		= product.m_matrix;


   			// Outer Loop for each element in the matrix.
   			for(size_type n = 0ULL; n < M; ++n)
			{
				const_pointer 	lhs_current 	= (lhs.m_matrix + (n*M));
   				const_pointer 	rhs_current 	= rhs.m_matrix;
		
					// Iterating across each element in this row.
   					for(size_type i = 0ULL; i < M; ++i)
					{
						sum 						= 0;
						const_pointer 	lhs_temp 	= (lhs_current);
						const_pointer	rhs_temp 	= (rhs_current + (i%M));

							// Traversing through every column in the matrix.
							for(size_type j = 0ULL; j < N; ++j)
							{
								sum 		+= (*lhs_temp) * (*rhs_temp);
								++lhs_temp;
								rhs_temp 	 = (rhs_temp + N);
							}

						(*current) = sum;
						++current;
					}
				}

		return product;
	}



// 	Public Operation Member Functions.
//***************************************************************************//

	//	"determinant"
	//	throw(std::out_of_range, std::domain_error, std::bad_alloc)
	//
	inline void determinant(void) const
	{
		const size_type			N 				= this->m_rows, 
			  					M 				= this->m_columns;
		matrix *				minors			= nullptr;
		matrix *				min_current		= nullptr;
		std::out_of_range 		empty_matrix("Cannot take determinant of empty"
											 " matrix (no allocated memory).\n");
		std::domain_error		not_square("Determinant is only valid for "
										   "SQUARE Matrices (dims N = M).\n");
		
			//	CASE 0 : This matrix is EMPTY.
			if (!this->m_matrix)
					throw empty_matrix;

			// 	CASE 1 : This matrix is not SQUARE.
			//  		 Meaning, dimensions [N x M], where N = M.
			if (N != M && N > 1)
				throw not_square;


		//	1. Allocate dynamic memory for the minor matrices.
		try
		{
			minors			= new matrix [M];
			min_current		= minors;
			
				for (size_type i = 0ULL; i < M; ++i)
					*min_current++ = matrix(N-1, M-1);
		}
		catch(std::bad_alloc & new_error)
		{
			throw;
		}

		this->determinant(minors);

		//	2. Displaying the minor matrices (DEBUG).
		min_current = minors;
			for (size_type i = 0ULL; i < M; ++i)
			{
				std::cout << "\n\nMinor \"" << i << "\":\n"
						  << *min_current++;
			}

		delete [] minors;
		minors = nullptr;

		return;
	}


	//	"swap_rows"
	//	throw(std::out_of_range)
	//
	inline void swap_rows(size_type n1, size_type n2)
	{
		const size_type		N			= this->m_rows,
			  				M			= this->m_columns;
		value_type			temp		= 0;
		pointer				n1_ptr		= this->m_matrix + n1*N;
		pointer				n2_ptr		= this->m_matrix + n2*N;
		std::out_of_range 	bad_rows("The provided indices are outside the"
									 " range of the rows in this matrix.\n");

		//	CASE 0 : The matrix is EMPTY OR the rows that have
		//			 been indicated to swap are invalid / out-of-range.
		if (!this->m_matrix	|| n1 == n2	||
				n1 >= N		|| n2 >= N)
			throw bad_rows;

		for (size_type i = 0ULL; i < M;
			 	++i, ++n1_ptr, ++n2_ptr)
		{
			temp 		= std::move(*n1_ptr);
			(*n1_ptr) 	= std::move(*n2_ptr);
			(*n2_ptr)	= temp;
		}

		return;
	}


	//	"swap_columns"
	//	throw(std::out_of_range)
	//
	inline void swap_columns(size_type m1, size_type m2)
	{
		const size_type		N			= this->m_rows,
			  				M			= this->m_columns;
		value_type			temp		= 0;
		pointer				m1_ptr		= this->m_matrix + m1;
		pointer				m2_ptr		= this->m_matrix + m2;
		std::out_of_range 	bad_cols("The provided indices are outside the "
									 "range of the columns in this matrix.\n");

			//	CASE 0 : The matrix is EMPTY OR the rows that have
			//			 been indicated to swap are invalid / out-of-range.
			if (!this->m_matrix	|| m1 == m2	||
					m1 >= M		|| m2 >= M)
				throw bad_cols;

			for (size_type i = 0ULL; i < N; 
			 		++i, m1_ptr += N, m2_ptr += N)
			{
				temp 		= std::move(*m1_ptr);
				(*m1_ptr) 	= std::move(*m2_ptr);
				(*m2_ptr)	= std::move(temp);
			}

		return;
	}

/*
	//	"set_row"
	//	throw(std::out_of_range)
	//
	inline void set_row(size_type n1)
	{
		const size_type		N			= this->m_rows,
			  				M			= this->columns;
		//auto				[N, M] 		= this->dimensions();
		value_type			temp		= 0;
		pointer				n1_ptr		= this->m_matrix + n1*N;
		pointer				n2_ptr		= this->m_matrix + n2*N;
		std::out_of_range 	bad_row("The provided indices are outside the"
									 " range of the rows in this matrix.\n");

			//	CASE 0 : The matrix is EMPTY OR the rows that have
			//			 been indicated to swap are invalid / out-of-range.
			if (!this->m_matrix	|| n1 == n2	||
					n1 >= N		|| n2 >= N)
				throw bad_rows;

			for (size_type i = 0ULL; i < M;
			 		++i, ++n1_ptr, ++n2_ptr)
			{
				temp 		= std::move(*n1_ptr);
				(*n1_ptr) 	= std::move(*n2_ptr);
				(*n2_ptr)	= temp;
			}

		return;
	}
*/


	//	"begin"
	//
	[[nodiscard]] inline iterator begin(void) noexcept
	{ return iterator(this->m_matrix); }


	//	"end"
	//
	[[nodiscard]] inline iterator end(void) noexcept
	{ return iterator(this->m_matrix + (this->m_rows * this->m_columns)); }


	//	"clear"
	//
	inline void clear(void) noexcept
	{
			if (this->m_matrix)
			{
					delete [] this->m_matrix;
					this->m_matrix = nullptr;
			}
	
		this->m_rows = this->m_columns 	= 0ULL;
		return;
	}


	//	"at" (Non-Const)
	//	throw(std::out_of_range)
	//
	[[nodiscard]] inline reference at(size_type pos_N, size_type pos_M)
	{
		const size_type		index	= (this->m_rows * pos_M) + pos_N;
		std::out_of_range 	bad_index("Provided index is outside the range of"
									  " allocated memory for the Matrix.\n");

			// CASE 0 : Provided coordinate indices are 
			// 			out-of-range of the Matrix's Memory.
			if (!this->m_matrix 		|| 
				pos_N >= this->m_rows 	|| 	pos_M >= this->m_columns)
			{
				throw bad_index;
			}

		return *(this->m_matrix + index);
	}


	//	"at" (Const)
	//	throw(std::out_of_range)
	//
	[[nodiscard]] inline const_reference at(size_type pos_N, 
											size_type pos_M) const
	{
		const size_type		index	= (this->m_rows * pos_M) + pos_N;
		std::out_of_range 	bad_index("Provided index is outside the range of"
									  " allocated memory for the Matrix.\n");

			// CASE 0 : Provided coordinate indices are 
			// 			out-of-range of the Matrix's Memory.
			if (!this->m_matrix 		|| 
				pos_N >= this->m_rows 	|| 	pos_M >= this->m_columns)
			{
				throw bad_index;
			}

		return *(this->m_matrix + index);
	}


// 	Public Utility Member Functions.
//***************************************************************************//

	// 	"size"
	//
	[[nodiscard]] inline size_type size(void) const noexcept
	{ return (this->m_rows * this->m_columns); }


	// 	"dimensions"
	//
	[[nodiscard]] inline std::tuple<size_type, size_type> dimensions(void) 
															const noexcept
	{ return { this->m_rows, this->m_columns }; }


	//	"empty"
	//
	[[nodiscard]] inline bool empty(void) const noexcept
	{ return (this->m_matrix == nullptr); }


//***************************************************************************//

//	END MATRIX INLINE CLASS DEFINITION.
//***************************************************************************//
//***************************************************************************//
};

// 	END NAMESPACE.
}






//***************************************************************************//
//***************************************************************************//
}// END NAMESPACE   "cb".



#endif /* _CBLIB_MATRIX_H */
