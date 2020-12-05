#include <algorithm>
#include <iostream>
#include <sstream>
#include <numeric>
#include <set>
#include <string>
#include <vector>
#include <ranges>
#include <optional>

using namespace std;
namespace
{
   struct Seat
   {
      string rowCode;
      string colCode;
      int row = 0;
      int col = 0;
      int seatId = 0;
   };

   int calc(string code, const int max)
   {
      auto lo = 0;
      auto hi = max;
      for (const auto& c : code)
      {
         if (c == 'F' || c == 'L')
         {
            hi = hi - ((hi - lo) / 2) - ((hi - lo) % 2);
         }
         else if (c == 'B' || c == 'R')
         {
            lo = lo + ((hi - lo) / 2) + ((hi - lo) % 2);
         }
         else
         {
            throw range_error("Invalid char: " + c);
         }
      }

      if (lo != hi)
         throw range_error("Row ambiguous: " + to_string(lo) + "-" + to_string(hi));

      return lo;
   }
}

int main()
{
   string line;
   vector<Seat> input;

   while (getline(cin, line))
   {
      Seat s{ line.substr(0, 7), line.substr(7, 3) };
      input.push_back(s);
      
   }

   // Input check
   //for (auto s : input)
   //{
   //   cout << s << endl;
   //}

   // Part 1
   // Calculate seat row & col
   // Then calculate seat ID
   // Then find highest
   for (auto& s : input)
   {
      s.row = calc(s.rowCode, 127);
      s.col = calc(s.colCode, 7);
      s.seatId = s.row * 8 + s.col;
   }
   ranges::sort(input, [](Seat a, Seat b){ return a.seatId > b.seatId; }); // Sorts in reverse (highest)

   cout << "Part 1: " << input[0].seatId << endl;

   // Part 2
   // Find the first seat whose next seat is not 1 less
   auto firstGap = ranges::adjacent_find(input, [](int a, int b){ return a - b > 1; }, &Seat::seatId);
   cout << "Part 2: " << firstGap->seatId-1 << endl;

   return 0;
}


