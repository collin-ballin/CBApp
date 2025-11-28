/**
 * @file            _ndring_buffer.h
 * @author          Collin A. Bond
 * @date            2025-11-11
 * @brief           Declaration-only scaffold for a dynamic, STL-like ring buffer (Step 0).
 *
 *
 * @details         No definitions here — this header only declares the public surface so we can
 *                  integrate it gradually and verify compilation before implementing behavior.
 *                  STL naming: value_type/size_type/difference_type, size/empty/capacity, data,
 *                  operator[], at, front/back, push_back/emplace_back, iterators incl. cbegin/cend.
 *                  Iteration order is intended to be chronological (oldest → newest).
 *
 * @note            [Any additional notes]
 * @warning         [Any important warnings]
 *
 *
 * @todo            Flush-out signal handlers to provide graceful application exits across a variety of unexpected termination events.
 *
 *
 *
 * @since           [Version when introduced]
 * @see             [Optional: related symbol]
 * @deprecated      [Use NewFunction() instead]
 */
/***********************************************************************************
*
*       ********************************************************************
*       ****       _ N D R I N G _ B U F F E R . H  ____  F I L E       ****
*       ********************************************************************
*
*              AUTHOR:      Collin A. Bond.
*               DATED:      November 11, 2025.
*
*       ********************************************************************
*                FILE:      [./PyStream.h]
*
*
*
**************************************************************************************
**************************************************************************************/
#ifndef _CBLIB_CONTAINERS_NDRINGBUFFER_H
#define _CBLIB_CONTAINERS_NDRINGBUFFER_H 1


//      1.          SYSTEM HEADERS...
#include <cstddef>          //  std::size_t, std::ptrdiff_t
#include <iterator>         //  iterator tags, std::reverse_iterator
#include <memory>           //  std::allocator, std::allocator_traits, std::to_address
#include <type_traits>      //  trait-based noexcept in signatures
#include <utility>          //  std::move, std::forward
#include <stdexcept>        //  std::out_of_range
#include <algorithm>        //  std::min
#include <iterator>         //  std::make_reverse_iterator




namespace cblib {   //     BEGINNING NAMESPACE "cblib"...
// *************************************************************************** //
// *************************************************************************** //






// *************************************************************************** //
// *************************************************************************** //
//                PRIMARY CLASS INTERFACE:
// 		Class-Interface for the "MyClass" Abstraction.
// *************************************************************************** //
// *************************************************************************** //

//  "ndRingBuffer"
//
/// @class      ndRingBuffer
/// @brief      Highly optimized, templated-type ring buffer for real-time plotting.
///
/// @tparam     T           element type
/// @tparam     Allocator   optional parameter to use container with a custom allocator
///
/// @note       Capacity is chosen at construction and does not change implicitly.
//
template<
      typename      T
    , typename 	    Allocator 	= std::allocator<T>
>
class ndRingBuffer
{
//      0.          CONSTANTS AND ALIASES...
// *************************************************************************** //
// *************************************************************************** //
public:

    // *************************************************************************** //
    //      0. |    NESTED TYPENAME ALIASES.
    // *************************************************************************** //
    //                              ALLOCATOR-AWARE CONTAINER TYPEDEFS:
	using 	                            allocator_type		            = Allocator;
    using                               storage_traits                  = std::allocator_traits<allocator_type>;
    using                               storage_pointer                 = typename storage_traits::pointer;   // may be fancy
    //
    //                              STANDARD-LIBRARY CONTAINER TYPEDEFS:
    using                               value_type                      = T;
    using                               size_type                       = std::size_t;
    using                               difference_type                 = std::ptrdiff_t;
    using                               reference                       = T &;                  //  now _mutable_
    using                               const_reference                 = const T &;
    using                               pointer                         = T *;
    using                               const_pointer                   = const T *;
    //
    //                              FORWARD DECLARATIONS:
    class                               iterator                        ;
    class                               const_iterator                  ;
    using                               reverse_iterator                = std::reverse_iterator<iterator>;
    using                               const_reverse_iterator          = std::reverse_iterator<const_iterator>;
    //
    //                              CUSTOM ALIASES:
    using                               Alloc                           = allocator_type;
    using	                            This				            = ndRingBuffer<T, Allocator>;
    
    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      0. |    STATIC CONSTEXPR CONSTANTS.
    // *************************************************************************** //
    
    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      0. |    INTERNAL TYPE DEFINITIONS.
    // *************************************************************************** //
    enum class                          Preserve : uint8_t {
          Latest = 0
        , Oldest
        , COUNT
    };
    
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
protected:
    
