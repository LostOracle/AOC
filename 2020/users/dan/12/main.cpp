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
   enum class Dir
   {
      N,
      E,
      S,
      W,
      L,
      R,
      F
   };

   struct Action
   {
      Dir d;
      int val;
   };

   struct Ship
   {
      Dir facing = Dir::E;
      int x = 0;
      int y = 0;
      // Waypoint coords
      int wX = 10;
      int wY = 1;
   };

   using Nav = vector<Action>;

   Dir fromString(char c)
   {
      return c == 'N' ? Dir::N
         : (c == 'S' ? Dir::S
         : (c == 'E' ? Dir::E
         : (c == 'W' ? Dir::W
         : (c == 'L' ? Dir::L
         : (c == 'R' ? Dir::R
         : Dir::F)))));
   }

   char toString(Dir d)
   {
      return d == Dir::N ? 'N' 
         : (d == Dir::S ? 'S'
         : (d == Dir::E ? 'E'
         : (d == Dir::W ? 'W'
         : (d == Dir::L ? 'L'
         : (d == Dir::R ? 'R'
         : 'F')))));
   }

   ostream& operator<<(ostream& os, const Action& a)
   {
      os << toString(a.d) << a.val;
      return os;
   }

   ostream& operator<<(ostream& os, Ship s)
   {
      os << toString(s.facing) << ": " << s.x << ", " << s.y;
      return os;
   }

   void move(Ship& s, Action a)
   {
      auto dir = a.d;
      if (dir == Dir::F) dir = s.facing;

      switch(dir)
      {
      case Dir::N:
         s.y += a.val;
         break;
      case Dir::S:
         s.y -= a.val;
         break;
      case Dir::E:
         s.x += a.val;
         break;
      case Dir::W:
         s.x -= a.val;
         break;
      case Dir::L:
      {
         auto newF = (int)s.facing - (a.val / 90);
         if (newF < 0) newF += 4;
         s.facing = static_cast<Dir>(newF);
         break;
      }
      case Dir::R:
      {
         auto newF = (int)s.facing + (a.val / 90);
         if (newF > 3) newF -= 4;
         s.facing = static_cast<Dir>(newF);
         break;
      }
      }
   }

   void rotWaypoint(Ship& s, int deg, bool clockwise)
   {
      // Count # of rotations
      const auto numRot = (deg / 90) % 4;
      switch(numRot)
      {
      case 0: // No change
         break;
      case 1:
      {
         const auto tmp = s.wX;
         s.wX = (clockwise ? 1 : -1) * s.wY;
         s.wY = (clockwise ? -1 : 1) * tmp;
         break;
      }
      case 2:
         s.wX = -s.wX;
         s.wY = -s.wY;
         break;
      case 3:
      {
         const auto tmp = s.wX;
         s.wX = (clockwise ? -1 : 1) * s.wY;
         s.wY = (clockwise ? 1 : -1) * tmp;
         break;
      }
      }
   }

   void moveWaypoint(Ship& s, Action a)
   {
      switch(a.d)
      {
      case Dir::N:
         s.wY += a.val;
         break;
      case Dir::S:
         s.wY -= a.val;
         break;
      case Dir::E:
         s.wX += a.val;
         break;
      case Dir::W:
         s.wX -= a.val;
         break;
      case Dir::L:
         rotWaypoint(s, a.val, false);
         break;
      case Dir::R:
         rotWaypoint(s, a.val, true);
         break;
      case Dir::F:
         s.x += s.wX * a.val;
         s.y += s.wY * a.val;
         break;
      }
   }
}

int main()
{
   string line;
   Nav input;

   while (getline(cin, line))
   {
      Action a;
      char c;
      sscanf(line.data(), "%c%d", &c, &a.val);
      a.d = fromString(c);
      input.push_back(a);
   }

   // Input check
   //for (auto item : input)
   //{
   //   cout << item << endl;
   //}

   // Part 1
   Ship s1;
   for (auto a : input)
   {
      move(s1, a);
   }
   cout << s1 << endl;
   const auto manDist = abs(s1.x) + abs(s1.y);
   cout << "Part 1: " << manDist << endl;

   // Part 2
   Ship s2;
   for (auto a : input)
   {
      moveWaypoint(s2, a);
   }
   cout << s2 << endl;
   const auto manDist2 = abs(s2.x) + abs(s2.y);
   cout << "Part 2: " << manDist2 << endl;

   return 0;
}


