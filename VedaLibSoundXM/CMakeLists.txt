add_library(VedaLibSoundXM STATIC
    EquationXMEvent.cpp
    RegisterLibSoundXM.cpp
    XMSoundObject.cpp
    uniminixm_Init.c
    uniminixm_Refresh.c
    uniminixm_StereoFloatMixer.c
)

target_include_directories(VedaLibSoundXM PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(VedaLibSoundXM PUBLIC Veda VedaLibMath VedaLibImage)
target_compile_definitions(VedaLibSoundXM PRIVATE
    FPM_INTEL
    HAVE_CONFIG_H
    ASO_ZEROCHECK
    _UNIMINIXM_SEEKTIME_
)
