/***********************************************************************************
*
*       ********************************************************************
*       ****              _ O R C H I D . H  ____  F I L E              ****
*       ********************************************************************
*
*              AUTHOR:      Collin A. Bond
*               DATED:      October 6, 2025.
*
*       ********************************************************************
*                FILE:      [./PyStream.h]
*
*
*
**************************************************************************************
**************************************************************************************/
#ifndef _CBLIB_ORCHID_ORCHID_H
#define _CBLIB_ORCHID_ORCHID_H 1



//      1.      SYSTEM HEADERS.
#include <iostream>
#include <iomanip>
#include <type_traits>
#include <algorithm>                //  std::clamp
#include <concepts>

#include <cstddef>
#include <cassert>
#include <limits>
#include <cmath>                    //  std::nextafter

#include <array>
#include <vector>
#include <initializer_list>
#include <string>
#include <string_view>


	
//      2.      MY HEADERS.
#ifndef _CBLIB_ORCHID_ACTION_H
# include "templates/containers/orchid/orchid/_action.h"
#endif	// _CBLIB_ORCHID_ACTION_H  //






namespace cblib { namespace containers {   //     BEGINNING NAMESPACE "cblib::containers"...
// *************************************************************************** //
// *************************************************************************** //



// *************************************************************************** //
//
//
//
//      0.      MAIN CLASS INTERFACE FOR  "Orchid"  UNDO/REDO STACK...
// *************************************************************************** //
// *************************************************************************** //

//  template <typename T = orchid::Action>
class Orchid
{
//      0.          CONSTANTS AND ALIASES...
// *************************************************************************** //
// *************************************************************************** //
public:

    // *************************************************************************** //
    //      NESTED TYPENAME ALIASES.
    // *************************************************************************** //
    
    //                                      CALLING-ROUTIENE TYPE ALIASES:
    using                                       ABC                             = orchid::Action                                        ;
    //
    using                                       Item                            = ABC                                                   ;
    using                                       smart_pointer                   = std::unique_ptr<Item>                                 ;
    using                                       container_type                  = std::vector<smart_pointer>                            ;
    //
    //
    //
    //                                      VALUE TYPES:
    using                                       value_type                      = typename container_type::value_type                   ;
    using                                       pointer                         = typename container_type::pointer                      ;
    using                                       const_pointer                   = typename container_type::const_pointer                ;
    using                                       reference                       = typename container_type::reference                    ;
    using                                       const_reference                 = typename container_type::const_reference              ;
    //
    //                                      SIZE / INDEX TYPES:
    using                                       size_type                       = typename container_type::size_type                    ;   //  container_type::size_type;
    using                                       difference_type                 = typename container_type::difference_type              ;
    //
    //                                      ITERATOR TYPES:
    using                                       iterator                        = typename container_type::iterator                     ;
    using                                       const_iterator                  = typename container_type::const_iterator               ;
    using                                       reverse_iterator                = typename container_type::reverse_iterator             ;
    using                                       const_reverse_iterator          = typename container_type::const_reverse_iterator       ;
    
    //  using                               MyAlias                         = MyTypename_t;
    //  using                               IDType                          = uint32_t;
    //  using                               ObjectType                      = MyObject_t<IDType>;
    //  using                               State                           = cb::MyAppState;
    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      STATIC CONSTEXPR CONSTANTS.
    // *************************************************************************** //
    
//
//
// *************************************************************************** //
// *************************************************************************** //   END "CONSTANTS AND ALIASES".



// *************************************************************************** //
//
//
//      1.          CLASS DATA-MEMBERS...
// *************************************************************************** //
// *************************************************************************** //
protected:

    // *************************************************************************** //
    //      IMPORTANT DATA-MEMBERS.
    // *************************************************************************** //
    container_type                      m_history                       {   };              //  [ 0, 1, 2, ‥. , (cursor - 1) ] are PAST-ENTRIES (Undos).
                                                                                            //  [ past, ≥ cursor             ] are FUTURE-ENTRIES (Redos).
                                                                                        
    size_type                           m_cursor                        = size_type(0);     //  index of next entry to redo
    size_type                           m_uuid                          = size_type(0);     //  monotonic counter of all UNDO/REDO actions.
    
//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "CLASS DATA-MEMBERS".



// *************************************************************************** //
//
//
//      2.A.        PUBLIC MEMBER FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //
public:
    
    // *************************************************************************** //
    //      INITIALIZATION METHODS.         |   "init.cpp" ...
    // *************************************************************************** //
    //  explicit                        Orchid                  (app::AppState & );             //  Def. Constructor.
        //                                    Orchid                  (void) noexcept                 = default;
        //                                    ~Orchid                 (void)                          = default;
    //
        //  void                                initialize              (void);
    
