/***********************************************************************************
*
*       **************************************************************************
*       ****                C O M M O N . C P P  ____  F I L E                ****
*       **************************************************************************
*
*              AUTHOR:      Collin A. Bond
*               DATED:      August 1, 2025.
*
**************************************************************************************
**************************************************************************************/
#include "widgets/functional_testing/functional_testing.h"
#include "app/state/state.h"
#include "app/state/_types.h"



namespace cb { namespace ui { //     BEGINNING NAMESPACE "cb::ui"...
// *************************************************************************** //
// *************************************************************************** //


//  "_delete_composition"
//
void ActionComposer::_delete_composition(const int index)
{
    if ( index < 0 || index >= static_cast<int>(m_compositions.size()) )    { return; }

    _save_actions_to_comp();                                           // persist edits

    m_compositions.erase(m_compositions.begin() + index);

    const int next_index = std::clamp(
        index, 0, static_cast<int>(m_compositions.size()) - 1);

    _load_actions_from_comp(next_index);
    return;
}


//  "_duplicate_composition"
//
void ActionComposer::_duplicate_composition(const int index)
{
    if ( index < 0 || index >= static_cast<int>(m_compositions.size()) )    { return; }

    _save_actions_to_comp();                                           // persist edits

    Composition_t copy = m_compositions[static_cast<std::size_t>(index)];
    m_compositions.insert(m_compositions.begin() + index + 1, copy);

    _load_actions_from_comp(index + 1);
    
    return;
}






//  "_delete_action"
//
void ActionComposer::_delete_action(const int index)
{
    return;
}


//  "_duplicate_action"
//
void ActionComposer::_duplicate_action(const int index)
{
    return;
}
    
    



// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
} }//   END OF "cb" "ui" NAMESPACE.











// *************************************************************************** //
// *************************************************************************** //
//
//  END.
