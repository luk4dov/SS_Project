#include "./terminal.hpp"
#include <termios.h>
#include <unistd.h>

int Terminal::inNonBlock() {
    termios oldt{}, raw{};
    if (tcgetattr(STDIN_FILENO, &oldt) < 0) return -1;
    raw = oldt;
    raw.c_lflag &= ~(ICANON | ECHO);
    raw.c_cc[VMIN]  = 0;
    raw.c_cc[VTIME] = 0;
    if (tcsetattr(STDIN_FILENO, TCSANOW, &raw) < 0) return -1;

    unsigned char ch;
    ssize_t n = read(STDIN_FILENO, &ch, 1);

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return (n == 1) ? ch : -1;
}

void Terminal::in() {
    int ret = inNonBlock();
    if(ret == -1) return;
    
    uint32 term_in = cpu->getTermIn();
    cpu->writeMem(term_in, ret);

    cpu->interrupt(INT_CAUSE::TERMINAL);
}

void Terminal::out() {
    int c = (int)(cpu->readMem(cpu->getTermOut()));
    if(c != -1) {
        std::cout << static_cast<char>(c) << std::flush;
        cpu->writeMem(cpu->getTermOut(), -1);
    }
}

void Terminal::check() {
    in();

    out();
}