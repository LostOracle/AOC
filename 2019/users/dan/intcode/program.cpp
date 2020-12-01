#include <algorithm>
#include <iostream>
#include <numeric>
#include <set>
#include <string>
#include <vector>
#include <sstream>

#include "program.h"

using namespace std;

namespace IC
{
   void formatProgram(Instr& prog)
   {
      prog.resize(100000); // Ensures we have enough space for "large addresses"
   }

   class Param
   {
   public:
      Param(Instr& prog_, Int base_, Int p_, Int mode_): _m(mode_), _p(p_), _prog(prog_), _base(base_) {}

      Int& use()
      {
         if (_m == 0) // Position
            return _prog[_p];
         else if (_m == 1)  // Immediate
            return _p;
         else
            return _prog[_base + _p];
      }

   private:
      Int _m;
      Int _p;
      Instr& _prog;
      Int _base;
   };
 
   int Program::exec(Int*& fullOpcode, Int*& a, Int*& b, Int*& c)
   {
      auto state = OK;
      auto opSize = 4;

      const auto opcode = *fullOpcode % 100; 
      auto pA = Param{ _prog, _relBase, *a, ((*fullOpcode / 100) % 10) };
      auto pB = Param{ _prog, _relBase, *b, ((*fullOpcode / 1000) % 10) };
      auto pC = Param{ _prog, _relBase, *c, ((*fullOpcode / 10000) % 10) };

      bool jump = false;

      //cout << "Opcode: " << *fullOpcode << " || A: " << *a << ", B: " << *b << ", C: " << *c << endl;
      if (opcode == 1)
      {
         // Add
         //cout << "Add: " << pC.use() << " = " << pA.use() << " + " << pB.use() << endl;
         pC.use() = pA.use() + pB.use();
      }
      else if (opcode == 2)
      {
         // Multiply
         //cout << "Mult: " << pC.use() << " = " << pA.use() << " * " << pB.use() << endl;
         pC.use() = pA.use() * pB.use();
      }
      else if (opcode == 3)
      {
         // Save
         pA.use() = _input();
         opSize = 2;
      }
      else if (opcode == 4)
      {
         // Output 
         _output(pA.use());
         opSize = 2;
         state = PAUSE;
      }
      else if (opcode == 5)
      {
         // Jump-if-true
         if (pA.use() != 0)
         {
            // If jump, set next opcode to address pointed to by B
            fullOpcode = &_prog[pB.use()];
            jump = true;
         }
         opSize = 3;
      }
      else if (opcode == 6)
      {
         // Jump-if-false
         if (pA.use() == 0)
         {
            // If jump, set next opcode to address pointed to by B
            fullOpcode = &_prog[pB.use()];
            jump = true;
         }
         opSize = 3;
      }
      else if (opcode == 7)
      {
         // Less than
         const bool less = (pA.use() < pB.use());
         pC.use() = (less ? 1 : 0);
      }
      else if (opcode == 8)
      {
         // Equals
         const bool eq = (pA.use() == pB.use());
         pC.use() = (eq ? 1 : 0);
      }
      else if (opcode == 9)
      {
         // Adjust relative base
         _relBase += pA.use();
         opSize = 2;
      }
      else if (opcode == 99)
      {
         // Stop
         return DONE;
      }
      else
      {
         return FAIL;
      }

      if (!jump)
      {
         fullOpcode += opSize;
      }

      a = fullOpcode + 1;
      b = fullOpcode + 2;
      c = fullOpcode + 3;

      return state;
   }

   int Program::runUntilPause()
   {
      // We'll need to parse 4 instructions at a time, operating on each instruction
      Int*& opcode = _currOpcode;
      auto a = opcode+1;
      auto b = opcode+2;
      auto c = opcode+3;

      auto state = OK;
      while (state == OK)
      {
         state = exec(opcode, a, b, c);
      }

      if (state == FAIL)
      {
         std::cout << "Unknown opcode: " << *opcode << std::endl;
      }

      return state;
   }

   int Program::run()
   {
      // We'll need to parse 4 instructions at a time, operating on each instruction
      auto opcode = &_prog[0];
      auto a = &_prog[1];
      auto b = &_prog[2];
      auto c = &_prog[3];

      auto state = OK;
      while (state == OK
            || state == PAUSE)
      {
         state = exec(opcode, a, b, c);
      }

      if (state == DONE)
      {
         //std::cout << "Program halt. Final val: " << _prog[0] << std::endl;
      }
      else if (state == FAIL)
      {
         std::cout << "Unknown opcode: " << *opcode << std::endl;
      }

      return _prog[0];
   }
}


