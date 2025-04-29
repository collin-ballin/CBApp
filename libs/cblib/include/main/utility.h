#ifndef _CBLIB_UTILITY_H
#define _CBLIB_UTILITY_H	1

#include <iostream>
#include <iomanip>
#ifndef _MSC_VER
#	include <cxxabi.h>
#endif	//   _MSC_VER   //
#include <stdexcept>//      <======| To inherit from std::exception types.
#include <string>//         <======| Need to implement custom exceptions.


// 	Pre-Processor 'Function-Esque' Macros / for Utility Usage.
// *************************************************************************** //


#if defined(_GLIBCXX_HAVE_ATTRIBUTE_VISIBILITY) && !defined(_GLIBCXX_VISIBILITY)
# define _GLIBCXX_VISIBILITY(V) __attribute__ ((__visibility__ (#V)))
//#else
//# define _GLIBCXX_VISIBILITY(V)
#endif  //  _GLIBCXX_HAVE_ATTRIBUTE_VISIBILITY  //


// 	Pre-Processor Macros / Global Constants for Utility Usage.
// *************************************************************************** //
#define 	DEF_OSTREAM_FILL_CHAR	' '
#define 	DEF_PRINTLN_CHAR 		0x2a 	
#define 	DEF_PRINTLN_WIDTH 		75


// 	Pre-Processor Macros / Global Constants for Output Text Colors.
// *************************************************************************** //
//  Regular Typeface Colors (30-37).
#define 	BLACK 					"\033[30m"
#define 	RED 					"\033[31m"
#define 	BLUE 					"\033[32m"
#define 	GREEN					"\033[34m"
#define 	YELLOW					"\033[35m"
#define 	MAGENTA 				"\033[33m"
#define 	CYAN 					"\033[36m"
#define 	WHITE					"\033[37m"

//  Bold Typeface Colors (1 + 30-37).
#define 	BLACK_BOLD 				"\033[1m\033[30m"
#define 	RED_BOLD				"\033[1m\033[31m"
#define 	BLUE_BOLD				"\033[1m\033[32m"
#define 	GREEN_BOLD				"\033[1m\033[34m"
#define 	YELLOW_BOLD				"\033[1m\033[35m"
#define 	MAGENTA_BOLD			"\033[1m\033[33m"
#define 	CYAN_BOLD				"\033[1m\033[36m"
#define 	WHITE_BOLD				"\033[1m\033[37m"

//  Bright Typeface Colors (90-97).
#define     BLACK_BRIGHT            "\033[90m"
#define     RED_BRIGHT              "\033[91m"
#define     GREEN_BRIGHT            "\033[92m"
#define     YELLOW_BRIGHT           "\033[93m"
#define     BLUE_BRIGHT             "\033[94m"
#define     MAGENTA_BRIGHT          "\033[95m"
#define     CYAN_BRIGHT             "\033[96m"
#define     WHITE_BRIGHT            "\033[97m"

//  Bold Bright Colors (1 + 90-97).
#define     BLACK_BB                "\033[1m\033[90m"
#define     RED_BB                  "\033[1m\033[91m"
#define     GREEN_BB                "\033[1m\033[92m"
#define     YELLOW_BB               "\033[1m\033[93m"
#define     BLUE_BB                 "\033[1m\033[94m"
#define     MAGENTA_BB              "\033[1m\033[95m"
#define     CYAN_BB                 "\033[1m\033[96m"
#define     WHITE_BB                "\033[1m\033[97m"

//  Typeface Styles and Misc Commands.
#define 	RESET					"\033[0m"
#define     BOLD                    "\033[1m"
#define     DIM                     "\033[2m"
#define     ITALIC                  "\033[3m"
#define     UNDERLINE               "\033[4m"
#define     BLINK                   "\033[5m"
#define     INVERSE                 "\033[7m"
#define     HIDDEN                  "\033[8m"
#define     STRIKETHROUGH           "\033[9m"


// *************************************************************************** //
// 	END PRE-PROCESSOR MACROS.






