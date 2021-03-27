set(target_name "glad")

aux_source_directory(src/. src_files)

add_library(${target_name} STATIC ${src_files})

target_include_directories(${target_name} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)