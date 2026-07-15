# makefile include to unify the source management.
LIBNAME = VedaLibImage

# SRC should list all .cpp and .c 
SRC  = \
BubbleGreyImage.cpp \
GreyImage.cpp \
GreyImage2nSquareSized.cpp \
ImplicitEquationImage.cpp \
PerlinNoiseImage.cpp \
ProceduralImage.cpp \
RegisterLibImage.cpp \
RGBAImage.cpp \
VirtualImage.cpp \

# these are the dependant include directories:

INCDEP = -I"../Veda" -I"../VedaLibMath" -I"../VedaLibImage" 
 
