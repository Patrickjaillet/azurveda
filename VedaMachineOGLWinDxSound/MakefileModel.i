# Dreamcast Makefile model, to be included.

WINDRES = windres.exe
CPP  = g++.exe
CC   = gcc.exe

OBJCPP = $(patsubst %.cpp,$(OBJREP)/%.o,$(SRC))
OBJ   = $(patsubst %.c,$(OBJREP)/%.oc,$(OBJCPP))
OBJED = $(patsubst $(OBJREP)/%,$(OBJREPED)/%,$(OBJ))
LINKOBJ = $(OBJ)
LIBS =   -L"D:/code/Dev-Cpp/lib"
INCS = -I"D:/code/Dev-Cpp/lib/gcc/mingw32/3.4.2/include"  -I"D:/code/Dev-Cpp/include/c++/3.4.2/backward"  -I"D:/code/Dev-Cpp/include/c++/3.4.2/mingw32"  -I"D:/code/Dev-Cpp/include/c++/3.4.2"  -I"D:/code/Dev-Cpp/include" $(INCDEP)
CXXINCS =  -I"D:/code/Dev-Cpp/lib/gcc/mingw32/3.4.2/include"  -I"D:/code/Dev-Cpp/include/c++/3.4.2/backward"  -I"D:/code/Dev-Cpp/include/c++/3.4.2/mingw32"  -I"D:/code/Dev-Cpp/include/c++/3.4.2"  -I"D:/code/Dev-Cpp/include"  $(INCDEP)

CXXFLAGS = $(CXXINCS) -O1 -fno-rtti -fno-exceptions -fno-strict-aliasing -fno-common -ffreestanding -fomit-frame-pointer
CFLAGS = $(INCS) -O1 -fno-rtti -fno-exceptions -fno-strict-aliasing -fno-common -ffreestanding -fomit-frame-pointer
RM = rm -f

.PHONY: all all-before all-after clean clean-custom

all: $(OBJREP) $(BIN)

clean: clean-custom
	@echo ""
	@echo "		Clean:" $(BIN)
	@echo ""
	${RM} $(OBJ) $(BIN)

$(BIN): $(LINKOBJ)
	@echo ""
	@echo "		Link:" $(BIN)
	@echo ""
	ar r $(BIN) $(LINKOBJ)
	ranlib $(BIN)
#	${RM} $(LINKOBJ)

$(OBJREP):
	@mkdir $(OBJREP)
	
$(OBJREP)/%.o: $(SRCREP)/%.cpp
	$(CPP) -c $< -o $@ $(CXXFLAGS)

$(OBJREP)/%.oc: $(SRCREP)/%.c
	$(CC) -c $< -o $@ $(CFLAGS)