    // *************************************************************************** //
    //      DELETED FUNCTIONS.              |   ...
    // *************************************************************************** //
    //                                      Orchid                  (const Orchid &    src)        = delete;   //  Copy. Constructor.
    //                                      Orchid                  (Orchid &&         src)        = delete;   //  Move Constructor.
    //  Orchid &                            operator =              (const Orchid &    src)        = delete;   //  Assgn. Operator.
    //  Orchid &                            operator =              (Orchid &&         src)        = delete;   //  Move-Assgn. Operator.
    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      MAIN API.                       |   "interface.cpp" ...
    // *************************************************************************** //

    //  "push"
    //
    inline void                                 push                            (smart_pointer act) noexcept
    {
        assert(act != nullptr);
        
        //      Discard any redoable commands beyond the cursor
        this->m_history.erase(
              this->m_history.begin() + static_cast<std::ptrdiff_t>( this->m_cursor )
            , this->m_history.end()
        );
        
        
        //      *APPLY* THE CHANGE AS WE PUSH IT TO THE STACK.  This is a  *POLICY DECISION*  we have to make...
        //          The reason we want to apply it from the Action itself is because it will be immediately obvious
        //          if there is an error in our Action class implementation if we apply it from here instead
        //          of having to notice the error IF/WHEN we use the UNDO action.
        act->redo();
    
    
        this->m_history.emplace_back( std::move(act) );
        ++this->m_cursor;
        ++this->m_uuid;
        
        return;
    }

    //  "undo"
    //
    inline void                                 undo                            (void) noexcept
    {
        if ( !this->can_undo() )        { return; }
    
        this->m_history[ --this->m_cursor ]->undo();
        ++this->m_uuid;
        return;
    }

    //  "redo"
    //
    inline void                                 redo                            (void) noexcept
    {
        if ( !this->can_redo() )        { return; }
        
        m_history[ m_cursor++ ]->redo();
        ++this->m_uuid;
        return;
    }
    
    
    
    // *************************************************************************** //

//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "PUBLIC MEMBER FUNCS".


    
// *************************************************************************** //
//
//
//      2.B.        PROTECTED MEMBER FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //
protected:

    // *************************************************************************** //
    //      "RULE-OF ..." FUNCTIONS.        |   "init.cpp" ...
    // *************************************************************************** //
    //  inline void                         init                                (void);
    //  void                                load                                (void);
    //  void                                destroy                             (void);
    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      OPERATION FUNCTIONS.            |   "tools.cpp" ...
    // *************************************************************************** //
    void                                        _do_some_math                   (void);
    
//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "PROTECTED" FUNCTIONS.

    
   
// *************************************************************************** //
//
//
//      2.C.        INLINE FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //
public:

    // *************************************************************************** //
    //      QUERY FUNCTIONS.
    // *************************************************************************** //

    //  "can_undo"
    [[nodiscard]] inline bool                   can_undo                        (void) const noexcept       { return ( this->m_cursor > 0 );                        }
    [[nodiscard]] inline bool                   can_redo                        (void) const noexcept       { return ( this->m_cursor < this->m_history.size() );   }

    //  "size"
    [[nodiscard]] inline size_type              size                            (void) const noexcept       { return this->m_history.size();                        }


    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      SETTER/GETTER FUNCTIONS.
    // *************************************************************************** //
    
    //  "get_uuid"
    [[nodiscard]] inline size_type              get_uuid                        (void) const noexcept       { return this->m_uuid; }
    
    
    //  "get_undo_label"
    [[nodiscard]] inline std::optional< std::string_view >      get_undo_label  (void) const noexcept       {
        if ( this->can_undo() )     { return std::nullopt; }
        return ( this->m_history[ m_cursor - 1 ]->label() );
    }
    //
    [[nodiscard]] inline std::optional< std::string_view >      get_redo_label  (void) const noexcept       {
        if ( this->can_redo() )     { return std::nullopt; }
        return ( this->m_history[ m_cursor + 1 ]->label() );
    }
    
    
    //  "undo_count"
    [[nodiscard]] inline size_type              undo_count                      (void) const noexcept       { return this->m_cursor;                                }
    [[nodiscard]] inline size_type              redo_count                      (void) const noexcept       { return this->m_history.size() - this->m_cursor;       }



    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      CENTRALIZED STATE MANAGEMENT FUNCTIONS.
    // *************************************************************************** //
    
    //  "clear"
    inline void                                 clear                           (void) noexcept             { this->m_history.clear(); this->m_cursor = 0;          }
    
    
    
    // *************************************************************************** //
    
//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "INLINE" FUNCTIONS.



// *************************************************************************** //
// *************************************************************************** //
};//	END "Orchid" INLINE CLASS DEFINITION.






//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "0.  Orchid Interface".






// *************************************************************************** //
//
//
//
//      1.      MEMBER IMPLEMENTATIONS FOR  "Orchid"  UNDO/REDO STATCK...
// *************************************************************************** //
// *************************************************************************** //




//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "1.  Orchid Implementation".












// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
} }//   END OF "cblib::containers" NAMESPACE.


















// *************************************************************************** //
// *************************************************************************** //
#endif  //  _CBLIB_ORCHID_ORCHID_H  //
