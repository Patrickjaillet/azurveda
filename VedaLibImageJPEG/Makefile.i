# makefile include to unify the source management.
LIBNAME = VedaLibImageJPEG

# SRC should list all .cpp and .c 
SRC  = \
ImageJPEG.cpp \
jcomapi.c \
jdapimin.c \
jdapistd.c \
jdcoefct.c \
jdcolor.c \
jddctmgr.c \
jdhuff.c \
jdinput.c \
jdmainct.c \
jdmarker.c \
jdmaster.c \
jdmerge.c \
jdphuff.c \
jdpostct.c \
jdsample.c \
jerror.c \
jidctflt.c \
jidctfst.c \
jidctint.c \
jidctred.c \
jmemansi.c \
jmemmgr.c \
jquant1.c \
jquant2.c \
jutils.c \
RegisterLibImageJPEG.cpp

# these are the dependant include directories:

INCDEP = -I"../VedaLibImageJPEG" -I"../Veda" -I"../VedaLibImage" 
