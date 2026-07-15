# makefile include to unify the source management.
LIBNAME = VedaLibDemo

# SRC should list all .cpp and .c 
SRC  = \
Equ_XYZD_Particle.cpp \
Equ_XYZD_ParticleCubes.cpp \
Equ_XYZD_SinusWalls.cpp \
Equ_XYZD_Tore.cpp \
Equ_XYZD_Virtual.cpp \
Image2DEffectFire.cpp \
MarchinCubeSpace.cpp \
Object3DFontArial.cpp \
Object3DFontVirtual.cpp \
Object3DMarchCube.cpp \
Object3DText.cpp 

# these are the dependant include directories:

INCDEP =  -I"../VedaLibDemo" -I"../Veda" -I"../VedaLibMath" -I"../VedaLibImage" -I"../VedaLib3DEngine" -I"../VedaLibImageJPEG"  -I"../VedaLibSoundXM"  -I"../VedaLibSoundMP3"  -I"../VedaLibSoundBAOOM" 


