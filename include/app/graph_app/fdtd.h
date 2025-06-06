/***********************************************************************************
*
*       ********************************************************************
*       ****           _ G R A P H _ A P P . H  ____  F I L E           ****
*       ********************************************************************
*
*              AUTHOR:      Collin A. Bond
*               DATED:      May 18, 2025.
*
*       ********************************************************************
*                FILE:      [./PyStream.h]
*
*
*
**************************************************************************************
**************************************************************************************/
#ifndef _CB_FDTD_H
#define _CB_FDTD_H  1



//#define            CB_BAREBONES                       1
#define                _CBAPP_DISABLE_FDTD_FILE_IO            1



#ifndef _GLIBCXX_IOSTREAM
#    include <iostream>
#endif    /*  _GLIBCXX_IOSTREAM  */

#ifndef _GLIBCXX_CMATH
#    include <cmath>
#endif    /*  _GLIBCXX_CMATH  */

#ifndef _GLIBCXX_ARRAY
#    include <array>
#endif    /*  _GLIBCXX_ARRAY  */

#ifndef _GLIBCXX_STDEXCEPT
#    include <stdexcept>
#endif    /*  _GLIBCXX_STDEXCEPT  */

#if __cplusplus >= 201103L
#    ifndef _GLIBCXX_INITIALIZER_LIST
#         include <initializer_list>
#    endif    /*  _GLIBCXX_INITIALIZER_LIST  */
#endif    // C++11.

#ifndef _GLIBCXX_UTILITY
#    include <utility>
#endif    /*  _GLIBCXX_UTILITY  */

#ifndef _GLIBCXX_CSTRING
#    include <cstring>
#endif    /*  _GLIBCXX_CSTRING  */

#ifndef _GLIBCXX_CSTDINT
#    include <cstdint>
#endif    /*  _GLIBCXX_CSTDINT  */

#ifndef _LIBCPP_FILESYSTEM
#    include <filesystem>
#endif     /*    _LIBCPP_FILESYSTEM    */

#ifndef _GLIBCXX_COMPLEX
#    include <complex>
#endif     /*     _GLIBCXX_COMPLEX    */

#ifndef _GLIBCXX_TYPEINFO
#    include <typeinfo>
#endif     /*     _GLIBCXX_TYPEINFO    */


//  #ifndef _CB_FDTD_SOURCES_H
//  #    include <sources.h>
//  #endif /*    _CB_FDTD_SOURCES_H    */


/*#ifndef _CBLIB_BINARY_FILE_H
#    include "../../../../misc/binary_file.h"
#endif */ /*    _CBLIB_BINARY_FILE_H    */






namespace cb { namespace spc {//     BEGINNING NAMESPACE "cb" :: "spc"...
// *************************************************************************** //
// *************************************************************************** //

constexpr long double        pi                        = 3.141592653589793L;
constexpr long double        c                        = 2.997924580000000e+08L;
constexpr long double        eps_0                    = 8.854187812800001e-12L;
constexpr long double        mu_0                    = 1.256637062120000e-06L;
constexpr long double        eta_0                    = 3.767303136680000e+02L;
constexpr long double        electron_mass            = 9.109383701500001e-31L;
constexpr long double        elementary_charge        = 1.602176634000000e-19L;
constexpr long double        electron_volt            = 1.602176634000000e-19L;


// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
} }//   END OF "cb" :: "spc" NAMESPACE.












