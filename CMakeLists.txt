cmake_minimum_required(VERSION 3.2)
project(poi)

# Make bin dir binary dir
get_filename_component(CMAKE_BINARY_DIR "bin" ABSOLUTE)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})


# ============ Tools ================
# fcpp - C Preprocessor
IF (!MSVC)
    include_directories("3rd/fcpp")
    add_executable(fcpp 3rd/fcpp/cpp1.c 3rd/fcpp/cpp2.c 3rd/fcpp/cpp3.c 3rd/fcpp/cpp4.c 3rd/fcpp/cpp5.c 3rd/fcpp/cpp6.c 3rd/fcpp/usecpp.c)
    target_compile_definitions(fcpp PRIVATE unix pdc UNIX DEBUG)
ENDIF ()
# cids - c identifiers
include_directories("libs/stb")
add_executable(cids tools/cids.c)

# ============ Tests ================
include_directories("output")
add_executable(sds_test tests/sds_test.c)
add_executable(stb_ds_test tests/stb_ds_test.c)
add_executable(http_test tests/http_test.c)
IF (WIN32)
    target_link_libraries(http_test ws2_32)
ENDIF ()
add_executable(ini_test tests/ini_test.c)
add_executable(thread_test tests/thread_test.c)

