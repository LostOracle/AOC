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

std::vector<int> convert_input(const std::vector<std::string>& input)
{
  std::vector<int> numbers;
  for(const auto& line: input)
  {
    numbers.push_back(std::atoi(line.c_str()));
  }
  return numbers;
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

  std::vector<int> levels = convert_input(inputs);

  levels.push_back(0);
  std::sort(levels.begin(),levels.end());
  levels.push_back(levels[levels.size()-1]+3);

  int diff1 = 0;
  int diff3 = 0;

  for(unsigned int i = 1; i < levels.size(); i++)
  {
    int diff = levels[i] - levels[i-1];
    if(diff == 1)
      diff1++;
    if(diff == 3)
      diff3++;
  }
  int solution = diff1*diff3;

  printf("Solution: %d\n",solution);
  return 0;
}
