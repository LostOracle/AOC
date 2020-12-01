#include <algorithm>
#include <iostream>
#include <numeric>
#include <set>
#include <string>
#include <sstream>
#include <functional>
#include <vector>

using namespace std;
namespace
{
    using AdjFunc = std::function< bool(int) >;
    bool isValid(int currVal, AdjFunc adjFunc)
    {
        bool monotonic = true;
        int digitCount = 1;
        vector<int> adjCounts;
        int currDigit = currVal % 10;

        while (currVal > 0)
        {
            currVal = currVal / 10;
            auto nextDigit = currVal % 10;

            if (nextDigit == currDigit)
            {
                ++digitCount;
            }
            else
            {
                adjCounts.push_back(digitCount);
                digitCount = 1;
            }
            if (nextDigit > currDigit)
            {
                monotonic = false;
                break;
            }
            currDigit = nextDigit;
        }

        return monotonic
            && any_of(begin(adjCounts), end(adjCounts), adjFunc);
    }

    AdjFunc exactlyTwo = [](int count){ return count == 2; };
    AdjFunc atLeastTwo = [](int count){ return count >= 2; };

    void test()
    {   
        struct Case { int val; bool pass; };
        const auto initialTest = { 
            Case{111111, true},
            Case{223450, false},
            Case{123789, false},
            Case{122678, true},
            Case{233333, true},
            Case{323232, false},
            Case{556789, true}
        };

        for (auto c : initialTest)
        {
            cout << to_string(c.val) << ": " 
                << (isValid(c.val, atLeastTwo) == c.pass ? "Pass" : "Fail") << endl;
        }

        const auto partTwoTest = {
            Case{112233, true},
            Case{123444, false},
            Case{111122, true}
        };

        for (auto c : partTwoTest)
        {
            cout << to_string(c.val) << ": " 
                << (isValid(c.val, exactlyTwo) == c.pass ? "Pass" : "Fail") << endl;
        }
    }
}

int main()
{
    // Puzzle input
    const auto minVal = 156218;
    const auto maxVal = 652527;

    auto part1 = std::vector<int>();
    auto part2 = std::vector<int>();
    auto currVal = minVal;

    while(currVal < maxVal)
    {
        if (isValid(currVal, atLeastTwo))
            part1.push_back(currVal);
        if (isValid(currVal, exactlyTwo))
            part2.push_back(currVal);
        ++currVal;
    }

    test();

    std::cout << "Part1: Num of valid pws: " << part1.size() << std::endl;
    std::cout << "Part2: Num of valid pws: " << part2.size() << std::endl;

    return 0;
}
