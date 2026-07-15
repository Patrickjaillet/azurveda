# makefile include to unify the source management.
# This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
LIBNAME = VedaLib

# SRC should list all .cpp and .c 
SRC  = \
BaseType.cpp \
BaseObject.cpp \
BaseContext.cpp \
ClassNode.cpp \
InterfacePrefObject.cpp \
MediaAccess.cpp \
MediaTimePattern.cpp \
NamedObject.cpp \
PackDynamicType.cpp \
PackFloat.cpp \
PackFloat_FixedPoint.cpp \
PackFloatByte_Color.cpp \
PackFloatByte_Limits.cpp \
PackList.cpp \
PackList_TimeTrack.cpp \
PackList_TimeBlockTrack.cpp \
PackLong.cpp \
PackLong_WithLimits.cpp \
PackObjectReference.cpp \
PackResource.cpp \
PackString_MultipleLines.cpp \
PackString.cpp \
PackStruct.cpp \
PackTreeCell.cpp \
PackULong.cpp \
PackULong_Enums.cpp \
PackULong_Flags.cpp \
PackULong_RandomSeed.cpp \
VirtualMachine.cpp \
VirtualMedia.cpp \
TextLocalized.cpp 

# these are the dependant include directories:
INCDEP = -I"../Veda" 
