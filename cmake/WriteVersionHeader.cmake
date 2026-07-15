# Writes AZURVEDA_OUTPUT_HEADER from AZURVEDA_SOURCE_DIR/cmake/Version.h.in.
#
# Version source of truth, in priority order:
#   1. Latest annotated Git tag reachable from HEAD (format vMAJOR.MINOR.PATCH)
#   2. The VERSION file at the repository root (fallback when no tag exists)
#
# Runnable both as an included module (project/configure-time context, where
# AZURVEDA_SOURCE_DIR/AZURVEDA_OUTPUT_HEADER default to CMAKE_SOURCE_DIR and
# CMAKE_BINARY_DIR/generated/Version.h) and in `cmake -P` script mode (build-time
# context, where both must be passed via -D).

if(NOT DEFINED AZURVEDA_SOURCE_DIR)
    set(AZURVEDA_SOURCE_DIR "${CMAKE_SOURCE_DIR}")
endif()
if(NOT DEFINED AZURVEDA_OUTPUT_HEADER)
    set(AZURVEDA_OUTPUT_HEADER "${CMAKE_BINARY_DIR}/generated/Version.h")
endif()

find_package(Git QUIET)

set(AZURVEDA_VERSION_GIT_HASH "unknown")
set(_git_tag_version "")

if(Git_FOUND)
    execute_process(
        COMMAND ${GIT_EXECUTABLE} describe --tags --match "v[0-9]*.[0-9]*.[0-9]*" --abbrev=0
        WORKING_DIRECTORY ${AZURVEDA_SOURCE_DIR}
        OUTPUT_VARIABLE _git_tag_output
        OUTPUT_STRIP_TRAILING_WHITESPACE
        RESULT_VARIABLE _git_tag_result
        ERROR_QUIET
    )
    if(_git_tag_result EQUAL 0)
        string(REGEX REPLACE "^v" "" _git_tag_version "${_git_tag_output}")
    endif()

    execute_process(
        COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
        WORKING_DIRECTORY ${AZURVEDA_SOURCE_DIR}
        OUTPUT_VARIABLE _git_hash_output
        OUTPUT_STRIP_TRAILING_WHITESPACE
        RESULT_VARIABLE _git_hash_result
        ERROR_QUIET
    )
    if(_git_hash_result EQUAL 0)
        set(AZURVEDA_VERSION_GIT_HASH "${_git_hash_output}")
    endif()
endif()

if(_git_tag_version)
    set(AZURVEDA_VERSION_STRING "${_git_tag_version}")
else()
    file(READ "${AZURVEDA_SOURCE_DIR}/VERSION" AZURVEDA_VERSION_STRING)
    string(STRIP "${AZURVEDA_VERSION_STRING}" AZURVEDA_VERSION_STRING)
endif()

if(NOT AZURVEDA_VERSION_STRING MATCHES "^[0-9]+\\.[0-9]+\\.[0-9]+$")
    message(FATAL_ERROR "AzurVeda version '${AZURVEDA_VERSION_STRING}' does not match MAJOR.MINOR.PATCH")
endif()

string(REPLACE "." ";" _version_parts "${AZURVEDA_VERSION_STRING}")
list(GET _version_parts 0 AZURVEDA_VERSION_MAJOR)
list(GET _version_parts 1 AZURVEDA_VERSION_MINOR)
list(GET _version_parts 2 AZURVEDA_VERSION_PATCH)

configure_file(
    "${AZURVEDA_SOURCE_DIR}/cmake/Version.h.in"
    "${AZURVEDA_OUTPUT_HEADER}"
    @ONLY
)
