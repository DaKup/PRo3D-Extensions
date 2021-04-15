# - Try to find CSPICE
#  Once done this will define
#  CSPICE_FOUND - System has CSPICE
#  CSPICE_INCLUDE_DIR - The CSPICE include directory
#  CSPICE_LIBRARY - The CSPICE main library file

include(FindPackageHandleStandardArgs)

find_path( CSPICE_INCLUDE_DIR cspice/SpiceUsr.h )

find_library( CSPICE_LIBRARY_DEBUG
    NAMES cspice
    PATH_SUFFIXES "debug/lib")

find_library( CSPICE_LIBRARY_RELEASE
    NAMES cspice
    CMAKE_IGNORE_PATH debug
    PATH_SUFFIXES "lib")

set(CSPICE_LIBRARY ${CSPICE_LIBRARY_RELEASE})

if(DEFINED VCPKG_TARGET_TRIPLET)
    #fix debug/release libraries mismatch for vcpkg
    set(CSPICE_LIBRARY_RELEASE ${CSPICE_LIBRARY_DEBUG}/../../../lib/${CMAKE_STATIC_LIBRARY_PREFIX}cspice${CMAKE_STATIC_LIBRARY_SUFFIX})
    get_filename_component(CSPICE_LIBRARY_RELEASE ${CSPICE_LIBRARY_RELEASE} REALPATH)
endif()

find_package_handle_standard_args(CSpice DEFAULT_MSG CSPICE_INCLUDE_DIR CSPICE_LIBRARY)