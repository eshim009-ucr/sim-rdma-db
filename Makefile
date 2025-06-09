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
SRC_LIB=$(wildcard host/*.cpp)
SRC_EXE=$(wildcard *.cpp)
CORE_INC=krnl/core
# Object Files
OBJ_LIB=$(subst .cpp,.o,$(SRC_LIB))
OBJ_EXE=$(subst .cpp,.o,$(SRC_EXE))
OBJ=$(OBJ_LIB) $(OBJ_EXE)
# Output Files
EXE=blink-bench
LIB=blink-fpga.so
# Compiler Flags
CXXFLAGS=-Wall $(addprefix -D,$(DEFS)) -I$(CORE_INC) -g -O0
# Linker Flags
LDLIBS+=-l:blink.a
LDFLAGS+=-L.
# If FPGA
ifneq (,$(findstring FPGA,$(TEST_PLATFORMS)))
	CXXFLAGS+=-DFPGA
	CFLAGS+=$(opencl_CFLAGS)
	CXXFLAGS+=$(opencl_CXXFLAGS)
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

# Manual
%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) -Ihost -o $@ $<
$(EXE): $(OBJ_EXE) $(LIB) | check-xrt
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LDFLAGS) -l:$(LIB) $(LDLIBS)

host/%.o: host/%.cpp
	$(CXX) -fpic -shared -c $(CXXFLAGS) -o $@ $<
$(LIB): $(OBJ_LIB) | check-xrt
	$(CXX) -fpic -shared -o $@ $^ $(CXXFLAGS) $(LDFLAGS) $(LDLIBS)
