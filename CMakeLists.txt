cmake_minimum_required(VERSION 3.13.0)
project(MinusEngine VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)

set(src_dir "src")
set(thirdparty_dir "thirdparty")

project(MinElemental VERSION 0.0.1)

if(WIN32)
  add_definitions(-D MINE_PLATFORM_WIN32)
else()
  message(FATAL_ERROR "unsupported platform ${CMAKE_SYSTEM_NAME}")
endif()

add_subdirectory(${src_dir}/Core)
add_subdirectory(${src_dir}/App)
add_subdirectory(${thirdparty_dir}/glfw)
add_subdirectory(${thirdparty_dir}/glad)