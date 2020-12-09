#include <iostream>
#include <cstdio>
#include <fstream>
#include <vector>
#include <algorithm>
#include <map>
#include <tuple>
#include <cstring>
#include <functional>
#include <cctype>
#include <regex>
#include <cassert>
#include <memory>
#include <typeinfo>

#include "State.h"
#include "AuditLog.h"
#include "Operator.h"

class Instruction;
class Machine;


class Instruction
{
  public:
  std::unique_ptr<Operator> op;
  int argument;

  Instruction( std::unique_ptr<Operator> op, int argument ) : op(std::move(op)), argument(argument) {}

  ~Instruction() = default;
  Instruction( Instruction const& other) : op(other.op->clone()),argument(other.argument) {}
  Instruction& operator=(Instruction const& other) {op = other.op->clone(); return *this;}
};

using Program = std::vector<Instruction>;

class Machine
{
  public:
  State state;
  Program program;
  AuditLog auditLog;

  Machine(Program prog) : state(), program(prog){};

  virtual bool TerminationCondition() = 0;

  virtual void BeforeOp()
  {
    auditLog.audit(state);
  }

  virtual void AfterOp()
  {
  }

  int execute()
  {
    while(true)
    {
      assert( state.pc < program.size());

      const Instruction& instruction = program[state.pc];

      if(this->TerminationCondition())break;
      this->BeforeOp();
      if(this->TerminationCondition())break;
      state = instruction.op->compute(state,instruction.argument);
      if(this->TerminationCondition())break;
      this->AfterOp();
    }
    return state.return_code;
  }
};

class Day8Machine : public Machine
{
  public:
  Day8Machine(Program prog) : Machine(prog){}

  // For Day 8 Part1, we want to find the first pc address visted twice
  std::map<int,int> visited;

  bool TerminationCondition() override
  {
    if(visited[state.pc] > 1)
    {
      state.return_code = 1;
      return true;
    }
    else if(state.pc >= program.size())
    {
      state.return_code = 0;
      return true;
    }
    return false;
  }
  void BeforeOp() override
  {
    Machine::BeforeOp();
    // We're about to execute a given pc instruction, update our map
    visited[state.pc]++;
  }

  void AfterOp() override
  {
    Machine::AfterOp();
  }
};

std::vector<Instruction> parse_program(const std::vector<std::string>& input)
{
  Program program;
  for(const auto& line : input)
  {
    char op_buffer[50];
    char arg_buffer[50];
    assert(2 == sscanf(line.c_str(),"%s %s", op_buffer, arg_buffer));

    int argument = atoi(arg_buffer);

    if( strcmp(op_buffer,"nop") == 0 )
    {
      program.push_back( Instruction( std::make_unique<NOP>(), argument));
    }
    else if( strcmp(op_buffer,"acc") == 0 )
    {
      program.push_back( Instruction( std::make_unique<ACC>(), argument));
    }
    else if( strcmp(op_buffer,"jmp") == 0 )
    {
      program.push_back( Instruction( std::make_unique<JMP>(), argument));
    }
    else
    {
      assert(false);
    }
  }
  return program;
}

int main(int argc, char ** argv)
{
  if (argc != 2)
  {
    std::cout << "Usage: DayX <input>" << std::endl;
    return 0;
  }
  std::ifstream fin;
  fin.open(argv[1], std::ifstream::in);

  // Get Input as... vector, I guess? Probably doesn't matter
  std::vector<std::string> inputs;
  while (!fin.eof())
  {
    std::string in_str;
    std::getline(fin,in_str);
    if( fin.fail())
    {
      break;
    }
    inputs.push_back(in_str);
  }

  Program prog = parse_program(inputs);
  int solution;
  for(unsigned int i = 0; i < prog.size(); i++)
  {
    Program progCopy(prog);
    // If the instruction is a NOP or JMP, swap it and test for loop
    if(typeid(NOP) == typeid(*prog[i].op)/*dynamic_cast<NOP*>(&*prog[i].op)*/)
    {
      progCopy[i].op = std::make_unique<JMP>();
    }
    else if(typeid(JMP) == typeid(*prog[i].op))
    {
      progCopy[i].op = std::make_unique<NOP>();
    }
    // Check if the program still runs forever

    Day8Machine m(progCopy);
    int return_code = m.execute();
    if(return_code == 0)
    {
      solution = m.state.acc;
    }
  }
  printf("[%s]\n",typeid(NOP).name());
  printf("[%s]\n",typeid(JMP).name());
  printf("[%s]\n",typeid(ACC).name());
  printf("Day 2 Solution: %d\n",solution);

  //Day8Machine machine(prog);
  //machine.execute();
  //machine.auditLog.printLog();

  return 0;
}
