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
set(    CB_MINIMUM_CMAKE_VERSION        3.15)



#   1.      PREPROCESSOR MACRO DEFINITIONS PER CONFIGURATION...
########################################################################
########################################################################

#   "LIB_preprocessor_definitions"
#
add_library(LIB_preprocessor_defines INTERFACE)
target_compile_definitions(LIB_preprocessor_defines INTERFACE
    #
    #   1.1     CONFIG FILES...
    IMGUI_USER_CONFIG=\"my_imconfig.h\"     #   SET FILENAME OF THE CUSTOM IMGUI CONFIG FILE...
    CBAPP_USER_CONFIG=\"cbapp_config.h\"         #   SPECIFY CBAPP CONFIG FILE...
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
set(    CMAKE_CXX_STANDARD                  20                  CACHE STRING    ""              )
set(    CMAKE_CXX_STANDARD_REQUIRED         YES                 CACHE BOOL      ""              )
set(    CMAKE_OSX_DEPLOYMENT_TARGET         "13.3"              CACHE STRING    ""              )  # ignored on non-macOS






####################################################################################
#
#
#
#   3.          PROJECT-WIDE COMPILER FLAGS...
####################################################################################
####################################################################################


#############################################################
#       CLANG COMPILER FLAGS:
#############################################################
#                               |
#   -ferror-limit=123           |
#                               |
#                               |
#                               |
#   -Werror=foo                 | ENABLE ERRORS for the warning flag "foo".
#   -Wno-error=foo              | DISABLE ERRORS for the flag "foo" **even if** "-Werror" has been specified.
#                               |
#   -Wfoo                       | ENABLE WARNINGS for the flag "foo".
#   -Wno-foo                    | DISABLE WARNINGS for the flag "foo".
#                               |
#                               |
#                               |
#############################################################




#   "LIB_cxx_error_flags"
#
add_library(LIB_cxx_error_flags INTERFACE)
target_compile_options(LIB_cxx_error_flags INTERFACE
    -Wall
    -Wextra
    -Wpedantic
#
#
#   ENABLED ERRORS...
    -Werror=unknown-warning-option          #   ERROR if using an unknown "-W" flag to compiler.
    -Werror=comment
    -Werror=tautological-compare            #   When using comaprisons that are ALWAYS true ( e.g., unsigned x; if (x < 0) { ... } ).
    -Werror=uninitialized                   #   Using an uninitialized, local variable.
    -Wimplicit-fallthrough                  #   Missing [[fallthrough]] between switch statements.
#
#
#   DISABLED ERRORS...
    #-Wno-error=
    #-Wno-sign-compare
)



#   "LIB_cxx_warning_flags"
#
add_library(LIB_cxx_warning_flags INTERFACE)
target_compile_options(LIB_cxx_warning_flags INTERFACE
#
#
#   ENABLED WARNINGS...
    -Wshadow
    -Wcomma
    -Wunused-variable -Wunused-parameter -Wunused-result    #   Unused variable, func. argument, return value, etc...
#
#
#   DISABLED WARNINGS...
    -Wno-shadow
    -Wno-float-equal                #   Warning if comparing floats for equality.
#
    -Wno-documentation              #   DOxygen Errors.
#
#
#
#   -Wheader-hygiene
#   -Wweak-vtables
#
#   -Wconversion
#   -Wsign-conversion
)


#   "LIB_cxx_debug_flags"
#
add_library(LIB_cxx_debug_flags INTERFACE)
target_compile_options(LIB_cxx_warning_flags INTERFACE
    #
)












####################################################################################
####################################################################################    # END.
