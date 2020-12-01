#include "Process.h"

#include "Arg.h"

#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>

Intcode::Process::Process(VM &v, Instructions in) : vm(&v), script(in) {}

void Intcode::Process::run() {
    while (!halt && script.validPointer()) {
        auto [instruction, success] = script.getValue(true);

        if (!success) {
            throw std::runtime_error("failed to retrieve opcode");
        }

        int opcode = instruction % 100;

        auto opcode_iter = vm->opcodes.find(opcode);

        if (opcode_iter == vm->opcodes.end()) {
            throw std::runtime_error("unknown opcode given: " +
                                     std::to_string(opcode));
        }

        auto [op_id, op_data] = *opcode_iter;

        ArgList args;

        for (unsigned count = 0; count < op_data.args.size(); ++count) {
            auto [cached_value, good] = script.getValue(true);

            if (!good) {
                throw std::runtime_error("failed to retrieve argument");
            }

            Arg arg;

            if (op_data.args[count] == ArgType::SET) {
                arg.value = cached_value;

                args.push_back(arg);

                continue;
            }

            int mode = getMode(instruction, count + 3);

            switch (mode) {
            case 0: {
                // position mode, fetch value at position
                auto [fetched_value, success] = script.getValue(cached_value);

                if (!success) {
                    throw std::runtime_error(
                        "failed to retrieve value for argument");
                }

                arg.value = fetched_value;
                break;
            }
            case 1:
                // immediate mode, do nothing
                arg.value = cached_value;
                break;
            default:
                throw std::runtime_error("unknown argument mode: " +
                                         std::to_string(mode));
            }

            args.push_back(arg);
        }

        op_data.exec(args, *this);
    }
}

Intcode::Instructions &Intcode::Process::getIns() {
    return &script;
}

template <typename T> void Intcode::Process::print(T d) {
    std::cout << d << std::endl;
}

template <typename T, typename... Args>
void Intcode::Process::print(T d, Args &&... args) {
    std::cout << d;

    print(std::forward<Args>(args)...);
}

int Intcode::Process::requestInput() {
    std::string line;

    std::cout << '>' << std::flush;

    if (!std::getline(std::cin, line)) {
        throw std::runtime_error("failed to retrieve input");
    }

    return std::stoi(line);
}

void Intcode::Process::haltExecution() {
    halt = true;
}

int Intcode::Process::getMode(int code, unsigned pos) {
    auto cast = static_cast<double>(pos);
    auto modulus = std::pow(10.0, cast);
    auto divide = std::pow(10.0, cast - 1);

    return static_cast<int>(std::fmod(static_cast<double>(opcode), modulus) /
                            divide);
}