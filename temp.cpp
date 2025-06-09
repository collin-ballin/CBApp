


//  1.  MAIN FDTD FUNCTION...
    inline void run(void)
    {
        size_type        m = 0ULL,     q = 0ULL;
        this->init_grid();
 
        for (q=0ULL; q < NT; ++q) { //    MAIN FDTD LOOP...
            this->abc_1(); 		//    1.    SIMULATE...
            this->update_H();
            this->update_TFSF(q);
            this->abc_2();
            this->update_E();

            for (m = 0ULL; m < NX; ++m) { 		//    2.    COPYING DATA...
                this->m_Ez_T[q][m] = m_base.m_Ez[m];
                this->m_Hy_T[q][m] = m_base.m_Hy[m];
            }
        }

        for (q = 0ULL; q < NT; ++q) { 		//    3.    COMPUTING DISCRETE FOURIER TRANSFORM...
            this->m_Hy_F[q] = real_DFT<value_type>(this->m_Hy_T[q]);
            this->m_Ez_F[q] = real_DFT<value_type>(this->m_Ez_T[q]);
        }
   
        for (q = 0ULL; q < NT; ++q) { 		// 4. NORMALISE DFT magnitudes (real, 0‒1) -------------------------
            m_Hy_F_NORM[q] = normalize_spectrum<complex_t>( m_Hy_F[q] );
            m_Ez_F_NORM[q] = normalize_spectrum<complex_t>( m_Ez_F[q] );
        }
        return;
    }
    

//  2.  FDTD SIMULATION SETUP FUNCTIONS...
    inline void init_grid(void) noexcept {
        //  This function selects which simulation setup I wanted to use (at compile time). For now, let's only consider the thin_film().
        # ifdef CB_BAREBONES
            this->init_interface();
        # else
            this->thin_film();
        # endif    /*    CB_BAREBONES    */
        return;
    }
    
    inline void init_interface(void) noexcept { /* ... */ }
    
    inline void thin_film(void) noexcept {
        size_type    m = 0ULL;
        for (m = 0ULL; m < NX; ++m) {//    0.    Setting permittivity and permeability.
            base::m_mu_r[m]        = complex_t(1.0, 0.0);
            
            if (m >= this->m_material_pos &&
                m <= this->m_material_pos + this->m_material_width) {   //            REGION 2:    DIELEC.
                base::m_eps_r[m]    = complex_t(this->m_perm, 0.0);
            }
            else {//    REGION 1:    FREE-SPACE.
                base::m_eps_r[m]    = complex_t(1.0f, 0.0);
            }
        }
        for (m = 0ULL; m < NX; ++m) {//    1.    E-field Update Coeffs.
            base::m_Hy[m]        = 0.0f;
            base::m_Ez[m]        = 0.0f;

            if ( m < this->m_material_pos ||
                 m > this->m_material_pos + this->m_material_width ) {//            REGION 1:    FREE-SPACE.
                base::m_cezE[m]        = 1.0f;
                base::m_cezH[m]        = this->m_eta_0;
            }
            else {//    REGION 2:    DIELEC.
                base::m_cezE[m]            = 1.0f;
                base::m_cezH[m]            = this->m_eta_0 / this->base::m_eps_r[m].real();
            }
        }
        for (m = 0ULL; m < NX; ++m) {       //    2.    H-field Update Coeffs.
            if (m < this->m_loss_layer) {   //            REGION 1:    FREE-SPACE.
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
    
    
//  3.  FDTD ENGINE FUNCTIONS...
    inline void update_H(void) {
        for (size_type m = 0ULL; m < NX-1; ++m) {
                m_base.m_Hy[m] = m_base.m_chyH[m] * m_base.m_Hy[m] +
                                m_base.m_chyE[m] * (m_base.m_Ez[m + 1] - m_base.m_Ez[m]);
        }
        return;
    }
    
    inline void update_E(void) {
        for (size_type m = 1ULL; m < NX-1; ++m) {
                m_base.m_Ez[m] = m_base.m_cezE[m] * m_base.m_Ez[m] +
                                m_base.m_cezH[m] * (m_base.m_Hy[m] - m_base.m_Hy[m - 1]);
        }
        return;
    }
    

//  4.  FDTD BOUNDARY CONDITION FUNCTIONS...
    inline void update_TFSF(const size_type q) {
        const value_type    Sc    = 1.0f;
        m_base.m_Hy[this->m_TFSF_boundary - 1]    -= this->source(Sc, q, 0.0f) * m_base.m_chyE[this->m_TFSF_boundary];
        m_base.m_Ez[this->m_TFSF_boundary]        += this->source(Sc, q + 0.5f, -0.5f);
        return;
    }

    inline void abc_1(void) {
        m_base.m_Hy[NX-2] = m_base.m_Hy[NX-1];
        return;
    }

    inline void abc_2(void) {
        m_base.m_Ez[0] = m_base.m_Ez[1];
        return;
    }
    

//  5.  FDTD SOURCE FUNCTIONS...
    inline value_type source(const value_type Sc, const value_type q, const value_type m) {
        //  This function selects which source I wanted to use (at compile time). For now, let's only consider the harminic_source().
        //return gaussian_source(Sc, q, m); //return ricker_source(Sc, q, m);
        return harmonic_source(Sc, q, m);
    }

    inline value_type gaussian_source(const value_type Sc, const value_type q, const value_type m) { /* ... */ }

    inline value_type harmonic_source(const value_type Sc, const value_type q, const value_type m) {
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

    inline value_type ricker_source(const value_type Sc, const value_type q, const value_type m) { /* ... */ }











































