/***********************************************************************************
*
*       ********************************************************************
*       ****      R E S O U R C E  L O A D E R . H  ____  F I L E       ****
*       ********************************************************************
*              AUTHOR:      Collin A. Bond
*               DATED:      May 12, 2025.
*
**************************************************************************************
**************************************************************************************/
#ifdef _WIN32
//
//
#include "app/app.h"
#include "utility/resource_loader.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>



namespace cb { namespace utl { //     BEGINNING NAMESPACE "cb" :: "utl"...
// *************************************************************************** //
// *************************************************************************** //

std::span<const std::byte> load_embedded_resource(int id)
{
    // --- ANSI version avoids the wide‑char / LPSTR mismatch ---
    HRSRC hRes = FindResourceA(nullptr, MAKEINTRESOURCEA(id), RT_RCDATA);
    if (!hRes) return {};

    HGLOBAL hMem = LoadResource(nullptr, hRes);
    if (!hMem) return {};

    auto*  data = static_cast<const std::byte*>(LockResource(hMem));
    DWORD  size = SizeofResource(nullptr, hRes);
    return {data, size};
}






//
// *************************************************************************** //
// *************************************************************************** //
} }//   END OF "cb" :: "utl" NAMESPACE.






// *************************************************************************** //
// *************************************************************************** //
//
//  END.
// *************************************************************************** //
//
//
#endif  //  _WIN32  //
