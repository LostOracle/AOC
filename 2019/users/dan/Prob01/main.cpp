#include <algorithm>
#include <iostream>
#include <numeric>
#include <set>
#include <string>
#include <vector>

using namespace std;
namespace
{
    // Calculate fuel value for a single mass
    int calc(int mass)
    {
        return int{ mass / 3 } - 2;
    }

    // Calculate fuel for mass and its fuel, recursively
    int calcWFuel(int mass)
    {
        auto initFuel = calc(mass);
        if (initFuel > 0)
        {
            return initFuel + calcWFuel(initFuel);
        }
        else
        {
            return 0;
        }
    }
}

int main()
{
    std::string line;
    std::vector<int> masses;
    
    while (getline(cin, line))
    {
        int val = 0;
        sscanf(line.data(), "%d", &val);
        masses.push_back(val);
    }

    std::vector<int> fuelRaw;
    std::for_each(begin(masses), end(masses),
        [&fuelRaw](int mass){ fuelRaw.push_back(calc(mass)); });

    std::cout << "Raw fuel (just orig mass): "
        << std::accumulate(begin(fuelRaw), end(fuelRaw), 0) << std::endl;

    std::vector<int> fuelTotal;
    std::for_each(begin(masses), end(masses),
        [&fuelTotal](int mass){ fuelTotal.push_back(calcWFuel(mass)); });

    std::cout << "Total fuel (incl. fuel mass): "
        << std::accumulate(begin(fuelTotal), end(fuelTotal), 0) << std::endl;

    return 0;
}


