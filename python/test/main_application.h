/***********************************************************************************
*
*       ********************************************************************
*       ****    m a i n _ a p p l i c a t i o n . h  ____  F I L E    ****
*       ********************************************************************
*
*              AUTHOR:      Collin A. Bond
*               DATED:      May 03, 2025.
*
*       ********************************************************************
*                FILE:      [./main_application.h]
*
*
*
**************************************************************************************
**************************************************************************************/
namespace cb { // BEGINNING NAMESPACE "cb"...
// *************************************************************** //
// *************************************************************** //

// *************************************************************** //
// *************************************************************** //
// 3.  PRIMARY CLASS INTERFACE
// @brief NO DESCRIPTION PROVIDED
// *************************************************************** //
// *************************************************************** //

class main_application
{

// *************************************************************** //
public:
    // 1.1  Default Constructor, Destructor, etc.    [src/…]
    main_application(void);
    ~main_application(void);

    // 1.2  Primary Class Interface.                [src/…]
    void Begin(bool* p_open = nullptr);

    // 1.3  Deleted Operators, Functions, etc.
    main_application(const main_application&)     = delete;
    main_application(main_application&&) noexcept = delete;
    main_application& operator=(const main_application&)     = delete;
    main_application& operator=(main_application&&) noexcept = delete;

// *************************************************************** //
protected:
    // 2.A  PROTECTED DATA-MEMBERS…
    bool m_running = true;
    // … add more protected members here

    // 2.B  PROTECTED MEMBER FUNCTIONS…
    void init(void);
    void load(void);
    void destroy(void);

// *************************************************************** //
private:
    // 3.  PRIVATE MEMBER FUNCTIONS…
    // … add private helpers here

}; // END "main_application" INLINE CLASS DEFINITION.

// *************************************************************************** //
// *************************************************************************** //
}//   END OF "cb" NAMESPACE.




#endif      //  _CBAPP_MAIN_APPLICATION_H  //
// *************************************************************************** //
// *************************************************************************** //
//
//  END.