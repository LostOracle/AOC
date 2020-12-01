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
namespace
{
}

int main()
{
   std::string line;
   std::vector<int> program;

   // The program is one big line
   string fileName;
   cin >> fileName;
   ifstream in(fileName);

   string val;
   while(getline(in, val, ','))
   {
      program.push_back(stoi(val));
   }

   for (auto val : program)
      cout << val << ",";
   cout << endl;

   IC::Program prog(program);
   prog.run();

   return 0;
}


