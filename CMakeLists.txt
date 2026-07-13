cmake_minimum_required(VERSION 3.25)

project(cplib_initializers LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(CMAKE_EXPORT_COMPILE_COMMANDS)
  set(CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES ${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES})
endif()

add_library(cplib_initializers INTERFACE)
add_library(cplib-initializers::cplib-initializers ALIAS cplib_initializers)
target_compile_features(cplib_initializers INTERFACE cxx_std_20)
target_include_directories(
  cplib_initializers
  INTERFACE
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/third_party/cplib>
)

if(PROJECT_IS_TOP_LEVEL)
  include(CTest)
  if(BUILD_TESTING)
    add_subdirectory(tests)
  endif()
endif()