namespace cb { //     BEGINNING NAMESPACE "cb"...
// *************************************************************************** //
// *************************************************************************** //

//    TYPEDEFS, CONSTANT-STATIC VALUES, ETC...
// *************************************************************************** //
// *************************************************************************** //

//using typename         complex_t = std::complex<double>;


//    INLINE FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

//    "real_DFT"
//    Compute Discrete Fourier Transform (DFT) of a real-signal.
//
//        "sig_t"         ===>    time-domain signal.
//        "sig_f"            ===>     frequency-domain signal.
template< typename T, typename complex_t = std::complex<T> >
inline std::vector<complex_t>
real_DFT(const std::vector<T> & sig_t) noexcept
{
    const std::size_t   N   = sig_t.size();
    std::vector<complex_t> sig_f(N);

    for (std::size_t k = 0; k < N; ++k) {
        complex_t sum(0.0, 0.0);

        for (std::size_t n = 0; n < N; ++n) {
            const T angle = -( 2.0 * spc::pi * static_cast<T>(k) * static_cast<T>(n) )
                             / static_cast<T>(N);
            sum += sig_t[n] * complex_t( std::cos(angle), std::sin(angle) );
        }
        sig_f[k] = sum;
    }
    return sig_f;
}


//  "normalize_spectrum"
//
template< typename complex_t >
inline std::vector<typename complex_t::value_type>
normalize_spectrum(const std::vector<complex_t> & sig_f)
{
    using T = typename complex_t::value_type;

    std::vector<T> mag(sig_f.size());
    T peak = T(0);

    for (std::size_t k = 0; k < sig_f.size(); ++k) {
        mag[k] = std::abs(sig_f[k]);
        peak   = std::max(peak, mag[k]);
    }

    if (peak > T(0)) {
        for (auto & v : mag) v /= peak;
    }
    return mag;
}

//  "frequency_axis"
//
template< typename T >
inline std::vector<T>
frequency_axis(std::size_t N, T Fs)                      // N even preferred
{
    std::vector<T> f;
    if (N == 0) return f;

    const std::size_t M  = N/2 + 1;                      // # positive bins
    f.resize(M);

    const T df = Fs / static_cast<T>(N);
    for (std::size_t k = 0; k < M; ++k)
        f[k] = k * df;

    return f;
}

//  "normalised_freq_axis"
//
template< typename T = double >
inline std::vector<T>
normalised_freq_axis(std::size_t N)
{
    std::vector<T> f;
    if (N == 0) return f;

    const std::size_t M = N/2 + 1;
    f.resize(M);

    const T df = static_cast<T>(1) / static_cast<T>(N);
    for (std::size_t k = 0; k < M; ++k)
        f[k] = k * df;

    return f;
}

/*---------------------------------------------------------------------
    build_spectrum_frames ––
    Given NT time‑domain frames (each length NX), compute for each frame:
        1) DFT of the real signal (complex bins)
        2) Normalise magnitudes → [0,1]
    Returns NT × NX real‑valued matrix (vector< vector<T> >).
---------------------------------------------------------------------*/
template< typename T >
inline std::vector< std::vector<T> >
build_spectrum_frames(const std::vector< std::vector<T> > & time_frames)
{
    using complex_t = std::complex<T>;

    std::vector< std::vector<T> > spec_frames;
    spec_frames.reserve(time_frames.size());

    for (const auto & sig_t : time_frames) {
        auto sig_f  = real_DFT<T, complex_t>(sig_t);             // complex bins
        auto mags   = normalize_spectrum<complex_t>(sig_f);      // real [0,1]
        spec_frames.emplace_back(std::move(mags));               // store frame
    }
    return spec_frames;      // NT frames, each size NX
}


// *************************************************************************** //
// *************************************************************************** //






// *************************************************************************** //
//     PRIMARY TEMPLATE DECLARATION:
//     1-Dimensional FDTD Grid Class Template Definition
// *************************************************************************** //

template< std::size_t N, typename T = double, typename Allocator = std::allocator<T> >
struct grid_1D {

//     Struct-Defined Nested Public Alias Definitions.
// *************************************************************************** //
    using     allocator_type        = Allocator;
    using    Alloc                = allocator_type;
    using     value_type             = T;
    using     size_type            = std::size_t;
    using     difference_type     = std::ptrdiff_t;
    using     pointer             = std::allocator_traits<Allocator>::pointer;
    using     const_pointer         = std::allocator_traits<Allocator>::const_pointer;
    using     reference             = value_type &;
    using     const_reference     = const value_type &;
    using   iterator            = typename std::vector<T>::iterator;
    using   const_iterator      = typename std::vector<T>::const_iterator;

    using     complex_t             = std::complex<value_type>;
    using     re_array             = std::vector<value_type>;
    using     im_array             = std::vector<complex_t>;
    using     sbyte                 = std::int_fast8_t;
// *************************************************************************** //

// *************************************************************************** //

    //    Default Constructor.
    //
    inline grid_1D(void) :
        m_Ez(N, 0.0f),                          m_Hy(N, 0.0f),
        m_eps_r(N, complex_t(0.0f, 0.0f)),      m_mu_r(N, complex_t(0.0f, 0.0f)),
        m_cezE(N, 0.0f),                        m_cezH(N, 0.0f),
        m_chyE(N, 0.0f),                        m_chyH(N, 0.0f)
    {
        for (std::size_t m = 0ULL; m < N; ++m) {
            this->m_Ez[m]         = 0.0f;
            this->m_Hy[m]         = 0.0f;
            this->m_eps_r[m]     = complex_t(1.0, 0.0);
            this->m_mu_r[m]     = complex_t(1.0, 0.0);

            this->m_cezE[m]     = 1.0f;
            this->m_cezH[m]     = 1.0f;
            this->m_chyH[m]     = 1.0f;
            this->m_chyE[m]     = 1.0f;
        }
    }

    //    Copy Constructor.
    //
    inline grid_1D(const grid_1D & src) :
                   m_Ez(src.m_Ez),         m_Hy(src.m_Hy),         m_eps_r(src.m_eps_r),
                m_mu_r(src.m_mu_r),        m_cezE(src.m_cezE),        m_cezH(src.m_cezH),
                m_chyE(src.m_chyE),        m_chyH(src.m_chyH)        {}

