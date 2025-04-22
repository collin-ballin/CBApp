/******************************************************************************
 *	File:			"utility.cpp"
 *	
******************************************************************************/
#include "cblib.h"



// 	BEGIN NAMESPACE     "cblib" :: "utl".
// *************************************************************************** //
// *************************************************************************** //
namespace cblib {   namespace utl {

//      1.1.        EXCEPTIONS, TERMINATION, ERROR-HANDLER FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

// 	"terminate_handler"
void terminate_handler(void)
{
   	std::cerr << "\n\n" 
	     	  << std::setw(75) << std::setfill('/') << "\n"
   	     	  << std::setw(75) << std::setfill('*') << "\n"
	     	  << std::setfill(' ');

   	std::cerr << "\nCRITICAL ERROR:\nAn uncaught exception has been thrown, "
	     	  << "causing unexcepted program termination.\n";

   	std::cerr << "\n" 
	     	  << std::setw(75) << std::setfill('*') << "\n"
   	     	  << std::setw(75) << std::setfill('/') << "\n"
	     	  << std::setfill(' ') << "\n";
   	std::abort();

	return;
}


// 	"new_handler"
void new_handler(void)
{
   	utl::print_line();
   	std::cerr << "\nCRITICAL ERROR:\nMemory Allocation error has occured, "
	     	  << "causing unexcepted program termination.\n\n";
   	std::set_new_handler(nullptr);
   	exit(EXIT_FAILURE);

   	return;
}


// 	"normal_program_exit"
void normal_program_exit(void)
{
   	utl::print_line();
   	std::cout << "\nProgram has successfully terminated "
			  << "(Program Exit: Success).\n\n";

   	return;
}


// *************************************************************************** //
//
//
//
//      1.2.        MATHEMATICAL / HELPER FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

//	"is_close"
bool is_close(const double a, const double b, const double abs_tol, const double rel_tol) {
	double 		diff 		= std::fabs(a - b);
	double		max_diff	= std::max( rel_tol * std::max(std::fabs(a), std::fabs(b)), abs_tol );
	return diff <= max_diff;
}






// *************************************************************************** //
//
//
//
//      1.3.        LOGGING, TEXT-PARSING, FORMATTING FUNCTIONS...
// *************************************************************************** //

//	"log_err"
void log_err(const std::string & log)
{ std::cerr << RED << log << WHITE_BOLD << "\nProgram terminating." << std::endl << RESET; }


//	"log"
std::size_t log(const std::string & log, const char type, std::ostream & out) {
	static std::size_t	s_count 	= 1ULL;
#ifdef _CBLIB_LOG	/*	_CBLIB_LOG	*/
	std::string			log_fmt		= cb::utl::hanging_indent(log);
	std::ios::fmtflags	flags		= out.flags();
	std::streamsize		fmt			= out.precision();
	constexpr int		DIGITS		= 2;
	const char			tag1 []		= "LOG [";
	const char			tag2 []		= "]:\t";

	switch (type) {
		case 'w':
		case 'W': {//	1.	WARNING.
			out << YELLOW_BOLD 	<< tag1 << std::setw(DIGITS) << std::setfill('0')
				<< s_count++ 	<< tag2 << YELLOW;
			break;
		}

		case 'e':
		case 'E': {//	2.	ERROR.
			out << RED_BOLD 	<< tag1 << std::setw(DIGITS) << std::setfill('0')
				<< s_count++ 	<< tag2 << RED;
			break;
		}

		case 'n':
		case 'N': {//	3.	NONE.
			out << WHITE_BOLD 	<< tag1 << std::setw(DIGITS) << std::setfill('0')
				<< s_count++ 	<< tag2 << RESET;
		}

		case '!': {//	4.	HIGHLIGHT.
			out << BLUE_BOLD 	<< tag1 << std::setw(DIGITS) << std::setfill('0')
				<< s_count++ 	<< tag2 << BLUE;
			break;
		}
		
		default: 
		case 'l':
		case 'L': {//	5.	LOG  OR  Default Color.
			out << GREEN_BOLD 	<< tag1 << std::setw(DIGITS) << std::setfill('0')
				<< s_count++ 	<< tag2 << GREEN;
			break;
		}
	}
	out << log_fmt << RESET << std::endl;
	out.flags(flags);//			Restore default output format/precision.
	out.precision(fmt);

# else			/*	_CBLIB_LOG	*/
	++s_count;
# endif 		/*	_CBLIB_LOG	*/
	return s_count;
}


//	"utl::hanging_indent"
std::string hanging_indent(const std::string & input, const std::size_t level) {
	std::string		out			= input;
	std::string		target		= "\n";
	std::string		sub			= "\n";
	std::size_t		pos			= out.find(target);

	for (std::size_t i=0ULL; i < level; ++i)
		sub += "\t";

	while (pos != std::string::npos) {
		out.replace(pos, target.length(), sub);
		pos	= out.find( target, pos + sub.length() );
	}

	return out;
}


// *************************************************************************** //
//
//
//
//      1.4.        UTILITY / TESTING FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

// 	"utl::print_line"
std::ostream & print_line(char fill, unsigned short width,
						  std::ostream & output) noexcept
{
   	output << "\n"  << std::setfill(fill)                   << std::setw(width)
   	       << ""    << std::setfill(DEF_OSTREAM_FILL_CHAR)  << std::flush;

   	return output;
}


//  "test_output_colors"
void test_output_colors(const char * text) noexcept
{
    print_color_test("BLACK",   BLACK,      BLACK_BRIGHT,   BLACK_BOLD,     BLACK_BB,      text);
    print_color_test("RED",     RED,        RED_BRIGHT,     RED_BOLD,       RED_BB,        text);
    print_color_test("GREEN",   GREEN,      GREEN_BRIGHT,   GREEN_BOLD,     GREEN_BB,      text);
    print_color_test("YELLOW",  YELLOW,     YELLOW_BRIGHT,  YELLOW_BOLD,    YELLOW_BB,     text);
    print_color_test("BLUE",    BLUE,       BLUE_BRIGHT,    BLUE_BOLD,      BLUE_BB,       text);
    print_color_test("MAGENTA", MAGENTA,    MAGENTA_BRIGHT, MAGENTA_BOLD,   MAGENTA_BB,    text);
    print_color_test("CYAN",    CYAN,       CYAN_BRIGHT,    CYAN_BOLD,      CYAN_BB,       text);
    print_color_test("WHITE",   WHITE,      WHITE_BRIGHT,   WHITE_BOLD,     WHITE_BB,      text);
    
    print_style_test(text);
    return;
}


//  "print_style_test"
void print_style_test(const char * text) noexcept
{
    const int       width           = static_cast<int>( strnlen(text, strnlen("MAGENTA_BRIGHT", 99) ) + 4 );
    const char      tab             = 0x9;
    const char      nl              = 0xA;
    std::string     s1              = "BOLD";
    std::string     s2              = "DIM";
    std::string     s3              = "ITALIC";
    std::string     s4              = "UNDERLINE";
    std::string     s5              = "BLINK";
    std::string     s6              = "INVERSE";
    std::string     s7              = "HIDDEN";
    std::string     s8              = "STRIKETHROUGH";
    

    std::cout << "\nStyle:\t\t"
              << BOLD           << std::left    << std::setw(width)     <<  s1      << RESET   << tab
              << DIM            << std::left    << std::setw(width)     <<  s2      << RESET   << tab
              << ITALIC         << std::left    << std::setw(width)     <<  s3      << RESET   << tab
              << UNDERLINE      << std::left    << std::setw(width)     <<  s4      << RESET   << tab
              << RESET << nl;
      
      
    std::cout << "\nStyle:\t\t"
              << BLINK          << std::left    << std::setw(width)     <<  s5      << RESET   << tab
              << INVERSE        << std::left    << std::setw(width)     <<  s6      << RESET   << tab
              << HIDDEN         << std::left    << std::setw(width)     <<  s7      << RESET   << tab
              << STRIKETHROUGH  << std::left    << std::setw(width)     <<  s8
              << RESET << nl;
              
    return;
}


//  "print_color_test"
void print_color_test(const char *  color_name,         const char *    color_code,
                      const char *  bright_code,        const char *    bold_code,
                      const char *  bb_code,            const char *    text) noexcept
{
    const int       width           = static_cast<int>( strnlen(text, strnlen("MAGENTA_BRIGHT", 99) ) + 4 );
    const char      tab             = 0x9;
    const char      nl              = 0xA;
    std::string     name            = color_name;
    std::string     bright_name     = name + "_BRIGHT";
    std::string     bold_name       = name + "_BOLD";
    std::string     bb_name         = name + "_BB";
    std::string     inv_name        = bb_name + " INVERSE";
    std::string     und_name        = bb_name + " UNDERLINE";
    

    std::cout << "\nColor:\t\t"
              << color_code     << std::left    << std::setw(width)     << name             << RESET    << tab
              << bright_code    << std::left    << std::setw(width)     << bright_name      << RESET    << tab
              << bold_code      << std::left    << std::setw(width)     << bold_name        << RESET    << tab
              << bb_code        << std::left    << std::setw(width)     << bb_name          << RESET    << tab;
    std::cout << RESET          << bb_code
              << INVERSE        << std::left    << std::setw(width)     << inv_name         << RESET    << tab
              << RESET          << bb_code
              << UNDERLINE      << std::left    << std::setw(width)     << und_name         << RESET    << tab
              << RESET << nl;
              
    std::cout << nl << tab << tab
              << color_code     << std::left    << std::setw(width)     << text             << RESET    << tab
              << bright_code    << std::left    << std::setw(width)     << text             << RESET    << tab
              << bold_code      << std::left    << std::setw(width)     << text             << RESET    << tab
              << bb_code        << std::left    << std::setw(width)     << text             << RESET    << tab;
    std::cout << RESET          << bb_code
              << INVERSE        << std::left    << std::setw(width)     << text             << RESET    << tab
              << RESET          << bb_code
              << UNDERLINE      << std::left    << std::setw(width)     << text
              << RESET << nl;
              
    return;
}


//	"find_and_replace"
void find_and_replace(std::string & str, const std::string & oldStr,
					 					 const std::string & newStr)
{
	std::string::size_type 	pos		= 0U;

	while ( (pos = str.find(oldStr, pos)) != std::string::npos )
	{
		str.replace(pos, oldStr.length(), newStr);
		pos += oldStr.length();
	}

	return;
}


// *************************************************************************** //
//
//
//
// 	2.  STRUCTS & CLASSES.
// *************************************************************************** //
// *************************************************************************** //

//                  1.2.1.      "not_implemented" class.
// *************************************************************************** //
not_implemented::not_implemented(const std::string & msg)
                                 : std::logic_error(msg) {}
                                 
                                 
                                 
                                 
                                 
                                 

// *************************************************************************** //
// *************************************************************************** //
}   }// 	END NAMESPACE   "cblib" :: "utl".
