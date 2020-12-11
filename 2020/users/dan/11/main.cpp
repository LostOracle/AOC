#include <algorithm>
#include <iostream>
#include <sstream>
#include <numeric>
#include <map>
#include <string>
#include <vector>
#include <ranges>
#include <optional>

using namespace std;
namespace
{
   enum class Tile
   {
      floor,
      empty,
      occupied
   };

   using Grid = vector<vector<Tile>>;

   Tile fromString(char c)
   {
      return c == '.' ? Tile::floor
         : (c == 'L' ? Tile::empty
         : Tile::occupied);
   }

   ostream& operator<<(ostream& os, const Tile& t)
   {
      switch(t)
      {
         case Tile::floor:
            os << ".";
            break;
         case Tile::empty:
            os << "L";
            break;
         case Tile::occupied:
            os << "#";
            break;
      }

      return os;
   }

   void print(Grid g)
   {
      for (auto& row : g)
      {
         for (const auto& t : row)
         {
            cout << t;
         }
         cout << endl;
      }

      cout << endl;
   }

   struct SeatOff{ int i = 0; int j = 0; };

   bool checkState(const Grid& g, int i, int j, SeatOff)
   {
      return g[i][j] == Tile::occupied;
   }

   bool inBounds(const Grid& g, int i, int j)
   {
      const auto RowMax = static_cast<int>(g.size());
      const auto ColMax = static_cast<int>(g[0].size());
      return i >= 0 && i < RowMax
         && j >= 0 && j < ColMax;
   }

   // Counts # of adjacent occupied seats
   int countAdjacent(const Grid& g, int i, int j, auto checkFunc)
   {
      // Check all 8 positions. Treat out-of-bounds as floor
      const auto seats = vector<SeatOff>{
         SeatOff{ - 1, - 1},
         SeatOff{   0, - 1},
         SeatOff{ + 1, - 1 },
         SeatOff{ + 1,   0 },
         SeatOff{ + 1, + 1 },
         SeatOff{ - 1, + 1 },
         SeatOff{   0, + 1 },
         SeatOff{ - 1,   0 } };

      return ranges::count_if(seats, [&](SeatOff s){
         const auto newI = i + s.i;
         const auto newJ = j + s.j;
         return inBounds(g, newI, newJ)
            && checkFunc(g, newI, newJ, s); });
   }

   // Counts # of visible occupied seats (using a "ray")
   bool countRay(const Grid& g, int i, int j, SeatOff idx)
   {
      bool found = false;

      while (inBounds(g, i, j)
         && Tile::floor == g[i][j])
      {
         i += idx.i;
         j += idx.j;
      }

      if (inBounds(g, i, j))
      {
         found = (g[i][j] == Tile::occupied);
      }

      return found;
   }

   // Simulates one round. Modifies grid, checks for max occ
   bool simulate(Grid& g, auto checkFunc, int maxOcc)
   {
      bool changed = false;
      auto nextGrid = g;

      for (size_t i = 0; i < g.size(); ++i)
      {
         for (size_t j = 0; j < g[i].size(); ++j)
         {
            auto loc = g[i][j];
            const auto adjOcc = countAdjacent(g, i, j, checkFunc);
            switch(loc)
            {
            case Tile::floor:
               break;
            case Tile::empty:
               if (adjOcc == 0)
               {
                  nextGrid[i][j] = Tile::occupied;
                  changed = true;
               }
               break;
            case Tile::occupied:
               if (adjOcc >= maxOcc)
               {
                  nextGrid[i][j] = Tile::empty;
                  changed = true;
               }
               break;
            }
         }
      }

      // Important! Copy the grid over
      g = nextGrid;

      return changed;
   }
   
}

int main()
{
   string line;
   Grid input;

   while (getline(cin, line))
   {
      vector<Tile> row;
      for (auto c : line)
      {
         row.push_back(fromString(c));
      }

      input.push_back(row);
   }

   // Input check
   //for (auto item : input)
   //{
   //   cout << item << endl;
   //}
   print(input);

   // Part 1
   auto oneInput = input;
   while (simulate(oneInput, checkState, 4)){ }

   // Count # of occupied seats
   const auto countOcc = [](auto input){
      auto count = 0;
      for (const auto& row : input)
      {
         count += ranges::count_if(row, [](Tile t){ return t == Tile::occupied; });
      }
      return count;
   };
   cout << "Part 1: " << countOcc(oneInput) << endl;

   // Part 2
   auto twoInput = input;
   while (simulate(twoInput, checkStateRay, 5)){ }
   cout << "Part 2: " << countOcc(twoInput) << endl;

   return 0;
}