    //    Default Destructor.
    //
    inline virtual ~grid_1D(void) = default;


//    Data-Members...
// *************************************************************************** //
    re_array        m_Ez,       m_Hy;
    im_array        m_eps_r,    m_mu_r;
    re_array        m_cezE,        m_cezH,        m_chyE,        m_chyH;
// *************************************************************************** //

//    END GRID_1D INLINE STRUCT DEFINITION.
// *************************************************************************** //
};







// *************************************************************************** //
// *************************************************************************** //
//                 PRIMARY TEMPLATE DECLARATION:
//         1-Dimensional FDTD Class Template Definition
// *************************************************************************** //
// *************************************************************************** //
template< std::size_t     NX,
          std::size_t    NT,
          typename         T             = double,
          typename         Allocator     = std::allocator<T> >
class FDTD_1D : grid_1D<NX, T, Allocator> {
// *************************************************************************** //
// *************************************************************************** //
//
//
//    LOCAL, CLASS-DEFINED PUBLIC ALIAS DEFINITIONS ...
// *************************************************************************** //
public:
    using       base            = grid_1D<NX, T, Allocator>         ;
    using       typename        base::allocator_type                ;       using       typename            base::Alloc;
    using       typename        base::value_type                    ;       using       typename            base::complex_t;
    using       typename        base::size_type                     ;       using       typename            base::difference_type;
    using       typename        base::pointer                       ;       using       typename            base::const_pointer;
    using       typename        base::sbyte                         ;
//
    using       typename        base::reference                     ;       using       typename            base::const_reference;
    using       typename        base::iterator                      ;       using       typename            base::const_iterator;
//
    using       typename        base::re_array                      ;
    using       typename        base::im_array;
    using       re_frame        = std::vector<re_array>             ;       using       im_frame            = std::vector<im_array>;

// *************************************************************************** //
//
//
//    PROTECTED DATA-MEMBERS ...
// *************************************************************************** //
protected:
#ifndef _CBAPP_DISABLE_FDTD_FILE_IO
    std::string                         m_filename              = "";
    binary_footer                       m_footer;
#endif  //  _CBAPP_DISABLE_FDTD_FILE_IO  //

    std::array<value_type, NT>          m_time;
    re_array                            m_xvals;
    re_frame                            m_Hy_T,                 m_Ez_T,
                                        m_Hy_F_NORM,            m_Ez_F_NORM;
    im_frame                            m_Hy_F,                 m_Ez_F;
    

    const size_type                     m_src_pos               = 2ULL;//2ULL;
    const size_type                     m_TFSF_boundary         = 2ULL;  // NX+1;//2ULL;
    const size_type                     m_loss_layer            = 252ULL;   //      WORKS = NX+1;           //300ULL;
    size_type                           m_material_pos          = 252;       //      WORKS = NX+1;             //52ULL;
    size_type                           m_material_width        = 150; //     50ULL;    //      WORKS = 30ULL;

    const value_type                    m_wavelen               = 25.0f; // 24.0f;//BEFORE WE USED 24 for LAMBDA...     30.0f;
    const value_type                    m_src_delay             = 30.0f;//30.0f;
    const value_type                    m_src_width             = 10.0f;
    const size_type                     m_num_periods           = 10ULL;     //      WORKS = 100ULL;//4ULL;

    const value_type                    m_eta_0                 = spc::eta_0;
    const value_type                    m_loss                  = 0.0045f;    //      WORKS = 0.00f;
    const value_type                    m_perm                  = 7.0;      //      WORKS = 4.0f;


// *************************************************************************** //
//
//
//    CONSTRUCTORS, DESTRUCTOR, AND INITIALIZER FUNCTIONS ...
// *************************************************************************** //
public:

    //    Default Constructor.
    //
    inline FDTD_1D(void) noexcept : base(),
        m_xvals(NX, 0.0f),
        m_Hy_T( NT, re_array(NX, 0.0f) ),                           m_Ez_T( NT, re_array(NX, 0.0f) ),
        m_Hy_F_NORM( NT, re_array(NX, 0.0f) ),                      m_Ez_F_NORM( NT, re_array(NX, 0.0f) ),
        m_Hy_F( NT, im_array(NX, complex_t(0.0f, 0.0f)) ),          m_Ez_F( NT, im_array(NX, complex_t(0.0f, 0.0f)) )
    {
        //this->m_material_width     = size_type(4*this->m_wavelen / ( std::sqrt(this->m_perm) )) - 1;
    
        #ifdef CB_LOG
        std::cout << "\nValue of film thickness,\td = "     << this->m_material_width
                  << "\nValue of material position,\tx = "     << this->m_material_pos
                  << std::endl;
        #endif    /*    CB_LOG    */

        //this->m_material_width        = 20ULL;
        this->initialize();
    }


#ifndef _CBAPP_DISABLE_FDTD_FILE_IO
// *************************************************************************** //
    //    Assignment Constructor 1.
    //
    inline FDTD_1D(const char * filename) :
                base()
    {
        this->initialize();

        if (!filename)
            throw std::invalid_argument("Argument for \"filename\" cannot be \"nullptr\".");

        this->m_filename = filename;
    }
 
 
    //    Assignment Constructor 2.
    //
    inline FDTD_1D(const std::string & filename) noexcept :
            base(), m_filename(filename) {
        this->initialize();
    }
// *************************************************************************** //
#endif  //  _CBAPP_DISABLE_FDTD_FILE_IO  //


