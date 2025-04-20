/******************************************************************************
 *	
 *	File:			"matrix_driver.h"
 *	
 *	Type:			INTERNAL HEADER FILE.
 *
 *	Description:	
 *		This file is an internal HEADER file which contains the inline, 
 *		templated functions - as well as the non-templated function prototypes
 *		that are used for the matrix TEST / DRIVER code.  This test/driver 
 *		code is used to evaluate the performance of the library code contained
 *		within the "matrix" header.
 *
******************************************************************************/
#ifndef	_CB_MATRIX_DRIVER_H
#define	_CB_MATRIX_DRIVER_H	1

#include <sstream>//        <======| std::ostringstream
#include <functional>//     <======| std::function



// 	BEGIN NAMESPACE     "cb" :: "test".
//***************************************************************************//
//***************************************************************************//
namespace cb 	{   namespace test 	{



/*****************************************************************************/
//  1.  "ndmatrix" Tests / Driver Functions...
/*****************************************************************************/

void            test_ndmatrix                       (void)  noexcept;
void            test_ndmatrix_initializations       ( std::function<void(const std::pair<std::string, bool> &)>                 print_test,
                                                      [[maybe_unused]] std::function<void(const char *, const char *)>          print_ss,
                                                      [[maybe_unused]] std::function<void(const std::string &, const char *)>   print_sss ) noexcept;
void            test_ndmatrix_arithmetic            ( std::function<void(const std::pair<std::string, bool> &)>                 print_test,
                                                      [[maybe_unused]] std::function<void(const char *, const char *)>          print_ss,
                                                      [[maybe_unused]] std::function<void(const std::string &, const char *)>   print_sss ) noexcept;
void            test_ndmatrix_operations            ( std::ostringstream &                                                      out,
                                                      std::function<void(const std::pair<std::string, bool> &)>                 print_test,
                                                      [[maybe_unused]] std::function<void(const char *, const char *)>          print_ss,
                                                      [[maybe_unused]] std::function<void(const std::string &, const char *)>   print_sss ) noexcept;
                                                      
                                                      
//***************************************************************************//
//
//
//
//      3.1.    INITIALIZATION METHOD TESTS...
//***************************************************************************//

//  "copy_constructor"
template<typename T>
inline std::pair<std::string, bool> copy_constructor(const T & A)
{
    const T     A_copy(A);
    return std::pair("copy constructor", (A_copy == A) );
}

//  "move_constructor"
template<typename T>
inline std::pair<std::string, bool> move_constructor(const T & A)
{
    T       A_temp(A);
    T       A_move(std::move(A_temp));
    return std::pair("move constructor", (A_move == A) && (A_temp.empty()) );
}

//  "copy_assignment"
template<typename T>
inline std::pair<std::string, bool> copy_assignment(const T & A)
{
    T       A_copy;
            A_copy = A;
    return std::pair("copy assignment", (A_copy == A) );
}

//  "move_assignment"
template<typename T>
inline std::pair<std::string, bool> move_assignment(const T & A)
{
    T       A_temp(A);
    T       A_move;
            A_move = std::move(A_temp);
    return std::pair("move assignment", (A_move == A) && (A_temp.empty()) );
}


//***************************************************************************//
//
//
//
//      3.2.    ARITHMETIC OPERATOR TESTS...
//***************************************************************************//

//  "identity"
template<typename T>
inline std::pair<std::string, bool> identity(const T & arg)
{ return std::pair("identity", arg == arg); }


//  "a_plus_b"
template<typename T>
inline std::pair<std::string, bool> a_plus_b(const T & A, const T & B, const T & ref)
{ return std::pair("A + B", (A + B) == ref); }


//  "a_minus_b"
template<typename T>
inline std::pair<std::string, bool> a_minus_b(const T & A, const T & B, const T & ref)
{ return std::pair("A - B", (A - B) == ref); }

//  "a_times_b"
template<typename T>
inline std::pair<std::string, bool> a_times_b(const T & A, const T & B, const T & ref)
{ return std::pair("A * B", (A * B) == ref); }

//  "a_divide_b"
template<typename T>
inline std::pair<std::string, bool> a_divide_b(const T & A, const T & B, const T & ref)
{ return std::pair("A / B", (A / B) == ref); }

//  "a_mod_b"
template<typename T>
inline std::pair<std::string, bool> a_mod_b(const T & A, const T & B, const T & ref)
{ return std::pair("A % B", (A % B) == ref); }


//***************************************************************************//
//
//
//
//      3.3.    COMPOUND ASSIGNMENT OPERATOR TESTS...
//***************************************************************************//

//  "a_plus_equal_b"
template<typename T>
inline std::pair<std::string, bool> a_plus_equal_b(const T & A, const T & B, const T & ref)
{
    T   a(A);       a += B;
    return std::pair("A += B", a == ref);
}

//  "a_minus_equal_b"
template<typename T>
inline std::pair<std::string, bool> a_minus_equal_b(const T & A, const T & B, const T & ref)
{
    T   a(A);       a -= B;
    return std::pair("A -= B", a == ref);
}

//  "a_times_equal_b"
template<typename T>
inline std::pair<std::string, bool> a_times_equal_b(const T & A, const T & B, const T & ref)
{
    T   a(A);       a *= B;
    return std::pair("A *= B", a == ref);
}

//  "a_divide_equal_b"
template<typename T>
inline std::pair<std::string, bool> a_divide_equal_b(const T & A, const T & B, const T & ref)
{
    T   a(A);       a /= B;
    return std::pair("A /= B", a == ref);
}

//  "a_mod_equal_b"
template<typename T>
inline std::pair<std::string, bool> a_mod_equal_b(const T & A, const T & B, const T & ref)
{
    T   a(A);       a %= B;
    return std::pair("A %= B", a == ref);
}


/*****************************************************************************/
/*****************************************************************************/






/*****************************************************************************/
//	4.  SPECIFIC TESTING APPARATUS FOR "NDMATRIX"...
/*****************************************************************************/

template<typename T>
struct ndmatrix_test {
//  Aliases...
    using   mtx_t       = ndmatrix<T>;
    using   const_ref   = const mtx_t &;
    using   us_t        = unsigned short;
    using   size_type   = std::size_t;
    
    
//  Data-Members...
    std::string             name;
    mtx_t                   matrix;
    mtx_t                   cofactor;
    mtx_t                   adjoint;
    mtx_t                   transpose;
    mtx_t                   inverse;
    T                       determinant;
    std::vector< mtx_t >    minors;
    
    
//  Default / Deleted Member Functions...
    inline              ndmatrix_test    (void)  noexcept     = default;
    inline virtual      ~ndmatrix_test   (void)  noexcept     = default;
    
    
//  Member Functions...
    //  Parametric Constructor.
    inline ndmatrix_test(const std::string & label,     const_ref mtx,      const_ref cof,
                         const_ref adj,                 const_ref trans,    const_ref inv,
                                                        const T & det,      const std::vector< mtx_t > & minor)
        : name(label),      matrix(mtx),  cofactor(cof),        adjoint(adj),
          transpose(trans), inverse(inv), determinant(det),     minors(minor) {}
        
        
        
