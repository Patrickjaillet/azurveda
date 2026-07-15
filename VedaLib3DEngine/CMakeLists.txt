add_library(VedaLib3DEngine STATIC
    LightModel.cpp
    lwo2reader.c
    Media3DView.cpp
    ModifierObject3DColor.cpp
    ModifierObject3DSinusDisplace.cpp
    ModifierObject3DVirtual.cpp
    Object3DGrid.cpp
    Object3DLWOResource.cpp
    Object3DMesh3D.cpp
    Object3DMeshVirtual.cpp
    Object3DMultiplier.cpp
    Object3DScene.cpp
    Object3DVirtual.cpp
    Object3DVirtualDynamic.cpp
    ParticleSetEquation.cpp
    ParticleSetVirtual.cpp
    RegisterLib3DEngine.cpp
    RenderTargetRectangle.cpp
    RenderTargetTexture.cpp
    RenderTargetVirtual.cpp
    Texture3D.cpp
)

target_include_directories(VedaLib3DEngine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(VedaLib3DEngine PUBLIC Veda VedaLibMath VedaLibImage VedaLibImageJPEG)
