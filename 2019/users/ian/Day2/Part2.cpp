#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <exception>

void print_program(const std::vector<int>& program)
{
  std::cout << "Program: ";
  for(auto& val : program )
  {
    std::cout << std::to_string(val) << ",";
  }
  std::cout << std::endl;
}

void op1(std::vector<int>& program, int arg1, int arg2, int dest)
{
  program[dest] = program[arg1] + program[arg2];
}

void op2(std::vector<int>& program, int arg1, int arg2, int dest)
{
  program[dest] = program[arg1] * program[arg2];
}

bool process_op(std::vector<int>& program, int& pc)
{
  // Bounds check
  if( pc >= (int)program.size())
  {
    std::string err = "Attempted to access out of bound index: " + std::to_string(pc);
    throw std::runtime_error(err);
  }
  // Done check
  if( program[pc] == 99 )
  {
    // done
    return false;
  }

  // bounds check again
  if( pc + 3 >= (int)program.size())
  {
    std::string err = "Insufficient data remaining for operation at index: " + std::to_string(pc);
    print_program(program);
    throw std::runtime_error(err);
  }

  // Process op code
  int arg1 = program[pc + 1];
  if( arg1 >= (int)program.size())
  {
    std::string err = "Invalid argument for arg1: " + std::to_string(arg1) + " at position " + std::to_string(pc + 1);
    throw std::runtime_error(err);
  }
  int arg2 = program[pc + 2];
  if( arg2 >= (int)program.size())
  {
    std::string err = "Invalid argument: for arg2: " + std::to_string(arg2) + " at position " + std::to_string(pc + 2);
    throw std::runtime_error(err);
  }
  int dest = program[pc + 3];
  if( dest >= (int)program.size())
  {
    std::string err = "Invalid argument for dest: " + std::to_string(dest) + " at position " + std::to_string(pc + 3);
    throw std::runtime_error(err);
  }

  if( program[pc] == 1)
  {
    op1(program, arg1, arg2, dest);
  }
  else if( program[pc] == 2)
  {
    op2(program, arg1, arg2, dest);
  }
  else
  {
    std::string err = "Invalid op code: " + std::to_string(program[pc]);
    throw std::runtime_error(err);
  }
  pc += 4;
  return true;
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
  std::vector<int> inputs;
  while (!fin.eof())
  {
    std::string in_str;
    std::getline(fin,in_str,',');
    if( fin.fail() || fin.bad())
    {
     break;
    }
    inputs.push_back(std::atoi(in_str.c_str()));
  }

  // Try all combos of 0-99 to get the desired output, 19690720
  for(int noun = 0; noun <= 99; noun++)
  {
    for(int verb = 0; verb <= 99; verb++)
    {
      int pc = 0;
      std::vector<int> attempt = inputs;
      attempt[1] = noun;
      attempt[2] = verb;
      while(process_op(attempt, pc)){};
      int output = attempt[0];
      if( output == 19690720 )
      {
        std::cout << "Success: noun = " << std::to_string(noun) << " verb = " << std::to_string(verb) << std::endl;
        return 0;
      }
      else
      {
        //std::cout << "Fail: noun = " << std::to_string(noun) << " verb = " << std::to_string(verb) << std::endl;
        //std::cout << "Output: " << std::to_string(output) << std::endl;
      }
    }
  }
  return 0;
}

