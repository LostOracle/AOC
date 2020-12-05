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
#include <cmath>
#include <string>

int binary_partition(std::string encoding, int min, int max, char mindicator, char maxicator)
{
  int lower = min;
  int higher = max;
  for(const auto& c : encoding)
  {
    if(mindicator == c)
    {
      higher = floor( (higher - lower)/2.0 ) + lower;
    }
    if(maxicator == c)
    {
      lower = ceil( (higher - lower)/2.0 ) + lower;
    }
  }
  return lower;
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

  bool seats[128][8] = {false};
  for(const auto& seat_str : inputs)
  {
    //int row = get_row(seat_str);
    int row = binary_partition(seat_str.substr(0,7),0,127,'F','B');
    int col = binary_partition(seat_str.substr(7,3),0,8,'L','R');
    seats[row][col] = true;
  }

  bool first_full = false;
  // Search from front to back, looking for the first empty seat after the first full seat
  for(int r = 0; r < 127; r++)
  {
    for(int c = 0; c < 8; c++)
    {
      if(!first_full)
      {
        if(seats[r][c])
        {
          first_full = true;
        }
      }
      else
      {
        if(!seats[r][c])
        {
          int id = r*8 + c;
          printf("Solution: %d\n",id);
          return 0;
        }
      }
    }
  }
  return 0;
}
