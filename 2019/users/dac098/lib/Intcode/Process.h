#pragma once

#include "VM.h"

#include "Instructions.h"

namespace Intcode {

    class Process {
    public:
        Process(VM &, Instructions);

        void run();

        Instructions &getIns();

        template <typename T> void print(T);
        template <typename T, typename... Args> void print(T, Args&&...);

        int requestInput();

        void haltExecution();

    private:

        bool halt = false;

        VM* vm = nullptr;

        Instructions script;

        int getMode(int,unsigned);
    };

} // namespace Intcode