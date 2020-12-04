#include <algorithm>
#include <iostream>
#include <sstream>
#include <numeric>
#include <set>
#include <string>
#include <vector>
#include <ranges>

using namespace std;
namespace
{
   // Tree map is a double array - row major
   using TreeMap = vector<string>;

   struct Slope { int x = 0; int y = 0; };

   const char Tree = '#';
   const char Open = '.';

   uint64_t countTrees(const TreeMap& tm, const int xDiff, const int yDiff)
   {
      const auto maxX = tm[0].size();
      const auto maxY = tm.size();

      // Increment by Y position using iota (row index) and filter (row skips)
      auto selectedRows = views::iota(0U, maxY)
         | views::filter([yDiff](int i){ return 0 == i % yDiff; });

      auto x = 0;
      auto treeCount = 0;
      for ( const auto i : selectedRows )
      {
         const auto& row = tm[i];
         treeCount += (row[x] == Tree ? 1 : 0); // Are we on a tree?
         x = (x + xDiff) % maxX; // Increment X position
      }

      return treeCount;
   }
}

int main()
{
   std::string line;
   TreeMap tm;

   while (getline(cin, line))
   {
      tm.push_back(line);
   }

   // Input check
   //for (auto l : tm)
   //{
   //   cout << l << endl;
   //}

   // Part 1
   // Count # of trees going right 3, down 1
   cout << "Part 1: " << countTrees(tm, 3, 1) << endl;

   // Part 2
   // Check multiple slopes, incrementing by different values
   const auto slopes = vector<Slope>{ {1, 1}, {3, 1}, {5, 1}, {7, 1}, {1, 2} };

   // Calculate # of trees for each slope
   const auto treeCounts = slopes
      | views::transform( [&tm](Slope s){ return countTrees(tm, s.x, s.y); } );

   // Multiple all tree counts together
   const auto product = accumulate(begin(treeCounts), end(treeCounts), uint64_t{ 1 },
      std::multiplies<uint64_t>());
   
   std::cout << "Part 2: " << product << std::endl;

   return 0;
}


