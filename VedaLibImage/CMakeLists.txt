add_library(VedaLibImage STATIC
    BubbleGreyImage.cpp
    GreyImage.cpp
    GreyImage2nSquareSized.cpp
    ImplicitEquationImage.cpp
    PerlinNoiseImage.cpp
    ProceduralImage.cpp
    RegisterLibImage.cpp
    RGBAImage.cpp
    VirtualImage.cpp
)

target_include_directories(VedaLibImage PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(VedaLibImage PUBLIC Veda VedaLibMath)
