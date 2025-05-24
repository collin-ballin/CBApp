/******************************************************************************
 *	
 *	File:			"cb_ndmatrix.h"
 *
 *	Type:			INTERNAL HEADER FILE.
 *
 *	Description:	
 *		This file is an INTERNAL header file - meaning this header is int-
 *	-ended to be included and used by other LIBRARY header files.  DO NOT
 *	include this header file directly.  Instead, include the associated 
 *	LIBRARY header file which utilizes this header file, "ndmatrix.h".
 *
******************************************************************************/
//#ifndef _CBLIB_MATRIX_H
//#define _CBLIB_MATRIX_H 1


//#ifndef	_CBLIB_ARRAY_H
//# include "../../array/array.h"
//#endif	//  _CBLIB_ARRAY_H  //


#include <iostream>
#include <stdexcept>
#if __cplusplus >= 201103L
# include <initializer_list>
#endif	// C++11.
#include <utility>
#include <cstring>
#include <cmath>//      <======| std::pow
#include <iomanip>//    <======| std::fill,  std::setw,  std::left,  std::fixed, ...
#include <sstream>//    <======| std::ostringstream



// 	BEGIN NAMESPACE     "cblib".
// *************************************************************************** //
// *************************************************************************** //
namespace cblib    { // _GLIBCXX_VISIBILITY(default)  //


// *************************************************************************** //
// *************************************************************************** //
// 				   PRIMARY TEMPLATE DECLARATION:
// 		Dynamic-Size Matrix Class Template Definition
// *************************************************************************** //
// *************************************************************************** //

//	"ndmatrix"
//
template< typename 		T,
		  typename 		Allocator 	= std::allocator<T> >
class ndmatrix
{
// *************************************************************************** //
// *************************************************************************** //
//
//
//
// 	1.1.    CLASS-DEFINED, NESTED PUBLIC ALIAS DEFINITIONS...
// *************************************************************************** //
public:
	using 	allocator_type		= Allocator;
	using	Alloc				= allocator_type;
	using 	value_type 			= T;
	using 	size_type			= std::size_t;
	using 	difference_type 	= std::ptrdiff_t;
	using 	pointer 			= std::allocator_traits<Allocator>::pointer;
	using 	const_pointer 		= std::allocator_traits<Allocator>::const_pointer;
	using 	reference 			= value_type &;
	using 	const_reference 	= const value_type &;
    using   iterator            = typename std::vector<T, Allocator>::iterator;
    using   const_iterator      = typename std::vector<T, Allocator>::const_iterator;
    
    using   row_t               = std::vector< T >;
    using   matrix_t            = std::vector< row_t >;


// *************************************************************************** //
//
//
//
// 	1.2.    "CONCEPTS" / "REQUIRES" CLAUSES FOR TEMPLATE-TYPE ARGUMENTS...
// *************************************************************************** //
#ifdef _GLIBCXX_CONCEPT_CHECKS
# if __cplusplus >= 201103L
	//	Value-Type.
	static_assert(is_matrix_type<T>::value, 
			  	  "cb::ndmatrix value-type requirements not met.\n");

	//	Allocator-Type.
	static_assert(is_matrix_allocator<T, Allocator>::value,
			  	  "cb::ndmatrix allocator-type requirements not met.\n");

# endif	// C++ 2011,03L.
#endif 	// _GLIBCXX_CONCEPT_CHECKS.


// *************************************************************************** //
//
//
//
// 	1.3.    PROTECTED DATA-MEMBERS...
// *************************************************************************** //
protected:
    matrix_t                m_data;
    size_type               m_R             = 0ULL;
    size_type               m_C             = 0ULL;


// *************************************************************************** //
//
//
//
// 	1.4.    CONSTRUCTORS, DESTRUCTOR, INITIALIZATION FUNCTIONS...
// *************************************************************************** //
public:

	//	Default Constructor.
	inline ndmatrix(void)   = default;


	//	Copy Constructor.
	inline ndmatrix(const ndmatrix & src)
                    : m_data(src.m_data), m_R(src.m_R), m_C(src.m_C) {}


	//	Move Constructor.
	inline ndmatrix(ndmatrix && src)
        : m_data(std::move(src.m_data)), m_R(std::move(src.m_R)), m_C(std::move(src.m_C)) {}


