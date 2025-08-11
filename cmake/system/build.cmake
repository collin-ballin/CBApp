####################################################################################
#
#       *********************************************************************
#       ****            B U I L D . C M A K E  _____  F I L E            ****
#       *********************************************************************
#
#              AUTHOR:      Collin A. Bond
#               DATED:      August 03, 2025.
#
#   PURPOSE:
#           Central file to store the compiler flags and other data
#       needed for the build system of my project.
#
#
####################################################################################
####################################################################################
cmake_minimum_required(VERSION ${CB_MINIMUM_CMAKE_VERSION} FATAL_ERROR)



#   1.      PREPROCESSOR MACRO DEFINITIONS PER CONFIGURATION...
########################################################################
########################################################################

#   "CBAPP_CXX_CONFIG_DEFINES"      "CBAPP_CXX_CONFIG_DEFINES"
#
add_library(CBAPP_CXX_CONFIG_DEFINES INTERFACE)
target_compile_definitions(CBAPP_CXX_CONFIG_DEFINES
    INTERFACE
        #   1.1     CONFIG FILES...
        IMGUI_USER_CONFIG=\"my_imconfig.h\"         #   SET FILENAME OF THE CUSTOM IMGUI CONFIG FILE...
        CBAPP_USER_CONFIG=\"cbapp_config.h\"        #   SPECIFY CBAPP CONFIG FILE...
)



#   "CBAPP_CXX_PREPROCESSOR_DEFINES"  "CBAPP_CXX_PREPROCESSOR_DEFINES"
#
add_library(CBAPP_CXX_PREPROCESSOR_DEFINES INTERFACE)
#   Inherit all items in "CBAPP_CXX_CONFIG_DEFINES"...
target_link_libraries(CBAPP_CXX_CONFIG_DEFINES
   INTERFACE
       CBAPP_CXX_PREPROCESSOR_DEFINES
)


#       Now, add the additional items...
target_compile_definitions(CBAPP_CXX_PREPROCESSOR_DEFINES INTERFACE
    #
    #   1.1     CONFIG FILES...
    #   IMGUI_USER_CONFIG=\"my_imconfig.h\"         #   SET FILENAME OF THE CUSTOM IMGUI CONFIG FILE...
    #   CBAPP_USER_CONFIG=\"cbapp_config.h\"        #   SPECIFY CBAPP CONFIG FILE...
    #
    #   1.2     MACROS FOR  "ALL"  BUILD TYPES...UILD TYPES...
    _CBLIB_LOG
    # CBAPP_USE_VIEWPORT
    # CBAPP_USE_DOCKSPACE
        #   ...Add additional GENERIC macros here...


    #   1.A     MACROS FOR  "DEBUG"  BUILD TYPES...
    $<$<CONFIG:Debug>:__CBAPP_DEBUG__>
    $<$<CONFIG:Debug>:_CBAPP_LOG>
        #   ...Add additional Debug-only macros here...


    #   2.B     MACROS FOR  "RELEASE"  BUILD TYPES...
    $<$<CONFIG:Release>:NDEBUG>
    $<$<CONFIG:Release>:__CBAPP_RELEASE__>
        #   ...Add additional Release-only macros here...


    #   3.C     MACROS FOR  "RelWithDebInfo"  BUILD TYPES...
    $<$<CONFIG:RelWithDebInfo>:__CBLIB_RELEASE_WITH_DEBUG_INFO__>
        #   ...Add additional RelWithDebInfo-only macros here...


    #   4.D     MACROS FOR  "MinSizeRel"  BUILD TYPES...
    $<$<CONFIG:MinSizeRel>:__CBLIB_MIN_SIZE_RELEASE__>
        #   ...Add additional MinSizeRel-only macros here...
)






####################################################################################
#
#
#
#   2.          LANGUAGE AND PLATFORM DEFAULTS...
####################################################################################
####################################################################################
set(    CMAKE_CXX_STANDARD                  20                                  CACHE STRING    ""              )
set(    CMAKE_CXX_STANDARD_REQUIRED         YES                                 CACHE BOOL      ""              )
set(    CMAKE_OSX_DEPLOYMENT_TARGET         "${CBAPP_MIN_MACOS_VERSION}"        CACHE STRING    ""              )  # ignored on non-macOS






####################################################################################
#
#
#
#   3.          PROJECT-WIDE COMPILER FLAGS...
####################################################################################
####################################################################################

#   CLANG COMPILER FLAGS:
    #####################################################################################################################
    #   FLAG.                       |   DESCRIPTION.                                                                    #
    ################################|####################################################################################
    #                               |                                                                                   #
    #   -Werror=foo                 | ENABLE ERRORS for the warning flag "foo".                                         #
    #   -Wno-error=foo              | DISABLE ERRORS for the flag "foo" **even if** "-Werror" has been specified.       #
    #                               |                                                                                   #
    #   -Wfoo                       | ENABLE WARNINGS for the flag "foo".                                               #
    #   -Wno-foo                    | DISABLE WARNINGS for the flag "foo".                                              #
    #                               |                                                                                   #
    #                               |                                                                                   #
    #                               |                                                                                   #
    #####################################################################################################################



