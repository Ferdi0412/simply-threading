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

target_include_directories(Threading INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

add_library(Simply::Threading ALIAS Threading)