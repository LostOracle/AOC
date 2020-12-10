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

std::vector<unsigned long long int> convert_input(std::vector<std::string> inputs)
{
  std::vector<unsigned long long> numbers;
  for(const auto& str : inputs)
  {
    numbers.push_back(std::atoi(str.c_str()));
  }
  return numbers;
}

bool num_valid(unsigned long long int num, std::vector<unsigned long long int> preceeding)
{
  // For each possible pair of numbers
  for(const auto& n1 : preceeding)
  {
    for(const auto& n2 : preceeding)
    {
      if( (n1 != n2) && (n1 + n2 == num))
      {
        return true;
      }
    }
  }
  return false;
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

  std::vector<unsigned long long> numbers = convert_input(inputs);
  const int PREAMBLE = 25;
  unsigned long long int solution = 0;

  // Skip the preamble
  for(unsigned int i = PREAMBLE; i < numbers.size(); i++)
  {
    std::vector<unsigned long long int> subVec(numbers.begin() - PREAMBLE + i, numbers.begin() + i);
    if(!num_valid(numbers[i], subVec))
    {
      solution = numbers[i];
      break;
    }
  }

  printf("Solution: %llu\n",solution);
  return 0;
}
