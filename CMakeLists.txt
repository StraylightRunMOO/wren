cmake_minimum_required(VERSION 3.15)

project(Wren
    VERSION 0.4.0
    DESCRIPTION "A small, fast, class-based concurrent scripting language"
    LANGUAGES C
)

# Set C standard
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# Build options
option(BUILD_SHARED_LIBS "Build shared library instead of static" OFF)
option(BUILD_TESTS "Build test executable" ON)
option(WREN_NAN_TAGGING "Enable NaN tagging optimization" ON)

# Output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Wren VM source files
set(WREN_VM_SOURCES
    src/vm/wren_compiler.c
    src/vm/wren_core.c
    src/vm/wren_debug.c
    src/vm/wren_primitive.c
    src/vm/wren_utils.c
    src/vm/wren_value.c
    src/vm/wren_vm.c
)

set(WREN_VM_HEADERS
    src/vm/wren_common.h
    src/vm/wren_compiler.h
    src/vm/wren_core.h
    src/vm/wren_debug.h
    src/vm/wren_math.h
    src/vm/wren_opcodes.h
    src/vm/wren_primitive.h
    src/vm/wren_utils.h
    src/vm/wren_value.h
    src/vm/wren_vm.h
)

# Wren optional modules
set(WREN_OPT_SOURCES
    src/optional/wren_opt_meta.c
    src/optional/wren_opt_random.c
)

set(WREN_OPT_HEADERS
    src/optional/wren_opt_meta.h
    src/optional/wren_opt_random.h
)

# Public header
set(WREN_PUBLIC_HEADERS
    src/include/wren.h
)

# Static library target
add_library(wren STATIC
    ${WREN_VM_SOURCES}
    ${WREN_VM_HEADERS}
    ${WREN_OPT_SOURCES}
    ${WREN_OPT_HEADERS}
    ${WREN_PUBLIC_HEADERS}
)

target_include_directories(wren
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/vm
        ${CMAKE_CURRENT_SOURCE_DIR}/src/optional
)

# Shared library target
add_library(wren_shared SHARED
    ${WREN_VM_SOURCES}
    ${WREN_VM_HEADERS}
    ${WREN_OPT_SOURCES}
    ${WREN_OPT_HEADERS}
    ${WREN_PUBLIC_HEADERS}
)

set_target_properties(wren_shared PROPERTIES
    OUTPUT_NAME wren
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
)

target_include_directories(wren_shared
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/vm
        ${CMAKE_CURRENT_SOURCE_DIR}/src/optional
)

# Compiler definitions
if(NOT WREN_NAN_TAGGING)
    target_compile_definitions(wren PRIVATE WREN_NAN_TAGGING=0)
    target_compile_definitions(wren_shared PRIVATE WREN_NAN_TAGGING=0)
endif()

# Platform-specific settings
if(WIN32)
    target_compile_definitions(wren PRIVATE _CRT_SECURE_NO_WARNINGS)
    target_compile_definitions(wren_shared PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()

if(UNIX AND NOT APPLE)
    target_link_libraries(wren PUBLIC m)
    target_link_libraries(wren_shared PUBLIC m)
endif()

if(CMAKE_SYSTEM_NAME MATCHES "BSD")
    target_link_libraries(wren PUBLIC m)
    target_link_libraries(wren_shared PUBLIC m)
endif()

# Compiler warnings and optimizations
if(MSVC)
    target_compile_options(wren PRIVATE /W4)
    target_compile_options(wren_shared PRIVATE /W4)
else()
    target_compile_options(wren PRIVATE -Wall -Wextra -Werror=implicit-function-declaration)
    target_compile_options(wren_shared PRIVATE -Wall -Wextra -Werror=implicit-function-declaration)
endif()

# Test executable
if(BUILD_TESTS)
    file(GLOB WREN_API_TEST_SOURCES test/api/*.c)
    file(GLOB WREN_API_TEST_HEADERS test/api/*.h)

    add_executable(wren_test
        test/main.c
        test/test.c
        test/test.h
        ${WREN_API_TEST_SOURCES}
        ${WREN_API_TEST_HEADERS}
    )

    target_include_directories(wren_test PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/include
    )

    target_link_libraries(wren_test PRIVATE wren)

    # Add test target
    enable_testing()
    add_test(NAME wren_test COMMAND wren_test)
endif()

# Installation rules
include(GNUInstallDirs)

install(TARGETS wren wren_shared
    EXPORT WrenTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(FILES ${WREN_PUBLIC_HEADERS}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# Export targets
install(EXPORT WrenTargets
    FILE WrenTargets.cmake
    NAMESPACE Wren::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Wren
)

# Create config file
include(CMakePackageConfigHelpers)

configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/WrenConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/WrenConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Wren
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/WrenConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/WrenConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/WrenConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Wren
)
