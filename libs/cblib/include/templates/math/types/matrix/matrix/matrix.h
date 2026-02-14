/***********************************************************************************
*
*       ********************************************************************
*       ****        M A T R I X / M A T R I X . H  ____  F I L E        ****
*       ********************************************************************
*
*              AUTHOR:      Collin A. Bond
*               DATED:      February 11, 2025.
*
*       ********************************************************************
*                FILE:      [./PyStream.h]
*
*	Description:
*		This file is an INTERNAL header file - meaning this header is int-
*	-ended to be included and used by other LIBRARY header files.  DO NOT
*	include this header file directly.  Instead, include the associated
*	LIBRARY header file which utilizes this header file, "matrix.h".
*
**************************************************************************************
**************************************************************************************/
#ifndef _CBLIB_MATH_TYPES_MATRIX_MATRIX_H
#define	_CBLIB_MATH_TYPES_MATRIX_MATRIX_H	    1

#pragma once

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <initializer_list>
#include <limits>
#include <memory>
#include <span>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>
#include <cmath>
#include <complex>






namespace cblib { namespace math {   //     BEGINNING NAMESPACE "cblib" :: "math"...
// *************************************************************************** //
// *************************************************************************** //

namespace matrix { //     BEGINNING NAMESPACE "matrix"...
// *************************************************************************** //
// *************************************************************************** //






// *************************************************************************** //
//
//
//
//      0.      SMALL_INTERNAL_NAMESPACE...
// *************************************************************************** //
// *************************************************************************** //
//
namespace impl { //     BEGINNING NAMESPACE "impl"...



// *************************************************************************** //
//      "impl" |    TYPE-TRAITS / META-PROGRAMMING.
// *************************************************************************** //
    template <class X>
    struct                          is_complex                          : std::false_type       {   };

    template <class U>
    struct                          is_complex<std::complex<U>>         : std::true_type        {   };



    template <class X>
    struct                          is_std_complex                      : std::false_type           {   };

    template <class U>
    struct                          is_std_complex<std::complex<U>>     : std::true_type            {   };

    template <class T_>
    using                           real_scalar_t                       = std::conditional_t<impl::is_complex<T_>::value, typename T_::value_type, T_>;



// *************************************************************************** //
//      "impl" |    CONCEPTS.
// *************************************************************************** //
    template <typename X>
    concept     matrix_element                  = std::is_arithmetic_v<X> || is_complex<X>::value;

    // Add to impl namespace (after existing contents)
    template <typename X>
    concept     numerical_matrix                = matrix_element<X> && std::is_floating_point_v<real_scalar_t<X>>;



// *************************************************************************** //
//      "impl" |    TYPES.
// *************************************************************************** //

//  "TextureData"
//      POD STRUCT FOR GPU/ImGui INTEROP (exact briefing spec)
//
template<typename T>
struct TextureData
{
    using                           size_type                       = T;
    //
    const void *                    data_ptr                        = nullptr;
    size_type                       width                           = 0;
    size_type                       height                          = 0;
    size_type                       stride_bytes                    = 0;
};


//  "QRResult"
//      QRResult (nested POD struct for decomposition return)
//
template<class M>
struct QRResult
{
    using                           matrix_type                     = M;
    //
    matrix_type                       Q
                                    , R;
};


//  "EigenResult"
//      EigenResult (nested POD struct for eigen decomposition)
//
template<class M>
struct EigenResult
{
    using                           matrix_type                     = M;
    //
    matrix_type                       eigenvalues
                                    , eigenvectors;
};




//
// *************************************************************************** //
// *************************************************************************** //   END [ 1.0.  "TYPES" ].



// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
}// END NAMESPACE "impl".












// *************************************************************************** //
//
//
//
//      1.      PRIMARY MATRIX-CLASS IMPLEMENTATION...
// *************************************************************************** //
// *************************************************************************** //



// *************************************************************************** //
// *************************************************************************** //
//                PRIMARY CLASS INTERFACE:
// 		Class-Interface for the "Matrix" Abstraction.
// *************************************************************************** //
// *************************************************************************** //


template <typename T, typename Allocator = std::allocator<T>>
class Matrix
{
//      0.          CONSTANTS AND ALIASES...
// *************************************************************************** //
// *************************************************************************** //
public:

    // *************************************************************************** //
    //      0. |    NESTED TYPENAME ALIASES.
    // *************************************************************************** //
    //                      0A.     BASIC NESTED-ALIASES:
    using                               value_type                      = T;
    using                               allocator_type                  = Allocator;
    using                               container_type                  = std::vector<value_type, allocator_type>           ;
    using                               size_type                       = typename container_type::size_type                ;
    using                               difference_type                 = typename container_type::difference_type          ;
    using                               reference                       = typename container_type::reference                ;
    using                               const_reference                 = typename container_type::const_reference          ;
    using                               pointer                         = typename container_type::pointer                  ;
    using                               const_pointer                   = typename container_type::const_pointer            ;
    using                               iterator                        = typename container_type::iterator                 ;
    using                               const_iterator                  = typename container_type::const_iterator           ;
    using                               reverse_iterator                = typename container_type::reverse_iterator         ;
    using                               const_reverse_iterator          = typename container_type::const_reverse_iterator   ;
    //
    //
    //                      0B.     ALIASES FOR NESTED ABSTRACTIONS:
    using                               Alloc                           = Allocator;
    using                               TextureData                     = impl::TextureData     <size_type>     ;
    using                               QRResult                        = impl::QRResult        <Matrix>        ;
    using                               EigenResult                     = impl::EigenResult     <Matrix>        ;
    //
    //  template <class T>
    //  using                               Matrix                          = std::vector<T>;
    //  using                               const_reverse_iterator          = typename container_type::const_reverse_iterator;


    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      0. |    NESTED POD STRUCTS (INTEROP / RETURN TYPES).
    // *************************************************************************** //
    
    /*
    //      POD STRUCT FOR GPU/ImGui INTEROP (exact briefing spec)
    struct TextureData
    {
        const void *                    data_ptr                        = nullptr;
        size_type                       width                           = 0;
        size_type                       height                          = 0;
        size_type                       stride_bytes                    = 0;
    };

    //      QRResult (nested POD struct for decomposition return)
    struct QRResult
    {
        Matrix                           Q, R;
    };

    //      EigenResult (nested POD struct for eigen decomposition)
    struct EigenResult
    {
        Matrix                           eigenvalues, eigenvectors;
    };
    */
    
    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      0. |    STATIC CONSTEXPR CONSTANTS.
    // *************************************************************************** //
    static constexpr size_type          cv_MAX_SIZE                      = static_cast<size_type>(-1);



    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      0. |    INTERNAL VALIDATION / INTERNAL TYPE HELPERS.
    // *************************************************************************** //
private:
//
    static_assert   (   std::is_object_v<value_type>                                                                    );
    static_assert   (   !std::is_reference_v<value_type>                                                                );
    static_assert   (   std::is_same_v< typename std::allocator_traits<allocator_type>::value_type   , value_type >     );


    //      DETERMINANT / INVERSE SCALAR HELPERS...
    template <class X>
    struct                          is_std_complex                      : std::false_type           {   };