    // *************************************************************************** //
    //      1. |    STORAGE AND STATE VARIABLES.
    // *************************************************************************** //
    allocator_type                      m_alloc                         {   };
    storage_pointer                     m_storage                       {   };              //  points to contiguous T[N] or nullptr when capacity==0

    size_type                           m_capacity                      { 0 };
    size_type                           m_head                          { 0 };              //  index of next write (also oldest when full)
    size_type                           m_count                         { 0 };              //  number of valid elements (<= m_capacity)

    
//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "1.  CLASS DATA-MEMBERS".



// *************************************************************************** //
//
//
//      2A.        MEMBER FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //
public:
    
    // *************************************************************************** //
    //      2A. |  INITIALIZATION METHODS.          |   ...
    // *************************************************************************** //
                                        ndRingBuffer                (void) noexcept;                                    //  Def. Ctor.
    explicit                            ndRingBuffer                (const allocator_type & ) noexcept;                 //  Parametric Ctor.
    explicit                            ndRingBuffer                (const size_type );                                 //  Parametric Ctor.
                                        ndRingBuffer                (const size_type , const allocator_type & );        //  Parametric Ctor.
    //
                                        ~ndRingBuffer               (void);                                             //  Def. Dtor.
    
    
    // *************************************************************************** //
    //      2A. |  OTHER "RULE-OF ..." STUFF.       |   ...
    // *************************************************************************** //
                                        ndRingBuffer                (ndRingBuffer && ) noexcept;                //  Move Ctor.
    [[nodiscard]] inline This &         operator =                  (This && other) noexcept;                   //  Move-Assgn. Operator.
    
    
    // *************************************************************************** //
    //      2A. |  DELETED FUNCTIONS.               |   ...
    // *************************************************************************** //
                                        ndRingBuffer                (const ndRingBuffer & )     = delete;       //  Copy Ctor.
    ndRingBuffer &                      operator =                  (const ndRingBuffer & )     = delete;       //  Assgn. Operator.


    // *************************************************************************** //
    //      2A. |  PRIVATE UTILITIES.               |   ...
    // *************************************************************************** //
protected:
    inline storage_pointer                          _initialize         (const size_type );
    inline void                                     _destroy            (void);

//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "2A.  MEMBER FUNCTIONS".



// *************************************************************************** //
//
//
//      2B.        PUBLIC API FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //
public:
                            
    // *************************************************************************** //
    //      2B. |  CAPACITY / STATE QUERY.
    // *************************************************************************** //
    [[nodiscard]] inline size_type                  size                (void) const noexcept;
    [[nodiscard]] inline size_type                  capacity            (void) const noexcept;
    [[nodiscard]] inline bool                       empty               (void) const noexcept;
    
    
    // *************************************************************************** //
    //      2B. |  CONTIGUOUS ACCESS.
    // *************************************************************************** //
    [[nodiscard]] inline pointer                    data                (void)       noexcept;
    [[nodiscard]] inline const_pointer              data                (void) const noexcept;
    
    
    // *************************************************************************** //
    //      2B. |  ELEMENT ACCESSORS.
    // *************************************************************************** //
    [[nodiscard]] reference                         operator[]          (const size_type )       noexcept;
    [[nodiscard]] const_reference                   operator[]          (const size_type ) const noexcept;
    
    [[nodiscard]] reference                         at                  (const size_type );
    [[nodiscard]] const_reference                   at                  (const size_type ) const;
    
    [[nodiscard]] reference                         front               (void)       noexcept;
    [[nodiscard]] const_reference                   front               (void) const noexcept;
    
    [[nodiscard]] reference                         back                (void)       noexcept;
    [[nodiscard]] const_reference                   back                (void) const noexcept;
    
    [[nodiscard]] reference                         top                 (void)       noexcept;
    [[nodiscard]] const_reference                   top                 (void) const noexcept;
    //
    [[nodiscard]] inline const_pointer              raw                 (void)       noexcept   { return this->data(); }
    [[nodiscard]] inline const_pointer              raw                 (void) const noexcept   { return this->data(); }
    [[nodiscard]] inline size_type                  offset              (void) const noexcept   {
        return (m_capacity == 0)    ? size_type{0}
                                    : (m_head + m_capacity - m_count) % m_capacity;
    }
    
    
    // *************************************************************************** //
    //      2B. |  DATA MODIFIERS.
    // *************************************************************************** //
    void                                            clear               (void) noexcept;
    void                                            push_back           (const_reference && )   noexcept(std::is_nothrow_copy_assignable_v<value_type>);
    void                                            push_back           (value_type && )        noexcept(std::is_nothrow_copy_assignable_v<value_type>);
    template<class... Args>
    void                                            emplace_back        (Args&&... args)        noexcept(std::is_nothrow_constructible_v<value_type, Args...>);
    inline void                                     pop_front           (void) noexcept;
    inline void                                     pop_back            (void) noexcept;
    //
    void                                            set_capacity        (const size_type new_cap, const Preserve policy = Preserve::Latest);
    
    
    // *************************************************************************** //
    //      2B. |  ITERATORS.
    // *************************************************************************** //
    iterator                                        begin               (void)       noexcept;
    const_iterator                                  begin               (void) const noexcept;
    const_iterator                                  cbegin              (void) const noexcept;
    
