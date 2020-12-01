#include <algorithm>
#include <iostream>
#include <numeric>
#include <set>
#include <string>
#include <vector>

using namespace std;
namespace
{
}

int main()
{
   std::string line;
   std::vector<int> entries;

   while (getline(cin, line))
   {
      int val = 0;
      sscanf(line.data(), "%d", &val);
      entries.push_back(val);
   }

   // Part 1
   for (auto starter : entries)
   {
      for (auto ender : entries)
      {
         if (starter + ender == 2020)
         {
            std::cout << "Part 1 sum: " << starter << " + " << ender
               << " = 2020; Product: " << starter * ender << std::endl;
         }
      }
   }

   // Part 2
   for (auto a : entries)
   {
      for (auto b : entries)
      {
         for (auto c : entries)
         {
            if (a + b + c == 2020)
            {
               std::cout << "Part 2 sum: " << a << " + " << b << " + " << c
                  << " = 2020; Product: " << a * b * c << std::endl;
            }
         }
      }
   }

   return 0;
}