    template <class U>
    struct                          is_std_complex<std::complex<U>>     : std::true_type            {   };

    template <class T_>
    using                           real_scalar_t                       = std::conditional_t<impl::is_complex<T_>::value, typename T_::value_type, T_>;

    template <class T_>
    [[nodiscard]]
    static real_scalar_t<T_>        abs_scalar                          (const T_ & x) noexcept
    {
        using std::abs;     // ADL for complex
        return static_cast<real_scalar_t<T_>>(abs(x));
    }

//
//
// *************************************************************************** //
// *************************************************************************** //   END "0.  CONSTANTS AND ALIASES".



// *************************************************************************** //
//
//
//      1.          CLASS DATA-MEMBERS...
// *************************************************************************** //
// *************************************************************************** //
private:
//
    // *************************************************************************** //
    //      1. |    CORE DIMENSIONS / STORAGE.
    // *************************************************************************** //
    size_type                           m_rows                           = size_type{0};
    size_type                           m_cols                           = size_type{0};
    container_type                      m_data                           {   };

//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "1.  CLASS DATA-MEMBERS".



// *************************************************************************** //
//
//
//      2.A.        PUBLIC MEMBER FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //
public:

    // *************************************************************************** //
    //      2.A. |  INITIALIZATION METHODS.         |   "init.cpp" ...
    // *************************************************************************** //
                                        Matrix                           (void)                          = default;

    explicit                            Matrix                           (const allocator_type & alloc)
        : m_rows(0)
        , m_cols(0)
        , m_data(alloc)
    {   }

                                        Matrix                           (const size_type rows, const size_type cols)
        : m_rows(rows)
        , m_cols(cols)
        , m_data(_checked_mul(rows, cols), value_type{})
    {   }

                                        Matrix                           (const size_type rows, const size_type cols, const value_type & fill_value)
        : m_rows(rows)
        , m_cols(cols)
        , m_data(_checked_mul(rows, cols), fill_value)
    {   }

                                        Matrix                           (const size_type rows, const size_type cols, const allocator_type & alloc)
        : m_rows(rows)
        , m_cols(cols)
        , m_data(_checked_mul(rows, cols), value_type{}, alloc)
    {   }

                                        Matrix                           (const size_type rows, const size_type cols, const value_type & fill_value, const allocator_type & alloc)
        : m_rows(rows)
        , m_cols(cols)
        , m_data(_checked_mul(rows, cols), fill_value, alloc)
    {   }

                                        Matrix                           (std::initializer_list<std::initializer_list<value_type>> rows_init)
    {
        this->assign(rows_init);
    }

                                        Matrix                           (const Matrix &)                = default;
                                        Matrix                           (Matrix &&) noexcept            = default;

    Matrix &                            operator =                       (const Matrix &)                = default;
    Matrix &                            operator =                       (Matrix &&) noexcept            = default;


    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      2.A. |  STATIC FACTORIES.               |   ...
    // *************************************************************************** //
    [[nodiscard]]
    static Matrix                        zeros                           (const size_type rows, const size_type cols)
    {
        return Matrix(rows, cols, value_type{});
    }

    [[nodiscard]]
    static Matrix                        ones                            (const size_type rows, const size_type cols)
    {
        return Matrix(rows, cols, value_type{1});
    }

    [[nodiscard]]
    static Matrix                        constant                        (const size_type rows, const size_type cols, const value_type & v)
    {
        return Matrix(rows, cols, v);
    }

    [[nodiscard]]
    static Matrix                        identity                        (const size_type n)
    {
        Matrix out(n, n, value_type{});

        for (size_type i = 0; i < n; ++i)       { out(i, i) = value_type{1}; }

        return out;
    }


    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      2.A. |  BASIC OBSERVERS / INTEROP.      |   ...
    // *************************************************************************** //
    [[nodiscard]] size_type              rows                           (void) const noexcept           { return this->m_rows; }
    [[nodiscard]] size_type              cols                           (void) const noexcept           { return this->m_cols; }
    [[nodiscard]] size_type              size                           (void) const noexcept           { return this->m_data.size(); }
    [[nodiscard]] bool                   empty                          (void) const noexcept           { return this->m_data.empty(); }
    [[nodiscard]]
    size_type                            stride_bytes                   (void) const noexcept
    {
        return this->m_cols * sizeof(value_type);
    }
    [[nodiscard]] pointer                data                           (void) noexcept                 { return this->m_data.data(); }
    [[nodiscard]] const_pointer          data                           (void) const noexcept           { return this->m_data.data(); }
    [[nodiscard]]
    [[nodiscard]] TextureData           as_texture_data                 (void) const noexcept
    {
        TextureData         td      {   };

        td.data             = this->data();
        td.width            = static_cast<int>(this->m_cols);
        td.height           = static_cast<int>(this->m_rows);
        td.stride_bytes     = this->stride_bytes();

        return td;
    }
    [[nodiscard]] allocator_type         get_allocator                   (void) const                   { return this->m_data.get_allocator(); }


    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      2.A. |  ITERATORS.                      |   ...
    // *************************************************************************** //
    [[nodiscard]] iterator               begin                          (void) noexcept                 { return this->m_data.begin(); }
    [[nodiscard]] const_iterator         begin                          (void) const noexcept           { return this->m_data.begin(); }
    [[nodiscard]] const_iterator         cbegin                         (void) const noexcept           { return this->m_data.cbegin(); }

    [[nodiscard]] iterator               end                            (void) noexcept                 { return this->m_data.end(); }
    [[nodiscard]] const_iterator         end                            (void) const noexcept           { return this->m_data.end(); }
    [[nodiscard]] const_iterator         cend                           (void) const noexcept           { return this->m_data.cend(); }

    [[nodiscard]] reverse_iterator       rbegin                         (void) noexcept                 { return this->m_data.rbegin(); }
    [[nodiscard]] const_reverse_iterator rbegin                         (void) const noexcept           { return this->m_data.rbegin(); }
    [[nodiscard]] const_reverse_iterator crbegin                        (void) const noexcept           { return this->m_data.crbegin(); }

    [[nodiscard]] reverse_iterator       rend                           (void) noexcept                 { return this->m_data.rend(); }
    [[nodiscard]] const_reverse_iterator rend                           (void) const noexcept           { return this->m_data.rend(); }
    [[nodiscard]] const_reverse_iterator crend                          (void) const noexcept           { return this->m_data.crend(); }


    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      2.A. |  ELEMENT ACCESS.                 |   ...
    // *************************************************************************** //
    [[nodiscard]]
    reference                            operator ()                    (const size_type r, const size_type c)
    {
        this->_require_in_bounds(r, c);
        return this->m_data[_index(r, c, this->m_cols)];
    }

    [[nodiscard]]
    const_reference                      operator ()                    (const size_type r, const size_type c) const
    {
        this->_require_in_bounds(r, c);
        return this->m_data[_index(r, c, this->m_cols)];
    }

    [[nodiscard]] reference              at                             (const size_type r, const size_type c)                { return (*this)(r, c); }
    [[nodiscard]] const_reference        at                             (const size_type r, const size_type c) const          { return (*this)(r, c); }

