add_executable(MineApp main.cpp ShadowPipeline.cpp)

target_link_libraries(MineApp PUBLIC minecore)

set(ASSETS_SRC "${CMAKE_SOURCE_DIR}/asset")
set(ASSETS_DST "$<TARGET_FILE_DIR:MineApp>/asset")

if(WIN32)
    set(ASSETS_CMD copy_directory)
    set(ASSETS_MSG "Copy assets")
else()
    set(ASSETS_CMD create_symlink)
    set(ASSETS_MSG "Link assets")
endif()

add_custom_command(
    TARGET MineApp POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo
    "${ASSETS_MSG}: ${ASSETS_SRC} to ${ASSETS_DST}"
)
add_custom_command(
    TARGET MineApp POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E ${ASSETS_CMD}
    "${ASSETS_SRC}" "${ASSETS_DST}"
)