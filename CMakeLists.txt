cmake_minimum_required(VERSION 3.9)
project(unicode)

set (CMAKE_CXX_STANDARD 11)

##################################################################
###### unicode
##################################################################

file(GLOB_RECURSE UNICODE_HDR_FILES ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h)

add_library(unicode-header-only INTERFACE) 
target_sources(unicode-header-only INTERFACE ${UNICODE_HDR_FILES})
target_include_directories(unicode-header-only INTERFACE  "${CMAKE_CURRENT_SOURCE_DIR}/include")
#set_target_properties(unicode PROPERTIES LINKER_LANGUAGE CXX)

add_custom_target(unicode SOURCES ${UNICODE_HDR_FILES})

##################################################################
###### tests
##################################################################

add_subdirectory(tests)