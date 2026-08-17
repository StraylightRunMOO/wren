cmake_minimum_required(VERSION 3.15)

project(Pigeon
    VERSION 0.5.0
    DESCRIPTION "A small, fast, class-based concurrent scripting language (derived from Wren)"
    LANGUAGES C
)

include(FetchContent)
find_package(Threads REQUIRED)

# Set C standard. C11 is required by Suspenders/Memento.
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# ---------------------------------------------------------------------------
# Memento 2.2.1 + Suspenders 1.1.1 (header-only). Prefer sibling checkouts.
# ---------------------------------------------------------------------------
set(PIGEON_SUSPENDERS_SOURCE "" CACHE PATH "Optional local suspenders.h tree")
set(PIGEON_MEMENTO_SOURCE "" CACHE PATH "Optional local memento tree")

if(NOT PIGEON_SUSPENDERS_SOURCE AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../suspenders.h/include/suspenders.h")
    set(PIGEON_SUSPENDERS_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/../suspenders.h")
endif()
if(NOT PIGEON_MEMENTO_SOURCE AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../memento/include/memento.h")
    set(PIGEON_MEMENTO_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/../memento")
endif()

if(PIGEON_MEMENTO_SOURCE)
    set(memento_SOURCE_DIR "${PIGEON_MEMENTO_SOURCE}")
    message(STATUS "Memento: local ${memento_SOURCE_DIR}")
else()
    FetchContent_Declare(
        memento
        GIT_REPOSITORY https://github.com/StraylightRunMOO/memento.git
        GIT_TAG        v2.2.1
        GIT_SHALLOW    TRUE
    )
    FetchContent_GetProperties(memento)
    if(NOT memento_POPULATED)
        FetchContent_Populate(memento)
    endif()
    message(STATUS "Memento: FetchContent v2.2.1 -> ${memento_SOURCE_DIR}")
endif()
set(MEMENTO_INCLUDE_DIR "${memento_SOURCE_DIR}/include")

if(PIGEON_SUSPENDERS_SOURCE)
    set(suspenders_SOURCE_DIR "${PIGEON_SUSPENDERS_SOURCE}")
    message(STATUS "Suspenders: local ${suspenders_SOURCE_DIR}")
else()
    FetchContent_Declare(
        suspenders
        GIT_REPOSITORY https://github.com/StraylightRunMOO/suspenders.h.git
        GIT_TAG        v1.1.1
        GIT_SHALLOW    TRUE
    )
    FetchContent_GetProperties(suspenders)
    if(NOT suspenders_POPULATED)
        FetchContent_Populate(suspenders)
    endif()
    message(STATUS "Suspenders: FetchContent v1.1.1 -> ${suspenders_SOURCE_DIR}")
endif()
set(SUSPENDERS_INCLUDE_DIR "${suspenders_SOURCE_DIR}/include")

set(PIGEON_RUNTIME_LIBS Threads::Threads)
if(UNIX AND NOT APPLE)
    find_package(PkgConfig QUIET)
    if(PKG_CONFIG_FOUND)
        pkg_check_modules(LIBURING liburing>=2.0)
    endif()
    if(NOT LIBURING_FOUND)
        find_path(LIBURING_INCLUDE_DIR NAMES liburing.h)
        find_library(LIBURING_LIBRARY NAMES uring)
        if(LIBURING_INCLUDE_DIR AND LIBURING_LIBRARY)
            set(LIBURING_FOUND TRUE)
            set(LIBURING_INCLUDE_DIRS ${LIBURING_INCLUDE_DIR})
            set(LIBURING_LIBRARIES ${LIBURING_LIBRARY})
        endif()
    endif()
    if(NOT LIBURING_FOUND)
        message(FATAL_ERROR "liburing not found. Install liburing-dev or set PIGEON paths.")
    endif()
    list(APPEND PIGEON_RUNTIME_LIBS ${LIBURING_LIBRARIES})
endif()

# Build options
option(BUILD_SHARED_LIBS "Build shared library instead of static" OFF)
option(BUILD_TESTS "Build test executable" ON)
option(WREN_NAN_TAGGING "Enable NaN tagging optimization" ON)

# Output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Wren VM source files (wren_runtime.c is the Memento + Suspenders impl TU)
set(WREN_VM_SOURCES
    src/vm/wren_compiler.c
    src/vm/wren_core.c
    src/vm/wren_debug.c
    src/vm/wren_primitive.c
    src/vm/wren_utils.c
    src/vm/wren_value.c
    src/vm/wren_vm.c
    src/vm/wren_iterator.c
    src/vm/wren_runtime.c
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
    src/optional/wren_opt_random.c
)

set(WREN_OPT_HEADERS
    src/optional/wren_opt_random.h
)

# Wren standard library modules
set(WREN_STDLIB_SOURCES
    src/std/wren_stdlib.c
    src/std/io/io.c
    src/std/fs/fs.c
    src/std/os/os.c
    src/std/strconv/strconv.c
    src/std/time/time.c
    src/std/math/math.c
    src/std/math/random/random_module.c
    src/std/strings/strings.c
    src/std/meta/meta.c
    src/std/meta/reflection/reflection.c
    src/std/encoding/json/json.c
)

# .c files #include generated .wren.inc; rebuild when the Wren source changes.
set_source_files_properties(src/std/fs/fs.c PROPERTIES
    OBJECT_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/std/fs/fs.wren.inc)
set_source_files_properties(src/std/io/io.c PROPERTIES
    OBJECT_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/std/io/io.wren.inc)
set_source_files_properties(src/std/os/os.c PROPERTIES
    OBJECT_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/std/os/os.wren.inc)
set_source_files_properties(src/std/time/time.c PROPERTIES
    OBJECT_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/std/time/time.wren.inc)
set_source_files_properties(src/std/math/random/random_module.c PROPERTIES
    OBJECT_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/std/math/random/random.wren.inc)
set_source_files_properties(src/std/encoding/json/json.c PROPERTIES
    OBJECT_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/std/encoding/json/json.wren.inc)

set(WREN_STDLIB_HEADERS
    src/std/wren_stdlib.h
    src/std/io/io.h
    src/std/fs/fs.h
    src/std/os/os.h
    src/std/strconv/strconv.h
    src/std/time/time.h
    src/std/math/math.h
    src/std/math/random/random.h
    src/std/math/random/random_module.h
    src/std/strings/strings.h
    src/std/meta/meta.h
    src/std/meta/reflection/reflection.h
    src/std/encoding/json/json.h
)

# Public headers. wren.h / wren.hpp are a one-release compatibility shim.
set(WREN_PUBLIC_HEADERS
    src/include/pigeon.h
    src/include/pigeon.hpp
    src/include/wren.h
    src/include/wren.hpp
)

# Static library target (libpigeon.a). `wren` remains an in-tree alias.
add_library(pigeon STATIC
    ${WREN_VM_SOURCES}
    ${WREN_VM_HEADERS}
    ${WREN_OPT_SOURCES}
    ${WREN_OPT_HEADERS}
    ${WREN_STDLIB_SOURCES}
    ${WREN_STDLIB_HEADERS}
    ${WREN_PUBLIC_HEADERS}
)
add_library(wren ALIAS pigeon)
add_library(Pigeon::pigeon ALIAS pigeon)

target_include_directories(pigeon
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/vm
        ${CMAKE_CURRENT_SOURCE_DIR}/src/optional
        ${CMAKE_CURRENT_SOURCE_DIR}/src/std
        ${MEMENTO_INCLUDE_DIR}
        ${SUSPENDERS_INCLUDE_DIR}
)
target_compile_definitions(pigeon PRIVATE PIGEON_NO_WREN_DEPRECATION)
if(LIBURING_FOUND)
    target_include_directories(pigeon PRIVATE ${LIBURING_INCLUDE_DIRS})
endif()

# _GNU_SOURCE is defined in the .c files that include suspenders.h.

# Shared library target (libpigeon.so)
add_library(pigeon_shared SHARED
    ${WREN_VM_SOURCES}
    ${WREN_VM_HEADERS}
    ${WREN_OPT_SOURCES}
    ${WREN_OPT_HEADERS}
    ${WREN_STDLIB_SOURCES}
    ${WREN_STDLIB_HEADERS}
    ${WREN_PUBLIC_HEADERS}
)
add_library(wren_shared ALIAS pigeon_shared)
add_library(Pigeon::pigeon_shared ALIAS pigeon_shared)

set_target_properties(pigeon_shared PROPERTIES
    OUTPUT_NAME pigeon
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
)

target_include_directories(pigeon_shared
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/vm
        ${CMAKE_CURRENT_SOURCE_DIR}/src/optional
        ${CMAKE_CURRENT_SOURCE_DIR}/src/std
        ${MEMENTO_INCLUDE_DIR}
        ${SUSPENDERS_INCLUDE_DIR}
)
target_compile_definitions(pigeon_shared PRIVATE PIGEON_NO_WREN_DEPRECATION)
if(LIBURING_FOUND)
    target_include_directories(pigeon_shared PRIVATE ${LIBURING_INCLUDE_DIRS})
endif()


# Compiler definitions
if(NOT WREN_NAN_TAGGING)
    target_compile_definitions(pigeon PRIVATE WREN_NAN_TAGGING=0)
    target_compile_definitions(pigeon_shared PRIVATE WREN_NAN_TAGGING=0)
endif()

# Platform-specific settings
if(WIN32)
    target_compile_definitions(pigeon PRIVATE _CRT_SECURE_NO_WARNINGS)
    target_compile_definitions(pigeon_shared PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()

if(UNIX AND NOT APPLE)
    target_link_libraries(pigeon PUBLIC m ${PIGEON_RUNTIME_LIBS})
    target_link_libraries(pigeon_shared PUBLIC m ${PIGEON_RUNTIME_LIBS})
elseif(CMAKE_SYSTEM_NAME MATCHES "BSD")
    target_link_libraries(pigeon PUBLIC m ${PIGEON_RUNTIME_LIBS})
    target_link_libraries(pigeon_shared PUBLIC m ${PIGEON_RUNTIME_LIBS})
else()
    target_link_libraries(pigeon PUBLIC ${PIGEON_RUNTIME_LIBS})
    target_link_libraries(pigeon_shared PUBLIC ${PIGEON_RUNTIME_LIBS})
endif()

# Compiler warnings and optimizations
if(MSVC)
    target_compile_options(pigeon PRIVATE /W4)
    target_compile_options(pigeon_shared PRIVATE /W4)
else()
    target_compile_options(pigeon PRIVATE -Wall -Wextra -Werror=implicit-function-declaration)
    target_compile_options(pigeon_shared PRIVATE -Wall -Wextra -Werror=implicit-function-declaration)
endif()

# Test executable
if(BUILD_TESTS)
    file(GLOB WREN_API_TEST_SOURCES test/api/*.c)
    file(GLOB WREN_API_TEST_HEADERS test/api/*.h)

    add_executable(pigeon_test
        test/main.c
        test/test.c
        test/test.h
        ${WREN_API_TEST_SOURCES}
        ${WREN_API_TEST_HEADERS}
    )

    target_include_directories(pigeon_test PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/include
        ${CMAKE_CURRENT_SOURCE_DIR}/src/vm
        ${CMAKE_CURRENT_SOURCE_DIR}/src/optional
        ${CMAKE_CURRENT_SOURCE_DIR}/src/std
    )

    target_compile_definitions(pigeon_test PRIVATE PIGEON_NO_WREN_DEPRECATION)
    target_link_libraries(pigeon_test PRIVATE pigeon)

    # Add test target
    enable_testing()
    add_test(NAME pigeon_test COMMAND pigeon_test)
endif()

# Tools (REPL, etc.)
add_subdirectory(tools)

# Installation rules
include(GNUInstallDirs)

install(TARGETS pigeon pigeon_shared
    EXPORT PigeonTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(FILES ${WREN_PUBLIC_HEADERS}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# Export targets
install(EXPORT PigeonTargets
    FILE PigeonTargets.cmake
    NAMESPACE Pigeon::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Pigeon
)

# Create config file
include(CMakePackageConfigHelpers)

configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/PigeonConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/PigeonConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Pigeon
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/PigeonConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/PigeonConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/PigeonConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Pigeon
)

# One-release find_package(Wren) wrapper.
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
