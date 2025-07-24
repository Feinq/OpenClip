cmake_minimum_required(VERSION 3.20)
project(OpenClipNative LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(PROJECT_VERSION "0.1.0")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_library(OpenClipNative SHARED
    src/pch.cpp
    src/dllmain.cpp
    src/OCNativeAudio.cpp
    src/BackendFactory.cpp
)

set(prefix ${CMAKE_INSTALL_PREFIX})
set(exec_prefix \${prefix})
set(libdir \${exec_prefix}/lib)
set(includedir \${exec_prefix}/include)
set(PACKAGE_VERSION ${PROJECT_VERSION})

configure_file(
    ${PROJECT_SOURCE_DIR}/OpenClipNative.pc.in
    ${CMAKE_CURRENT_BINARY_DIR}/OpenClipNative.pc
    @ONLY
)

install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/OpenClipNative.pc
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
)

if(NOT WIN32)
    set_source_files_properties(src/dllmain.cpp PROPERTIES HEADER_FILE_ONLY TRUE)
endif()

target_include_directories(OpenClipNative
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
    PRIVATE
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}/platform
)

if(WIN32)
    add_subdirectory(platform/windows)
elseif(UNIX)
    add_subdirectory(platform/linux)
endif()

if(WIN32)
    target_compile_definitions(OpenClipNative PRIVATE OPENCLIP_NATIVE_EXPORTS)
endif()

if(WIN32)
    include_directories(
        ${PROJECT_SOURCE_DIR}/generated
    )
    
    target_link_libraries(OpenClipNative PRIVATE
        ole32
        uuid
        winmm
    )
endif()

if(MSVC)
    target_compile_options(OpenClipNative PRIVATE /W4 /permissive-)
else()
    target_compile_options(OpenClipNative PRIVATE -Wall -Wextra -Wpedantic)
endif()
