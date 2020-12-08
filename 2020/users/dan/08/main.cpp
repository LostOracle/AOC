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
   enum class Opcode
   {
      acc,
      jmp,
      nop,
   };

   Opcode fromString(string s)
   {
      return s == "acc" ? Opcode::acc
         : (s == "jmp" ? Opcode::jmp
         : Opcode::nop);
   }

   ostream& operator<<(ostream& os, const Opcode& op)
   {
      switch(op)
      {
         case Opcode::acc:
            os << "acc";
            break;
         case Opcode::jmp:
            os << "jmp";
            break;
         case Opcode::nop:
            os << "nop";
            break;
      }

      return os;
   }

   struct Instruction
   {
      Opcode op;
      int arg;
   };

   struct Registers
   {
      int pc = 0; // Program counter
      int acc = 0; // Accumulate register
   };

   ostream& operator<<(ostream& os, const Instruction& inst)
   {
      os << inst.op << " " << inst.arg;
      return os;
   }

   // Executes the program.
   // Throws if an out-of-bounds instruction is attempted, with ONE exception: 1-past-the-end
   pair<bool, int> execute(Registers reg, vector<Instruction> prog)
   {
      map<int, bool> executed;

      while (executed.find(reg.pc) == end(executed)
            && reg.pc != static_cast<int>(prog.size()))
      {
         // Add the current instruction to our map as executed
         executed.insert({reg.pc, true});

         // Throw on invalid instruction offset
         const auto inst = prog.at(reg.pc);
         cout << inst << endl;
         switch(inst.op)
         {
            case Opcode::acc: // Accumulate (adds arg to accumulate reg)
               reg.acc += inst.arg;
               ++reg.pc;
               break;
            case Opcode::jmp: // Jump - moves PC based on arg
               reg.pc += inst.arg;
               break;
            case Opcode::nop: // BORING
               ++reg.pc;
               break;
         }
      }

      return {reg.pc == static_cast<int>(prog.size()), reg.acc};
   }
}

int main()
{
   string line;
   vector<Instruction> input;

   while (getline(cin, line))
   {
      Instruction inst;
      stringstream ss(line);
      string s;
      ss >> s; // opcode
      inst.op = fromString(s);
      ss >> inst.arg; // opcode arg

      input.push_back(inst);
   }

   // Input check
   //for (auto item : input)
   //{
   //   cout << item << endl;
   //}

   // Part 1
   // Find value of accumulator prior to first infinite loop
   Registers reg;
   auto result = execute(reg, input).second;
   cout << "Part 1: " << result << endl;

   // Part 2
   // Figure out which instruction to change to fix the program (nop or jmp)
   auto result2 = pair<bool, int>{ false, 0 };
   for (auto i = 0U; i < input.size(); ++i)
   {
      // Copy input so we can modify
      auto input2 = input;
      auto& currInst = input2[i];
      if (currInst.op != Opcode::nop)
      {
         if (currInst.op == Opcode::jmp) currInst.op = Opcode::nop;
         else if (currInst.op == Opcode::nop) currInst.op = Opcode::jmp;

         try
         {
            Registers reg;
            result2 = execute(reg, input2);
         } catch(...){}
         if (result2.first) break; // Bail if we found the correct program
      }
   }
   cout << "Part 2: " << result2.second << endl;

   return 0;
}


