set(CMAKE_VERBOSE_MAKEFILE ON)

configure_file("words_medium.txt" "words_medium.txt" COPYONLY)

add_executable(tests "test.cpp")
target_link_libraries(tests Catch2::Catch2 z3 hackross)
include(Catch)
catch_discover_tests(tests)

set_property(TARGET tests PROPERTY CXX_STANDARD 14)