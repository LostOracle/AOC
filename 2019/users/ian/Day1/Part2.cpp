#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib>

int get_fuel_requirement_part1(int mass)
{
  // mass / 3, round down, subtract 2
  return ((int)(mass / 3))-2;
}

int get_fuel_requirement_part2(int mass)
{
  int total = 0;
  int remaining_mass = mass;

  // Until we hit a step where we get 0, repeat
  while( remaining_mass > 0)
  {
    int fuel_needed = ((int)(remaining_mass / 3))-2;
    if( fuel_needed >= 0 )
    {
      total += fuel_needed;
    }
    remaining_mass = fuel_needed;
  }
  return total;
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
    std::getline(fin,in_str);
    if( fin.fail())
    {
     break;
    }
    inputs.push_back(std::atoi(in_str.c_str()));
  }
  int total = 0;
  for (auto val : inputs)
  {
    total += get_fuel_requirement_part2(val);
  }
  std::cout << "Total: " << total << std::endl;
  return 0;
}

