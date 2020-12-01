#include <algorithm>
#include <fstream>
#include <iostream>
#include <numeric>
#include <set>
#include <string>
#include <vector>
#include <sstream>

#include "../intcode/program.h"

using namespace std;
using namespace IC;

namespace
{
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

   // Part 1: Diagnostic
   {
      Program p(program);
      p.run();
   }


   return 0;
}