    //    Copy Constructor.
    //
#ifndef _CBAPP_DISABLE_FDTD_FILE_IO
    inline FDTD_1D(const FDTD_1D & src) noexcept :
            base(src),                 m_filename(src.m_filename),
            m_Hy_T(src.m_Hy_T),     m_Ez_T(src.m_Ez_T) {}
//
# else
//
    inline FDTD_1D(const FDTD_1D & src) noexcept :
        base(src),                  m_Hy_T(src.m_Hy_T),
        m_Ez_T(src.m_Ez_T) {}
#endif  //  _CBAPP_DISABLE_FDTD_FILE_IO  //


    //    Default Destructor.
    //
    inline virtual ~FDTD_1D(void) = default;


// *************************************************************************** //
//
//
//    OVERLOADED OPERATORS ...
// *************************************************************************** //
// *************************************************************************** //

    //    Overloaded extraction operator.
    //
    inline friend std::ostream & operator << (std::ostream & out, const FDTD_1D & arg1)
    { return arg1.display(out); }


// *************************************************************************** //
//
//
//    PRIVATE MEMBER FUNCTIONS ...
// *************************************************************************** //
// *************************************************************************** //
private:

    //    "initialize"
    //
    inline void initialize(void) noexcept
    {
        //    INITIALIZE X-VALS.
        for (size_type m = 0ULL; m < NX; ++m)
            this->m_xvals[m] = m;

        for (size_type q = 0; q < NT; ++q) {//    INITIALIZE  Hy(t), Ez(t)  TO ZERO.
            this->m_time[q] = q;

            for (size_type m = 0; m < NX; ++m) {
                this->m_Hy_T[q][m] = 0.0f;
                this->m_Ez_T[q][m] = 0.0f;

                this->m_Hy_F[q][m] = complex_t(0.0, 0.0);
                this->m_Ez_F[q][m] = complex_t(0.0, 0.0);
            }
        }
        
    #ifndef _CBAPP_DISABLE_FDTD_FILE_IO
        this->m_footer = binary_footer( {{NX, 1ULL, 1ULL, NT}, {1.0f, 0.0f, 0.0f, 1.0f}},
                                        "here is some default metadata.", "CB1DFDTD" );
    #endif  //  _CBAPP_DISABLE_FDTD_FILE_IO  //
    
        return;
    }

    //    "display"
    //
    inline std::ostream & display(std::ostream & out) const noexcept
    {
        out << WHITE_BOLD << "1-DIMENSIONAL FDTD SIMULATION:\n"                                 << RESET;

        out << RED_BOLD        << "1.\tTEMPLATE PARAMETERS ..."                                    << RESET
            << "\n\tvalue type\t\t:\t"                                 << typeid(T).name()
            << "\n\t# spatial nodes\t\t:\t"                         << NX                         << " X,"
            << "\n\t# temporal nodes\t:\t"                             << NT                         << " T.";


        out << RED_BOLD     << "\n\n2.\tSOURCE ..."                        << RESET
            << "\n\tposition\t\t:\t"                                 << this->m_src_pos             << " X,"
            << "\n\twavelength\t\t:\t"                                 << this->m_wavelen             << " DX,"
            << "\n\tdelay\t\t\t:\t"                                 << this->m_src_delay         << " DT,"
            << "\n\tduration\t\t:\t"                                 << this->m_num_periods        << " Periods.";

        out << RED_BOLD     << "\n\n3.\tINHOMOGENEITIES ..."            << RESET
            << "\n\tmaterial position\t:\t"                         << this->m_material_pos        << " X,"
            << "\n\tlossy position\t\t:\t"                             << this->m_loss_layer        << " X,"
            << "\n\tmaterial thickness\t:\t"                         << this->m_material_width    << " DX,"
            << "\n\teps_r\t\t\t:\t"                                 << this->m_perm                << " ,"
            << "\n\tloss\t\t\t:\t"                                     << this->m_loss                << " .";

    #ifndef _CBAPP_DISABLE_FDTD_FILE_IO
        out << RED_BOLD     << "\n\n4.\tOTHER ..."                    < RESET
            << "\n\tfilename\t\t:\t"                                 << GREEN << this->m_filename << RESET;
    #endif  //  _CBAPP_DISABLE_FDTD_FILE_IO  //
    
        out << std::endl;

        return out;
    }

// *************************************************************************** //
//
//
//
//    PROTECTED MEMBER FUNCTIONS ...
// *************************************************************************** //
protected:

