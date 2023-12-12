# QEMU with FPGA

```bash
export CXXFLAGS="-I/usr/share/verilator/include $CXXFLAGS"
export LDFLAGS="-L/usr/share/verilator/lib $LDFLAGS"
./configure --target-list=riscv64-softmmu
make -j
```