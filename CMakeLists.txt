cmake_minimum_required(VERSION 3.2)
project(poi)

# Make bin dir binary dir
get_filename_component(CMAKE_BINARY_DIR "bin" ABSOLUTE)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})


# ============ Tools ================
# fcpp - C Preprocessor
include_directories("3rd/fcpp")
add_executable(fcpp 3rd/fcpp/cpp1.c 3rd/fcpp/cpp2.c 3rd/fcpp/cpp3.c 3rd/fcpp/cpp4.c 3rd/fcpp/cpp5.c 3rd/fcpp/cpp6.c 3rd/fcpp/usecpp.c)
target_compile_definitions(fcpp PRIVATE unix pdc UNIX DEBUG)
# cids - c identifiers
include_directories("libs/stb")
add_executable(cids tools/cids.c)

# ============ Tests ================
include_directories("output")
add_executable(sds_include_test tests/sds_include_test.c)
