﻿cmake_minimum_required(VERSION 3.5)

project("Anfora ModLoader")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MD")

set(BINARY_OUTPUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${BINARY_OUTPUT_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${BINARY_OUTPUT_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BINARY_OUTPUT_DIR})

add_compile_definitions(_CRT_SECURE_NO_WARNINGS) # _fpcontrol & std::sscanf


# Sources

file(GLOB_RECURSE SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/src/loader/*.cpp)
list(APPEND LOADER_SOURCES ${SOURCES})

file(GLOB_RECURSE SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/src/launcher/*.cpp)
list(APPEND LAUNCHER_SOURCES ${SOURCES} "${CMAKE_CURRENT_SOURCE_DIR}/win_resource.rc")

file(GLOB_RECURSE SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/src/example-mod/*.cpp)
list(APPEND MOD_SOURCES ${SOURCES})


# Detours

list(APPEND LOADER_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/lib/detours/include)
list(APPEND LOADER_LIBS ${CMAKE_CURRENT_SOURCE_DIR}/lib/detours/x86/detours.lib)

list(APPEND LAUNCHER_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/lib/detours/include)
list(APPEND LAUNCHER_LIBS ${CMAKE_CURRENT_SOURCE_DIR}/lib/detours/x86/detours.lib)

# CM2

list(APPEND LOADER_INCLUDES $ENV{CM2_ROOT}/lib/cm2/include)
list(APPEND LOADER_LIBS $ENV{CM2_ROOT}/lib/cm2/x86/cm2.lib)

list(APPEND LAUNCHER_INCLUDES $ENV{CM2_ROOT}/lib/cm2/include)
list(APPEND LAUNCHER_LIBS $ENV{CM2_ROOT}/lib/cm2/x86/cm2.lib)

list(APPEND MOD_INCLUDES $ENV{CM2_ROOT}/lib/cm2/include)
list(APPEND MOD_LIBS $ENV{CM2_ROOT}/lib/cm2/x86/cm2.lib)

# Faktum

list(APPEND LOADER_INCLUDES $ENV{CM2_ROOT}/lib/faktum/include)
list(APPEND LOADER_LIBS $ENV{CM2_ROOT}/lib/faktum/x86/faktum.lib)

list(APPEND LAUNCHER_INCLUDES $ENV{CM2_ROOT}/lib/faktum/include)
list(APPEND LAUNCHER_LIBS $ENV{CM2_ROOT}/lib/faktum/x86/faktum.lib)

list(APPEND MOD_INCLUDES $ENV{CM2_ROOT}/lib/faktum/include)
list(APPEND MOD_LIBS $ENV{CM2_ROOT}/lib/faktum/x86/faktum.lib)

# ZLib

list(APPEND LOADER_INCLUDES $ENV{CM2_ROOT}/lib/zlib/include)

list(APPEND LAUNCHER_INCLUDES $ENV{CM2_ROOT}/lib/zlib/include)

list(APPEND MOD_INCLUDES $ENV{CM2_ROOT}/lib/zlib/include)

# Loader

list(APPEND LOADER_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/lib/anforaloader/include)

list(APPEND LAUNCHER_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/lib/anforaloader/include)
list(APPEND LAUNCHER_LIBS ${CMAKE_CURRENT_SOURCE_DIR}/lib/anforaloader/x86/anforaloader.lib)

list(APPEND MOD_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/lib/anforaloader/include)
list(APPEND MOD_LIBS ${CMAKE_CURRENT_SOURCE_DIR}/lib/anforaloader/x86/anforaloader.lib)

# WinAPI Unicode

add_compile_definitions(UNICODE _UNICODE)


# Targets

add_library("anforaloader" SHARED ${LOADER_SOURCES})
target_link_libraries("anforaloader" ${LOADER_LIBS})
target_include_directories("anforaloader" PRIVATE ${LOADER_INCLUDES})

add_executable("anfora" WIN32 ${LAUNCHER_SOURCES})
target_link_libraries("anfora" ${LAUNCHER_LIBS})
target_include_directories("anfora" PRIVATE ${LAUNCHER_INCLUDES})

add_library("example-mod" SHARED ${MOD_SOURCES})
target_link_libraries("example-mod" ${MOD_LIBS})
target_include_directories("example-mod" PRIVATE ${MOD_INCLUDES})
