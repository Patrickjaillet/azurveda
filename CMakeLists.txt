cmake_minimum_required(VERSION 3.28)

if(NOT CMAKE_TOOLCHAIN_FILE AND DEFINED ENV{VCPKG_ROOT})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
        CACHE STRING "Vcpkg toolchain file")
endif()

project(AzurVeda LANGUAGES CXX C)

if(NOT CMAKE_SIZEOF_VOID_P EQUAL 8)
    message(FATAL_ERROR "AzurVeda targets x64 exclusively. Configure with -A x64 (or the windows-x64 preset).")
endif()

if(NOT MSVC)
    message(FATAL_ERROR "AzurVeda targets the MSVC toolset exclusively (Windows 10/11, Visual Studio 2026).")
endif()

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")

# Windows 10/11 only (roadmap.md "Conventions de développement"): 0x0A00 = _WIN32_WINNT_WIN10.
add_compile_definitions(WINVER=0x0A00 _WIN32_WINNT=0x0A00)

# The codebase uses plain fopen/sprintf/strncpy throughout; the _s replacements have
# different signatures/error semantics, so blanket-rewriting every call site without a
# test rig risks introducing real bugs. Standard practice for this kind of legacy port.
add_compile_definitions(_CRT_SECURE_NO_WARNINGS)

add_compile_options(/W4)

option(AZURVEDA_BUILD_GPL_MODULES "Build the GPL2-licensed modules (VedaLibSoundMP3, VedaLibDemo, VedaGUIWindowsMFC)" ON)
option(AZURVEDA_BUILD_EXAMPLES "Build the Veda/CodeExamples sample programs" ON)
option(AZURVEDA_BUILD_MFC_EDITOR "Build VedaGUIWindowsMFC (requires the MFC/ATL optional VS component and AZURVEDA_BUILD_GPL_MODULES)" ON)

include(cmake/GenerateVersion.cmake)
set(AZURVEDA_VERSION_HEADER ${CMAKE_BINARY_DIR}/generated/Version.h)
azurveda_generate_version_header(${AZURVEDA_VERSION_HEADER})

add_library(AzurVedaVersion INTERFACE)
target_include_directories(AzurVedaVersion INTERFACE ${CMAKE_BINARY_DIR}/generated)

add_subdirectory(Veda)
azurveda_add_version_header_target(Veda ${AZURVEDA_VERSION_HEADER})
add_subdirectory(VedaLibMath)
add_subdirectory(VedaLibImage)
add_subdirectory(VedaLibImageJPEG)
add_subdirectory(VedaLibSoundXM)
add_subdirectory(VedaMachineOGL)
add_subdirectory(VedaMachineOGLWinDxSound)
add_subdirectory(VedaLib3DEngine)

if(AZURVEDA_BUILD_GPL_MODULES)
    add_subdirectory(VedaLibSoundMP3)
    add_subdirectory(VedaLibDemo)
    if(AZURVEDA_BUILD_MFC_EDITOR)
        add_subdirectory(VedaGUIWindowsMFC)
    endif()
endif()

if(AZURVEDA_BUILD_EXAMPLES)
    add_subdirectory(Veda/CodeExamples)
endif()
