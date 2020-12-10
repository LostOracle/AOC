#include <algorithm>
#include <iostream>
#include <sstream>
#include <numeric>
#include <map>
#include <string>
#include <vector>
#include <ranges>
#include <optional>
#include <span>
#include <cmath>

using namespace std;
namespace
{
}

int main()
{
   string line;
   vector<uint64_t> input;

   input.push_back(0); // Add an entry for the starting point
   while (getline(cin, line))
   {
      input.push_back(stoull(line));
   }

   // Input check
   ranges::sort(input); // Sort because we are ordering adapters
   input.push_back(input.back() + 3); // Add the ending point
   //for (auto item : input)
   //{
   //   cout << item << endl;
   //}

   // Part 1
   // Count the differences (gaps) between numbers
   auto input2 = input; // Copy for adjacent_difference
   adjacent_difference(begin(input2), end(input2), begin(input2));
   const auto ones = ranges::count_if(input2, [](auto a){ return a == 1; });
   const auto threes = ranges::count_if(input2, [](auto a){ return a == 3; });
   cout << "Part 1: " << ones * threes << endl;

   // Part 2
   // Figure out permutations of valid arrangements

   // First, group numbers, with separations whenever a 3-gap is encountered
   // We can't change the numbers on an edge of a 3-gap (because it would violate our rule)
   vector<vector<uint64_t>> groups;
   vector<uint64_t> currGroup;
   auto prev = 0;
   for (auto i : input)
   {
      // Gap < 3 goes in current group
      if (i - prev < 3)
      {
         currGroup.push_back(i);
      }
      else
      {
         // Larger gaps go in a new group
         groups.push_back(currGroup);
         currGroup.clear();
         currGroup.push_back(i);
      }
      prev = i;
   }

   // Print so we can examine the data
   for (auto g : groups)
   {
      cout << "[ ";
      for (auto i : g)
      {
         cout << i << " ";
      }
      cout << "], ";
   }
   cout << groups.size() << endl;

   // Count # of groups of each size.
   // We don't care about 1 and 2, because those don't contribute to permutations
   const auto three = ranges::count_if(groups, [](auto g){ return g.size() == 3; });
   const auto four = ranges::count_if(groups, [](auto g){ return g.size() == 4; });
   const auto five = ranges::count_if(groups, [](auto g){ return g.size() == 5; });

   // four-size groups have 2 values which can change independently (in the middle)
   const auto pow2 = three + (four*2);
   const auto pow7 = five;

   // In the end, we have independent values (three- & four-groups) and dependent (5-groups)
   // Dependent values can have all permutations except the 8th (all removed).
   // So dependent groups' value is 7.
   cout << "Part 2: " << static_cast<uint64_t>( pow(2, pow2) * pow(7, pow7) ) << endl;

   return 0;
}