    // 	"Nested Initialization List"        | Parametric Constructor.
	// 	throw(std::invalid_argument, std::bad_alloc)
	//
	inline ndmatrix(const std::initializer_list<std::initializer_list<T>> & list)
	{
		using 	        iter_t	        = std::initializer_list<std::initializer_list<T>>::iterator;
        using 	        const_iter_t    = std::initializer_list<std::initializer_list<T>>::const_iterator;
		using	        error_t	        = std::invalid_argument;
		iter_t	        src_it		    = list.begin();
		const_iter_t    src_end	        = list.end();
        bool            bad_dims	    = false;
                        this->m_R       = list.size();
                        this->m_C       = src_it->size();
        
        for (; (src_it != src_end) && (!bad_dims); ++src_it)// 	    CASE 1.1 : INVALID LIST DIMENSIONS.
            bad_dims = (src_it->size() != this->m_C);
        //
        if (bad_dims)
            throw error_t("Invalid Initializer-List Dimensions.\nEach row of the "
                          "initializer list must have the same number of columns.");
                          
        this->m_data.reserve(this->m_R);
        for (src_it=list.begin(); (src_it != src_end) && (!bad_dims); ++src_it)
            this->m_data.emplace_back((*src_it));
	}
 
    
    // 	1D "size_type"                      | Parametric Constructor.
	//
	inline ndmatrix(const size_type N) : m_R(N), m_C(N)   { this->resize(N, N); }
 
    
    // 	2D "size_type"                      | Parametric Constructor.
	//
	inline ndmatrix(const size_type R, const size_type C)
                    : m_R(R), m_C(C)   { this->resize(R, C); }
 
    
    // 	"pair"                              | Parametric Constructor.
	//
	inline ndmatrix(const std::pair<size_type, size_type> & shape)
                    : m_R(shape.first), m_C(shape.second)   { this->resize(shape.first, shape.second); }
    
    
    // 	"std::vector"                       | Parametric Constructor.
	//
	inline explicit ndmatrix(const std::vector< std::vector<T> > & src)
                             : m_data(src)
    {
        this->m_R   = this->m_data.size();
        this->m_C   = (this->m_R) ? this->m_data[0].size() : 0;
    }


	// 	Destructor.
	//
	inline virtual ~ndmatrix(void) noexcept     { this->clear(); }


	// 	Nested Initialization List Constructor.
	// 	throw(std::invalid_argument, std::bad_alloc)
	//
	//inline ndmatrix(const std::initializer_list<std::initializer_list<T>> & list)
 
 
// *************************************************************************** //
//
//
//
// 	2.1.    PRIVATE "UTILITY" FUNCTIONS...
// *************************************************************************** //
private:

    //  "resize"
    //
    inline void resize(const size_type R, const size_type C)
    {
        if ( (R < 1) || (C < 1) )
            throw std::invalid_argument("Matrix dimensions must be greater-than (0, 0).");
        
        this->m_R = R;  this->m_C = C;
        this->m_data.resize(R);
        for (auto & row : this->m_data)
            row.resize(C);
            
        return;
    }
    
    
	//	"new_handler"
    //
	inline void new_handler(const char * location = nullptr) noexcept
	{
		std::cerr << "Matrix Class-Template's \"new_handler\" has been called after "
				  << "exception was thrown by \"new\" operator due to memory allocat"
				  << "ion failure in location: \""
                  << (location ? location : "UNKNOWN")
                  << "\".\n";
		return;
	}


// *************************************************************************** //
//
//
//
// 	3.1.    PROTECTED "OPERATION" FUNCTIONS...
// *************************************************************************** //
protected:

