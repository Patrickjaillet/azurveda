# makefile include to unify the source management.
LIBNAME = VedaLibSoundMP3

# SRC should list all .cpp and .c 

SRC = \
decoder.c \
fixed.c \
frame.c \
huffman.c \
layer12.c \
layer3.c \
MP3SoundObject.cpp \
RegisterLibSoundMP3.cpp \
stream.c \
synth.c \
timer.c \
version.c \
bit.c

# these are the dependant include directories:

INCDEP = -I"../Veda" -I"../VedaLibSoundMP3"

