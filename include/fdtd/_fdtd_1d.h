/***********************************************************************************
*
*       ********************************************************************
*       ****             _ F D T D _ 1 D . H  ____  F I L E             ****
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
#ifndef _CB_FDTD_1D_H
#define _CB_FDTD_1D_H  1


//  0.1.        ** MY **  HEADERS...
#include CBAPP_USER_CONFIG
#include "fdtd/_fdtd_impl.h"


//  0.2     STANDARD LIBRARY HEADERS...
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






namespace cb { namespace fdtd {//     BEGINNING NAMESPACE "cb" :: "fdtd"...
// *************************************************************************** //
// *************************************************************************** //



// *************************************************************************** //
// *************************************************************************** //
//                 PRIMARY TEMPLATE DECLARATION:
//         1-Dimensional FDTD Class Template Definition
// *************************************************************************** //
// *************************************************************************** //
template< typename          T               = double,
          typename          Allocator       = std::allocator<T> >
class FDTD_1D {
// *************************************************************************** //
// *************************************************************************** //
//
//
//    LOCAL, CLASS-DEFINED PUBLIC ALIAS DEFINITIONS ...
// *************************************************************************** //
public:
    using       base            = grid_1D<T, Allocator>;
    using       allocator_type  = base::allocator_type              ;       using       Alloc               = base::allocator_type;
    using       value_type      = base::value_type                  ;       using       complex_t           = base::complex_t;
    using       size_type       = base::size_type                   ;       using       difference_type     = base::difference_type;
    using       pointer         = base::pointer                     ;       using       const_pointer       = base::const_pointer;
    using       sbyte           = base::sbyte                       ;
//
    using       reference       = base::reference                   ;       using       const_reference     = base::const_reference;
    using       iterator        = base::iterator                    ;       using       const_iterator      = base::const_iterator;
//
    using       re_array        = base::re_array                    ;       using       im_array            = base::im_array;
    using       re_frame        = std::vector<re_array>             ;       using       im_frame            = std::vector<im_array>;

// *************************************************************************** //
//
//
//    PROTECTED DATA-MEMBERS ...
// *************************************************************************** //
protected:
    size_type                           NX                      = 0ULL;
    size_type                           NT                      = 0ULL;
    bool                                initialized             = false;
    bool                                complete                = false;
    grid_1D<T, Allocator>               m_base;
//
    std::vector<value_type>             m_time;
    re_array                            m_xvals;
    re_frame                            m_Hy_T,                 m_Ez_T,
                                        m_Hy_F_NORM,            m_Ez_F_NORM;
    im_frame                            m_Hy_F,                 m_Ez_F;
    

    const size_type                     m_src_pos               = 2ULL;     //2ULL;
    const size_type                     m_TFSF_boundary         = 2ULL;     // NX+1;//2ULL;
    const size_type                     m_loss_layer            = 252ULL;   //      WORKS = NX+1;           //300ULL;
    size_type                           m_material_pos          = 252;      //      WORKS = NX+1;             //52ULL;
    size_type                           m_material_width        = 150;      //     50ULL;    //      WORKS = 30ULL;

    const value_type                    m_wavelen               = 25.0f;    // 24.0f;//BEFORE WE USED 24 for LAMBDA...     30.0f;
    const value_type                    m_src_delay             = 30.0f;    //30.0f;
    const value_type                    m_src_width             = 10.0f;
    const size_type                     m_num_periods           = 10ULL;    //      WORKS = 100ULL;//4ULL;

    const value_type                    m_eta_0                 = spc::eta_0;
    const value_type                    m_loss                  = 0.0045f;  //      WORKS = 0.00f;
    const value_type                    m_perm                  = 7.0;      //      WORKS = 4.0f;


// *************************************************************************** //
//
//
//    CONSTRUCTORS, DESTRUCTOR, AND INITIALIZER FUNCTIONS ...
// *************************************************************************** //
public:

    //    Default Constructor.
    //
    inline FDTD_1D(void) noexcept
        :   m_base(),
            m_xvals     ( 0,    value_type(0.0f) ),
            m_Hy_T      ( 0,    re_array(0.0f) ),                           m_Ez_T      ( 0,    re_array(0.0f) ),
            m_Hy_F_NORM ( 0,    re_array(0.0f) ),                           m_Ez_F_NORM ( 0,    re_array(0.0f) ),
            m_Hy_F      ( 0,    im_array(0, complex_t(0.0f, 0.0f)) ),       m_Ez_F      ( 0,    im_array(0, complex_t(0.0f, 0.0f)) )
    {
        // ...
    }
    
    
    /*inline FDTD_1D(void) noexcept : base(),
        m_xvals(NX, 0.0f),
        m_Hy_T( NT, re_array(NX, 0.0f) ),                           m_Ez_T( NT, re_array(NX, 0.0f) ),
        m_Hy_F_NORM( NT, re_array(NX, 0.0f) ),                      m_Ez_F_NORM( NT, re_array(NX, 0.0f) ),
        m_Hy_F( NT, im_array(NX, complex_t(0.0f, 0.0f)) ),          m_Ez_F( NT, im_array(NX, complex_t(0.0f, 0.0f)) )
    {
        //this->m_material_width     = size_type(4*this->m_wavelen / ( std::sqrt(this->m_perm) )) - 1;
    

        //this->m_material_width        = 20ULL;
        this->initialize();
    }*/
    
    




    //    Default Destructor.
    //
    inline virtual ~FDTD_1D(void) = default;