    //  "det_minor"
    //
    [[nodiscard]] inline value_type det_minor(const size_type i, const size_type j) const
    {
        std::array< std::array<T, 2>, 2>    minor;
        size_type                           N       = this->m_data.size() - 1;
        size_type                           r       = ( (i+1) % this->m_R );
        size_type                           c       = ( (j+1) % this->m_C );
        size_type                           idx = 0,    jdx = 0,    shift = 0;
        auto kronecker_delta = [](auto i_, auto j_)   { return (i_ == j_) ? 1 : 0; };
        
        if (this->m_R != this-> m_C)//  CASE 0  :   NON-SQUARE MATRIX.
            throw std::invalid_argument("Cannot compute minor of a non-square matrix.");
        if (this->m_R < 0)//            CASE 1  :   INVALID MATRIX.
            throw utl::not_implemented("Cannot use \"det_minor\" helper function for matrices larger-than 2 x 2.");
        
        
        for (idx=0; idx < N; ++idx) {
            
            for (jdx=0; jdx < N; ++jdx) {
                shift                   = static_cast<size_type>( -1 * (j < c) );
                minor[idx][c+shift]     = this->m_data[r][c];
                c                       = ( c + 1 + kronecker_delta((c+1)%this->m_R, j) ) % this->m_R;
            }
            
            r   = ( r + 1 + kronecker_delta((r+1)%this->m_C, i) ) % this->m_C;
            c   = ( (j+1) % this->m_C );
        }
        
        return (minor[0][0]*minor[1][1]) - (minor[0][1]*minor[1][0]);
    }


// *************************************************************************** //
//
//
//
// 	3.2.    PROTECTED "UTILITY" FUNCTIONS...
// *************************************************************************** //

	// 	"display"
    //
	inline std::ostream & display(std::ostream & output) const noexcept
    {
        char                space   = 0x0;
        int                 width   = 10,       precision   = 2;
        size_type           r       = 0,        c           = 0;
        auto format = [&output, width, precision](const T value) {
            output << std::scientific << std::setprecision(precision)
                   << std::setw(width) << std::internal << value;
        };

        //     CASE 0 : This Matrix is EMPTY.
        if (this->m_data.empty()) {
            output << "[ ]";
            return output;
        }
        output << "[\t";
        //     Looping through each row in the ndmatrix.
        for(r=0ULL; r < this->m_R; ++r)
        {
            space = char(0x09 * (r != 0ULL));
            output << char(0x0a * (r != 0ULL))
                   << space << "[";

            //     Looping through each column in the ndmatrix.
            for(c=0ULL; c < this->m_C; ++c)
            {
                format(this->m_data[r][c]);
                output << char(0x2c * (c != (this->m_C-1ULL)))
                       << char(0x20 * (c != (this->m_C-1ULL)))
                       << char(0x20 * (c != (this->m_C-1ULL)));
            }
            output << "]" << char(0x2c * (r != (this->m_R-1ULL)));
        }
        output << "\t]";

        return output;
    }
    


// *************************************************************************** //
//
//
//
// 	4.1.    OVERLOADED MEMBER OPERATORS...
// *************************************************************************** //
public:

    //  Conversion Operator #1.
    //
    inline operator matrix_t () const
    { return this->m_data; }
    
	//	Overloaded Assignment Operator = (Matrix).
    //
	inline ndmatrix & operator = (const ndmatrix & source)
    {
        //	CASE 0 : Check for Self-Assignment.
		if (&source == this)    return (*this);
  
        this->m_R       = source.m_R;
        this->m_C       = source.m_C;
        this->m_data    = source.m_data;
            
        return (*this);
    }


	// 	Overloaded Assignment Operator = (std::initialization_list).
	//	throw(std::invalid_argument)
	//
	//inline ndmatrix & operator = (const std::initializer_list<std::initializer_list<T>> & list)


	//	Overloaded "Move" Assignment Operator = (Matrix).
	//
	inline ndmatrix & operator = (ndmatrix && source) noexcept
    {
        //	CASE 0 : Check for Self-Assignment.
		if (&source == this)    return (*this);
  
        this->m_R       = std::move(source.m_R);
        this->m_C       = std::move(source.m_C);
        this->m_data    = std::move(source.m_data);
            
        return (*this);
    }


	// 	Overloaded Compound Assignment Operator += (ndmatrix and ndmatrix).
	//	throw(std::invalid_argument)
	inline ndmatrix & operator += (const ndmatrix & B)
    {
        auto                [RA, CA]    = this->shape();
        const auto          [RB, CB]    = B.shape();
        size_type           i = 0ULL, j = 0ULL;
        
        if (  (RA != RB) || (CA != CB) || this->empty() || B.empty()  )//   CASE 1 :    INVALID MATRIX DIMENSIONS.
            throw std::invalid_argument("Can only add non-empty matrices of equal dimensions.");
        
        for (i=0ULL; i < RA; ++i)
        {
            for (j=0ULL; j < CB; ++j) {
                this->m_data[i][j] += B[i][j];
            }
        }
        
        return (*this);
    }