    //    "save"
    //
    inline void save(void)
    {
#ifndef _CBAPP_DISABLE_FDTD_FILE_IO
//
//
        bool            empty    = this->m_filename.empty();
        
        //    CASE 0 : Filename has NOT been assigned.
        if (empty) {
            this->m_filename = ".UNASSIGNED_1D_FDTD_OUTPUT.bin";
            # ifdef CB_LOG
            std::cerr << YELLOW_BOLD << "Runtime Warning:\t" << GREEN
                      << "Saving output data for FDTD_1D without a specified \"filename\"...\n\t"
                      << "Output will be saved under default name:\t\"" << MAGENTA_BOLD << this->m_filename
                      << GREEN << "\".\n" << RESET;
            # endif /*    CB_LOG    */
        }
        std::ofstream    outfile(this->m_filename, std::ios::binary | std::ios::app);

        //    CASE 1 : FILE CANNOT BE OPENED.
        if (!outfile) {
            throw std::runtime_error("ERROR. The file \"" + std::string(this->m_filename) + "\".");
        }



        //    WRITING OUTPUT DATA
        //
        //    0.    ERASE CONTENTS OF EXISTING BINARY FILE ...
        erase_binary_file(this->m_filename.c_str());

        //    1.    WRITING BODY DATA [PART 1 OF 2] ...
        //        Writing (min, max) values for         Hy(t),        Ez(t)
        {
            auto [Hy_min, Hy_max] = find_min_max(this->m_Hy_T);
            auto [Ez_min, Ez_max] = find_min_max(this->m_Ez_T);

            outfile.write( reinterpret_cast<const char *>(&Hy_min), sizeof(T) );
            outfile.write( reinterpret_cast<const char *>(&Hy_max), sizeof(T) );
            outfile.write( reinterpret_cast<const char *>(&Ez_min), sizeof(T) );
            outfile.write( reinterpret_cast<const char *>(&Ez_max), sizeof(T) );
            outfile.close();
        }


        //    2.    WRITING BODY DATA [PART 2 OF 2] ...
        //
        //        2.1.    WRITING SINGULAR ARRAY VALUES
        //                Write the x-value array to the file.
        write_binary_file<value_type, NX>(this->m_filename.c_str(), this->m_xvals);
        //        Write the time-array to the file.
        write_binary_file<value_type, NT>(this->m_filename.c_str(), this->m_time);
        //        Write the permeability values.
        write_binary_file<complex_t, NX>(this->m_filename.c_str(), base::m_mu_r);
        //        Write the permitivity values.
        write_binary_file<complex_t, NX>(this->m_filename.c_str(), base::m_eps_r);


        //        2.2.    WRITING MULTIPLE ARRAY VALUES...
        //                Write the Hy_T-array to the file.
        for (size_type q = 0ULL; q < NT; ++q)
            write_binary_file(this->m_filename.c_str(), this->m_Hy_T[q]);
        //
        //                Write the Ez_T-array to the file.
        for (size_type q = 0ULL; q < NT; ++q)
            write_binary_file(this->m_filename.c_str(), this->m_Ez_T[q]);
        //
        //                Write the Ez_F-array to the file.
        for (size_type q = 0ULL; q < NT; ++q)
            write_binary_file(this->m_filename.c_str(), this->m_Ez_F[q]);


        //    3.    Write the FOOTER CONTENT to the file ...
        this->m_footer.write_to(this->m_filename.c_str());

        if (empty) this->m_filename.clear();    //    Remove the default name after saving...
//
//
#endif  //  _CBAPP_DISABLE_FDTD_FILE_IO  //
        return;
    }
    

    //    "create_film"
    //
    inline void create_film(const size_type pos, const size_type d,  const value_type epsilon) noexcept
    {
        size_type               m       = 0ULL;
        const size_type         len     = pos + d;

        //    0.    Setting permittivity and permeability.
        for (m = pos; m < len; ++m) {
            base::m_eps_r[m]    = complex_t(epsilon, 0.0);

            //    E-Field Update Coefficient.
            base::m_cezE[m]        = 1.0f;
            base::m_cezH[m]        = this->m_eta_0 / this->base::m_eps_r[m].real();

            base::m_chyH[m]     = 1.0f;
            base::m_chyE[m]     = 1.0f / this->m_eta_0;
        }
        
        return;
    }