    [[nodiscard]]
    std::span<value_type>                row_span                       (const size_type r)
    {
        if (r >= this->m_rows)              { throw std::out_of_range("Matrix: row out of range"); }
        return std::span<value_type>(this->data() + _index(r, 0, this->m_cols), this->m_cols);
    }

    [[nodiscard]]
    std::span<const value_type>          row_span                       (const size_type r) const
    {
        if (r >= this->m_rows)              { throw std::out_of_range("Matrix: row out of range"); }
        return std::span<const value_type>(this->data() + _index(r, 0, this->m_cols), this->m_cols);
    }

    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      2.A. |  CAPACITY / MODIFIERS.           |   ...
    // *************************************************************************** //
    void                                reserve                         (const size_type element_capacity)                  { this->m_data.reserve(element_capacity);   }
    [[nodiscard]] size_type             capacity                        (void) const noexcept                               { return this->m_data.capacity();           }

    void                                clear                           (void) noexcept
    {
        this->m_rows = 0;
        this->m_cols = 0;
        this->m_data.clear();
    }

    void                                reset                           (void)                                                // zero-fill existing elements (additive identity)
    {
        std::fill(this->m_data.begin(), this->m_data.end(), value_type{});
    }

    void                                fill                            (const value_type & v)
    {
        std::fill(this->m_data.begin(), this->m_data.end(), v);
    }

    void                                resize                          (const size_type rows, const size_type cols)
    {
        const size_type new_size = _checked_mul(rows, cols);

        this->m_data.resize(new_size, value_type{});
        this->m_rows = rows;
        this->m_cols = cols;
    }

    //      9b.       RESET (resize + zero-init) per briefing
    void                                reset                           (const size_type rows, const size_type cols)
    {
        this->resize(rows, cols);
        this->reset();                                              // zero-fill
    }

    void                                reshape_inplace                 (const size_type rows, const size_type cols)
    {
        const size_type new_size = _checked_mul(rows, cols);
        if (new_size != this->m_data.size())        { _throw_dim_mismatch("Matrix: reshape size mismatch"); }
        this->m_rows = rows;
        this->m_cols = cols;
    }

    void                                assign                          (std::initializer_list<std::initializer_list<value_type>> rows_init)
    {
        const size_type rows                = rows_init.size();
        size_type cols                      = 0;

        for (const auto & row : rows_init)
        {
            if (cols == 0)                  { cols = row.size(); }
            else if (row.size() != cols)    { _throw_dim_mismatch("Matrix: ragged initializer_list"); }
        }

        this->m_rows = rows;
        this->m_cols = cols;
        this->m_data.assign(_checked_mul(rows, cols), value_type{});

        size_type r = 0;
        for (const auto & row : rows_init)
        {
            size_type c = 0;
            for (const auto & v : row)
            {
                this->m_data[_index(r, c, this->m_cols)] = v;
                ++c;
            }
            ++r;
        }
    }

    void                                swap                            (Matrix & other) noexcept(std::is_nothrow_swappable_v<container_type>)
    {
        using std::swap;
        swap(this->m_rows, other.m_rows);
        swap(this->m_cols, other.m_cols);
        swap(this->m_data, other.m_data);
    }
    

    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      2.A. |  INTENT-DRIVEN OPS (RETURN NEW).  |   ...
    // *************************************************************************** //
    [[nodiscard]]
    Matrix                              transpose                       (void) const
    {
        Matrix out(this->m_cols, this->m_rows);

        for (size_type r = 0; r < this->m_rows; ++r)
        {
            for (size_type c = 0; c < this->m_cols; ++c)
            {
                out(c, r) = (*this)(r, c);
            }
        }

        return out;
    }
    [[nodiscard]] Matrix                add                             (const Matrix & rhs) const         { return (*this) + rhs; }
    [[nodiscard]] Matrix                sub                             (const Matrix & rhs) const         { return (*this) - rhs; }

    template <class S>
    [[nodiscard]]
    Matrix                              scale                           (S && scalar) const
        requires (!std::is_same_v<std::remove_cvref_t<S>, Matrix> && std::is_constructible_v<value_type, S>)
    {
        Matrix out(*this);
        out.scale_inplace(std::forward<S>(scalar));
        return out;
    }
    [[nodiscard]] Matrix                matmul                          (const Matrix & rhs) const         { return (*this) * rhs; }


    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      2.A. |  INPLACE OPS.                    |   ...
    // *************************************************************************** //
    void                                transpose_inplace               (void)
    {
        if (this->m_data.empty())               { return; }

        const size_type new_rows                = this->m_cols;
        const size_type new_cols                = this->m_rows;

        if (this->m_rows == this->m_cols)
        {
            for (size_type r = 0; r < this->m_rows; ++r)
            {
                for (size_type c = r + 1; c < this->m_cols; ++c)
                {
                    using std::swap;
                    swap((*this)(r, c), (*this)(c, r));
                }
            }
            return;
        }

        container_type tmp{};
        tmp.reserve(this->m_data.size());
        tmp.resize(this->m_data.size(), value_type{});

        for (size_type r = 0; r < this->m_rows; ++r)
        {
            for (size_type c = 0; c < this->m_cols; ++c)
            {
                tmp[_index(c, r, new_cols)] = (*this)(r, c);
            }
        }

        this->m_data.swap(tmp);
        this->m_rows = new_rows;
        this->m_cols = new_cols;
    }

    void                                add_inplace                     (const Matrix & rhs)
    {
        this->_require_same_shape(rhs, "Matrix: add shape mismatch");
        for (size_type i = 0; i < this->m_data.size(); ++i)     { this->m_data[i] += rhs.m_data[i]; }
    }

    void                                sub_inplace                     (const Matrix & rhs)
    {
        this->_require_same_shape(rhs, "Matrix: sub shape mismatch");
        for (size_type i = 0; i < this->m_data.size(); ++i)     { this->m_data[i] -= rhs.m_data[i]; }
    }

    template <class S>
    void                                scale_inplace                   (S && scalar)
        requires (!std::is_same_v<std::remove_cvref_t<S>, Matrix> && std::is_constructible_v<value_type, S>)
    {
        const value_type s = value_type(std::forward<S>(scalar));
        for (auto & v : this->m_data)               { v *= s; }
    }

    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      2.A. |  OPERATORS.                      |   ...
    // *************************************************************************** //
    Matrix &                            operator +=                     (const Matrix & rhs)               { this->add_inplace(rhs);    return *this; }
    Matrix &                            operator -=                     (const Matrix & rhs)               { this->sub_inplace(rhs);    return *this; }

    template <class S>
    Matrix &                            operator *=                     (S && scalar)
        requires (!std::is_same_v<std::remove_cvref_t<S>, Matrix> && std::is_constructible_v<value_type, S>)
    {
        this->scale_inplace(std::forward<S>(scalar));
        return *this;
    }

    [[nodiscard]]
    friend bool                         operator ==                     (const Matrix & a, const Matrix & b)
    {
        return (a.m_rows == b.m_rows) && (a.m_cols == b.m_cols) && (a.m_data == b.m_data);
    }