    iterator                                        end                 (void)       noexcept;
    const_iterator                                  end                 (void) const noexcept;
    const_iterator                                  cend                (void) const noexcept;
    
    reverse_iterator                                rbegin              (void)       noexcept;
    const_reverse_iterator                          rbegin              (void) const noexcept;
    const_reverse_iterator                          crbegin             (void) const noexcept;
    
    reverse_iterator                                rend                (void)       noexcept;
    const_reverse_iterator                          rend                (void) const noexcept;
    const_reverse_iterator                          crend               (void) const noexcept;
                            
    // *************************************************************************** //
    //      2B. |  OTHER.
    // *************************************************************************** //
    void                                            swap                (ndRingBuffer & ) noexcept;
    [[nodiscard]] allocator_type                    get_allocator       (void) const noexcept;
    
//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "2B.  PUBLIC API FUNCS".


    
// *************************************************************************** //
//
//
//      2B.        PUBLIC INLINE MEMBER FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //
public:
//
    // *************************************************************************** //
    //      2B. |  CAPACITY HELPER-FUNCTIONS.
    // *************************************************************************** //
    //  [[nodiscard]] inline bool                       full            (void) const noexcept       { return count == N;    }
    //  [[nodiscard]] inline size_type                  size            (void) const noexcept       { return count;         }
    //  [[nodiscard]] inline bool                       empty           (void) const noexcept       { return count == 0;    }
    //  [[nodiscard]] inline constexpr size_type        capacity        (void) const noexcept       { return N;             }
    

    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      2B. |  ELEMENT ACCESSORS.
    // *************************************************************************** //
    //[[nodiscard]] inline reference                  operator[]      (size_type idx) noexcept;

    //[[nodiscard]] inline reference                front           (void)       noexcept       { return (*this)[0]; }
    //[[nodiscard]] inline const_reference          front           (void) const noexcept       { return (*this)[0]; }
    

    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      2B. |  DATA MODIFIERS.
    // *************************************************************************** //
    

    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      2B. |  RAW ACCESSORS  [ used for APIs like ImGui, ImPlot, etc -- pointer+stride overload ].
    // *************************************************************************** //
    //[[nodiscard]] inline const_pointer              raw             (void) const noexcept   { return buf; }
    //[[nodiscard]] inline size_type                  offset          (void) const noexcept   { return full() ? head : 0; }
    

    // *************************************************************************** //
    //
    // *************************************************************************** //
    //      2B. |  ITERATOR SUPPORT.
    // *************************************************************************** //
    //[[nodiscard]] inline iterator                   begin           (void) const noexcept               { return iterator(this, 0);        }
    //[[nodiscard]] inline iterator                   end             (void)   const noexcept             { return iterator(this, count);    }
    
//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "2B.  PUBLIC FUNCTIONS".

    
   
// *************************************************************************** //
//
//
//      2C.        PROTECTED INLINE MEMBER FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //
protected:
//
    // *************************************************************************** //
    //      2C. |  CENTRALIZED STATE MANAGEMENT FUNCTIONS.
    // *************************************************************************** //
    
    
    
    // *************************************************************************** //
    
//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "2C.  PROTECTED MEMBER FUNCS".






// *************************************************************************** //
// *************************************************************************** //
};//	END "ndRingBuffer" INLINE CLASS DEFINITION.



//
//
//
// *************************************************************************** //
// *************************************************************************** //   END [[ 0.  "CLASS INTERFACE" ]].












// *************************************************************************** //
//
//
//
//      2.      INTERNAL DEFINITIONS FOR "ndRingBuffer"...
// *************************************************************************** //
// *************************************************************************** //

//  "iterator"
//
template<class T, class Allocator>
class ndRingBuffer<T,Allocator>::iterator {
public:
    using                           iterator_category       = std::bidirectional_iterator_tag;
    using                           value_type              = typename ndRingBuffer::value_type;
    using                           difference_type         = typename ndRingBuffer::difference_type;
    using                           pointer                 = typename ndRingBuffer::pointer;
    using                           reference               = typename ndRingBuffer::reference;


