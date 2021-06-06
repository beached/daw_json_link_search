cmake_minimum_required(VERSION 3.19)
project(data_stuff)

set(CMAKE_CXX_STANDARD 20)

include( FetchContent )
set( FETCHCONTENT_UPDATES_DISCONNECTED ON )

FetchContent_Declare(
        daw_json_link
        GIT_REPOSITORY https://github.com/beached/daw_json_link.git
        GIT_TAG develop
)
FetchContent_MakeAvailable(daw_json_link)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GMP IMPORTED_TARGET gmpxx>=6)

if (NOT GMP_FOUND) # some systems don't install the .pc :eyeroll:
    message(STATUS "searching for gmpxx in system directories")
    find_library(GMP_LIB gmp REQUIRED)
    find_library(GMPXX_LIB gmpxx REQUIRED)
    add_library(GMP INTERFACE)
    target_link_libraries(GMP INTERFACE ${GMPXX_LIB} ${GMP_LIB})
    add_library(PkgConfig::GMP ALIAS GMP)
endif (NOT GMP_FOUND)

add_executable(data_stuff main.cpp )
target_link_libraries( data_stuff daw::json_link PkgConfig::GMP )