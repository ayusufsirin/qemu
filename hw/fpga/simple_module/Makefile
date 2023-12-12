# Makefile for Verilator project

# Verilator executable
VERILATOR = verilator

# Verilator flags
VERILATOR_FLAGS = -Wall --cc

# Name of the top module
TOP_MODULE = simple_module

# Source files
VERILOG_SRC = simple_module.v
# TB_SRC = tb_simple_module.cpp
TB_SRC = tb_simple.cpp

# Directory for object files
OBJ_DIR = obj_dir

# Compilation settings
CXX = g++
CXXFLAGS = -I$(OBJ_DIR) -I/usr/share/verilator/include

# Linker settings
LDLIBS = -lelf
LINK.o = $(CXX) $(CXXFLAGS) -static $(LDLIBS)

# Default target
all: $(OBJ_DIR)/V$(TOP_MODULE)

# Rule to run Verilator
$(OBJ_DIR)/V$(TOP_MODULE): $(VERILOG_SRC) $(TB_SRC)
	$(VERILATOR) $(VERILATOR_FLAGS) $(VERILOG_SRC) --exe $(TB_SRC)
	cd $(OBJ_DIR) && make -f V$(TOP_MODULE).mk

# Clean target
clean:
	rm -rf $(OBJ_DIR)

.PHONY: all clean
