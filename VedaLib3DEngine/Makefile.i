# makefile include to unify the source management.
LIBNAME = VedaLib3DEngine

# SRC should list all .cpp and .c 
SRC  = \
LightModel.cpp \
Media3DView.cpp \
Object3DGrid.cpp \
Object3DMesh3D.cpp \
Object3DMeshVirtual.cpp \
Object3DLWOResource.cpp \
Object3DMultiplier.cpp\
ModifierObject3DColor.cpp \
ModifierObject3DSinusDisplace.cpp \
ModifierObject3DVirtual.cpp \
Object3DScene.cpp \
Object3DVirtual.cpp \
Object3DVirtualDynamic.cpp \
ParticleSetEquation.cpp \
ParticleSetVirtual.cpp \
RenderTargetRectangle.cpp \
RenderTargetTexture.cpp \
RenderTargetVirtual.cpp \
Texture3D.cpp \
RegisterLib3DEngine.cpp \
lwo2reader.c 

# these are the dependant include directories:
INCDEP =  -I"../VedaLib3DEngine" -I"../Veda" -I"../VedaLibMath" -I"../VedaLibImage" 