    [[nodiscard]]
    friend bool                         operator !=                     (const Matrix & a, const Matrix & b)
    {
        return !(a == b);
    }

    [[nodiscard]]
    friend Matrix                       operator +                      (const Matrix & a, const Matrix & b)
    {
        a._require_same_shape(b, "Matrix: operator+ shape mismatch");
        Matrix out(a.m_rows, a.m_cols);
        for (size_type i = 0; i < a.m_data.size(); ++i)     { out.m_data[i] = a.m_data[i] + b.m_data[i]; }
        return out;
    }

    [[nodiscard]]
    friend Matrix                       operator -                      (const Matrix & a, const Matrix & b)
    {
        a._require_same_shape(b, "Matrix: operator- shape mismatch");
        Matrix out(a.m_rows, a.m_cols);
        for (size_type i = 0; i < a.m_data.size(); ++i)     { out.m_data[i] = a.m_data[i] - b.m_data[i]; }
        return out;
    }

    [[nodiscard]]
    friend Matrix                       operator *                      (const Matrix & a, const Matrix & b)
        requires requires (value_type x) { x += x; x *= x; }
    {
        if (a.m_cols != b.m_rows)               { Matrix::_throw_dim_mismatch("Matrix: operator* matmul shape mismatch"); }

        Matrix out(a.m_rows, b.m_cols, value_type{});
        for (size_type i = 0; i < a.m_rows; ++i)
        {
            const size_type a_row = _index(i, 0, a.m_cols);
            const size_type c_row = _index(i, 0, out.m_cols);

            for (size_type k = 0; k < a.m_cols; ++k)
            {
                const value_type a_ik = a.m_data[a_row + k];
                const size_type b_row = _index(k, 0, b.m_cols);

                for (size_type j = 0; j < b.m_cols; ++j)
                {
                    out.m_data[c_row + j] += a_ik * b.m_data[b_row + j];
                }
            }
        }
        return out;
    }

    template <class S>
    [[nodiscard]]
    friend Matrix                       operator *                      (const Matrix & a, S && scalar)
        requires (!std::is_same_v<std::remove_cvref_t<S>, Matrix> && std::is_constructible_v<value_type, S>)
    {
        Matrix out(a);
        out.scale_inplace(std::forward<S>(scalar));
        return out;
    }

    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      2.A. |  ADVANCED OPS (DETERMINANT / INVERSE / ETC.). |   ...
    // *************************************************************************** //
    
    // Update determinant()
    [[nodiscard]] value_type            determinant                     (void) const
        requires impl::numerical_matrix<T>
    {
        //      1.        TYPE ALIASES + EARLY VALIDATION...
        using RealScalar = real_scalar_t<T>;

        static_assert(std::is_floating_point_v<RealScalar>,
            "Matrix::determinant(): T must be floating-point or std::complex<floating>");

        //      2.        SHAPE GUARDS...
        if (this->rows() != this->cols())
        {
            throw std::invalid_argument("Matrix::determinant(): matrix must be square");
        }

        const size_type n = this->rows();

        // Convention: det([]) = 1 (empty product, consistent with identity).
        if (n == 0)
        {
            return T(1);
        }

        //      3.        COPY INTO CONTIGUOUS SCRATCH BUFFER (row-major LU)...
        container_type lu(this->get_allocator());
        lu.resize(_checked_mul(n, n));

        RealScalar amax = RealScalar(0);    // global max |a_ij|

        for (size_type r = 0; r < n; ++r)
        {
            const size_type base = _index(r, 0, n);
            for (size_type c = 0; c < n; ++c)
            {
                const T v = (*this)(r, c);
                lu[base + c] = v;

                const RealScalar av = abs_scalar(v);
                if (av > amax)                  { amax = av; }
            }
        }

        // All-zero matrix → det = 0
        if (amax == RealScalar(0))
        {
            return T(0);
        }

        //      4.        SCALE-AWARE PIVOT THRESHOLD...
        const RealScalar eps = std::numeric_limits<RealScalar>::epsilon();
        const RealScalar piv_tol = eps * amax * static_cast<RealScalar>(n);

        bool odd_row_swaps = false;

        //      5.        IN-PLACE LU WITH PARTIAL PIVOTING (Doolittle-style, packed)...
        for (size_type k = 0; k < n; ++k)
        {
            // Find max-abs pivot in column k (rows k..n-1)
            size_type p = k;
            RealScalar p_abs = abs_scalar(lu[_index(k, k, n)]);

            for (size_type i = k + 1; i < n; ++i)
            {
                const RealScalar cand = abs_scalar(lu[_index(i, k, n)]);
                if (cand > p_abs)
                {
                    p_abs = cand;
                    p = i;
                }
            }

            // Near-singular detection
            if (p_abs <= piv_tol)
            {
                return T(0);
            }

            // Row swap if needed
            if (p != k)
            {
                T * row_k = lu.data() + _index(k, 0, n);
                T * row_p = lu.data() + _index(p, 0, n);
                std::swap_ranges(row_k, row_k + n, row_p);
                odd_row_swaps = !odd_row_swaps;
            }

            const T pivot = lu[_index(k, k, n)];

            // Eliminate below pivot
            for (size_type i = k + 1; i < n; ++i)
            {
                T &         aik     = lu[_index(i, k, n)];
                const T     factor  = aik / pivot;
                aik                 = factor;   // store L factor

                const size_type row_i = _index(i, 0, n);
                const size_type row_k = _index(k, 0, n);

                for (size_type j = k + 1; j < n; ++j)
                {
                    lu[row_i + j] -= factor * lu[row_k + j];
                }
            }
        }

        //      6.        ACCUMULATE DET FROM U DIAGONAL (mantissa/exponent guards)...
        RealScalar      mant        = RealScalar(1);
        int             exp2        = 0;
        T               phase       = T(1);

        for (size_type i = 0; i < n; ++i)
        {
            const T d = lu[_index(i, i, n)];
            const RealScalar mag = abs_scalar(d);

            if (mag == RealScalar(0))
            {
                return T(0);
            }

            int e = 0;
            RealScalar m = std::frexp(mag, &e);
            mant *= m;
            exp2 += e;

            phase *= (d / mag);

            // Renormalize mantissa
            int e_mant = 0;
            mant = std::frexp(mant, &e_mant);
            exp2 += e_mant;
        }

        if (odd_row_swaps)
        {
            phase = -phase;
        }

        // Exponent guards (overflow → inf, severe underflow → 0)
        const int max_exp = std::numeric_limits<RealScalar>::max_exponent;
        const int min_exp = std::numeric_limits<RealScalar>::min_exponent;

        if (exp2 > max_exp - 1)
        {
            const RealScalar inf = std::numeric_limits<RealScalar>::infinity();
            return phase * inf;
        }

        if (exp2 < min_exp - std::numeric_limits<RealScalar>::digits)
        {
            return T(0);
        }

        const RealScalar det_mag = std::ldexp(mant, exp2);
        return phase * det_mag;
    }
    

