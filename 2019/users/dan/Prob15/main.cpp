#include <algorithm>
#include <fstream>
#include <iostream>
#include <numeric>
#include <set>
#include <map>
#include <string>
#include <vector>
#include <sstream>
#include <chrono>
#include <thread>
#include <stack>
#include <queue>

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
      // Sorting in row major order lets us be lazy with display
      return std::tie(a.y, a.x) < std::tie(b.y, b.x);
   }

   bool operator==(const Coord& a, const Coord& b)
   {
      return std::tie(a.y, a.x) == std::tie(b.y, b.x);
   }

   enum class Type
   {
      wall = 0,
      empty = 1,
      oxygen = 2
   };

   struct Tile
   {
      Coord coord;
      Type t;
      int steps = 0;
   };

   ostream& operator<<(ostream& os, Coord c)
   {
      return os << "(" << c.x << ", " << c.y << ")";
   }

   ostream& operator<<(ostream& os, Tile tile)
   {
      const static auto m = map<Type, char>{
         {Type::wall,      'W'},
         {Type::empty,     '.'},
         {Type::oxygen,    'O'},
      };
      return os << m.at(tile.t);
   }

   enum class Direction
   {
      north = 1,
      south = 2,
      west = 3,
      east = 4
   };

   struct Move
   {
      Direction d;
      int steps = 0;
      bool backwards = false;
   };

   Direction opp(Direction d)
   {
      return (d == Direction::north ? Direction::south :
         d == Direction::south ? Direction::north :
         d == Direction::west ? Direction::east :
         d == Direction::east ? Direction::west : Direction::west);
   }

   Coord scootch(Coord start, Direction d)
   {
      if (d == Direction::north)
         return { start.x, start.y + 1 };
      else if (d == Direction::east)
         return { start.x + 1, start.y };
      else if (d == Direction::south)
         return { start.x, start.y - 1 };
      else
         return { start.x - 1, start.y };
   }

   // Depth first search
   void nextS(stack<Move>& s, Direction currDir, int currSteps)
   {
      const static auto dirs = { Direction::north, Direction::east, Direction::south, Direction::west };
      s.push({ opp(currDir), currSteps-1, true }); // Backwards FIRST
      for (auto d : dirs)
      {
         if (d != opp(currDir))
            s.push({ d, currSteps+1, false });
      }
   }

   class Map
   {
   public:

      // For use checking oxygen filling
      Map(Coord pos)
      : startPos(pos)
      {
         currPos = pos;
         m[pos] = Tile{ pos, Type::empty, currSteps };
         s.push({ Direction::north, 1, false });
         s.push({ Direction::east, 1, false });
         s.push({ Direction::south, 1, false });
         s.push({ Direction::west, 1, false });
      }

      Map()
      {
         m[currPos] = Tile{ currPos, Type::empty, currSteps };
         s.push({ Direction::north, 1, false });
         s.push({ Direction::east, 1, false });
         s.push({ Direction::south, 1, false });
         s.push({ Direction::west, 1, false });
      }

      Int botMove()
      {
         currDir = Direction::north;
         if (!s.empty())
         {
            // Pop the next direction off the stack
            auto n = s.top();
            s.pop();
            currDir = n.d;
         }
         return static_cast<Int>(currDir);
      }

      Int humanMove()
      {
         Int i;
         cin >> i;
         currDir = static_cast<Direction>(i);
         return i;
      }

      void humanStatus(Int stat)
      {
         status(stat);
         display();
      }

      void status(Int stat)
      {
         //cout << "Curr pos: " << currPos << ", Curr dir: " << (int)currDir << ", status: " << (int)stat << endl;
         const auto st = static_cast<Type>(stat);
         if (st == Type::wall)
         {
            auto wallPos = scootch(currPos, currDir);
            m[wallPos] = Tile{ wallPos, Type::wall, 0 };
         }
         else
         {
            // Moved, keep going that direction
            currPos = scootch(currPos, currDir);

            if (auto p = m.find(currPos); p != end(m))
            {
               currSteps = p->second.steps;
            }
            else
            {
               ++currSteps;
               m[currPos] = Tile{ currPos, Type::empty, currSteps };
               // Add all other dirs to our stack
               nextS(s, currDir, currSteps);
            }
         }

         if (st == Type::oxygen)
         {
            cout << "Oxygen found!" << endl;
            display();
            found = true;

            cout << "Curr steps: " << m.at(currPos).steps << endl;
         }
      }

      bool isFound() { return found; }
      Coord getPos() { return currPos; }

      void display()
      {
         for (auto row = 25; row > -50; --row)
         {
            for (auto col = -50; col < 50; ++col)
            {
               const auto c = Coord{col, row};
               if (currPos == c)
                  cout << 'D';
               else if (c == startPos)
                  cout << 'X';
               else if (auto p = m.find(c); p != end(m))
               {
                  cout << p->second;
               }
               else
                  cout << ' ';
            }
            cout << '\n';
         }

         cout << endl;
      }

      int getLongestStep()
      {
         return (max_element(begin(m), end(m),
            [](const auto& a, const auto& b)
            {
               return a.second.steps < b.second.steps;
            }))->second.steps;
      }

   private:
      Coord currPos = Coord{0, 0};
      const Coord startPos = Coord{0, 0};
      Direction currDir = Direction::north;
      int currSteps = 0;
      stack<Move> s;
      map<Coord, Tile> m;
      bool found = false;
   };
   
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

   // Part 1: Shortest path to oxygen
   Coord oxy;
   {
      Map m;
      auto input = [&](){ return m.botMove(); }; 
      auto output = [&](Int out){ m.status(out); };
      Program p(program, input, output);

      auto count = 0;
      do
      {
         p.runUntilPause();
         ++count;
      } while (!m.isFound() && count < 10000);
      m.display();
      oxy = m.getPos();
      program = p.getProg(); // Preserve our current state
   }

   // Part 2: Fill with oxygen
   {
      cout << "Oxygen location: " << oxy << endl;
      Map m(oxy); // Now we start at the sensor
      auto input = [&](){ return m.botMove(); }; 
      auto output = [&](Int out){ m.status(out); };
      Program p(program, input, output);

      auto count = 0;
      do
      {
         p.runUntilPause();
         ++count;
      } while (count < 10000); // Go for a while to map everything
      m.display();

      // Find the largest # of steps in our map
      cout << "Longest steps: " << m.getLongestStep() << endl;
   }



   return 0;
}