	// 	Overloaded Compound Assignment Operator -= (ndmatrix and ndmatrix).
	//	throw(std::invalid_argument)
	inline ndmatrix & operator -= (const ndmatrix & B)
    {
        auto                [RA, CA]    = this->shape();
        const auto          [RB, CB]    = B.shape();
        size_type           i = 0ULL, j = 0ULL;
        
        if (  (RA != RB) || (CA != CB) || this->empty() || B.empty()  )//   CASE 1 :    INVALID MATRIX DIMENSIONS.
            throw std::invalid_argument("Can only subtract non-empty matrices of equal dimensions.");
        
        for (i=0ULL; i < RA; ++i)
        {
            for (j=0ULL; j < CB; ++j) {
                this->m_data[i][j] -= B[i][j];
            }
        }
        
        return (*this);
    }


	// 	Overloaded Compound Assignment Operator *= (ndmatrix and ndmatrix).
	//	throw(std::invalid_argument)
	inline ndmatrix & operator *= (const ndmatrix & B)
    {
        auto                [RA, CA]    = this->shape();
        const auto          [RB, CB]    = B.shape();
        const matrix_t  &   A           = this->m_data;
        matrix_t            C;
        size_type           i = 0ULL, j = 0ULL, k = 0ULL;
        
        if ( (CA != RB)  &&  ( !A.empty() && !B.empty() ) )//   CASE 1 :    INVALID MATRIX DIMENSIONS.
            throw std::invalid_argument("Matrix multiplication only defined for dimensions \"(M, N) x (N, P)\".");
           
           
        this->m_C = (CA != CB) ? CB: CA;
        
        C.resize(RA);
        for (i=0ULL; i < RA; ++i)
        {
            C[i].resize(CB);
            for (j=0ULL; j < CB; ++j) {
                C[i][j] = 0;
                
                for (k=0ULL; k < CA; ++k)
                    C[i][j] += A[i][k] * B[k][j];
            }
        }
        
        this->m_data = C;
        
        return (*this);
    }
    
    
    // 	Overloaded Compound Assignment Operator *= (ndmatrix and T).
	//	throw(std::invalid_argument)
	inline ndmatrix & operator *= (const T & B)
    {
        const auto          [RA, CA]    = this->shape();
        size_type           i = 0ULL, j = 0ULL;
        
        if ( this->empty() )//   CASE 1 :    INVALID MATRIX DIMENSIONS.
            throw std::invalid_argument("Can only multiply non-empty matrices by a scalar.");
        
        for (i=0ULL; i < RA; ++i)
        {
            for (j=0ULL; j < CA; ++j)
                this->m_data[i][j] *= B;
        }
        
        return (*this);
    }
    
    
    // 	Overloaded Compound Assignment Operator /= (ndmatrix and ndmatrix).
	//	throw(std::invalid_argument)
	inline ndmatrix & operator /= (const ndmatrix & B)  = delete;
    
    
    // 	Overloaded Compound Assignment Operator %= (ndmatrix and ndmatrix).
	//	throw(std::invalid_argument)
	inline ndmatrix & operator %= (const ndmatrix & B)  = delete;


    //	Overloaded Subscript Operator [] (Non-Const).
    inline row_t & operator [] (const size_type pos)
    { return m_data[pos]; }


    //	Overloaded Subscript Operator [] (Const).
    inline const row_t & operator [] (const size_type pos) const
    { return m_data[pos]; }


	// 	Overloaded Function Call Operator () (Non-Const).
	//[[nodiscard]] inline reference operator () (size_type pos_N, size_type pos_M)
	//{ return *(this->m_data + (this->m_R * pos_M) + pos_N); }


	// 	Overloaded Function Call Operator () (Const).
	//[[nodiscard]] inline const_reference operator () (size_type pos_N,
	//												  size_type pos_M) const
	//{ return *(this->m_data + (this->m_R * pos_M) + pos_N); }


// *************************************************************************** //
//
//
//
// 	4.2.    OVERLOADED NON-MEMBER OPERATORS...
// *************************************************************************** //

	// 	Overloaded std::ostream Extraction Operator.
	//
	inline friend std::ostream & operator << (std::ostream & output, 
					  						  const ndmatrix & arg1) noexcept
	{ return arg1.display(output); }


