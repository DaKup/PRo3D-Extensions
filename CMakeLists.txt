cmake_minimum_required(VERSION 3.12)
project(PRo3D-Extensions)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS 1)

option(USE_PYTHON "Build with Python interface")

if(USE_PYTHON)
    find_package(Python3 COMPONENTS Development REQUIRED)
    set(BOOST_PYTHON_COMPONENTS python39)
else()
    set(BOOST_PYTHON_COMPONENTS "")
endif()


find_package(Boost COMPONENTS log filesystem program_options ${BOOST_PYTHON_COMPONENTS} system REQUIRED)
find_package(CSpice REQUIRED)

add_subdirectory(CooTransformation)
add_subdirectory(InstrumentPlatforms)
