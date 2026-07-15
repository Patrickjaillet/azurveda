# makefile include to unify the source management.
LIBNAME = VedaMachineOGL

# $(OBJREP) is used by the caller.
# SRC should list all .cpp and .c
SRC  = OGLMachine.cpp

# these are the dependant include directories:

INCDEP = -I"../Veda"  -I"../VedaMachineOGL"