	// 	Overloaded Equality-Comparison Operator (==, Matrix).
	//
	inline friend bool operator == (const ndmatrix & lhs, const ndmatrix & rhs)
    {
        const auto          dims        = lhs.shape();
        const size_type     R           = dims.first,       C           = dims.second;
        size_type           r           = 0ULL,             c           = 0ULL;
        bool                good_dims   = true,             match       = (dims == rhs.shape());
        
        if (!match)//  CASE 1 :    MATRICES HAVE DIFFERENT DIMENSIONS.
            throw std::invalid_argument("Cannot perform equality-comparison for matrices of different dimensions.");
        
        //  Iterate thru matrices and perform element-wise comparison...
        while ( match && good_dims && (r < R) )
        {
            good_dims               = ( lhs.m_data[r].size() == rhs.m_data[r].size() );
            const_pointer   ptr1    = lhs.m_data[r].data();
            const_pointer   ptr2    = rhs.m_data[r].data();
            
            for (c=0; (c < C) && (match && good_dims); match=is_close(*ptr1++, *ptr2++), ++c);
            ++r;
        }
        
        return (match && good_dims);
    }


	//	Overloaded Addition Operator (ndmatrix and ndmatrix).
	//	throw(std::invalid_argument)
	//
	[[nodiscard]] inline friend ndmatrix operator + (const ndmatrix & A, const ndmatrix & B)
    {
        auto                [RA, CA]    = A.shape();
        const auto          [RB, CB]    = B.shape();
        ndmatrix            C(RA, CA);
        size_type           i = 0ULL, j = 0ULL;
        
        if (  (RA != RB) || (CA != CB) || A.empty() || B.empty()  )//   CASE 1 :    INVALID MATRIX DIMENSIONS.
            throw std::invalid_argument("Can only add non-empty matrices of equal dimensions.");
        
        for (i=0ULL; i < RA; ++i)
        {
            for (j=0ULL; j < CB; ++j)
                C[i][j] = A[i][j] + B[i][j];
        }
        
        return C;
    }


	//	Overloaded Subtraction Operator (ndmatrix and ndmatrix).
	//	throw(std::invalid_argument)
	//
	[[nodiscard]] inline friend ndmatrix operator - (const ndmatrix & A, const ndmatrix & B)
    {
        auto                [RA, CA]    = A.shape();
        const auto          [RB, CB]    = B.shape();
        ndmatrix            C(RA, CA);
        size_type           i = 0ULL, j = 0ULL;
        
        if (  (RA != RB) || (CA != CB) || A.empty() || B.empty()  )//   CASE 1 :    INVALID MATRIX DIMENSIONS.
            throw std::invalid_argument("Can only subtract non-empty matrices of equal dimensions.");
        
        for (i=0ULL; i < RA; ++i)
        {
            for (j=0ULL; j < CB; ++j)
                C[i][j] = A[i][j] - B[i][j];
        }
        
        return C;
    }


	// 	Overloaded Multiplication Operator (ndmatrix and ndmatrix).
	// 	throw(std::invalid_argument)
	//
	[[nodiscard]] inline friend ndmatrix operator * (const ndmatrix & A, const ndmatrix & B)
    {
        const auto      [RA, CA]    = A.shape();
        const auto      [RB, CB]    = B.shape();
        ndmatrix        C(RA, CB);
        size_type       i = 0ULL, j = 0ULL, k = 0ULL;
        
        if ( (CA != RB)  &&  ( !A.empty() && !B.empty() ) )//   CASE 1 :    INVALID MATRIX DIMENSIONS.
            throw std::invalid_argument("Matrix multiplication only defined for dimensions \"(M, N) x (N, P)\".");
            
        
        for (i=0ULL; i < RA; ++i)
        {
            for (j=0ULL; j < CB; ++j) {
                C[i][j] = 0;
                
                for (k=0ULL; k < CA; ++k)
                    C[i][j] += A[i][k] * B[k][j];
            }
        }
        
        return C;
    }


	// 	Overloaded Multiplication Operator (ndmatrix and T).
	// 	throw(std::invalid_argument)
	//
	[[nodiscard]] inline friend ndmatrix operator * (const ndmatrix & A, const T & S)
    {
        auto                [RA, CA]    = A.shape();
        ndmatrix            C(RA, CA);
        size_type           i = 0ULL, j = 0ULL;
        
        if ( A.empty() )//   CASE 1 :    INVALID MATRIX DIMENSIONS.
            throw std::invalid_argument("Can only multiply non-empty matrices by a scalar.");
        
        for (i=0ULL; i < RA; ++i)
        {
            for (j=0ULL; j < CA; ++j)
                C[i][j] = A[i][j] * S;
        }
        
        return C;
    }
    
