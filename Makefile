### SETTINGS
DEFS=
TEST_PLATFORMS=CPU FPGA
HOST_ARCH:=x86


all:
### VARIABLES ###
# Includes
include ./utils.mk
include ./opencl.mk
# Source Code
SRC_CPP=$(wildcard *.cpp) $(wildcard host/*.cpp)
SRC=$(SRC_C) $(SRC_CPP)
CORE_INC=krnl/core
# Object Files
OBJ_C=$(subst .c,.o,$(SRC_C))
OBJ_CPP=$(subst .cpp,.o,$(SRC_CPP))
OBJ=$(OBJ_C) $(OBJ_CPP)
# Output Files
EXE=blink-bench
LIB=blink.a
# Compiler Flags
CXXFLAGS=-Wall $(addprefix -D,$(DEFS)) -I$(CORE_INC) -g -O0
# Linker Flags
LDLIBS=-lpthread -l:$(LIB)
LDFLAGS=-L.
# If FPGA
ifneq (,$(findstring FPGA,$(TEST_PLATFORMS)))
	CXXFLAGS+=-DFPGA
	CFLAGS += $(opencl_CFLAGS)
	CXXFLAGS += $(opencl_CXXFLAGS)
	LDFLAGS+=$(opencl_LDFLAGS)
	CXXFLAGS+=-fmessage-length=0
	LDFLAGS+=-lrt -lstdc++
endif

### TARGETS & RECIPES ###
.PHONY: all lib docs clean
# Compiler
all: $(EXE)
lib: $(LIB)
# Shell
docs: $(SRC) $(wildcard *.h) $(wildcard tests/*.hpp)
	cd docs && doxygen doxyfile
clean:
	rm -rf $(OBJ) $(EXE) $(LIB) $(LOG)
# Dependencies
# $(OBJ): defs.h types.h
%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $<
# Manual
$(EXE): $(OBJ) | check-xrt
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LDFLAGS) $(LDLIBS)
