# makefile include to unify the source management.
# This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
LIBNAME = VedaLibMath

# SRC should list all .cpp and .c 
SRC  = \
EquationList.cpp \
EquationSpline_X.cpp \
EquationSpline_XY.cpp \
EquationSpline_XYZ.cpp \
PackDynamicFloat.cpp \
RegisterLibMath.cpp \
VirtualEquation.cpp \
VirtualEquationSpline.cpp \
EquationGamma.cpp \
EquationSinus3D.cpp

# these are the dependant include directories:

INCDEP = -I"../Veda" -I"../VedaLibMath" 
 
