#include <iostream>
#include <cstdio>
#include <fstream>
#include <vector>
#include <algorithm>


struct LineItem
{
  int min;
  int max;
  char letter;
  std::string password;
};

void print_input(std::vector<LineItem> vec)
{
  for(auto &v : vec)
  {
    printf("min: %d, max: %d, letter: %c, password: %s\n",v.min, v.max, v.letter, v.password.c_str());
  }
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
  std::vector<LineItem> inputs;
  while (!fin.eof())
  {
    std::string in_str;
    std::getline(fin,in_str);
    if( fin.fail())
    {
      break;
    }
    // For each line, split into %d-%d %c: %s
    LineItem item;
    char buffer[1000];
    // Extract well-formed input using sscanf
    sscanf(in_str.c_str(), "%d-%d %c: %s",&item.min,&item.max,&item.letter,buffer);
    // Convert the c string to a std::string and let the stl handle
    // trimming lengths and stuff
    item.password = buffer;
    inputs.push_back(item);
  }

  print_input(inputs);

  int valid_passwords = 0;
  // For each line
  for(const auto & item : inputs)
  {
    int count = 0;
    // For each character in the password
    for(const auto & c : item.password)
    {
      if(c == item.letter)
      {
        ++count;
      }
    }
    if(count >= item.min && count <= item.max)
    {
      ++valid_passwords;
    }
  }
  printf("Solution: %d\n",valid_passwords);
  return 0;
}
