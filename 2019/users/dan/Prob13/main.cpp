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

   enum class Type
   {
      empty = 0,
      wall = 1,
      block = 2,
      paddle = 3,
      ball = 4
   };

   struct Tile
   {
      Coord coord;
      Type t;
   };

   ostream& operator<<(ostream& os, Coord c)
   {
      return os << "(" << c.x << ", " << c.y << ")";
   }

   ostream& operator<<(ostream& os, Tile tile)
   {
      const static auto m = map<Type, char>{
         {Type::empty,  ' '},
         {Type::wall,   'W'},
         {Type::block,  'B'},
         {Type::paddle, '='},
         {Type::ball,   'O'},
      };
      return os << m.at(tile.t);
   }

   enum class State
   {
      x,
      y,
      value
   };

   class Display
   {
   public:

      void handleValue(int i)
      {
         if (x == -1 && y == 0)
            score = i;
         else
         {
            t = static_cast<Type>(i);
            m[Coord{x, y}] = Tile{ Coord{x, y}, t };

            if (t == Type::ball)
            {
               ballVy = (ball.y > y ? -1 : 1); // prev > curr ? neg : pos
               ballVx = (ball.x > x ? -1 : 1); // prev > curr ? neg : pos
               ball.x = x;
               ball.y = y;
            }
            if (t == Type::paddle)
            {
               paddle.x = x;
               paddle.y = y;
            }
         }
      }

      void draw(int i)
      {
         switch(s)
         {
         case State::x:
            s = State::y;
            x = i;
            break;
         case State::y:
            s = State::value;
            y = i;
            break;
         case State::value:
            s = State::x;
            handleValue(i);
            break;
         }
      }

      int joystickPos()
      {
         if (ballVy == 1) // Positive means down!!!
         {
            // Aim the paddle at collision point of the ball on the paddle's row.
            const auto collX = ball.x + (ballVx * abs(ball.y - paddle.y)) -1;
            return (paddle.x < collX ? 1 :
               (paddle.x > collX ? -1 : 0));
         }
         else
         {
            // Otherwise, stay under the ball
            return ballVx;
         }
      }

      int blockCount()
      {
         auto count = 0;
         for (auto [k, v] : m)
         {
            if (v.t == Type::block)
               ++count;
         }

         return count;
      }

      int getScore() { return score; }

      void display()
      {
         auto currRow = 0;
         for (auto [k, v] : m)
         {
            if (currRow != k.y)
            {
               ++currRow;
               cout << '\n';
            }
            cout << v;
         }

         cout << endl;

         cout << "Ball pos: " << Coord{ ball.x, ball.y } << endl;
         cout << "Ball vel: " << ballVx << ", " << ballVy << endl;
      }

   private:
      int x = 0;
      int y = 0;
      Type t = Type::empty;
      State s = State::x;
      map<Coord, Tile> m;
      int score = 0;

      Coord ball = Coord{0, 0};
      int ballVx = 1;
      int ballVy = -1;
      Coord paddle = Coord{0, 0};
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

   // Part 1: Count block tiles remaining
   {
      Display d;
      auto input = [&](){ return 1; }; // Don't care about input
      auto output = [&](Int out){ d.draw(out); };
      Program p(program, input, output);
      p.run();

      auto tiles = d.blockCount();
      cout << "# of block tiles: " << tiles << endl;
   }

   // Part 2: For reals
   {
      auto prog = program;
      prog[0] = 2; // Insert "quarters"
      Display d;
      auto humanInput = [&]()
      {
         d.display();
         char c = 's';
         cin >> c;
         const static auto m = map<char, int>{ {'a', -1}, {'s', 0}, {'d', 1} };
         return m.at(c);
      }; 

      auto compInput = [&]()
      {
         using namespace std::this_thread;
         using namespace std::chrono;
         //d.display();
         //sleep_for(milliseconds(100));
         return d.joystickPos();
      };

      auto output = [&](Int out){ d.draw(out); };
      Program p(prog, compInput, output);
      p.run();

      cout << endl;
      cout << endl;
      auto tiles = d.blockCount();
      cout << "# of block tiles: " << tiles << endl;
      cout << "Score: " << d.getScore() << endl;

      d.display();
   }



   return 0;
}


