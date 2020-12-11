#include <iostream>
#include <cstdio>
#include <fstream>
#include <vector>
#include <algorithm>


struct LineItem
{
  int pos1;
  int pos2;
  char letter;
  std::string password;
};

void print_input(std::vector<LineItem> vec)
{
  for(auto &v : vec)
  {
    printf("pos1: %d, pos2: %d, letter: %c, password: %s\n",v.pos1, v.pos2, v.letter, v.password.c_str());
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
    sscanf(in_str.c_str(), "%d-%d %c: %s",&item.pos1,&item.pos2,&item.letter,buffer);

    // The input uses 1-index positioning. That's dumb so fix that
    --item.pos1;
    --item.pos2;

    // Convert the c string to a std::string and let the stl handle
    // trimming lengths and stuff
    item.password = buffer;
    inputs.push_back(item);
  }

  int valid_passwords = 0;
  // For each line
  for(const auto & item : inputs)
  {
    // valid if letter is in position[pos1] ^ position[pos2]
    int pos1 = 0;
    int pos2 = 0;
    if( item.pos1 >= 0 && item.pos1 < (int)item.password.length() && item.password[item.pos1] == item.letter )
    {
      pos1 = 1;
    }
    if( item.pos2 >= 0 && item.pos2 < (int)item.password.length() && item.password[item.pos2] == item.letter)
    {
      pos2 = 1;
    }
    if( pos1 ^ pos2 )
    {
      ++valid_passwords;
    }
  }
  printf("Solution: %d\n",valid_passwords);
  return 0;
}