#   "CBAPP_CXX_ERROR_FLAGS"
#
add_library(CBAPP_CXX_ERROR_FLAGS INTERFACE)
target_compile_options(CBAPP_CXX_ERROR_FLAGS INTERFACE
#
#   0.  GENERIC FLAGS...
    -Wall
    -Wextra
    -Wpedantic
#
#
#   1.  ENABLED ERRORS...
    -Werror=uninitialized                   #   Using an uninitialized, local variable.
    -Werror=shadow
#
#       1B.     TIER-2 ERRORS.
        -Werror=implicit-fallthrough            #   Missing [[fallthrough]] between switch statements.
        -Werror=tautological-compare            #   When using comaprisons that are ALWAYS true ( e.g., unsigned x; if (x < 0) { ... } ).
#
#       1C.     TIER-3 ERRORS.
        -Werror=unknown-warning-option          #   ERROR if using an unknown "-W" flag to compiler.
        -Werror=comment
#
#
#   2.  DISABLED ERRORS...
    #   -Wno-sign-compare
    #   -ferror-limit=64
)



#   "CBAPP_CXX_WARNING_FLAGS"
#
add_library(CBAPP_CXX_WARNING_FLAGS INTERFACE)
target_compile_options(CBAPP_CXX_WARNING_FLAGS INTERFACE
#
#
#   1.  ENABLED WARNINGS...
    -Wunused-variable                   #
    -Wunused-parameter                  #
    -Wunused-result                     #   Unused variable, func. argument, return value, etc...
#
#       1B.     TIER-2 WARNINGS.
        -Wfloat-equal                   #   Warning if comparing floats for equality.
        -Wsign-compare
#
#       1C.     TIER-3 WARNINGS.
        -Wcomma
        -Wdocumentation
        -Wheader-hygiene
#
#       1D.     TIER-4 WARNINGS.
        -Wweak-vtables
        #   -Wsign-conversion           #   Warns for IMPLICIT CONVERSIONS FROM:    "unsigned int = int",       "long long = size_t",       etc...
        #   -Wconversion                #   Warns for IMPLICIT CONVERSIONS FROM:    "float = double",           "int = long",               etc...
#
#
#
#   2.  DISABLED WARNINGS...
    -Wno-documentation                  #   DOxygen Errors.
)


#   "CBAPP_CXX_DEBUG_FLAGS"
#
add_library(CBAPP_CXX_DEBUG_FLAGS INTERFACE)
target_compile_options(CBAPP_CXX_WARNING_FLAGS INTERFACE
    #
)






####################################################################################
#
#
#
#   4.          TARGET PROPERTIES FOR MACOS...
####################################################################################
####################################################################################

#   "CB_apply_xcode_target_properties"
#
function(CB_apply_xcode_target_properties target)

    #   1.  CORE PROPERTIES...
    set_target_properties(${target} PROPERTIES
        XCODE_ATTRIBUTE_MARKETING_VERSION                       ${PROJECT_VERSION}
        XCODE_ATTRIBUTE_CURRENT_PROJECT_VERSION                 ${BUILD_NUMBER}
        XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER               "com.example.cbapp"
        XCODE_ATTRIBUTE_MACOSX_DEPLOYMENT_TARGET                "${CBAPP_MIN_MACOS_VERSION}"
        XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY                      "-"
        XCODE_ATTRIBUTE_ENABLE_HARDENED_RUNTIME                 "NO"
    #
    #
        #   XCODE_ATTRIBUTE_INFOPLIST_FILE                          "Path to your custom Info.plist"
        #   XCODE_ATTRIBUTE_ASSETCATALOG_COMPILER_APPICON_NAME      "Name of the App Icon set"
        #   XCODE_ATTRIBUTE_ENTITLEMENTS_PLIST                      "Path to *.entitlements"
        #
        #   XCODE_ATTRIBUTE_CODE_SIGN_STYLE                         "Manual" or "Automatic"
    #
    )


    #   2.  **KWARGS:   EXTRA (KEY, VALUE) PAIRS    [OPTIONAL]...
    #           USAGE:  CB_apply_xcode_target_properties(target EXTRA XCODE_ATTRIBUTE_* value …)
    #
    cmake_parse_arguments(X "" "" "EXTRA" ${ARGN})
    if(X_EXTRA)
        set_target_properties(${target} PROPERTIES ${X_EXTRA})
    endif()
endfunction()















####################################################################################
####################################################################################    # END.