// *************************************************************************** //
//
//
//    OVERLOADED OPERATORS ...
// *************************************************************************** //
// *************************************************************************** //



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
        
        return;
    }
    

// *************************************************************************** //
//
//
//
//    PROTECTED MEMBER FUNCTIONS ...
// *************************************************************************** //
// *************************************************************************** //
protected:

    //    "init_interface"
    //
    inline void init_interface(void) noexcept
    {
    /*
        size_type    m = 0ULL;

        //    0.    Setting permittivity and permeability.
        for (m = 0ULL; m < NX; ++m) {
            m_base.m_mu_r[m]            = complex_t(1.0, 0.0);

            if (m < this->m_material_pos)//        REGION 1:    FREE-SPACE.
                m_base.m_eps_r[m]    = complex_t(1.0f, 0.0);
            else//                                REGION 2:    DIELEC.
                m_base.m_eps_r[m]    = complex_t(this->m_perm, 0.0);
        }

        //    1.    E-field Update Coeffs.
        for (m = 0ULL; m < NX; ++m) {
            m_base.m_Hy[m]        = 0.0f;
            m_base.m_Ez[m]        = 0.0f;

            if (m < this->m_material_pos ) {//        REGION 1:    FREE-SPACE.
                m_base.m_cezE[m]                 = 1.0f;
                m_base.m_cezH[m]                 = this->m_eta_0;
            }
            else if (m < this->m_loss_layer) {//    REGION 2:    DIELEC.
                m_base.m_cezE[m]                 = 1.0f;
                m_base.m_cezH[m]                 = this->m_eta_0 / this->m_base.m_eps_r[m].real();
            }
            else {//                                REGION 3:    LOSSY-LAYER.
                value_type            re_eps    = this->m_base.m_eps_r[m].real();
                m_base.m_cezE[m]                 = (1.0 - this->m_loss)/(1.0 + this->m_loss);
                m_base.m_cezH[m]                 = this->m_eta_0 / this->m_perm / (1.0 + this->m_loss);

                this->m_base.m_eps_r[m]          = complex_t(re_eps, this->m_loss);
            }
        }

        //    2.    H-field Update Coeffs.
        for (m = 0ULL; m < NX; ++m) {
            if (m < this->m_loss_layer) {//         REGION 1:    FREE-SPACE.
                m_base.m_chyH[m]     = 1.0f;
                m_base.m_chyE[m]     = 1.0f / this->m_eta_0;
            }
            else {//                                REGION 2:    LOSSY-LAYER.
                m_base.m_chyH[m]     = (1.0f - this->m_loss)/(1.0f + this->m_loss);
                m_base.m_chyE[m]     = 1.0f / this->m_eta_0 / (1.0f + this->m_loss);
            }
        }
    */
        return;
    }


    //    "init_grid"
    //
    inline void init_grid(void) noexcept {
    /*
        this->thin_film();
    */
        return;
    }
    

