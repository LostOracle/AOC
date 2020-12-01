#pragma once

#include <functional>
#include <iostream>
#include <vector>

namespace IC
{
   using InputFunc = std::function< int64_t(void) >; // Provides the input
   using OutputFunc = std::function< void(int64_t) >; // Stores the output

   using Int = int64_t;
   using Instr = std::vector<Int>;

   const int OK = 0;
   const int DONE = 1;
   const int PAUSE = 2;
   const int FAIL = -1;

   void formatProgram(Instr& prog);

   class Program
   {
   public:

      Program(Instr prog_)
         : _prog(prog_)
         , _currOpcode(&_prog[0])
      {
         _input = []()
         {
            std::cout << "Enter input: \n" << std::flush;
            Int input = 0;
            std::cin >> input;
            std::cout << "Input found: " << input << std::endl;
            return input;
         };
         _output = [](Int out)
         {
            std::cout << "Output: " << out << std::endl;
         };
      }
      Program(Instr prog_, InputFunc in_, OutputFunc out_)
         : _prog(prog_), _input(in_), _output(out_), _currOpcode(&_prog[0]) {}

      Instr getProg() { return _prog; }

      int run();

      int runUntilPause();
   private:
      int exec(Int*& opcode, Int*& a, Int*& b, Int*& c);

      Instr _prog;

      InputFunc _input;
      OutputFunc _output;
      Int* _currOpcode;
      Int _relBase = 0;
   };
}
