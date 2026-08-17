# Pigeon CLI (REPL + file runner)

add_executable(pigeon_cli repl.c)
set_target_properties(pigeon_cli PROPERTIES OUTPUT_NAME pigeon)

target_include_directories(pigeon_cli PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/include
)

target_link_libraries(pigeon_cli PRIVATE pigeon)

install(TARGETS pigeon_cli
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
