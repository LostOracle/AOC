#include "Operation.h"

Intcode::Operation::Operation(OpMethodID i, std::vector<ArgType> a, Executer e)
    : id(i), args(a), exec(e) {}