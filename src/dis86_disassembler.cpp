#include <iostream>
#include <string>
#include <fstream>
#include <dis86_instruction_stream.h>

int main(int argc, char **argv) {
    if (argc != 2) {
        std::cerr << "please enter exactly 2 arguments" << std::endl;
        std::exit(1);
    }
    std::ifstream binfile(argv[1]);

    InstStream instStream(&binfile);
    Instruction inst;
    while (inst = instStream.NextInstruction()) {
        inst.Print();
    }
    return 0;
}