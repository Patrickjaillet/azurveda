# Generates cmake/Version.h from cmake/Version.h.in, both at configure time and again
# on every build (so AZURVEDA_VERSION_GIT_HASH always reflects the commit being built,
# not just the commit at the last `cmake` configure).
#
# Usage (called from the root CMakeLists.txt):
#   include(cmake/GenerateVersion.cmake)
#   azurveda_generate_version_header(${CMAKE_BINARY_DIR}/generated/Version.h)
#   azurveda_add_version_header_target(AzurVedaVersion ${CMAKE_BINARY_DIR}/generated/Version.h)

# Captured at include() time: CMAKE_CURRENT_LIST_DIR inside the functions below would
# otherwise resolve to the caller's directory (functions don't get their own listfile
# context), not this file's cmake/ directory.
set(_AZURVEDA_CMAKE_MODULE_DIR "${CMAKE_CURRENT_LIST_DIR}")

function(azurveda_generate_version_header OUTPUT_HEADER)
    set(AZURVEDA_SOURCE_DIR "${CMAKE_SOURCE_DIR}")
    set(AZURVEDA_OUTPUT_HEADER "${OUTPUT_HEADER}")
    include("${_AZURVEDA_CMAKE_MODULE_DIR}/WriteVersionHeader.cmake")
endfunction()

function(azurveda_add_version_header_target TARGET_NAME OUTPUT_HEADER)
    add_custom_target(${TARGET_NAME}_UpdateVersionHeader ALL
        COMMAND ${CMAKE_COMMAND}
            "-DAZURVEDA_SOURCE_DIR=${CMAKE_SOURCE_DIR}"
            "-DAZURVEDA_OUTPUT_HEADER=${OUTPUT_HEADER}"
            -P "${_AZURVEDA_CMAKE_MODULE_DIR}/WriteVersionHeader.cmake"
        BYPRODUCTS "${OUTPUT_HEADER}"
        COMMENT "Regenerating Version.h from Git/VERSION"
        VERBATIM
    )
    add_dependencies(${TARGET_NAME} ${TARGET_NAME}_UpdateVersionHeader)
endfunction()