// *************************************************************************** //
//
//
//
//    MAIN FDTD FUNCTIONS ...
// *************************************************************************** //
// *************************************************************************** //

    //    "update_H"
    //
    inline void update_H(void)
    {
        for (size_type m = 0ULL; m < NX-1; ++m) {
                m_base.m_Hy[m] = m_base.m_chyH[m] * m_base.m_Hy[m] +
                                m_base.m_chyE[m] * (m_base.m_Ez[m + 1] - m_base.m_Ez[m]);
        }

        return;
    }


    //    "update_E"
    //
    inline void update_E(void)
    {
        for (size_type m = 1ULL; m < NX-1; ++m) {
                m_base.m_Ez[m] = m_base.m_cezE[m] * m_base.m_Ez[m] +
                                m_base.m_cezH[m] * (m_base.m_Hy[m] - m_base.m_Hy[m - 1]);
        }

        return;
    }

    //    "abc_1"
    //
    inline void abc_1(void) {
        m_base.m_Hy[NX-2] = m_base.m_Hy[NX-1];
        return;
    }

    //    "abc_2"
    //
    inline void abc_2(void) {
        m_base.m_Ez[0] = m_base.m_Ez[1];
        return;
    }


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
        const size_type         pos             = this->m_src_pos;
        const size_type         q_total         = size_type(this->m_num_periods * this->m_src_delay);
        const value_type        tau             = (2 * spc::pi / this->m_wavelen);
        const value_type        t_soft          = q_total / (this->m_num_periods);
        value_type              temp            = std::sqrt( m_base.m_mu_r[pos].real() * m_base.m_eps_r[pos].real() );
        value_type              arg             = tau * (Sc * q - temp*m );

        //    Branchless "soft" ON-switch to gently ramp up the source amplitude.
        value_type              ON              = 1.0f - (q >= q_total);

        //    LAMBDA:    Compute quadratic smoothing from 0.0 ===> 1.0...
        auto soft_start = [](value_type x, const value_type t_final) {
            value_type    x_norm    = x / t_final;
            x_norm                  = x_norm - (x_norm > 1.0) * (x_norm - 1.0);
            return x_norm;
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
        m_base.m_Hy[this->m_TFSF_boundary - 1]    -= this->source(Sc, q, 0.0f) * m_base.m_chyE[this->m_TFSF_boundary];
        m_base.m_Ez[this->m_TFSF_boundary]        += this->source(Sc, q + 0.5f, -0.5f);
        return;
    }

    //    "hard_source"
    //
    inline void hard_source(const size_type q)
    {
        const value_type    Sc            = 1.0f;
        m_base.m_Ez[this->m_src_pos]       += this->gaussian_source(Sc, q, 0.0f);

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
    inline im_array & get_eps_r(void)
    { return this->m_base.m_eps_r; }
    


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
        size_type        m = 0ULL,     q = 0ULL;
        this->init_grid();
        
    


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
                this->m_Ez_T[q][m] = m_base.m_Ez[m];
                this->m_Hy_T[q][m] = m_base.m_Hy[m];
            }
        }


        //    3.    COMPUTING DISCRETE FOURIER TRANSFORM...
        for (q = 0ULL; q < NT; ++q) {
            this->m_Hy_F[q] = real_DFT<value_type>(this->m_Hy_T[q]);
            this->m_Ez_F[q] = real_DFT<value_type>(this->m_Ez_T[q]);
        }
    
    
        // 4. NORMALISE DFT magnitudes (real, 0‒1) -------------------------
        //using complex_t = typename m_base.complex_t;
        for (q = 0ULL; q < NT; ++q) {
            m_Hy_F_NORM[q] = normalize_spectrum<complex_t>( m_Hy_F[q] );
            m_Ez_F_NORM[q] = normalize_spectrum<complex_t>( m_Ez_F[q] );
        }
    
    
    
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
} }//   END OF "cb" :: "fdtd" NAMESPACE.












#endif      //  _CB_FDTD_1D_H  //
// *************************************************************************** //
// *************************************************************************** //
//
//  END.
