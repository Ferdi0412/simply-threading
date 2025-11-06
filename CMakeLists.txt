cmake_minimum_required(VERSION 3.14)

project(SimplyThreading
    VERSION 0.0.0
    DESCRIPTION "Simple threading utilities"
    LANGUAGES CXX
)

##   Setup for FetchContent
## It will include all the contents of ./include/simply
## and aliased using Simply::Threading
add_library(Threading INTERFACE)

target_compile_features(Threading INTERFACE cxx_std_17)

target_include_directories(Threading INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

add_library(Simply::Threading ALIAS Threading)

if (CMAKE_SYSTEM_NAME STREQUAL "Linux") # if (LINUX) didn't work
    target_link_libraries(Threading INTERFACE pthread)
endif()

##   Build examples
## If building from the simply-threading root,
## will build unless explicitly set:
## `-DSIMPLY_BUILD_EXAMPLES=OFF`
option(SIMPLY_BUILD_EXAMPLES "Build examples" ${PROJECT_IS_TOP_LEVEL})

if ( SIMPLY_BUILD_EXAMPLES )
    add_subdirectory(examples)
endif()


##   Build tests
## If building from the simply-threading root,
## will build unless explicitly turned off:
## `-SIMPLY_BUILD_TESTS=OFF`
option(SIMPLY_BUILD_TESTS "Build examples" ${PROJECT_IS_TOP_LEVEL})

if ( SIMPLY_BUILD_TESTS )
    add_subdirectory(tests)
endif()