    //  "det"
    //
    [[nodiscard]] value_type            det                             (void) const
    {
        return this->determinant();
    }
    

    //  "inverse"
    //
    [[nodiscard]] Matrix                inverse                         (void) const
        requires impl::numerical_matrix<T>
    {
        //      1.        TYPE ALIASES + SHAPE VALIDATION...
        using Real = real_scalar_t<T>;

        if (this->m_rows != this->m_cols)
        {
            throw std::invalid_argument("Matrix::inverse(): matrix must be square");
        }

        const size_type n = this->m_rows;

        // 0×0 is a well-defined empty inverse.
        if (n == 0)
        {
            return Matrix{};
        }

        //      2.        ROW SCALING FACTORS + INFINITY NORM...
        std::vector<Real> row_scale(n, Real(0));
        Real norm_inf = Real(0);

        for (size_type i = 0; i < n; ++i)
        {
            Real row_max = Real(0);
            Real row_sum = Real(0);

            const size_type base = _index(i, 0, n);

            for (size_type j = 0; j < n; ++j)
            {
                const Real aij = abs_scalar(this->m_data[base + j]);
                row_sum += aij;
                if (aij > row_max)          { row_max = aij; }
            }

            if (row_sum > norm_inf)         { norm_inf = row_sum; }
            row_scale[i] = row_max;
        }

        //      3.        SINGULARITY TOLERANCE...
        const Real eps = std::numeric_limits<Real>::epsilon();
        const Real tol = eps * static_cast<Real>(n) * (norm_inf > Real(0) ? norm_inf : Real(1));

        //      4.        COPY A INTO LU WORKING BUFFER...
        container_type lu = this->m_data;

        //      5.        PIVOT BOOKKEEPING...
        std::vector<size_type> piv(n);
        for (size_type i = 0; i < n; ++i)   { piv[i] = i; }

        //      6.        LU FACTORIZATION WITH SCALED PARTIAL PIVOTING...
        for (size_type k = 0; k < n; ++k)
        {
            // Find pivot row p maximizing |lu[p,k]| / row_scale[p]
            size_type p = k;
            Real best = Real(0);

            for (size_type i = k; i < n; ++i)
            {
                const Real scaled = abs_scalar(lu[_index(i, k, n)]) / row_scale[i];
                if (scaled > best)
                {
                    best = scaled;
                    p = i;
                }
            }

            const Real pivot_abs = abs_scalar(lu[_index(p, k, n)]);
            if (pivot_abs <= tol)
            {
                throw std::domain_error("Matrix::inverse(): singular or ill-conditioned matrix");
            }

            // Row swap if needed
            if (p != k)
            {
                for (size_type j = 0; j < n; ++j)
                {
                    std::swap(lu[_index(k, j, n)], lu[_index(p, j, n)]);
                }
                std::swap(row_scale[k], row_scale[p]);
                std::swap(piv[k], piv[p]);
            }

            const T pivot = lu[_index(k, k, n)];

            // Elimination
            for (size_type i = k + 1; i < n; ++i)
            {
                T & aik = lu[_index(i, k, n)];
                aik = aik / pivot;                  // L(i,k) factor

                const T mult = aik;
                for (size_type j = k + 1; j < n; ++j)
                {
                    lu[_index(i, j, n)] -= mult * lu[_index(k, j, n)];
                }
            }
        }

        //      7.        SOLVE A X = I (n right-hand sides)...
        Matrix out(n, n);
        std::vector<T> y(n), x(n);

        for (size_type col = 0; col < n; ++col)
        {
            // Build permuted RHS b' = P * e_col
            for (size_type i = 0; i < n; ++i)
            {
                y[i] = (piv[i] == col) ? T(1) : T(0);
            }

            // Forward substitution: L y = b' (unit lower triangular)
            for (size_type i = 0; i < n; ++i)
            {
                T sum = y[i];
                for (size_type j = 0; j < i; ++j)
                {
                    sum -= lu[_index(i, j, n)] * y[j];
                }
                y[i] = sum;
            }

            // Back substitution: U x = y
            for (size_type ii = n; ii-- > 0; )
            {
                T sum = y[ii];
                for (size_type j = ii + 1; j < n; ++j)
                {
                    sum -= lu[_index(ii, j, n)] * x[j];
                }
                x[ii] = sum / lu[_index(ii, ii, n)];
            }

            // Write column into out (public API, respects encapsulation)
            for (size_type r = 0; r < n; ++r)
            {
                out(r, col) = x[r];
            }
        }

        return out;
    }

    void                                inverse_inplace                 (void)
    {
        // Compute into temporary to avoid aliasing hazards, then swap (per skeleton pattern).
        Matrix tmp = this->inverse();
        this->m_data.swap(tmp.m_data);
        std::swap(this->m_rows, tmp.m_rows);
        std::swap(this->m_cols, tmp.m_cols);
    }

    [[nodiscard]]
    Matrix                               minor                           (size_type row, size_type col) const
    {
        this->_require_in_bounds(row, col);

        const size_type new_rows = (this->m_rows > 0) ? this->m_rows - 1 : 0;
        const size_type new_cols = (this->m_cols > 0) ? this->m_cols - 1 : 0;

        Matrix out(new_rows, new_cols, this->get_allocator());

        size_type dr = 0;
        for (size_type sr = 0; sr < this->m_rows; ++sr)
        {
            if (sr == row) continue;

            T * dst_row = out.data() + _index(dr, 0, new_cols);
            const T * src_row = this->data() + _index(sr, 0, this->m_cols);

            // Left block: columns [0 .. col-1]
            if (col > 0)
            {
                std::copy_n(src_row, col, dst_row);
            }

            // Right block: columns [col+1 .. cols-1]
            if (col + 1 < this->m_cols)
            {
                const size_type right_len = this->m_cols - (col + 1);
                std::copy_n(src_row + (col + 1), right_len, dst_row + col);
            }

            ++dr;
        }

        return out;
    }

    [[nodiscard]]
    Matrix                               cofactor                        (size_type row, size_type col) const
    {
        if (this->m_rows != this->m_cols)
        {
            throw std::domain_error("Matrix::cofactor: requires square matrix");
        }
        this->_require_in_bounds(row, col);

        const size_type n = this->m_rows;

        // Special cases (empty product / 1×1 identity)
        if (n == 0 || n == 1)
        {
            return Matrix(1, 1, T(1));
        }

        const Matrix m = this->minor(row, col);
        T val = m.determinant();

        if (((row + col) & 1) != 0)
        {
            val = -val;
        }

        Matrix out(1, 1);
        out(0, 0) = val;
        return out;
    }

    [[nodiscard]]
    Matrix                               cofactor_matrix                 (void) const
    {
        if (this->m_rows != this->m_cols)
        {
            throw std::domain_error("Matrix::cofactor_matrix: requires square matrix");
        }

        const size_type n = this->m_rows;
        Matrix C(n, n);

        if (n == 0) return C;

        for (size_type i = 0; i < n; ++i)
        {
            for (size_type j = 0; j < n; ++j)
            {
                const Matrix m = this->minor(i, j);
                T val = m.determinant();

                if (((i + j) & 1) != 0)         { val = -val; }
                C(i, j) = val;
            }
        }

        return C;
    }

