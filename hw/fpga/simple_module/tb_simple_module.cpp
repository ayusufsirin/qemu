#include "Vsimple_module.h"
#include "verilated.h"

int main(int argc, char **argv) {
    Verilated::commandArgs(argc, argv);
    Vsimple_module* top = new Vsimple_module;

    while (!Verilated::gotFinish()) {
        top->clk = !top->clk;
        top->eval();
    }
    delete top;
    return 0;
}
