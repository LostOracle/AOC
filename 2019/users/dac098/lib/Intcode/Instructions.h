#pragma once

#include <tuple>
#include <vector>

namespace Intcode {

    class Instructions {

        using GetValueResult = std::pair<int, bool>;

        Instructions();
        Instructions(const std::vector<int> &);

        bool validPointer();

        GetValueResult getValue(bool);
        GetValueResult getValue(int);
        GetValueResult getValue(std::size_t);

        bool setValue(std::size_t, int);
        bool setValue(int, int);

        std::size_t getPointer();

        bool setPointer(int pos);
        bool setPointer(std::size_t);

    private:
        std::size_t pointer = 0;

        std::vector<int> memory;
    };

} // namespace Intcode