# Simple Verilog Module Verilator Demo

## Run Verilator

```bash
verilator -Wall --cc simple_module.v --exe tb_simple_module.cpp
```

# Compile Verilator Output

```bash
cd obj_dir
make -f Vsimple_module.mk
```

# Run Simulation


```bash
./Vsimple_module
```