    [[nodiscard]]
    Matrix                               adjoint                         (void) const
    {
        if (this->m_rows != this->m_cols)
        {
            throw std::domain_error("Matrix::adjoint: requires square matrix");
        }

        const size_type n = this->m_rows;
        Matrix adj(n, n);

        if (n == 0) return adj;

        for (size_type i = 0; i < n; ++i)
        {
            for (size_type j = 0; j < n; ++j)
            {
                const Matrix m = this->minor(j, i);     // transpose effect of adjoint
                T val = m.determinant();

                if (((i + j) & 1) != 0)         { val = -val; }
                adj(i, j) = val;
            }
        }

        return adj;
    }

    [[nodiscard]]
    QRResult                             qr                              (void) const
    {
        Matrix Q(this->rows(), this->rows());
        Matrix R(this->rows(), this->cols());
        this->qr_inplace(Q, R);
        return QRResult{std::move(Q), std::move(R)};
    }


    //  "qr_inplace"
    //      Update qr_inplace() (and qr() via it)
    //
    void                                qr_inplace                      (Matrix & Q, Matrix & R) const
        requires impl::numerical_matrix<T>
    {
        //      1.        DIMENSION VALIDATION...
        const size_type m = this->rows();
        const size_type n = this->cols();

        if (Q.rows() != m || Q.cols() != m)
        {
            throw std::invalid_argument("Matrix::qr_inplace: Q must be m×m (full Q).");
        }
        if (R.rows() != m || R.cols() != n)
        {
            throw std::invalid_argument("Matrix::qr_inplace: R must be m×n.");
        }

        //      2.        COPY A INTO R (WORKSPACE) + INITIALIZE Q = I...
        for (size_type i = 0; i < m; ++i)
        {
            for (size_type j = 0; j < n; ++j)       { R(i, j) = (*this)(i, j); }
        }

        for (size_type i = 0; i < m; ++i)
        {
            for (size_type j = 0; j < m; ++j)
            {
                Q(i, j) = (i == j) ? T(1) : T(0);
            }
        }

        const size_type kmax = std::min(m, n);
        std::vector<long double> tau(kmax, 0.0L);

        //      3.        SCALED COLUMN NORM HELPER (STABILITY)...
        auto scaled_col_norm2 = [&](size_type k, size_type col) -> long double
        {
            long double scale = 0.0L;
            long double ssq   = 1.0L;

            for (size_type i = k; i < m; ++i)
            {
                const long double ax = std::abs(R(i, col));
                if (ax == 0.0L) continue;

                if (scale < ax)
                {
                    const long double t = scale / ax;
                    ssq = 1.0L + ssq * t * t;
                    scale = ax;
                }
                else
                {
                    const long double t = ax / scale;
                    ssq += t * t;
                }
            }

            return (scale == 0.0L) ? 0.0L : (scale * std::sqrt(ssq));
        };

        //      4.        HOUSEHOLDER REFLECTORS...
        for (size_type k = 0; k < kmax; ++k)
        {
            const long double norm_x = scaled_col_norm2(k, k);

            if (norm_x == 0.0L)
            {
                tau[k] = 0.0L;
                continue;
            }

            const T x0 = R(k, k);
            const long double ax0 = std::abs(x0);

            T s = (ax0 == 0.0L) ? T(1) : x0 / static_cast<decltype(x0)>(ax0);
            const T alpha = -s * static_cast<T>(norm_x);
            const T v0 = x0 - alpha;

            if (std::abs(v0) == 0.0L)
            {
                tau[k] = 0.0L;
                R(k, k) = alpha;
                continue;
            }

            R(k, k) = alpha;
            const T inv_v0 = T(1) / v0;

            long double sumsq = 0.0L;
            for (size_type i = k + 1; i < m; ++i)
            {
                const T ui = R(i, k) * inv_v0;
                R(i, k) = ui;
                const long double aui = std::abs(ui);
                sumsq += aui * aui;
            }

            tau[k] = 2.0L / (1.0L + sumsq);

            // Apply reflector to trailing columns
            for (size_type j = k + 1; j < n; ++j)
            {
                long double dot = static_cast<long double>(R(k, j));
                for (size_type i = k + 1; i < m; ++i)
                {
                    dot += std::conj(R(i, k)) * static_cast<long double>(R(i, j));
                }
                dot *= tau[k];

                const T w = static_cast<T>(dot);
                R(k, j) -= w;
                for (size_type i = k + 1; i < m; ++i)
                {
                    R(i, j) -= R(i, k) * w;
                }
            }
        }

        //      5.        BACKWARD ACCUMULATION OF Q FROM REFLECTORS...
        for (size_type kk = kmax; kk-- > 0; )
        {
            const long double t = tau[kk];
            if (t == 0.0L) continue;

            for (size_type j = 0; j < m; ++j)
            {
                long double dot = static_cast<long double>(Q(kk, j));
                for (size_type i = kk + 1; i < m; ++i)
                {
                    dot += std::conj(R(i, kk)) * static_cast<long double>(Q(i, j));
                }
                dot *= t;

                const T w = static_cast<T>(dot);
                Q(kk, j) -= w;
                for (size_type i = kk + 1; i < m; ++i)
                {
                    Q(i, j) -= R(i, kk) * w;
                }
            }
        }

        //      6.        ZERO STRICT LOWER TRIANGLE OF R (STANDARD OUTPUT)...
        for (size_type i = 0; i < m; ++i)
        {
            const size_type j_end = (i < n) ? i : n;
            for (size_type j = 0; j < j_end; ++j)
            {
                R(i, j) = T(0);
            }
        }
    }
    
    

    [[nodiscard]]
    EigenResult                          eigen                           (void) const
    {
        Matrix vals(this->rows(), 1);
        Matrix vecs(this->rows(), this->rows());
        this->eigen_inplace(vals, vecs);
        return EigenResult{std::move(vals), std::move(vecs)};
    }

