#include <algorithm>
#include <fstream>
#include <iostream>
#include <numeric>
#include <set>
#include <map>
#include <string>
#include <vector>
#include <sstream>

#include "../intcode/program.h"

using namespace std;
using namespace IC;

namespace
{
   struct Coord
   {
      int x = 0;
      int y = 0;
   };

   bool operator<(const Coord& a, const Coord& b)
   {
      return std::tie(a.x, a.y) < std::tie(b.x, b.y);
   }

   bool operator==(const Coord& a, const Coord& b)
   {
/*      return a.x == b.x
         && a.y == b.y;*/
      return std::tie(a.x, a.y) == std::tie(b.x, b.y);
   }

   enum class Color
   {
      black = 0,
      white = 1
   };

   struct Tile
   {
      Coord coord;
      Color col;
   };

   enum class Facing : int
   {
      N = 1, E = 2, S = 3, W = 4
   };
   Facing counter(Facing f)
   {
      using FType = underlying_type_t<Facing>;
      auto newF = static_cast<FType>(f) - 1;
      if (newF == 0)
         newF = 4;
      return static_cast<Facing>(newF);
   }
   Facing clockwise(Facing f)
   {
      using FType = underlying_type_t<Facing>;
      auto newF = static_cast<FType>(f) + 1;
      if (newF == 5)
         newF = 1;
      return static_cast<Facing>(newF);
   }
   class Bot
   {
   public:
      // Create a normal bot
      Bot() = default;

      // Create bot on a starting white square
      Bot(bool startingWhite)
         : pos()
         , painted()
      {
         painted[pos] = Color::white;
      }

      // Rotates the bot by direction
      void rotate(int dir)
      {
         if (dir == 0) // left
            fac = counter(fac);
         else if (dir == 1) // right
            fac = clockwise(fac);
         else
            cout << "Invalid direction: " << dir << endl;
      }

      // Moves the bot a single square, by current facing
      void move()
      {
         if (fac == Facing::N)
            pos.y += 1;
         else if (fac == Facing::E)
            pos.x += 1;
         else if (fac == Facing::S)
            pos.y -= 1;
         else if (fac == Facing::W)
            pos.x -= 1;
      }

      void paint(Color c)
      {
         //cout << "Painting: " << pos.x << ", " << pos.y << ": " << (c == Color::black ? "black" : "white") << endl;
         painted[pos] = c;
      }

      map<Coord, Color> getPainted(){ return painted; }

      // Returns color of current tile
      Color currTile()
      {
         auto color = Color::black;
         auto curr = painted.find(pos);
         if (curr != end(painted))
         {
            color = curr->second;
         }
         return color;
      }

      // Executes the command from the brain
      void brainCommand(Int cmd)
      {
         if (isPaintCmd)
         {
            isPaintCmd = false;
            paint(static_cast<Color>(cmd));
         }
         else
         {
            isPaintCmd = true;
            rotate(cmd);
            move();
         }
      }

   private:
      Coord pos;
      Facing fac = Facing::N;
      map<Coord, Color> painted;
      bool isPaintCmd = true;
   };

   void test()
   {
      auto outputs = std::vector<int>{
         1, 0, // white, left
         0, 0, // black, left
         1, 0, // white, left
         1, 0, // white, left
         0, 1, // black, right
         1, 0, // white, left
         1, 0  // white, left
      };

      Bot b{true};

      for (auto i = 0; i < outputs.size(); ++i)
      {
         cout << (int)b.currTile() << ' ';
         b.brainCommand(outputs[i]);
         b.brainCommand(outputs[i+1]);
         ++i;
      }

      cout << endl;
      cout << "Tiles painted: " << b.getPainted().size() << endl;
   }
}

int main()
{
   std::string line;
   Instr program;

   // The program is one big line
   string fileName;
   cin >> fileName;
   ifstream in(fileName);

   string val;
   while(getline(in, val, ','))
   {
      program.push_back(stoll(val));
   }
   for (auto val : program)
      cout << val << ",";
   cout << endl;

   formatProgram(program);

   test();

   // Part 1: Painting test run
   {
      Bot b;
      auto input = [&](){ return static_cast<Int>(b.currTile()); };
      auto output = [&](Int out){ b.brainCommand(out); };
      Program p(program, input, output);
      p.run();

      auto tiles = b.getPainted();
      cout << "# of painted tiles: " << tiles.size() << endl;
   }

   // Part 2: For reals
   {
      Bot b{true};
      auto input = [&](){ return static_cast<Int>(b.currTile()); };
      auto output = [&](Int out){ b.brainCommand(out); };
      Program p(program, input, output);
      p.run();

      // Print the painted tiles
      auto tiles = b.getPainted();
      auto tList = vector<Coord>{};
      for (auto t : tiles)
      {
         if (t.second == Color::white)
            tList.push_back(t.first);
      }

      auto maxX = std::max_element(begin(tList), end(tList), [](Coord a, Coord b){ return a.x < b.x; });
      auto maxY = std::max_element(begin(tList), end(tList), [](Coord a, Coord b){ return a.y < b.y; });
      auto minX = std::min_element(begin(tList), end(tList), [](Coord a, Coord b){ return a.x < b.x; });
      auto minY = std::min_element(begin(tList), end(tList), [](Coord a, Coord b){ return a.y < b.y; });

      cout << "min-max: " << (*minX).x << "-" << (*maxX).x << "; " << (*minY).y << "-" << (*maxY).y << endl;

      for (auto i = (*maxY).y; i >= (*minY).y; --i)
      {
         for (auto j = (*minX).x; j <= (*maxX).x; ++j)
         {
            auto white = std::find(begin(tList), end(tList), Coord{j, i});
            if (white != end(tList))
            {
               cout << 'O';
            }
            else
            {
               cout << ' ';
            }
         }
         cout << endl;
      }
   }



   return 0;
}