    // 	Overloaded Multiplication Operator (ndmatrix and T).
	// 	throw(std::invalid_argument)
	//
	[[nodiscard]] inline friend ndmatrix operator * (const T & S, const ndmatrix & A)
    {
        auto                [RA, CA]    = A.shape();
        ndmatrix            C(RA, CA);
        size_type           i = 0ULL, j = 0ULL;
        
        if ( A.empty() )//   CASE 1 :    INVALID MATRIX DIMENSIONS.
            throw std::invalid_argument("Can only multiply non-empty matrices by a scalar.");
        
        for (i=0ULL; i < RA; ++i)
        {
            for (j=0ULL; j < CA; ++j)
                C[i][j] = A[i][j] * S;
        }
        
        return C;
    }


	// 	Overloaded Multiplication Operator (ndmatrix and ndmatrix).
	// 	throw(std::invalid_argument)
	//
	[[nodiscard]] inline friend ndmatrix operator / (const ndmatrix & A, const ndmatrix & B)    = delete;


	// 	Overloaded Multiplication Operator (ndmatrix and ndmatrix).
	// 	throw(std::invalid_argument)
	//
	[[nodiscard]] inline friend ndmatrix operator % (const ndmatrix & A, const ndmatrix & B)    = delete;


// *************************************************************************** //
//
//
//
// 	5.1.    PUBLIC "OPERATION" FUNCTIONS...
// *************************************************************************** //
public:

    //  "det"
    //
    [[nodiscard]] inline value_type det(void) const
    {
        T           entry       = 0;
        T           det         = 0;
        T           sign        = 0;
        size_type   r = 0,    c = 0;
        
        if (this->m_R != this-> m_C)//  CASE 0  :   NON-SQUARE MATRIX.
            throw std::invalid_argument("Cannot take the determinant of a non-square matrix.");
            
        
        switch (this->m_R)
        {
            case 0  : {//                   CASE 1  :   EMPTY MATRIX.
                throw std::invalid_argument("Cannot take the determinant of an empty matrix.");
            }
            case 1  : {//                   CASE 2  :   (1 x 1) MATRIX.
                det = this->m_data[0][0];
                break;
            }
            case 2  : {//                   CASE 3  :   (2 x 2) MATRIX.
                det = (this->m_data[0][0] * this->m_data[1][1]) - (this->m_data[0][1] * this->m_data[1][0]);
                break;
            }
            default : {//                   DEFAULT  : Compute the determinant...
                for (c=0; c < this->m_C; ++c) {
                    entry               = this->m_data[r][c];
                    sign                = (c % 2) == 0 ? 1.0f : -1.0f;
                    ndmatrix    minor   = this->minor(r,c);
                    det                += sign * entry * minor.det();
                }
                break;
            }
        }
            
        return det;
    }
    
    
    //  "minor"
    //
    [[nodiscard]] inline ndmatrix minor(const size_type i, const size_type j) const
    {
        if (this->m_R != this->m_C)
            throw std::invalid_argument("Cannot compute minor of a non-square matrix.");

        size_type N = this->m_R;  // parent matrix dimension
        // The minor matrix has dimensions (N-1) x (N-1)
        std::vector<std::vector<T>> minorMat(N - 1, std::vector<T>(N - 1));

        for (size_type k = 0; k < N - 1; ++k) {
            // Compute the corresponding row index in the parent matrix.
            // Add 1 if we've passed the removed row.
            size_type parentRow = k + (k >= i ? 1 : 0);
            for (size_type l = 0; l < N - 1; ++l) {
                // Compute the corresponding column index in the parent matrix.
                size_type parentCol = l + (l >= j ? 1 : 0);
                minorMat[k][l] = this->m_data[parentRow][parentCol];
            }
        }
        return ndmatrix(minorMat);
    }

    
    //  "cof"
    //
    [[nodiscard]] inline ndmatrix cof(void) const
    {
        //ndmatrix<T>     cofactor(this->m_data);
        std::vector< std::vector<T> >   cofactor;
        //T   value   = 0;
        int sign    = 1;
        
        if (this->m_R != this-> m_C)//  CASE 0  :   NON-SQUARE MATRIX.
            throw std::invalid_argument("Cannot take the determinant of a non-square matrix.");
        if (this->empty())//            CASE 1  :             EMPTY MATRIX.
            throw std::invalid_argument("Cannot take the determinant of an empty matrix.");
        
        cofactor.resize(this->m_R);
        for (size_type r=0ULL; r < this->m_R; ++r)
        {
            cofactor[r].resize(this->m_C);
            for (size_type c=0ULL; c < this->m_C; ++c)
            {
                //sign            = ((r + c) % 2 == 0) ? 1 : -1;
                sign            = (int)(std::pow( -1.0f, (r+c) ));
                ndmatrix minor  = this->minor(r, c);
                cofactor[r][c]  = sign * minor.det();
            }
        }
        
        return ndmatrix(cofactor);
    }
    
    
    //  "adj"
    //
    [[nodiscard]] inline ndmatrix adj(void) const
    {
        return (this->cof().transpose());
    }
    
    
    //  "transpose"
    //
    [[nodiscard]] inline ndmatrix transpose(void) const
    {
        auto        [R, C]  = this->shape();
        matrix_t    AT;
        
        if (this->empty())//            CASE 1  :       EMPTY MATRIX.
            throw std::invalid_argument("Cannot take the determinant of an empty matrix.");
        if (R == C && C == 1)//         CASE 2  :       (1 x 1) Matrix.
            return ndmatrix(this->m_data);
            
            
        AT.resize(C);
        for (size_type r=0ULL; r < C; ++r)
        {
            AT[r].resize(R);
            for (size_type c=0ULL; c < R; ++c) {
                AT[r][c] = this->m_data[c][r];
            }
        }
            
        return ndmatrix(AT);
    }
    
    
    //  "inv"
    //
    [[nodiscard]] inline ndmatrix inv(void) const
    {
        const value_type    det         = this->det();
        
        if ( utl::is_close(det, 0.0f) )
            throw std::invalid_argument("Cannot compute the inverse of a matrix with a determinant of zero.");
            
        ndmatrix            adjoint     = this->adj();
        
        return adjoint * (1/det);
    }


// *************************************************************************** //
//
//
//
// 	5.2.    PUBLIC "UTILITY" FUNCTIONS...
// *************************************************************************** //

