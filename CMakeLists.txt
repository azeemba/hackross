# CMakeList.txt : CMake project for hackross, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.5)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/external/Catch2/contrib)

if (UNIX)
    find_package(OpenMP)
    if (OPENMP_FOUND)
        set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
        set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
        set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
    endif()
endif()

FILE(TO_NATIVE_PATH ${Z3_BASE}/bin/${Z3_LIB} Z3_BIN)
FILE(TO_NATIVE_PATH ${Z3_BASE}/include/ Z3_INCL)
message ("bin=${Z3_BIN}")
message ("incl=${Z3_INCL}")

add_library(z3 STATIC IMPORTED)
set_target_properties (
    z3 PROPERTIES
    IMPORTED_LOCATION ${Z3_BIN}
    INTERFACE_INCLUDE_DIRECTORIES ${Z3_INCL}
)

include(CTest)
add_subdirectory(${CMAKE_SOURCE_DIR}/external/Catch2)
add_subdirectory(tests)

# Add source to this project's executable.
add_executable (hackross "hackross.cpp" "hackross.h")
target_link_libraries (hackross z3)

set_property(TARGET hackross PROPERTY CXX_STANDARD 14)