// 	BEGIN NAMESPACE     "cblib" :: "utl".
// *************************************************************************** //
// *************************************************************************** //
namespace cblib {   namespace utl {

// *************************************************************************** //
// 	1.  Global, Namespace-Defined Funtion Prototypes.
// *************************************************************************** //

//      1.1.        EXCEPTIONS, TERMINATION, ERROR-HANDLER FUNCTIONS...
void                terminate_handler       (void);
void                new_handler             (void);
void                normal_program_exit     (void);

//      1.2.        MATHEMATICAL / HELPER FUNCTIONS...
bool                is_close                (const double   a,              const double b,
                                             const double   abs_tol=1e-9,   const double rel_tol=0.0f);

//      1.3.        LOGGING, TEXT-PARSING, FORMATTING FUNCTIONS...
void 			    log_err					(const std::string & log);
std::size_t		    log					    (const std::string & log,   const char type='L',
                                                                        std::ostream & out=std::cout);
std::string 	    hanging_indent 			(const std::string & input, const std::size_t level=1ULL);
void                find_and_replace        (std::string & str, const std::string & oldStr,
                                             const std::string & newStr);

//      1.4.        UTILITY / TESTING FUNCTIONS...
std::ostream &      print_line              (char fill=DEF_PRINTLN_CHAR,
                                             unsigned short	width=DEF_PRINTLN_WIDTH,
                                             std::ostream & 	output=std::cout)   noexcept;
void                test_output_colors      (const char * text = "Hello, World!")   noexcept;
void                print_style_test        (const char * text) noexcept;
void                print_color_test        (const char *  color_name,      const char *    color_code,
                                             const char *  bright_code,     const char *    bold_code,
                                             const char *  bb_code,         const char *    text
                                            ) noexcept;


//	"type_name"
//
template<typename T>
inline std::string type_name(void)
{
	typedef typename std::remove_reference<T>::type	TR;
	std::unique_ptr<char, void(*)(void *)> own
		(
#ifndef _MSC_VER		 
			abi::__cxa_demangle(typeid(TR).name(), 
								nullptr, nullptr, nullptr),
#else 
			nullptr, 
#endif
			std::free
		);

	std::string r = own != nullptr ? own.get() : typeid(TR).name();

	if (std::is_const<TR>::value)
		r += " const"; 
	if (std::is_volatile<TR>::value)
		r += " volatile"; 
	if (std::is_lvalue_reference<TR>::value)
		r += " &"; 
	else if (std::is_rvalue_reference<TR>::value)
		r += " &&"; 

	return r;
}
//
// *************************************************************************** //



// *************************************************************************** //
// 	2.  STRUCTS & CLASSES.
// *************************************************************************** //

//  "not_implemented"
class not_implemented : public std::logic_error {
    public:
        explicit not_implemented(const std::string & msg="ERROR.  This feature has not been implemented");
};



//	"printline"
struct printline
{
//  1.  DATA-MEMBERS...
    char                m_fill      = DEF_PRINTLN_CHAR;
    unsigned short      m_width     = DEF_PRINTLN_WIDTH;
    
    
// 	2.  DELETED MEMBER-FUNCTIONS & OVERLOADED OPERATORS...
    inline printline(void)                      noexcept                = default;
	inline ~printline(void) 					noexcept 			    = default;
	inline printline(const printline & source) 	noexcept 			    = delete;
	inline printline(printline && source) 		noexcept 			    = delete;
	inline printline & operator = (const printline & source) noexcept   = delete;
	inline printline & operator = (printline && source) 	 noexcept   = delete;


//  3.  MEMBER FUNCTIONS AND OPERATORS...
    //  Parametric Constructor 1.
    inline printline(const char fill) noexcept  : m_fill(fill) {}
                     
    //  Parametric Constructor 2.
    inline printline(const char fill, const unsigned short width) noexcept
                     : m_fill(fill), m_width(width) {}
    
	//	"display"
	inline std::ostream & display(std::ostream & out) const noexcept
	{ return print_line(this->m_fill, this->m_width, out); }

	// 	Overloaded std::ostream Extraction Operator (<<)
	inline friend std::ostream & operator << (std::ostream & output, 
									   		  const printline & arg1) noexcept
	{ return arg1.display(output); }
};



// *************************************************************************** //
// 	Global Constants & Constant-Objects.
// *************************************************************************** //
[[maybe_unused]] static const 	printline	println;
[[maybe_unused]] static const 	printline	printrule('-');



// *************************************************************************** //
// *************************************************************************** //
}   }// 	END NAMESPACE   "cblib" :: "utl".






// *************************************************************************** //
// *************************************************************************** //
#endif	//  _CBLIB_UTILITY_H  //
