# Wren Tools

add_executable(wren_repl repl.c)

target_include_directories(wren_repl PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/include
)

target_link_libraries(wren_repl PRIVATE wren)

# Install the REPL
install(TARGETS wren_repl
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