    //    "thin_film"
    //
    inline void thin_film(void) noexcept
    {
        size_type    m = 0ULL;

        //    0.    Setting permittivity and permeability.
        for (m = 0ULL; m < NX; ++m) {
            base::m_mu_r[m]        = complex_t(1.0, 0.0);
            
            //            REGION 2:    DIELEC.
            if (m >= this->m_material_pos &&
                m <= this->m_material_pos + this->m_material_width) {
                base::m_eps_r[m]    = complex_t(this->m_perm, 0.0);
            }
            else {//    REGION 1:    FREE-SPACE.
                base::m_eps_r[m]    = complex_t(1.0f, 0.0);
            }
        }

        //    1.    E-field Update Coeffs.
        for (m = 0ULL; m < NX; ++m) {
            base::m_Hy[m]        = 0.0f;
            base::m_Ez[m]        = 0.0f;

            //            REGION 1:    FREE-SPACE.
            if ( m < this->m_material_pos ||
                 m > this->m_material_pos + this->m_material_width ) {
                base::m_cezE[m]        = 1.0f;
                base::m_cezH[m]        = this->m_eta_0;
            }
            else {//    REGION 2:    DIELEC.
                base::m_cezE[m]            = 1.0f;
                base::m_cezH[m]            = this->m_eta_0 / this->base::m_eps_r[m].real();
            }
        }

        //    2.    H-field Update Coeffs.
        for (m = 0ULL; m < NX; ++m) {
            if (m < this->m_loss_layer) {//            REGION 1:    FREE-SPACE.
                base::m_chyH[m]     = 1.0f;
                base::m_chyE[m]     = 1.0f / this->m_eta_0;
            }
            else {//                                REGION 2:    LOSSY-LAYER.
                base::m_chyH[m]     = (1.0f - this->m_loss)/(1.0f + this->m_loss);
                base::m_chyE[m]     = 1.0f / this->m_eta_0 / (1.0f + this->m_loss);
            }
        }

        return;
    }


    //    "init_interface"
    //
    inline void init_interface(void) noexcept
    {
        size_type    m = 0ULL;

        //    0.    Setting permittivity and permeability.
        for (m = 0ULL; m < NX; ++m) {
            base::m_mu_r[m]            = complex_t(1.0, 0.0);

            if (m < this->m_material_pos)//        REGION 1:    FREE-SPACE.
                base::m_eps_r[m]    = complex_t(1.0f, 0.0);
            else//                                REGION 2:    DIELEC.
                base::m_eps_r[m]    = complex_t(this->m_perm, 0.0);
        }

        //    1.    E-field Update Coeffs.
        for (m = 0ULL; m < NX; ++m) {
            base::m_Hy[m]        = 0.0f;
            base::m_Ez[m]        = 0.0f;

            if (m < this->m_material_pos ) {//        REGION 1:    FREE-SPACE.
                base::m_cezE[m]                 = 1.0f;
                base::m_cezH[m]                 = this->m_eta_0;
            }
            else if (m < this->m_loss_layer) {//    REGION 2:    DIELEC.
                base::m_cezE[m]                 = 1.0f;
                base::m_cezH[m]                 = this->m_eta_0 / this->base::m_eps_r[m].real();
            }
            else {//                                REGION 3:    LOSSY-LAYER.
                value_type            re_eps    = this->base::m_eps_r[m].real();
                base::m_cezE[m]                 = (1.0 - this->m_loss)/(1.0 + this->m_loss);
                base::m_cezH[m]                 = this->m_eta_0 / this->m_perm / (1.0 + this->m_loss);

                this->base::m_eps_r[m]          = complex_t(re_eps, this->m_loss);
            }
        }

        //    2.    H-field Update Coeffs.
        for (m = 0ULL; m < NX; ++m) {
            if (m < this->m_loss_layer) {//            REGION 1:    FREE-SPACE.
                base::m_chyH[m]     = 1.0f;
                base::m_chyE[m]     = 1.0f / this->m_eta_0;
            }
            else {//                                REGION 2:    LOSSY-LAYER.
                base::m_chyH[m]     = (1.0f - this->m_loss)/(1.0f + this->m_loss);
                base::m_chyE[m]     = 1.0f / this->m_eta_0 / (1.0f + this->m_loss);
            }
        }

        return;
    }

    //    "init_grid"
    //
    inline void init_grid(void) noexcept {
        # ifdef CB_BAREBONES
            this->init_interface();
        # endif    /*    CB_BAREBONES    */

        this->thin_film();
        return;
    }

    //    "update_H"
    //
    inline void update_H(void)
    {
        //base::m_Hy[NX-2]     = base::m_Hy[NX-1];
        for (size_type m = 0ULL; m < NX-1; ++m) {
                base::m_Hy[m] = base::m_chyH[m] * base::m_Hy[m] +
                                base::m_chyE[m] * (base::m_Ez[m + 1] - base::m_Ez[m]);
        }

        return;
    }