    void                                eigen_inplace                   (Matrix & eigenvalues, Matrix & eigenvectors) const
    {
        //      1.        SHAPE VALIDATION...
        const size_type n = this->rows();

        if (n != this->cols())
        {
            throw std::invalid_argument("Matrix::eigen_inplace: matrix must be square");
        }

        //      2.        INITIALIZE EIGENVECTORS TO IDENTITY...
        for (size_type i = 0; i < n; ++i)
        {
            for (size_type j = 0; j < n; ++j)
            {
                eigenvectors(i, j) = (i == j) ? T(1) : T(0);
            }
        }

        //      3.        COPY DIAGONAL TO EIGENVALUES (INITIAL GUESS)...
        for (size_type i = 0; i < n; ++i)
        {
            eigenvalues(i, 0) = (*this)(i, i);
        }

        //      4.        WORK ON A COPY OF A...
        Matrix A = *this;

        const long double eps = std::numeric_limits<long double>::epsilon();
        const long double tol = eps * 100.0L;       // convergence tolerance factor

        //      5.        FIND LARGEST OFF-DIAGONAL |A[p][q]| FOR p < q...
        auto max_offdiag = [&](size_type & p_out, size_type & q_out) -> long double
        {
            long double maxVal = 0.0L;
            for (size_type p = 0; p < n; ++p)
            {
                for (size_type q = p + 1; q < n; ++q)
                {
                    long double val = std::fabs(static_cast<long double>(A(p, q)));
                    if (val > maxVal)
                    {
                        maxVal = val;
                        p_out = p;
                        q_out = q;
                    }
                }
            }
            return maxVal;
        };

        while (true)
        {
            size_type p = 0, q = 1;
            long double maxOff = max_offdiag(p, q);
            if (maxOff <= tol) break;               // converged

            long double app = static_cast<long double>(A(p, p));
            long double aqq = static_cast<long double>(A(q, q));
            long double apq = static_cast<long double>(A(p, q));

            long double tau = (aqq - app) / (2.0L * apq);
            long double t;
            if (tau >= 0)
            {
                t = 1.0L / (tau + std::sqrt(1.0L + tau * tau));
            }
            else
            {
                t = -1.0L / (-tau + std::sqrt(1.0L + tau * tau));
            }

            long double c = 1.0L / std::sqrt(1.0L + t * t);
            long double s = t * c;

            long double new_app = app - t * apq;
            long double new_aqq = aqq + t * apq;

            A(p, p) = static_cast<T>(new_app);
            A(q, q) = static_cast<T>(new_aqq);
            A(p, q) = T(0);
            A(q, p) = T(0);

            for (size_type i = 0; i < n; ++i)
            {
                if (i != p && i != q)
                {
                    long double aip = static_cast<long double>(A(i, p));
                    long double aiq = static_cast<long double>(A(i, q));
                    long double new_aip = aip * c - aiq * s;
                    long double new_aiq = aip * s + aiq * c;
                    A(i, p) = static_cast<T>(new_aip);
                    A(p, i) = static_cast<T>(new_aip);
                    A(i, q) = static_cast<T>(new_aiq);
                    A(q, i) = static_cast<T>(new_aiq);
                }

                long double vip = static_cast<long double>(eigenvectors(i, p));
                long double viq = static_cast<long double>(eigenvectors(i, q));
                long double new_vip = vip * c - viq * s;
                long double new_viq = vip * s + viq * c;
                eigenvectors(i, p) = static_cast<T>(new_vip);
                eigenvectors(i, q) = static_cast<T>(new_viq);
            }
        }

        //      6.        FINAL EIGENVALUES ARE DIAGONAL OF A...
        for (size_type i = 0; i < n; ++i)
        {
            eigenvalues(i, 0) = A(i, i);
        }
    }

    [[nodiscard]]
    Matrix                               rref                            (void) const
    {
        //      1.        DIMENSIONS + WORK COPY...
        const size_type m = this->rows();
        const size_type n = this->cols();

        Matrix R = *this;                               // full deep copy (respects allocator)

        //      2.        SCALE-AWARE TOLERANCE...
        long double max_entry = 0.0L;
        for (size_type i = 0; i < m; ++i)
        {
            for (size_type j = 0; j < n; ++j)
            {
                long double val = std::abs(static_cast<long double>(R(i, j)));
                if (val > max_entry)                    { max_entry = val; }
            }
        }

        const long double eps = std::numeric_limits<long double>::epsilon();
        long double tol = eps * static_cast<long double>(std::max(m, n))
                          * (max_entry > 0.0L ? max_entry : 1.0L);

        //      3.        ROW SCALING FACTORS...
        std::vector<long double> row_max(m, 0.0L);
        for (size_type i = 0; i < m; ++i)
        {
            for (size_type j = 0; j < n; ++j)
            {
                row_max[i] = std::max(row_max[i], std::abs(static_cast<long double>(R(i, j))));
            }
        }

        //      4.        MAIN GAUSS-JORDAN LOOP WITH SCALED PARTIAL PIVOTING...
        size_type piv_col = 0;
        for (size_type piv_row = 0; piv_row < m; ++piv_row)
        {
            // Find best pivot in current column (rows piv_row..m-1)
            size_type p = piv_row;
            long double best = 0.0L;
            for (size_type i = piv_row; i < m; ++i)
            {
                long double scaled = std::abs(static_cast<long double>(R(i, piv_col))) / row_max[i];
                if (scaled > best)
                {
                    best = scaled;
                    p = i;
                }
            }

            long double pivot = std::abs(static_cast<long double>(R(p, piv_col)));
            if (pivot <= tol)
            {
                ++piv_col;
                if (piv_col >= n) break;
                --piv_row;                              // retry this row on next column
                continue;
            }

            // Swap rows if needed
            if (p != piv_row)
            {
                for (size_type j = 0; j < n; ++j)
                {
                    std::swap(R(piv_row, j), R(p, j));
                }
                std::swap(row_max[piv_row], row_max[p]);
            }

            // Normalize pivot row to leading 1
            T pivot_val = R(piv_row, piv_col);
            for (size_type j = 0; j < n; ++j)
            {
                R(piv_row, j) /= pivot_val;
            }

            // Eliminate in EVERY other row (full Gauss-Jordan)
            for (size_type i = 0; i < m; ++i)
            {
                if (i == piv_row) continue;

                T factor = R(i, piv_col);
                for (size_type j = 0; j < n; ++j)
                {
                    R(i, j) -= factor * R(piv_row, j);
                }
            }

            ++piv_col;
            if (piv_col >= n) break;
        }

        return R;
    }

    void                                rref_inplace                    (void)
    {
        *this = this->rref();                           // matches .operation_inplace() convention
    }

    [[nodiscard]]
    size_type                           rank                             (void) const
    {
        if (this->empty()) return 0;

        Matrix R = this->rref();

        // Recompute tolerance for exact consistency with rref()
        const size_type m = R.rows();
        const size_type n = R.cols();
        long double max_entry = 0.0L;
        for (size_type i = 0; i < m; ++i)
        {
            for (size_type j = 0; j < n; ++j)
            {
                long double val = std::abs(static_cast<long double>(R(i, j)));
                if (val > max_entry)                    { max_entry = val; }
            }
        }

        const long double eps = std::numeric_limits<long double>::epsilon();
        long double tol = eps * static_cast<long double>(std::max(m, n))
                          * (max_entry > 0.0L ? max_entry : 1.0L);

        size_type r = 0;
        for (size_type i = 0; i < m; ++i)
        {
            bool zero_row = true;
            for (size_type j = 0; j < n; ++j)
            {
                if (std::abs(static_cast<long double>(R(i, j))) > tol)
                {
                    zero_row = false;
                    break;
                }
            }
            if (!zero_row) ++r;
        }
        return r;
    }

