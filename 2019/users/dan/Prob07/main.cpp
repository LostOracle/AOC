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
   struct DataHolder
   {
      DataHolder(int phase, int input): _input(input), _phase(phase) {}

      // Input function alternates between two modes:
      //   1. "Phase setting" - list of unique integers to set amps
      //   2. "Input value" - previous output value
      int input()
      {
         if (_isPhase)
         {
            _isPhase = false;
            return _phase;
         }
         else
         {
            return _input;
         }
      }

      // Output function retains the output for next provided input
      void output(int out)
      {
         _output = out;
      }

      int _input = 0;
      bool _isPhase = true;
      int _phase = 0;
      int _output = 0;
   };

   struct AmpLoop
   {
      AmpLoop(vector<int> phases, vector<int> program)
      {
         for (auto phase : phases)
         {
            //cout << phase << ",";
            _dhs.emplace_back(DataHolder{phase, 0});
         }
         //cout << endl;

         for (unsigned i = 0U; i < _dhs.size(); ++i)
         {
            auto next = (i+1 < _dhs.size() ? i+1 : 0);
            auto inFunc = [=, this](){ return _dhs[i].input(); };
            auto outFunc = [=, this](int out){ _dhs[next]._input = out; };
            _amps.emplace_back(IC::Program{program, inFunc, outFunc});
         }
      }

      // Returns final output
      int run()
      {
         auto currOutput = 0;

         auto state = IC::OK;
         while (state != IC::DONE
            && state != IC::FAIL)
         {
            for(auto& amp : _amps)
            {
               state = amp.runUntilPause();
            }
            currOutput = _dhs[0]._input;
            //cout << "Curr output: " << currOutput << endl;
         }

         return currOutput;
      }

      vector<IC::Program> _amps;
      vector<DataHolder> _dhs;
   };
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

   // Part 1 - Linear
   {
   auto phaseVals = std::vector<int>{ 4, 3, 2, 1, 0 };
   std::sort(begin(phaseVals), end(phaseVals));

   auto maxVal = 0;
   auto maxPhases = phaseVals;
   do
   {
      auto tempProg = program;
      int input = 0;
      for (const auto phase : phaseVals)
      {
         DataHolder dh(phase, input);
         auto inFunc = [&](){ return dh.input(); };
         auto outFunc = [&](int out){ dh.output(out); };

         IC::Program prog(tempProg, inFunc, outFunc);
         prog.run();
         input = dh._output;
      }
      if (input > maxVal)
      {
         maxVal = input;
         maxPhases = phaseVals;
      }
   } while( next_permutation(begin(phaseVals), end(phaseVals)) );

   cout << "Final output: " << maxVal << endl;
   cout << "Max phases: ";
   for (const auto p : maxPhases)
      cout << p << ",";
   cout << endl;
   }

   // Part 2: Feedback
   {
   auto fbPhases = std::vector<int>{ 9, 8, 7, 6, 5 };
   std::sort(begin(fbPhases), end(fbPhases));

   auto maxVal = 0;
   auto maxPhases = fbPhases;
   do
   {
      auto loop = AmpLoop(fbPhases, program);
      auto output = loop.run();
      if (output > maxVal)
      {
         maxVal = output;
         maxPhases = fbPhases;
      }
   } while( next_permutation(begin(fbPhases), end(fbPhases)) );

   cout << "Final output: " << maxVal << endl;
   cout << "Max phases: ";
   for (const auto p : maxPhases)
      cout << p << ",";
   cout << endl;
   }


   return 0;
}


