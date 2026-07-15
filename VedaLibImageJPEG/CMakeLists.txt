find_package(JPEG REQUIRED)

add_library(VedaLibImageJPEG STATIC
    ImageJPEG.cpp
    RegisterLibImageJPEG.cpp
)

target_include_directories(VedaLibImageJPEG PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(VedaLibImageJPEG PUBLIC Veda VedaLibMath VedaLibImage JPEG::JPEG)
