# makefile include to unify the source management.
LIBNAME = VedaLibSoundXM

# SRC should list all .cpp and .c 
SRC  = \
uniminixm_Refresh.c \
uniminixm_Init.c \
uniminixm_StereoFloatMixer.c \
XMSoundObject.cpp \
EquationXMEvent.cpp \
RegisterLibSoundXM.cpp \

# these are the dependant include directories:

INCDEP = -I"../Veda" -I"../VedaLibSoundXM" -I"../VedaLibMath" 
 
