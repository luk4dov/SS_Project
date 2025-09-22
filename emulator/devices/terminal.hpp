#ifndef TERMINAL_HPP
#define TERMINAL_HPP

#include "./cpu.hpp"

class Terminal {

public:
    Terminal(CPU* cpu) : cpu(cpu) {}
    ~Terminal() {}

    void check();

private:
    CPU* cpu;

    void in();
    void out();
    int inNonBlock();
};

#endif