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

std::map<long long int, long long int> solutions;

void print_path(std::vector<long long int> path)
{
  printf("Path: ");
  for(const auto& p : path)
  {
    printf("%lld, ",p);
  }
  printf("\n");
}

long long int find_paths(const std::vector<long long int>& levels, long long int index, std::vector<long long int> pathSoFar)
{
  pathSoFar.push_back(levels[index]);
  if( solutions.end() != solutions.find(levels[index]))
  {
    print_path(pathSoFar);
    printf("Sub-solution: %lld\n", solutions[levels[index]]);
    return solutions[levels[index]];
  }
  if(0 == levels[index])
  {
    print_path(pathSoFar);
    return 1;
  }

  long long int paths = 0;
  // Not a base case, need to search all possible paths from here
  for(long long int i = index -1; i >= 0 && i >= i-3; i--)
  {
    if(levels[index] - levels[i] <=3)
    {
      paths += find_paths(levels,i,pathSoFar);
    }
  }
  solutions[levels[index]] = paths;
  return paths;
}

std::vector<long long int> convert_input(const std::vector<std::string>& input)
{
  std::vector<long long int> numbers;
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

  std::vector<long long int> levels = convert_input(inputs);

  levels.push_back(0);
  std::sort(levels.begin(),levels.end());
  levels.push_back(levels[levels.size()-1]+3);

  // establish base case for solutions tracking map
  solutions[0] = 1;

  long long int solution = find_paths(levels,levels.size()-1,{});

  printf("Solution: %lld\n",solution);
  return 0;
}
