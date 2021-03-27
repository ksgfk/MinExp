set(target_name "minecore")

aux_source_directory(. src_files)

add_library(${target_name} STATIC ${src_files})

if(WIN32)
  message(STATUS "Use Win32 platform")
  target_link_libraries(${target_name} PUBLIC glfw)
  target_link_libraries(${target_name} PUBLIC glad)
else()
  message(FATAL_ERROR "unsupported platform ${CMAKE_SYSTEM_NAME}")
endif()

target_include_directories(${target_name} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(${target_name} PUBLIC ${PROJECT_SOURCE_DIR}/${thirdparty_dir}/glad/include)
target_include_directories(${target_name} PUBLIC ${PROJECT_SOURCE_DIR}/${thirdparty_dir}/stb)