    [[nodiscard]]
    Matrix                               gram_schmidt                    (void) const
    {
        const size_type m = this->rows();
        const size_type n = this->cols();

        Matrix Q(m, n);
        Matrix A = *this;                           // working copy (respects allocator)

        for (size_type k = 0; k < n; ++k)
        {
            // Subtract projections onto all previous q_j
            for (size_type j = 0; j < k; ++j)
            {
                long double dot = 0.0L;
                for (size_type i = 0; i < m; ++i)
                {
                    dot += static_cast<long double>(Q(i, j)) * static_cast<long double>(A(i, k));
                }
                for (size_type i = 0; i < m; ++i)
                {
                    A(i, k) -= Q(i, j) * static_cast<T>(dot);
                }
            }

            // Normalize a_k → q_k
            long double norm = 0.0L;
            for (size_type i = 0; i < m; ++i)
            {
                long double val = static_cast<long double>(A(i, k));
                norm += val * val;
            }

            if (norm <= std::numeric_limits<long double>::epsilon())
            {
                // linear dependence
                for (size_type i = 0; i < m; ++i)   { Q(i, k) = T(0); }
            }
            else
            {
                long double invn = 1.0L / std::sqrt(norm);
                for (size_type i = 0; i < m; ++i)
                {
                    Q(i, k) = static_cast<T>(A(i, k) * invn);
                }
            }
        }
        return Q;
    }

    [[nodiscard]]
    Matrix                               gram_schmidt_modified           (void) const
    {
        // identical body to classical version per excerpt (both implement sequential subtraction)
        return this->gram_schmidt();
    }

    void                                gram_schmidt_inplace            (void)
    {
        const size_type m = this->rows();
        const size_type n = this->cols();

        for (size_type k = 0; k < n; ++k)
        {
            for (size_type j = 0; j < k; ++j)
            {
                long double dot = 0.0L;
                for (size_type i = 0; i < m; ++i)
                {
                    dot += static_cast<long double>((*this)(i, j)) * static_cast<long double>((*this)(i, k));
                }
                for (size_type i = 0; i < m; ++i)
                {
                    (*this)(i, k) -= (*this)(i, j) * static_cast<T>(dot);
                }
            }

            long double norm = 0.0L;
            for (size_type i = 0; i < m; ++i)
            {
                long double val = static_cast<long double>((*this)(i, k));
                norm += val * val;
            }

            if (norm <= std::numeric_limits<long double>::epsilon())
            {
                for (size_type i = 0; i < m; ++i)   { (*this)(i, k) = T(0); }
            }
            else
            {
                long double invn = 1.0L / std::sqrt(norm);
                for (size_type i = 0; i < m; ++i)
                {
                    (*this)(i, k) = static_cast<T>((*this)(i, k) * invn);
                }
            }
        }
    }

    void                                gram_schmidt_modified_inplace    (void)
    {
        // identical body to classical inplace version per excerpt
        this->gram_schmidt_inplace();
    }

    [[nodiscard]]
    T                                   dot                             (const Matrix & other) const
    {
        const size_type N = this->size();
        if (N != other.size())
        {
            throw std::invalid_argument("Matrix::dot: size mismatch");
        }

        const T * a = this->data();
        const T * b = other.data();

        long double sum = 0.0L;
        for (size_type i = 0; i < N; ++i)
        {
            sum += static_cast<long double>(a[i]) * static_cast<long double>(b[i]);
        }

        return static_cast<T>(sum);
    }

    [[nodiscard]]
    Matrix                               outer                           (const Matrix & other) const
    {
        const size_type m = this->size();
        const size_type n = other.size();

        Matrix result(m, n);

        const T * a = this->data();
        const T * b = other.data();

        for (size_type i = 0; i < m; ++i)
        {
            const T xi = a[i];
            for (size_type j = 0; j < n; ++j)
            {
                result(i, j) = xi * b[j];
            }
        }

        return result;
    }

    [[nodiscard]]
    Matrix                               hadamard                        (const Matrix & other) const
    {
        if (this->rows() != other.rows() || this->cols() != other.cols())
        {
            throw std::invalid_argument("Matrix::hadamard: dimension mismatch");
        }

        Matrix result(this->rows(), this->cols());

        for (size_type r = 0; r < this->rows(); ++r)
        {
            for (size_type c = 0; c < this->cols(); ++c)
            {
                result(r, c) = (*this)(r, c) * other(r, c);
            }
        }

        return result;
    }

    void                                dot_inplace                     (const Matrix & other)
    {
        throw std::runtime_error("Matrix::dot_inplace not supported: dot returns a scalar");
    }

//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "2A.  PUBLIC MEMBER FUNCS".



// *************************************************************************** //
//
//
//      2.B.        PRIVATE MEMBER FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //
private:

    // *************************************************************************** //
    //      2.B. |  INTERNAL VALIDATION + HELPERS.  |   ...
    // *************************************************************************** //
    [[nodiscard]]
    static constexpr bool                _mul_overflows                  (size_type a, size_type b) noexcept
    {
        if (a == 0)                     { return false; }
        if (b == 0)                     { return false; }
        if (a > (cv_MAX_SIZE / b))      { return true; }
        return false;
    }

    [[nodiscard]]
    static constexpr size_type           _checked_mul                    (size_type a, size_type b)
    {
        if (_mul_overflows(a, b))       { throw std::length_error("Matrix: size overflow"); }
        return a * b;
    }

    [[nodiscard]]
    static constexpr size_type           _index                          (size_type r, size_type c, size_type cols)
    {
        return _checked_mul(r, cols) + c;
    }

    [[noreturn]]
    static void                          _throw_dim_mismatch             (const char * what)
    {
        throw std::invalid_argument(what);
    }

    void                                _require_in_bounds              (size_type r, size_type c) const
    {
        if (r >= this->m_rows)          { throw std::out_of_range("Matrix: row out of range"); }
        if (c >= this->m_cols)          { throw std::out_of_range("Matrix: col out of range"); }
    }

    void                                _require_same_shape             (const Matrix & other, const char * what) const
    {
        if (this->m_rows != other.m_rows)   { _throw_dim_mismatch(what); }
        if (this->m_cols != other.m_cols)   { _throw_dim_mismatch(what); }
    }

//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "2B.  PRIVATE" FUNCTIONS.


// *************************************************************************** //
// *************************************************************************** //
};//	END "Matrix" INLINE CLASS DEFINITION.












// *************************************************************************** //
//
//
//
//      2.      EXTERNAL FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //


//  OPERATOR *      <Matrix, Matrix>
//
template <class T, class Allocator, class S>
[[nodiscard]]
Matrix<T, Allocator> operator * (S && scalar, const Matrix<T, Allocator> & a)
    requires (!std::is_same_v<std::remove_cvref_t<S>, Matrix<T, Allocator>> && std::is_constructible_v<T, S>)
{
    return a * std::forward<S>(scalar);
}


//  "swap"
//
template <class T, class Allocator>
void swap(Matrix<T, Allocator> & a, Matrix<T, Allocator> & b) noexcept(noexcept(a.swap(b)))
{
    a.swap(b);
}



//
//
//
// *************************************************************************** //
// *************************************************************************** //   END [[ 2.  "EXTERNAL FUNCTIONS" ]].








// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
}//   END OF "matrix" NAMESPACE.



// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
} }//   END OF "cblib" :: "math" NAMESPACE.












// *************************************************************************** //
// *************************************************************************** //
#endif 	//  _CBLIB_MATH_TYPES_MATRIX_MATRIX_H  //


