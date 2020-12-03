#include <iostream>
#include <cstdio>
#include <fstream>
#include <vector>
#include <algorithm>

const char Tree = '#';
const char Empty = '.';

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

  unsigned int row = 0;
  unsigned int col = 0;
  unsigned int slope_col = 3;
  unsigned int slope_row = 1;
  unsigned int trees = 0;
  while(row <= inputs.size())
  {
    if( inputs[row][col] == Tree )
    {
      ++trees;
    }
    col += slope_col;
    row += slope_row;
    if( row >= inputs.size())
    {
      break;
    }
    if( col >= inputs[row].length() )
    {
      col -= inputs[row].length();
    }
  }

  printf("Solution: %d\n",trees);
  return 0;
}