    inline                          iterator            (void) noexcept                             : rb_{nullptr}  , idx_{0}   {  }
    inline                          iterator            (ndRingBuffer * rb, size_t i) noexcept      : rb_{rb}       , idx_{i}   {  }
    //
    inline reference                operator *          (void) const noexcept                               { return (*rb_)[idx_];                                  }
    inline pointer                  operator ->         (void) const noexcept                               { return std::addressof((*rb_)[idx_]);                  }
    inline iterator &               operator --         (void) noexcept                                     { --idx_; return *this;                                 }
    inline iterator                 operator --         (int) noexcept                                      { iterator tmp{*this}; --(*this); return tmp;           }
    inline iterator &               operator ++         (void) noexcept                                     { ++idx_; return *this;                                 }
    inline iterator                 operator ++         (int) noexcept                                      { iterator tmp{*this}; ++(*this); return tmp;           }
    friend inline bool              operator ==         (const iterator & a, const iterator & b) noexcept   { return ( (a.rb_ == b.rb_)  &&  (a.idx_ == b.idx_) );  }
    friend inline bool              operator !=         (const iterator & a, const iterator & b) noexcept   { return !(a == b);                                     }


private:
    ndRingBuffer *                  rb_             ;
    size_t                          idx_            ;
};



//  "const_iterator"
//
template<class T, class Allocator>
class ndRingBuffer<T,Allocator>::const_iterator {
public:
    using                           iterator_category       = std::bidirectional_iterator_tag;
    using                           value_type              = typename ndRingBuffer::value_type;
    using                           difference_type         = typename ndRingBuffer::difference_type;
    using                           pointer                 = typename ndRingBuffer::const_pointer;
    using                           reference               = typename ndRingBuffer::const_reference;

    inline                          const_iterator      (void) noexcept                                 : rb_{nullptr}  , idx_{0}           {  }
    inline                          const_iterator      (const ndRingBuffer * rb, size_t i) noexcept    : rb_{rb}       , idx_{i}           {  }
    inline                          const_iterator      (const iterator & it) noexcept                  : rb_{it.rb_}   , idx_{it.idx_}     {  }    //  implicit conversion from iterator


