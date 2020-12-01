#include <algorithm>
#include <iostream>
#include <numeric>
#include <set>
#include <string>
#include <vector>
#include <sstream>

#include "../intcode/program.h"

using namespace std;
namespace
{
   const int OK = 0;
   const int DONE = 1;
   const int FAIL = -1;

   bool runProgram(std::vector<int> program, int target)
   {
      IC::Program prog(program);
      return prog.run() == target;
   }
}

int main()
{
   std::string line;
   std::vector<int> program;

   // The program is one big line    
   getline(cin, line);

   stringstream in(line);

   string val;
   while(getline(in, val, ','))
   {
      program.push_back(stoi(val));
   }

   for (auto& i : program)
      std::cout << i << " ";
   std::cout << std::endl;

   // Run program for each possible input until we find the combo
   const auto target = 19690720;

   // Given input range, bounds should be < 110
   const auto rangeMax = 110;

   // For each noun & verb combo, search for the correct program
   for (auto noun = 0; noun < rangeMax; ++noun)
   {
      for (auto verb = 0; verb < rangeMax; ++verb)
      {
         // Before running, replace noun & verb values
         auto tempProg = program;
         tempProg[1] = noun;
         tempProg[2] = verb;
         if(runProgram(tempProg, target))
         {
            std::cout << "Found: " << (noun * 100) + verb << std::endl;
            return 0;
         }
      }
   }

   return 0;
}