    //	"begin"
	//
	[[nodiscard]] inline iterator begin(void) noexcept
	{ return this->m_data.begin(); }


    //	"cbegin"
	//
	[[nodiscard]] inline iterator cbegin(void) noexcept
	{ return this->m_data.cbegin(); }


	//	"end"
	//
	[[nodiscard]] inline iterator end(void) noexcept
	{ return this->m_data.end(); }


	//	"cend"
	//
	[[nodiscard]] inline iterator cend(void) noexcept
	{ return this->m_data.cend(); }
 

    //  "for_each_element"
    //
    template <typename Func>
    inline void for_each_element(Func && func)
    {
        for (auto & row : this->m_data) {
            for (auto & elem : row)
                func(elem);
        }
        
        return;
    }


	//	"clear"
	//
	inline void clear(void) noexcept
	{
        this->m_data.clear();
        this->m_R = this->m_C = 0ULL;
		return;
	}
 
 
    //	"data"
	//
	[[nodiscard]] inline std::vector<std::vector<T>> & data(void) noexcept
	{ return this->m_data; }
 
 
    //	"to_string"
    //
	[[nodiscard]] inline std::string to_string(void) const noexcept
	{
		std::ostringstream 		oss;
		this->display(oss);
		return oss.str();
	}
    
    
	// 	"size"
	//
	[[nodiscard]] inline size_type size(void) const noexcept
	{ return (this->m_R * this->m_C); }


    // 	"dimensions"
	//
	[[nodiscard]] inline std::tuple<size_type, size_type> dimensions(void)
                                                            const noexcept
	{ return { this->m_R, this->m_C }; }
    
    
	// 	"shape"
	//
	[[nodiscard]] inline std::pair<size_type, size_type> shape(void)
                                                      const noexcept
	{ return std::make_pair(this->m_R, this->m_C); }


	//	"empty"
	//
	[[nodiscard]] inline bool empty(void) const noexcept
	{ return this->m_data.empty(); }


//	END MATRIX INLINE CLASS DEFINITION.
// *************************************************************************** //
// *************************************************************************** //
};






// *************************************************************************** //
// *************************************************************************** //
}// END NAMESPACE   "cblib".



//#endif /* _CBLIB_MATRIX_H  //
