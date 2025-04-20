/******************************************************************************
 *	
 *	File:			"matrix_driver.cpp"
 *	
 *	Type:			INTERNAL SOURCE FILE.
 *
 *	Description:	
 *		This file is an internal SOURCE file which contains the TEST / DRIVER
 *		code needed to evaluate the performance of the library code contained
 *		within the "matrix" header.
 *
******************************************************************************/
#include "main.h"
#include <assert.h>

#ifndef _CB_LIB_MATRIX_H
# include "matrix.h"
#endif	// _CB_LIB_MATRIX_H






// 	BEGIN NAMESPACE     "cb" :: "test".
//***************************************************************************//
//***************************************************************************//
namespace cb 	{   namespace test 	{



/*****************************************************************************/
//  1.  "ndmatrix" Tests / Driver Functions...
/*****************************************************************************/

//  "test_ndmatrix"
//
void test_ndmatrix(void) noexcept
{
    using                               T               =  double;
    using                               us_t            =  unsigned short;
    //using                               matrix_t        = ndmatrix<T>;
    
    std::string                         class_name      = utl::type_name< cb::ndmatrix<T> >();
    const us_t                          width           = 20;
    const std::array< std::string, 2 >  TF_fmt          = { std::string(RED_BB) + "FAIL" + RESET + "\n",
                                                            std::string(GREEN_BB) + "PASS" + RESET + "\n" };
    std::array<us_t, 3>                 sec             = {0, 0, 0};
    std::ostringstream                  out;
    
    
    //  LAMBDAS...
    auto group_header = [&](const char * title) mutable {
        out << WHITE_BB     << INVERSE              << ++sec[0]     << ".\t"    << title    << "..."
            << RESET        << cb::utl::println     << RESET        << std::endl;
    };
    auto group_footer = [&](void) { out << std::endl; }; //out << BLUE_BOLD << UNDERLINE << cb::utl::printrule << RESET << std::endl;  };
    
    //  Print Tests...
    auto print_test_impl = [&](const std::string & name, const bool result) {
        out << "\t"         << YELLOW_BB    << std::setw(width)
            << std::right   << name         << RESET
            << "\t: "       << (result ? TF_fmt[1] : TF_fmt[0]);
    };
    //
    auto print_test_pair = [&](const std::pair<std::string, bool> & test)
    {  const auto [name, result] = test; return print_test_impl(name, result);  };
    
    //  Print Sub-Section...
    auto print_ss_impl = [&sec, &out](const char * title, const char * extra) mutable
    { out << extra << CYAN_BB << sec[0] << "." << ++sec[1] << ".\t" << UNDERLINE << title << "..." << RESET << std::endl; };
    
    //  Print Sub-Sub-Section...
    auto print_sss_impl = [&sec, &out](const std::string & title, const char * extra) mutable
    { out << extra << "\t" << WHITE_BB << sec[0] << "." << sec[1] << "." << ++sec[2] << ".\t" << title << "." << RESET << "\n"; };
    
    
    
    std::function<void(const std::string &, const bool)>        print_test      = print_test_impl;
    std::function<void(const std::pair<std::string, bool> &)>   print_test_p    = print_test_pair;
    std::function<void(const char *, const char *)>             print_ss        = print_ss_impl;
    std::function<void(const std::string &, const char *)>      print_sss       = print_sss_impl;


    //  0.  HEADER FOR THE UNIT-TESTS...
    std::cout << WHITE_BRIGHT       << DIM          << "UNIT TESTS FOR CLASS \""    << RED_BRIGHT
              << UNDERLINE          << class_name   << RESET                        << WHITE_BRIGHT
              << DIM                << "\"..."      << cb::utl::println             << cb::utl::println
              << RESET              << "\n\n";
              
     
    //  1.   INVOKING "AUDIT" OF CLASS...
    {
        //auditor<matrix_t>   my_audit;
    }
    
    
    //  2.  INITIALIZATION METHODS...
    {
        group_header("INITIALIZATION METHODS");
        test_ndmatrix_initializations(print_test_p, print_ss, print_sss);
        group_footer();
    }
    
    
    //  3.  ARITHMETIC OPERATORS...
    {
        group_header("ARITHMETIC OPERATORS");
        test_ndmatrix_arithmetic(print_test_p, print_ss, print_sss);
        group_footer();
    }
    
    
    //  4.  MATRIX OPERATIONS...
    {
        group_header("MATRIX OPERATIONS");
        test_ndmatrix_operations(out, print_test_p, print_ss, print_sss);
        group_footer();
    }
                    
              
    //  5.  Display test information and the footer.
    std::cout << out.str()  << std::endl
              << DIM        << cb::utl::println << cb::utl::println
              << RESET      << std::endl;
              
    return;
}




//  "test_ndmatrix_initializations"
//
void test_ndmatrix_initializations( std::function<void(const std::pair<std::string, bool> &)>                   print_test,
                                    [[maybe_unused]] std::function<void(const char *, const char *)>            print_ss,
                                    [[maybe_unused]] std::function<void(const std::string &, const char *)>     print_sss ) noexcept
{
    using                               T           = double;
    using                               matrix_t    = ndmatrix<T>;
    
    
    {//  SUB-GROUP 1.    (3 x 3)  MATRICES...
        const matrix_t                  A1              = {  {1,   2,  3},      { 4,  5,  6},       {  7,   8,   9}  };
        const matrix_t                  B1              = {  {1,   2,  3},      { 4,  5,  6},       {  7,   8,   9}  };
        
        {
            //          1.3.                Display Results.
            print_sss("Misc.", "");
            print_test( test::copy_constructor(A1) );
            print_test( test::move_constructor(A1) );
            print_test( test::copy_assignment(A1) );
            print_test( test::move_assignment(A1) );
        }
    }
    
    return;
}


//  "test_ndmatrix_arithmetic"
//
void test_ndmatrix_arithmetic(  std::function<void(const std::pair<std::string, bool> &)>                   print_test,
                                [[maybe_unused]] std::function<void(const char *, const char *)>            print_ss,
                                [[maybe_unused]] std::function<void(const std::string &, const char *)>     print_sss ) noexcept
{
    using                               T               = double;
    using                               matrix_t        = ndmatrix<T>;
    
    
    {//  SUB-GROUP 1.    (3 x 3)  MATRICES...
        std::string                     MSG_1           = "";
        const matrix_t                  A1              = {  {1,   2,  3},      { 4,  5,  6},       {  7,   8,   9}  };
        const matrix_t                  B1              = {  {1,   2,  3},      { 4,  5,  6},       {  7,   8,   9}  };
        const matrix_t                  A1_PLUS_B1      = {  {2,   4,  6},      { 8, 10, 12},       { 14,  16,  18}  };
        const matrix_t                  A1_MINUS_B1     = {  {0,   0,  0},      { 0,  0,  0},       {  0,   0,   0}  };
        const matrix_t                  A1_TIMES_B1     = {  {30, 36, 42},      {66, 81, 96},       {102, 126, 150}  };
        const matrix_t                  A1_TIMES_S1     = {  {.5,  1,  1.5},    { 2,  2.5,  3},     {  3.5,   4,   4.5} };
                      
                      
        print_ss("(3 x 3) Matrices", "");
        
        {//     TESTS...
            print_sss("Non-Member Operators", "");
            print_test( test::identity(A1) );                           //  1.1.1.     A1 == A1.
            print_test( test::a_plus_b(A1, B1, A1_PLUS_B1) );           //  1.1.2.     A1 + B1.
            print_test( test::a_minus_b(A1, B1, A1_MINUS_B1) );         //  1.1.3.     A1 - B1.
            print_test( test::a_times_b(A1, B1, A1_TIMES_B1) );         //  1.1.4.     A1 * B1.
            
            print_sss("Member Operators", "");
            print_test( test::a_plus_equal_b(A1, B1, A1_PLUS_B1) );     //  1.2.1.     A1 += B1.
            print_test( test::a_times_equal_b(A1, B1, A1_TIMES_B1) );   //  1.2.2.     A1 *= B1.
        }
    }
        
    return;
}
    


//  "test_ndmatrix_operations"
//
void test_ndmatrix_operations(  std::ostringstream &                                                        out,
                                std::function<void(const std::pair<std::string, bool> &)>                   print_test,
                                [[maybe_unused]] std::function<void(const char *, const char *)>            print_ss,
                                [[maybe_unused]] std::function<void(const std::string &, const char *)>     print_sss ) noexcept
{
    using                               T           = double;
    using                               matrix_t    = ndmatrix<T>;
    using                               test_t      = ndmatrix_test<T>;
    
    
    //  EXAMPLE MATRICES...
    std::vector<test_t> examples = {
        //  (2 x 2) Identity
        test_t(
            std::string("(2 x 2) Identity"),
            matrix_t({ { 1, 0 }, { 0, 1 } }),		// original
            matrix_t({ { 1, 0 }, { 0, 1 } }),		// cofactor
            matrix_t({ { 1, 0 }, { 0, 1 } }),		// adjoint
            matrix_t({ { 1, 0 }, { 0, 1 } }),		// transpose
            matrix_t({ { 1, 0 }, { 0, 1 } }),		// inverse
            T(1),					                // determinant
            std::vector<matrix_t>{			        // minors
                matrix_t({ { 1 } }), 	matrix_t({ { 0 } }), 	matrix_t({ { 0 } }),
                matrix_t({ { 1 } })
            }
        ),
        
        //	(2 x 2) Test #1
        test_t(
            std::string("(2 x 2) Test #1"),
            matrix_t({ { 4, 7 }, { 2, 6 } }),
            matrix_t({ { 6, -2 }, { -7, 4 } }),
            matrix_t({ { 6, -7 }, { -2, 4 } }),
            matrix_t({ { 4, 2 }, { 7, 6 } }),
            matrix_t({ { 0.60, -0.70 }, { -0.20, 0.40 } }),
            T(10),
            std::vector<matrix_t>{
                matrix_t({ { 6 } }), 	matrix_t({ { 7 } }), 	matrix_t({ { 2 } }),
                matrix_t({ { 4 } })
            }
        ),
        
        //  (3 x 3) Identity
        test_t(
            std::string("(3 x 3) Identity"),
            matrix_t({ { 1, 0, 0 }, { 0, 1, 0 }, { 0, 0, 1 } }),		// original
            matrix_t({ { 1, 0, 0 }, { 0, 1, 0 }, { 0, 0, 1 } }),		// cofactor
            matrix_t({ { 1, 0, 0 }, { 0, 1, 0 }, { 0, 0, 1 } }),		// adjoint
            matrix_t({ { 1, 0, 0 }, { 0, 1, 0 }, { 0, 0, 1 } }),		// transpose
            matrix_t({ { 1, 0, 0 }, { 0, 1, 0 }, { 0, 0, 1 } }),		// inverse
            T(1),					                                    // determinant
            std::vector<matrix_t>{			                            // minors
                matrix_t({ { 1, 0 }, { 0, 1 } }), 	matrix_t({ { 0, 0 }, { 0, 1 } }), 	matrix_t({ { 0, 0 }, { 1, 0 } }),
                matrix_t({ { 0, 0 }, { 0, 1 } }), 	matrix_t({ { 1, 0 }, { 0, 1 } }), 	matrix_t({ { 1, 0 }, { 0, 0 } }),
                matrix_t({ { 0, 1 }, { 0, 0 } }), 	matrix_t({ { 1, 0 }, { 0, 0 } }), 	matrix_t({ { 1, 0 }, { 0, 1 } })
            }
        ),
        
        //	YouTube Matrix
        test_t(
            std::string("YouTube Matrix"),
            matrix_t({ { 0, 2, 3 }, { -1, 4, 0 }, { 5, 0, -2 } }),
            matrix_t({ { -8, -2, -20 }, { 4, -15, 10 }, { -12, -3, 2 } }),
            matrix_t({ { -8, 4, -12 }, { -2, -15, -3 }, { -20, 10, 2 } }),
            matrix_t({ { 0, -1, 5 }, { 2, 4, 0 }, { 3, 0, -2 } }),
            matrix_t({ { 0.12, -0.06, 0.19 }, { 0.03, 0.23, 0.05 }, { 0.31, -0.16, -0.03 } }),
            T(-64),
            std::vector<matrix_t>{
                matrix_t({ { 4, 0 }, { 0, -2 } }), 	    matrix_t({ { 2, 3 }, { 0, -2 } }), 	matrix_t({ { 2, 3 }, { 4, 0 } }),
                matrix_t({ { -1, 0 }, { 5, -2 } }), 	matrix_t({ { 0, 3 }, { 5, -2 } }), 	matrix_t({ { 0, 3 }, { -1, 0 } }),
                matrix_t({ { -1, 4 }, { 5, 0 } }), 	    matrix_t({ { 0, 2 }, { 5, 0 } }), 	matrix_t({ { 0, 2 }, { -1, 4 } })
            }
        ),
        
        //	(3 x 3) Test #1
        test_t(
            std::string("(3 x 3) Test #1"),
            matrix_t({ { 0, 0, 1 }, { 2, -1, 3 }, { 1, 1, 4 } }),
            matrix_t({ { -7, -5, 3 }, { 1, -1, 0 }, { 1, 2, 0 } }),
            matrix_t({ { -7, 1, 1 }, { -5, -1, 2 }, { 3, 0, 0 } }),
            matrix_t({ { 0, 2, 1 }, { 0, -1, 1 }, { 1, 3, 4 } }),
            matrix_t({ { -2.333333333333333f, 0.333333333333333f, 0.333333333333333f }, { -1.666666666666667f, -0.333333333333333f, 0.666666666666667f }, { 1, 0, 0 } }),
            T(3),
            std::vector<matrix_t>{
                matrix_t({ { -1, 3 }, { 1, 4 } }), 	matrix_t({ { 0, 1 }, { 1, 4 } }), 	matrix_t({ { 0, 1 }, { -1, 3 } }),
                matrix_t({ { 2, 3 }, { 1, 4 } }), 	matrix_t({ { 0, 1 }, { 1, 4 } }), 	matrix_t({ { 0, 1 }, { 2, 3 } }),
                matrix_t({ { 2, -1 }, { 1, 1 } }), 	matrix_t({ { 0, 0 }, { 1, 1 } }), 	matrix_t({ { 0, 0 }, { 2, -1 } })
            }
        ),
    
    };

    
    //std::function<void(const std::string &, const bool)> display = print_test;
    for (auto example : examples)
    {
        example.test(out, print_test, print_sss);
    }
    
    return;
}














//***************************************************************************//
//***************************************************************************//
} }// END NAMESPACE   "cb" :: "test".
