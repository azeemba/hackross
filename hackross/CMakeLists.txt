# CMakeList.txt : CMake project for hackross, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)
set(CMAKE_VERBOSE_MAKEFILE ON)

FILE(TO_NATIVE_PATH ${Z3_BASE}/bin/libz3.lib Z3_BIN)
FILE(TO_NATIVE_PATH ${Z3_BASE}/include/ Z3_INCL)
message ("bin=${Z3_BIN}")
message ("incl=${Z3_INCL}")

add_library(z3 STATIC IMPORTED)
set_target_properties (
	z3 PROPERTIES
	IMPORTED_LOCATION ${Z3_BIN}
	INTERFACE_INCLUDE_DIRECTORIES ${Z3_INCL}
)

# Add source to this project's executable.
add_executable (hackross "hackross.cpp" "hackross.h")
target_link_libraries (hackross z3)

# TODO: Add tests and install targets if needed.
