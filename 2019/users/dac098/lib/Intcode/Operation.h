#pragma once

#include <functional>
#include <vector>

#include "Arg.h"
#include "Process.h"

namespace Intcode {

    using Executer = std::function<void(ArgList &, Process &)>;

    using OpMethodID = int;

    struct Operation {

        OpMethodID id;

        std::vector<ArgType> args;

        Executer exec;

        Operation(OpMethodID, std::vector<ArgType>, Executer);
    };

} // namespace Intcode