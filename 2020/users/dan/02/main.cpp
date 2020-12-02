#include <algorithm>
#include <iostream>
#include <sstream>
#include <numeric>
#include <set>
#include <string>
#include <vector>

using namespace std;
namespace
{
   struct Pass
   {
      int min = 0;
      int max = 0;
      char letter = 'a';
      string password = "";
   };
}

int main()
{
   std::string line;
   std::vector<Pass> entries;

   while (getline(cin, line))
   {
      char tossMe;
      Pass p;
      istringstream ss(line);
      // Format: %d-%d %c: %s
      ss >> p.min >> tossMe >> p.max >> p.letter >> tossMe >> p.password;
      entries.push_back(p);
   }

   // Input check
   //for (auto e : entries)
   //{
   //   std::cout << e.min << "-" << e.max << " " << e.letter << ": " << e.password << std::endl;
   //}

   // Part 1
   const auto validPassCount = ranges::count_if(entries,
      [](Pass p){
         const auto lCount = ranges::count(p.password, p.letter);
         return p.min <= lCount
            && p.max >= lCount;
      });

   std::cout << "Valid passwords (part 1): " << validPassCount << std::endl;

   // Part 2
   // Validate whether the password contains a letter in either (but not both) of the positions (1-based)
   const auto validPass2 = ranges::count_if(entries,
      [](Pass p){
         const bool firstPos = p.password[p.min-1] == p.letter;
         const bool secPos = p.password[p.max-1] == p.letter;
         return firstPos ^ secPos; // Either or, not both
      });

   std::cout << "Valid passwords (part 2): " << validPass2 << std::endl;

   return 0;
}


