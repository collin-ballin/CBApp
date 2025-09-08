/***********************************************************************************
*
*       ********************************************************************
*       ****           _ F D T D _ I M P L . H  ____  F I L E           ****
*       ********************************************************************
*
*              AUTHOR:      Collin A. Bond.
*               DATED:      June 6, 2025.
*
*       ********************************************************************
*                FILE:      [include/fdtd.h]
*
*
*
**************************************************************************************
**************************************************************************************/
#ifndef _CB_FDTD_IMPL_H
#define _CB_FDTD_IMPL_H  1



#include <iostream>
#include <filesystem>
#include <stdexcept>

#include <cmath>
#include <complex>
#include <cstdint>

#include <vector>
#include <array>
#include <initializer_list>

#include <utility>
#include <cstring>

#include <typeinfo>



namespace cb { namespace fdtd { namespace spc {//     BEGINNING NAMESPACE "cb" :: "fdtd" :: "spc"...
// *************************************************************************** //
// *************************************************************************** //

constexpr long double        pi                         = 3.141592653589793L;
constexpr long double        c                          = 2.997924580000000e+08L;
constexpr long double        eps_0                      = 8.854187812800001e-12L;
constexpr long double        mu_0                       = 1.256637062120000e-06L;
constexpr long double        eta_0                      = 3.767303136680000e+02L;
constexpr long double        electron_mass              = 9.109383701500001e-31L;
constexpr long double        elementary_charge          = 1.602176634000000e-19L;
constexpr long double        electron_volt              = 1.602176634000000e-19L;



// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
} } }//   END OF "cb" :: "fdtd" :: "spc" NAMESPACE.













namespace cb { namespace fdtd {//     BEGINNING NAMESPACE "cb" :: "fdtd"...
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

template< typename T = double, typename Allocator = std::allocator<T> >
struct grid_1D {

//     Struct-Defined Nested Public Alias Definitions.
// *************************************************************************** //
    using       allocator_type      = Allocator;
    using       Alloc               = allocator_type;
    using       value_type          = T;
    using       size_type           = std::size_t;
    using       difference_type     = std::ptrdiff_t;
    using       pointer             = std::allocator_traits<Allocator>::pointer;
    using       const_pointer       = std::allocator_traits<Allocator>::const_pointer;
    using       reference           = value_type &;
    using       const_reference     = const value_type &;
    using       iterator            = typename std::vector<T>::iterator;
    using       const_iterator      = typename std::vector<T>::const_iterator;

    using       complex_t           = std::complex<value_type>;
    using       re_array            = std::vector<value_type>;
    using       im_array            = std::vector<complex_t>;
    using       sbyte               = std::int_fast8_t;
    
    
// *************************************************************************** //
// *************************************************************************** //

    //    Default Constructor.
    //
    inline grid_1D(void) :
        m_Ez        (0, value_type(0.0f)        ),      m_Hy    ( 0, value_type(0.0f)       ),
        m_eps_r     (0, complex_t(0.0f, 0.0f)   ),      m_mu_r  ( 0, complex_t(0.0f, 0.0f)  ),
        m_cezE      (0, value_type(0.0f)        ),      m_cezH  ( 0, value_type(0.0f)       ),
        m_chyE      (0, value_type(0.0f)        ),      m_chyH  ( 0, value_type(0.0f)       )
    {
        //  ...
    }
    
    
    /*inline grid_1D(void) :
        m_Ez(N, 0.0f),                          m_Hy(N, 0.0f),
        m_eps_r(N, complex_t(0.0f, 0.0f)),      m_mu_r(N, complex_t(0.0f, 0.0f)),
        m_cezE(N, 0.0f),                        m_cezH(N, 0.0f),
        m_chyE(N, 0.0f),                        m_chyH(N, 0.0f)
    {
        for (std::size_t m = 0ULL; m < N; ++m) {
            this->m_Ez[m]       = 0.0f;
            this->m_Hy[m]       = 0.0f;
            this->m_eps_r[m]    = complex_t(1.0, 0.0);
            this->m_mu_r[m]     = complex_t(1.0, 0.0);

            this->m_cezE[m]     = 1.0f;
            this->m_cezH[m]     = 1.0f;
            this->m_chyH[m]     = 1.0f;
            this->m_chyE[m]     = 1.0f;
        }
    }*/

    //    Copy Constructor.
    //
    inline grid_1D(const grid_1D & src) :
                m_Ez(src.m_Ez),             m_Hy(src.m_Hy),             m_eps_r(src.m_eps_r),
                m_mu_r(src.m_mu_r),         m_cezE(src.m_cezE),         m_cezH(src.m_cezH),
                m_chyE(src.m_chyE),         m_chyH(src.m_chyH)          { }

    //    Default Destructor.
    //
    inline virtual ~grid_1D(void) = default;


//    Data-Members...
// *************************************************************************** //
    size_type       N = 0ULL;
    re_array        m_Ez,           m_Hy;
    im_array        m_eps_r,        m_mu_r;
    re_array        m_cezE,         m_cezH,         m_chyE,         m_chyH;
// *************************************************************************** //

//    END GRID_1D INLINE STRUCT DEFINITION.
// *************************************************************************** //
};



// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
} }//   END OF "cb" :: "fdtd" NAMESPACE.































#endif      //  _CB_FDTD_IMPL_H  //
// *************************************************************************** //
// *************************************************************************** //
//
//  END.