    inline reference                operator *          (void) const noexcept                                           { return (*rb_)[idx_];                                  }
    inline pointer                  operator ->         (void) const noexcept                                           { return std::addressof((*rb_)[idx_]);                  }
    inline iterator &               operator --         (void) noexcept                                     { --idx_; return *this;                                 }
    inline iterator                 operator --         (int) noexcept                                      { iterator tmp{*this}; --(*this); return tmp;           }
    inline const_iterator &         operator ++         (void) noexcept                                                 { ++idx_; return *this;                                 }
    inline const_iterator           operator ++         (int) noexcept                                                  { const_iterator tmp{*this}; ++(*this); return tmp;     }
    friend inline bool              operator ==         (const const_iterator & a, const const_iterator & b) noexcept   { return ( (a.rb_ == b.rb_)  &&  (a.idx_ == b.idx_) );  }
    friend inline bool              operator !=         (const const_iterator & a, const const_iterator & b) noexcept   { return !(a == b);                                     }


private:
    const ndRingBuffer *            rb_             ;
    size_t                          idx_            ;
};



//
//
//
// *************************************************************************** //
// *************************************************************************** //   END [[ 2.  "INTERNAL STUFF" ]].












// *************************************************************************** //
//
//
//
//      2A.        MAIN MEMBER FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //


    
// *************************************************************************** //
//      2A. MAIN |  CONSTRUCTORS / DESTRUCTOR.
// *************************************************************************** //

//  Default Ctor.
//
template<class T, class Allocator>
ndRingBuffer<T, Allocator>::ndRingBuffer(void) noexcept
    : m_alloc       {   }
    , m_storage     {   }
    , m_capacity    { 0 }
    , m_head        { 0 }
    , m_count       { 0 }
{  }


//  Parametric Ctor #1.     [ allocator ]
//
template<class T, class Allocator>
ndRingBuffer<T, Allocator>::ndRingBuffer(const allocator_type & alloc) noexcept
    : m_alloc       { alloc }
    , m_storage     {       }
    , m_capacity    {   0   }
    , m_head        {   0   }
    , m_count       {   0   }
{  }


//  Parametric Ctor #2.     [ capacity ]
//
template<class T, class Allocator>
ndRingBuffer<T, Allocator>::ndRingBuffer(const size_type capacity)
    : m_alloc       {   }
    , m_storage     {   }
    , m_capacity    { 0 }
    , m_head        { 0 }
    , m_count       { 0 }
{
    if (capacity) {
        m_storage   = _initialize(capacity);
        m_capacity  = capacity;
    }
}


//  Parametric Ctor #3.     [ allocator + capacity ]
//
template<class T, class Allocator>
ndRingBuffer<T, Allocator>::ndRingBuffer(const size_type capacity, const allocator_type & alloc)
    : m_alloc       { alloc }
    , m_storage     {       }
    , m_capacity    {   0   }
    , m_head        {   0   }
    , m_count       {   0   }
{
    if (capacity) {
        m_storage   = _initialize(capacity);
        m_capacity  = capacity;
    }
}


//  Dtor.
//
template<class T, class Allocator>
ndRingBuffer<T, Allocator>::~ndRingBuffer(void)
{
    this->_destroy();
}

    
    
// *************************************************************************** //
//      2A. MAIN |  OTHER "RULE-OF ..." STUFF.       |   ...
// *************************************************************************** //

//  Move Ctor.
//
template<class T, class Allocator>
ndRingBuffer<T, Allocator>::ndRingBuffer(ndRingBuffer && other) noexcept
    : m_alloc       { std::move(other.m_alloc)  }
    , m_storage     { other.m_storage           }
    , m_capacity    { other.m_capacity          }
    , m_head        { other.m_head              }
    , m_count       { other.m_count             }
{
    other.m_storage     = storage_pointer{};
    other.m_capacity    = 0;
    other.m_head        = 0;
    other.m_count       = 0;
}


//  Move Assignment Operator
//
template<class T, class Allocator>
inline auto
ndRingBuffer<T,Allocator>::operator = (ndRingBuffer && other) noexcept -> ndRingBuffer &
{
    if (this != &other)
    {
        this->_destroy();
        m_alloc             = std::move(other.m_alloc);
        m_storage           = other.m_storage;
        m_capacity          = other.m_capacity;
        m_head              = other.m_head;
        m_count             = other.m_count;

        other.m_storage     = storage_pointer{ };
        other.m_capacity    = 0;
        other.m_head        = 0;
        other.m_count       = 0;
    }
    return *this;
}
    
    
    
    
    

// *************************************************************************** //
//      2A. API |  PRIVATE UTILITIES.
// *************************************************************************** //

//  "_initialize"
//
template<class T, class Allocator>
inline auto
ndRingBuffer<T,Allocator>::_initialize(const size_type cap) -> ndRingBuffer::storage_pointer
{
    if (cap == 0) {
        return storage_pointer{};
    }
    storage_pointer  p   = storage_traits::allocate(m_alloc, cap);
    T *              raw = std::to_address(p);
    
    for (size_type i = 0; i < cap; ++i)
    {
        storage_traits::construct(m_alloc, raw + i);    // default-construct T
    }
    
    return p;
}



//  "_destroy"
//
template<class T, class Allocator>
inline auto
ndRingBuffer<T,Allocator>::_destroy(void) -> void
{
    if (m_storage)
    {
        T * raw = std::to_address(m_storage);
        for (size_type i = 0; i < m_capacity; ++i) {
            storage_traits::destroy(m_alloc, raw + i);
        }
        storage_traits::deallocate(m_alloc, m_storage, m_capacity);
        m_storage   = storage_pointer{};
    }
    m_capacity  = 0;
    m_head      = 0;
    m_count     = 0;
    
    return;
}
    


//
//
//
// *************************************************************************** //
// *************************************************************************** //   END [[ 2A.  "MAIN MEMBER FUNCTIONS" ]].












// *************************************************************************** //
//
//
//
//      2B.        PUBLIC API FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

                            
// *************************************************************************** //
//      2B. API |  CAPACITY / STATE QUERY.
// *************************************************************************** //


//  "size"
//
template<class T, class Allocator>
[[nodiscard]] inline auto ndRingBuffer<T,Allocator>::size(void) const noexcept -> size_type
{
    return m_count;
}


//  "capacity"
//
template<class T, class Allocator>
[[nodiscard]] inline auto ndRingBuffer<T,Allocator>::capacity(void) const noexcept -> size_type
{
    return m_capacity;
}


//  "empty"
//
template<class T, class Allocator>
[[nodiscard]] inline bool ndRingBuffer<T,Allocator>::empty(void) const noexcept
{
    return (m_count == 0);
}



// *************************************************************************** //
//      2B. API |  CONTIGUOUS ACCESS.
// *************************************************************************** //

//  "data"
//
template<class T, class Allocator>
[[nodiscard]] inline auto ndRingBuffer<T,Allocator>::data(void) noexcept -> pointer
{
    return (m_capacity != 0) ? std::to_address(m_storage) : nullptr;
}


//  "data"
//
template<class T, class Allocator>
[[nodiscard]] inline auto ndRingBuffer<T,Allocator>::data(void) const noexcept -> const_pointer
{
    return (m_capacity != 0)    ? std::to_address(m_storage)    : nullptr;
}



// *************************************************************************** //
//      2B. API |  ELEMENT ACCESSORS.
// *************************************************************************** //

//  Array Subscript Operator    [ ].
//
template<class T, class Allocator>
[[nodiscard]] inline auto ndRingBuffer<T,Allocator>::operator [] (const size_type i) noexcept -> reference
{
    // Pre: i < m_count (unchecked)
    const size_type start = (m_capacity == 0) ? 0 : (m_head + m_capacity - m_count) % m_capacity;
    size_type pos = start + i;
    if (pos >= m_capacity) { pos -= m_capacity; }
    return std::to_address(m_storage)[pos];
}


//  Array Subscript Operator    [ ].
//
template<class T, class Allocator>
[[nodiscard]] inline auto ndRingBuffer<T,Allocator>::operator [] (const size_type i) const noexcept -> const_reference
{
    // Pre: i < m_count (unchecked)
    const size_type start = (m_capacity == 0) ? 0 : (m_head + m_capacity - m_count) % m_capacity;
    size_type pos = start + i;
    if (pos >= m_capacity) { pos -= m_capacity; }
    return std::to_address(m_storage)[pos];
}


//  "at"
template<class T, class Allocator>
[[nodiscard]] inline auto ndRingBuffer<T,Allocator>::at(const size_type i) -> reference {
    if (i >= m_count)   { throw std::out_of_range{"ndRingBuffer::at: index out of range"}; }
    return (*this)[i];
}


//  "at"
template<class T, class Allocator>
[[nodiscard]] inline auto ndRingBuffer<T,Allocator>::at(const size_type i) const -> const_reference {
    if (i >= m_count)   { throw std::out_of_range{"ndRingBuffer::at: index out of range"}; }
    return (*this)[i];
}


//  "front"
template<class T, class Allocator>
[[nodiscard]] auto ndRingBuffer<T,Allocator>::front(void) noexcept -> reference                 { return (*this)[0]; }              //  Pre: m_count > 0


//  "front"
template<class T, class Allocator>
[[nodiscard]] auto ndRingBuffer<T,Allocator>::front(void) const noexcept -> const_reference     { return (*this)[0]; }              //  Pre: m_count > 0



//  "back"
template<class T, class Allocator>
[[nodiscard]] auto ndRingBuffer<T,Allocator>::back(void) noexcept -> reference                  { return (*this)[m_count - 1]; }    //  Pre: m_count > 0


//  "back"
template<class T, class Allocator>
[[nodiscard]] auto ndRingBuffer<T,Allocator>::back(void) const noexcept -> const_reference      { return (*this)[m_count - 1]; }    //  Pre: m_count > 0


//  "top"
template<class T, class Allocator>
[[nodiscard]] auto ndRingBuffer<T,Allocator>::top(void) noexcept -> reference                   { return this->back(); }


//  "top"
template<class T, class Allocator>
[[nodiscard]] auto ndRingBuffer<T,Allocator>::top(void) const noexcept -> const_reference       { return this->back(); }






// *************************************************************************** //
//      2B. API |  DATA MODIFIERS.
// *************************************************************************** //

//  "clear"
//
template<class T, class Allocator>
void ndRingBuffer<T,Allocator>::clear(void) noexcept {
    m_head  = 0;        //  ** LOGICAL CLEAR ONLY **
    m_count = 0;        //  Reset indices so existing elements will be overwritten; DO NOT DEALLOCATE...
    return;
}


//  "push_back"
//
template<class T, class Allocator>
void ndRingBuffer<T,Allocator>::push_back(const_reference && item) noexcept(std::is_nothrow_copy_assignable_v<value_type>)
{
    T * const base  = std::to_address(m_storage);

    if (m_capacity == 0)            { return; }                                 //  Precondition violation in spirit; defined as a no-op to avoid UB.


    base[m_head]    = item;                                                     //  copy-assign into existing object
    m_head          = (m_head + 1 == m_capacity)    ? 0     : (m_head + 1);     //  advance head, update count
    if (m_count < m_capacity)       { ++m_count; }
    return;
}


//  "push_back"
//
template<class T, class Allocator>
void ndRingBuffer<T,Allocator>::push_back(value_type && item) noexcept(std::is_nothrow_copy_assignable_v<value_type>)
{
    T * const base  = std::to_address(m_storage);
    
    if (m_capacity == 0)            { return; }                                 //  Precondition violation in spirit; defined as a no-op to avoid UB.

    base[m_head]    = std::move(item);                                          //  move-assign into existing object
    m_head          = (m_head + 1 == m_capacity)    ? 0     : (m_head + 1);     //  advance head, update count
    if (m_count < m_capacity)       { ++m_count; }
    return;
}


//  "emplace_back"
//
template<class T, class Allocator>
template<class... Args>
void ndRingBuffer<T,Allocator>::emplace_back(Args&&... args) noexcept(std::is_nothrow_constructible_v<value_type, Args...>)
{
    T * const base = std::to_address(m_storage);
    
    if (m_capacity == 0)            { return; }                                         //  Precondition violation in spirit; defined as a no-op to avoid UB.

    storage_traits::destroy(m_alloc, base + m_head);                                    //  Reconstruct in-place to avoid an intermediate temporary:
    storage_traits::construct(m_alloc, base + m_head, std::forward<Args>(args)...);     //  destroy current object at m_head, then placement-construct with args...
    m_head  = (m_head + 1 == m_capacity)    ? 0     : (m_head + 1);                     //  advance head, update count
    if (m_count < m_capacity)       { ++m_count; }
    return;
}


//  "pop_front"
//
template<class T, class Allocator>
inline void ndRingBuffer<T,Allocator>::pop_front(void) noexcept
{
    // Pre: m_count > 0. We treat empty as a no-op for safety.
    if (m_count == 0) { return; }
    // Oldest element logically removed by reducing size;
    // start index advances implicitly via the general start formula.
    --m_count;
}


//  "pop_back"
//
template<class T, class Allocator>
inline void ndRingBuffer<T,Allocator>::pop_back(void) noexcept
{
    // Pre: m_count > 0. We treat empty as a no-op for safety.
    if (m_count == 0) { return; }
    // Newest element is at (m_head + cap - 1) % cap.
    // After removing it, move head back by one so next write overwrites that slot.
    if (m_head == 0) { m_head = m_capacity ? (m_capacity - 1) : 0; }
    else             { --m_head; }
    --m_count;
}


//  "set_capacity"
//
template<class T, class Allocator>
void ndRingBuffer<T,Allocator>::set_capacity(const size_type new_cap, const Preserve policy)
{
    storage_pointer     new_storage     {   };


    if (new_cap == m_capacity)          { return; }


    //      1.      FAST-PATH TO EMPTY...
    if (new_cap == 0)
    {
        //      Destroy old storage after committing to empty state
        auto    old_storage     = m_storage;
        auto    old_capacity    = m_capacity;
        m_storage               = storage_pointer{};
        m_capacity              = 0;
        m_head                  = 0;
        m_count                 = 0;

        if (old_storage)
        {
            pointer     raw         = std::to_address(old_storage);
            for (size_type i = 0; i < old_capacity; ++i) {
                storage_traits::destroy(m_alloc, raw + i);
            }
            storage_traits::deallocate(m_alloc, old_storage, old_capacity);
        }
        return;
    }

    //      Allocate and default-construct the new block (keeps our invariant that
    //      all slots are constructed, matching push/assign semantics).
    try
    {
        new_storage                         = storage_traits::allocate(m_alloc, new_cap);
        pointer                 new_raw     = std::to_address(new_storage);
        for (size_type i = 0; i < new_cap; ++i) {
            storage_traits::construct(m_alloc, new_raw + i);
        }

        // Decide how many elements to preserve and from where.
        const size_type         keep        = std::min(m_count, new_cap);
        const size_type         src_cap     = m_capacity;
        const bool              has_src     = ( (src_cap != 0)  &&  (m_storage != storage_pointer{}) );


        if (keep  &&  has_src)
        {
            const size_type         start       = (m_capacity == 0)     ? 0     : (m_head + m_capacity - m_count) % m_capacity;
            size_type               first       = start; // Oldest by default
            
            //      3.      COMPUTE THE FIRST PRESERVED LOGICAL INDEX FOR THE CURRENT POLICY...
            if (policy == Preserve::Latest)
            {
                const size_type     skip    = m_count - keep;                 // number of oldest elements to drop
                first                       = start + skip;
                if (first >= m_capacity)    { first -= m_capacity; }
            }
            pointer         dst         = new_raw;
            const_pointer   src         = std::to_address(m_storage);


            //  Assign into the new block in chronological order [0, keep).
            //  Use move-if-noexcept style to preserve strong guarantee:
            //      - If move-construct/assign is nothrow OR copying is impossible, move.
            //      - Otherwise, copy to avoid altering the source on potential throw.
            size_type       constructed = 0;
            try
            {
                for (size_type i = 0; i < keep; ++i)
                {
                    size_type       pos         = first + i;
                    if (pos >= m_capacity)      { pos -= m_capacity; }

                    if constexpr (std::is_nothrow_move_assignable_v<T> || !std::is_copy_assignable_v<T>) {
                        dst[i] = std::move(src[pos]);
                    }
                    else    { dst[i] = src[pos]; }
                    ++constructed;
                }
            }
            //
            //      CASER 2 :   ROLL-BACK THE NEW BLOCK...
            catch (...)
            {
                for (size_type i = 0; i < new_cap; ++i) {
                    storage_traits::destroy(m_alloc, new_raw + i);
                }
                storage_traits::deallocate(m_alloc, new_storage, new_cap);
                throw;
            }
        }

        // Commit: swap in the new block; compute new head/count
        auto    old_storage     = m_storage;
        auto    old_capacity    = m_capacity;

        m_storage               = new_storage;
        m_capacity              = new_cap;
        m_count                 = std::min(m_count, new_cap);
        m_head                  = (m_count < m_capacity)    ? m_count   : 0;    //  next write; makes start==0

        //  Destroy old block
        if (old_storage)
        {
            T* raw = std::to_address(old_storage);
            for (size_type i = 0; i < old_capacity; ++i) {
                storage_traits::destroy(m_alloc, raw + i);
            }
            storage_traits::deallocate(m_alloc, old_storage, old_capacity);
        }
    }
    catch (...)
    {
        // Allocation/construct failure: leave *this unchanged.
        throw;
    }
}






// *************************************************************************** //
//      2B. API |  ITERATORS.
// *************************************************************************** //

//  "begin"
template<class T, class Allocator>
auto ndRingBuffer<T,Allocator>::begin(void) noexcept -> iterator
{
    return iterator{this, 0};
}

//  "begin"
template<class T, class Allocator>
auto ndRingBuffer<T,Allocator>::begin(void) const noexcept -> const_iterator
{
    return const_iterator{this, 0};
}

//  "cbegin"
template<class T, class Allocator>
auto ndRingBuffer<T,Allocator>::cbegin(void) const noexcept -> const_iterator
{
    return const_iterator{this, 0};
}



//  "end"
template<class T, class Allocator>
auto ndRingBuffer<T,Allocator>::end(void) noexcept -> iterator
{
    return iterator{this, m_count};
}

//  "end"
template<class T, class Allocator>
auto ndRingBuffer<T,Allocator>::end(void) const noexcept -> const_iterator
{
    return const_iterator{this, m_count};
}

//  "cend"
template<class T, class Allocator>
auto ndRingBuffer<T,Allocator>::cend(void) const noexcept -> const_iterator
{
    return const_iterator{this, m_count};
}



//  "rbegin"
template<class T, class Allocator>
auto ndRingBuffer<T,Allocator>::rbegin() noexcept -> reverse_iterator
{
    return reverse_iterator(end());     //   std::make_reverse_iterator(end());
}

//  "rbegin"
template<class T, class Allocator>
auto ndRingBuffer<T,Allocator>::rbegin(void) const noexcept -> const_reverse_iterator
{
    return const_reverse_iterator{end()};
}

//  "crbegin"
template<class T, class Allocator>
auto ndRingBuffer<T,Allocator>::crbegin(void) const noexcept -> const_reverse_iterator
{
    return const_reverse_iterator{cend()};
}



//  "rend"      (non-const)
template<class T, class Allocator>
auto ndRingBuffer<T,Allocator>::rend(void) noexcept -> reverse_iterator
{
    return reverse_iterator(begin());
}


//  "rend"      (const)
template<class T, class Allocator>
auto ndRingBuffer<T,Allocator>::rend(void) const noexcept -> const_reverse_iterator
{
    return const_reverse_iterator{begin()};
}

//  "crend"
template<class T, class Allocator>
auto ndRingBuffer<T,Allocator>::crend(void) const noexcept -> const_reverse_iterator
{
    return const_reverse_iterator{cbegin()};
}
               
               
 
 
 
 
// *************************************************************************** //
//      2B. API |  OTHER.
// *************************************************************************** //

//  "swap"
//
template<class T, class Allocator>
void ndRingBuffer<T,Allocator>::swap(ndRingBuffer & other) noexcept
{
    std::swap    (m_alloc       , other.m_alloc     );
    std::swap    (m_storage     , other.m_storage   );
    std::swap    (m_capacity    , other.m_capacity  );
    std::swap    (m_head        , other.m_head      );
    std::swap    (m_count       , other.m_count     );
    
    return;
}


//  "get_allocator"
//
template<class T, class Allocator>
[[nodiscard]] auto ndRingBuffer<T,Allocator>::get_allocator(void) const noexcept -> allocator_type
{
    return m_alloc;
}

    

//
//
//
// *************************************************************************** //
// *************************************************************************** //   END [[ 2B.  "PUBLIC API" ]].









// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
}//   END OF "cblib" NAMESPACE.







// *************************************************************************** //
// *************************************************************************** //
#endif  //  _CBLIB_CONTAINERS_NDRINGBUFFER_H  //