    //    "update_E"
    //
    inline void update_E(void)
    {
        for (size_type m = 1ULL; m < NX-1; ++m) {
                base::m_Ez[m] = base::m_cezE[m] * base::m_Ez[m] +
                                base::m_cezH[m] * (base::m_Hy[m] - base::m_Hy[m - 1]);
        }

        return;
    }

    //    "abc_1"
    //
    inline void abc_1(void) {
        base::m_Hy[NX-2] = base::m_Hy[NX-1];
        return;
    }

    //    "abc_2"
    //
    inline void abc_2(void) {
        base::m_Ez[0] = base::m_Ez[1];
        return;
    }

    //    "order_2_abc_L"
    //
    /*
    void value_type order_2_abc_L(void) {
        const value_type    Sc            = 1.0f;
        const value_type    Sc_prime    = Sc / std::sqrt( base::m_eps_r[0] * base::m_mu_r[0] );
        const value_type    E_q0m0        = base::m_Ez[0],        E_q1m1 = base::m_Ez[2],            E_q0m0    = base::m_Ez[0];

        const value_type    coef_0        = ( (1/Sc_prime) + 2 + Sc_prime);
        const value_type    T1            = coef_0 * ( E_q1m2 + E_q0m0 );
        const value_type    T2            = coef_0 * ( E_q1m2 + E_q0m0 );




        base::m_Ez[0]                     = (1.0f / coef_0) * ( T1 + 2.0f*T2 - 4.0f*T3 ) - C1;
        return;
    }*/

    //    "gaussian_source"
    //
    inline value_type gaussian_source(const value_type Sc, const value_type q, const value_type m)
    {
        value_type arg    = std::pow( (q - this->m_src_delay - m/Sc) / this->m_src_width, 2 );
        return std::exp( -arg );
    }

    //    "harmonic_source"
    //
    inline value_type harmonic_source(const value_type Sc, const value_type q, const value_type m)
    {
        const size_type        pos            = this->m_src_pos;
        const size_type        q_total        = size_type(this->m_num_periods * this->m_src_delay);
        const value_type    tau            = (2 * spc::pi / this->m_wavelen);
        const value_type    t_soft        = q_total / (this->m_num_periods);
        value_type            temp         = std::sqrt( base::m_mu_r[pos].real() * base::m_eps_r[pos].real() );
        value_type             arg         = tau * (Sc * q - temp*m );

        //    Branchless "soft" ON-switch to gently ramp up the source amplitude.
        value_type            ON            = 1.0f - (q >= q_total);

        //    LAMBDA:    Compute quadratic smoothing from 0.0 ===> 1.0...
        auto soft_start = [](value_type x, const value_type t_final) {
            value_type    x_norm    = x / t_final;
            x_norm                 = x_norm - (x_norm > 1.0) * (x_norm - 1.0);

            return x_norm;
            //return x_norm * x_norm * (3 - 2*x_norm);
        };

        return ON * soft_start(q, t_soft) * ( std::sin(arg) );
    }

    //    "ricker_source"
    //
    inline value_type ricker_source(const value_type Sc, const value_type q, const value_type m)
    {
        const value_type    d        = 1;
        value_type            pi_2    = spc::pi * spc::pi;
        value_type            term    = std::pow( (Sc * q - m)/this->m_wavelen - d, 2);
    
        return (1 - 2 * pi_2 * term) * ( std::exp(-pi_2 * term) );
    }

    //    "source"
    //
    inline value_type source(const value_type Sc, const value_type q, const value_type m)
    {
        //return ricker_source(Sc, q, m);
        return harmonic_source(Sc, q, m);
        //return gaussian_source(Sc, q, m);
    }

    //    "update_TFSF"
    //
    inline void update_TFSF(const size_type q)
    {
        const value_type    Sc    = 1.0f;
        base::m_Hy[this->m_TFSF_boundary - 1]    -= this->source(Sc, q, 0.0f) * base::m_chyE[this->m_TFSF_boundary];
        base::m_Ez[this->m_TFSF_boundary]        += this->source(Sc, q + 0.5f, -0.5f);
        return;
    }

    //    "hard_source"
    //
    inline void hard_source(const size_type q)
    {
        const value_type    Sc            = 1.0f;
        base::m_Ez[this->m_src_pos]       += this->gaussian_source(Sc, q, 0.0f);

        return;
    }


// *************************************************************************** //
//
//
//    PUBLIC UTILITY FUNCTIONS ...
// *************************************************************************** //
// *************************************************************************** //
public:

    //  "get_E_time_data"
    //
    inline re_frame * get_E_time_data(void) {
        return std::addressof( this->m_Ez_T );
    }
    
    //  "get_E_freq_data"
    //
    inline re_frame * get_E_freq_data(void) {
        return std::addressof( this->m_Ez_F_NORM );
        //return std::addressof( this->m_Ez_F );
    }