    //  "test"
    inline void test(std::ostringstream &                                       out,
                     std::function<void(const std::pair<std::string, bool> &)>  print_test,
                     std::function<void(const std::string &, const char *)>     print_sss) const
    {
        [[maybe_unused]] const us_t     width       = 25U;
        const us_t                      NT          = 6U;
        us_t                            score       = 0U;
        const mtx_t &                   mtx         = this->matrix;
        std::string                     grade       = std::string("SCORE : ") + UNDERLINE;
        std::string                     MSG         = "";
        std::array<bool, NT>            results;
        
        
        print_sss(this->name, "");
        
        
        results[0]  = (mtx.cof()        == this->cofactor);
        results[1]  = (mtx.adj()        == this->adjoint);
        results[2]  = (mtx.transpose()  == this->transpose);
        
        results[3]  = (mtx.inv()        == this->inverse);
        
        results[4]  = (mtx.det()        == this->determinant);
        results[5]  = (this->test_minors());
        
        
        
        
        
        
        
        for (us_t i=0U; i < NT; ++i)    score += results[i];
            
            
        print_test( std::make_pair("cofactor",      results[0]) );
        if (!results[0])
        {
            MSG += std::string("\n") + RED + "MY ANSWER:\n" + mtx.cof().to_string();
            MSG += std::string("\n") + GREEN + "EXAMPLE:\n" + this->cofactor.to_string();
            out << utl::hanging_indent(MSG, 2) << std::endl;
        }    
        print_test( std::make_pair("adjoint",       results[1]) );
        print_test( std::make_pair("transpose",     results[2]) );
        print_test( std::make_pair("inverse",       results[3]) );
        if (!results[3])
        {
            MSG = std::string("\n") + RED + "MY INVERSE:\n" + mtx.inv().to_string();
            MSG += std::string("\n") + GREEN + "EXAMPLE INVERSE:\n" + this->inverse.to_string();
            out << utl::hanging_indent(MSG, 3) << std::endl;
        }
        print_test( std::make_pair("determinant",   results[4]) );
        print_test( std::make_pair("minors",        results[5]) );
        if (!results[5])
            this->display_minors(out);
        
        
        switch (score) {
            case (NT) : {
                grade = std::string(GREEN_BB) + grade + std::to_string(score)
                        + " / " + std::to_string(NT) + "." + RESET + "\n";
                break;
            }
            
            case (NT-1) :
            case (NT-2) : {
                grade = std::string(YELLOW_BB) + grade + std::to_string(score)
                        + " / " + std::to_string(NT) + "." + RESET + "\n";
                break;
            }
            
            default : {
                grade = std::string(RED_BB) + grade + BLINK + std::to_string(score)
                        + " / " + std::to_string(NT) + "." + RESET + "\n";
                break;
            }
        }
        out << "\t" << grade << RESET << std::endl;
    
        return;
    }
    
    
    
