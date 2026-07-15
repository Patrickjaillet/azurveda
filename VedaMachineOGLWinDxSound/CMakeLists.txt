add_library(VedaMachineOGLWinDxSound STATIC
    OGLMachineWinDxSound.cpp
    OGLMachineWinDxSound_Sound.cpp
)

target_include_directories(VedaMachineOGLWinDxSound PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(VedaMachineOGLWinDxSound PUBLIC Veda VedaMachineOGL dsound dxguid)