    //  "get_fourier_freqs"
    //
    inline re_array get_fourier_freqs(void) const
    {
        // Normalised 0 … 0.5 cycles/step grid (size NX/2+1)
        return normalised_freq_axis<value_type>(NX);
    }

    //  "get_perm_E_data"
    //
    //inline std::array<complex_t, NX> * get_perm_E_data()       noexcept
    //{ return &base::m_eps_r; }
    
    inline im_array & get_eps_r(void)
    { return this->base::m_eps_r; }
    

    //  // or, idiomatically, reference
    //  inline std::array<complex_t, NX> &       get_perm_E_data()       noexcept
    //  { return m_eps_r; }
    //
    //  inline const std::array<complex_t, NX> & get_perm_E_data() const noexcept
    //  { return m_eps_r; }

    //  im_array        m_eps_r,    m_mu_r;


// *************************************************************************** //
//
//
//    PUBLIC MEMBER FUNCTIONS ...
// *************************************************************************** //
// *************************************************************************** //

    //    "run"
    //
    inline void run(void)
    {
        //static_assert(0 <= m_TFSF_boundary && m_TFSF_boundary < NX);
        
        size_type        m = 0ULL,     q = 0ULL;
        this->init_grid();
        
        
    
        this->create_film(77ULL, 50, 2.2);
        
        
        
        //  this->create_film(210ULL, 25);
        //  this->create_film(350ULL, 25);


        //    MAIN FDTD LOOP...
        for (q=0ULL; q < NT; ++q)
        {
            //    1.    SIMULATE...
            this->abc_1();
            this->update_H();
            this->update_TFSF(q);
            this->abc_2();
            this->update_E();

            //    2.    COPYING DATA...
            for (m = 0ULL; m < NX; ++m) {
                this->m_Ez_T[q][m] = base::m_Ez[m];
                this->m_Hy_T[q][m] = base::m_Hy[m];
            }
        }


        //    3.    COMPUTING DISCRETE FOURIER TRANSFORM...
        for (q = 0ULL; q < NT; ++q) {
            this->m_Hy_F[q] = real_DFT<value_type>(this->m_Hy_T[q]);
            this->m_Ez_F[q] = real_DFT<value_type>(this->m_Ez_T[q]);
        }

    
        //    Wrapping up simulation and saving data...
    #ifndef _CBAPP_DISABLE_FDTD_FILE_IO
        this->m_filename = "../Python/DATA/DESTRUCTIVE_MULTI/DESTRUCTIVE_MULTI.bin";
        this->m_filename = "../Python/DATA/DESTRUCTIVE/DESTRUCTIVE.bin";
        this->save();
    #endif  //  _CBAPP_DISABLE_FDTD_FILE_IO  //
    
    
        // 4. NORMALISE DFT magnitudes (real, 0‒1) -------------------------
        using complex_t = typename base::complex_t;
        for (q = 0ULL; q < NT; ++q) {
            m_Hy_F_NORM[q] = normalize_spectrum<complex_t>( m_Hy_F[q] );
            m_Ez_F_NORM[q] = normalize_spectrum<complex_t>( m_Ez_F[q] );
        }
    
    
    
        return;
    }


    //    "barebones"
    //
    inline void barebones(void)
    {
        size_type        m = 0ULL,     q = 0ULL;
        this->init_grid();

        //    MAIN FDTD LOOP...
        for (q=0ULL; q < NT; ++q)
        {
            //    1.    SIMULATE...
            //this->abc_1();
            this->update_H();
            //this->update_TFSF(q);
            //this->abc_2();
            this->update_E();
            this->hard_source(q);

            //    2.    COPYING DATA...
            for (m = 0ULL; m < NX; ++m) {
                this->m_Ez_T[q][m] = base::m_Ez[m];
                this->m_Hy_T[q][m] = base::m_Hy[m];
            }
        }


        //    3.    COMPUTING DISCRETE FOURIER TRANSFORM...
        for (q = 0ULL; q < NT; ++q) {
            this->m_Hy_F[q] = real_DFT<value_type, NX>(this->m_Hy_T[q]);
            this->m_Ez_F[q] = real_DFT<value_type, NX>(this->m_Ez_T[q]);
        }

        //    Wrapping up simulation and saving data...
    #ifndef _CBAPP_DISABLE_FDTD_FILE_IO
        //this->m_filename = "../Python/DATA/BAREBONES/barebones.bin";
        this->save();
    #endif  //  _CBAPP_DISABLE_FDTD_FILE_IO  //
        return;
    }


// *************************************************************************** //
//
//
//
//    ...
// *************************************************************************** //


// *************************************************************************** //
// *************************************************************************** //
//    END FDTD_1D INLINE CLASS DEFINITION.
};






// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
}//   END OF "cb" NAMESPACE.












#endif      //  _CB_FDTD_H  //
// *************************************************************************** //
// *************************************************************************** //
//
//  END.