    //  "test_minors"
    [[nodiscard]] inline bool test_minors(void) const noexcept
    {
        const auto      [R, C]          = this->matrix.shape();
        bool            good_value      = true;
        
        for (std::size_t r=0ULL; (r < R) && good_value; ++r) {
            for (std::size_t c=0ULL; (c < C) && good_value; ++c)
                good_value  = (this->matrix.minor(r,c) == this->minors[r + C*c]);
        }
        
        return good_value;
    }
    
    //  "display_minors"
    inline void display_minors(std::ostringstream & out) const noexcept
    {
        const auto          [R, C]          = this->matrix.shape();
        bool                good_value      = true;
        
        for (std::size_t r=0ULL; (r < R); ++r)
        {
            for (std::size_t c=0ULL; (c < C); ++c)
            {
                const mtx_t     minor   = this->matrix.minor(r,c);
                const mtx_t     ref     = this->minors[ r + C*c ];
                good_value              = (minor == ref);
                
                out << WHITE_BB << UNDERLINE << "Minor (" << r << ", " << c << "):\n" << RESET;
                out << ((good_value) ? GREEN_BRIGHT : RED_BRIGHT);
                out << minor << RESET << std::endl;
                out << WHITE_BRIGHT << ref << RESET << "\n" << std::endl;
            }
        }
        
        return;
    }
    
};




//***************************************************************************//
//
//
//
//      4.1.    TESTING FUNCTIONS...
//***************************************************************************//
//***************************************************************************//

//  "cofactor"
//
template<typename T>
inline std::pair<std::string, bool> cofactor(const T & A, const T & ref)
{
    T   a(A);
    return std::pair("cofactor", a.cof() == ref);
}





//***************************************************************************//
//***************************************************************************//
} }// END NAMESPACE   "cb" :: "test".






/*****************************************************************************/
/*****************************************************************************/
#endif	/* _CB_MATRIX_DRIVER_H */
