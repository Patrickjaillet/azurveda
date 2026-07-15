add_library(VedaLibDemo STATIC
    Equ_XYZD_Particle.cpp
    Equ_XYZD_ParticleCubes.cpp
    Equ_XYZD_SinusWalls.cpp
    Equ_XYZD_Tore.cpp
    Equ_XYZD_Virtual.cpp
    Image2DEffectFire.cpp
    MarchinCubeSpace.cpp
    Object3DFontArial.cpp
    Object3DFontVirtual.cpp
    Object3DMarchCube.cpp
    Object3DText.cpp
    RegisterLibDemo.cpp
)

target_include_directories(VedaLibDemo PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(VedaLibDemo PUBLIC
    Veda
    VedaLibMath
    VedaLibImage
    VedaLibImageJPEG
    VedaLibSoundMP3
    VedaLibSoundXM
    VedaLib3DEngine
)
