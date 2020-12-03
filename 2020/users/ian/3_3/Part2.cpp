#include <iostream>
#include <cstdio>
#include <fstream>
#include <vector>
#include <algorithm>

const char Tree = '#';
const char Empty = '.';

struct Slope
{
  unsigned int slope_row;
  unsigned int slope_col;
  Slope(unsigned int slope_row, unsigned int slope_col):slope_row(slope_row),slope_col(slope_col){}
};

int count_trees(std::vector<std::string>& inputs, unsigned int slope_row, unsigned int slope_col)
{
  unsigned int row = 0;
  unsigned int col = 0;
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
  return trees;
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

  std::vector<Slope> slopes;
  slopes.push_back(Slope(1,1));
  slopes.push_back(Slope(1,3));
  slopes.push_back(Slope(1,5));
  slopes.push_back(Slope(1,7));
  slopes.push_back(Slope(2,1));
  unsigned long long solution = 1;

  for( auto & slope : slopes )
  {
    int trees = count_trees(inputs, slope.slope_row, slope.slope_col);
    solution *= trees;
    printf("trees: %d\n", trees);
    printf("Accumulated: %llu\n",solution);
  }

  printf("Solution: %llu\n",solution);
  return 0;
}
