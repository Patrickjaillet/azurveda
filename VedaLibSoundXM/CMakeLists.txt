add_library(VedaLibSoundXM STATIC
    EquationXMEvent.cpp
    RegisterLibSoundXM.cpp
    XMSoundObject.cpp
    uniminixm_Init.c
    uniminixm_Refresh.c
    uniminixm_StereoFloatMixer.c
)

# Vendored uniminixm decoder: relax /W4 /WX to /W1 /WX- (third-party code, not
# held to AzurVeda's warning bar; see root CMakeLists.txt).
set_source_files_properties(
    uniminixm_Init.c uniminixm_Refresh.c uniminixm_StereoFloatMixer.c
    PROPERTIES COMPILE_OPTIONS "/W1;/WX-"
)

target_include_directories(VedaLibSoundXM PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(VedaLibSoundXM PUBLIC Veda VedaLibMath VedaLibImage)
target_compile_definitions(VedaLibSoundXM PRIVATE
    FPM_INTEL
    HAVE_CONFIG_H
    ASO_ZEROCHECK
    _UNIMINIXM_SEEKTIME_
)
