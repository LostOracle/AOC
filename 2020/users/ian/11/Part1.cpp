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

struct Coord
{
  Coord(int row, int col) : row(row), col(col){}
  int row;
  int col;
};

void print_puzzle(std::vector<std::string> puzzle)
{
  printf("**********\n");
  for(const auto& line : puzzle)
  {
    printf("%s\n",line.c_str());
  }
  printf("**********\n\n");
}

void print_adjacent(std::vector<Coord> adjacent)
{
  printf("**********\n");
  for(const auto& item : adjacent)
  {
    printf("Row: %d, Col %d\n",item.row, item.col);
  }
  printf("**********\n\n");
}

std::vector<Coord> generate_neighbors(int row, int row_max, int col, int col_max)
{
  std::vector<Coord> adjacent;
  for(int r = row -1; r <= row +1; r++)
  {
    for(int c = col -1; c <= col +1; c++)
    {
      if(r >= 0 && c >= 0 && r < row_max && c < col_max && (r != row || c != col))
      {
        adjacent.push_back(Coord(r,c));
      }
    }
  }
  print_adjacent(adjacent);
  return adjacent;
}

int count_occupied(const std::vector<std::string>& puzzle, std::vector<Coord> neighbors)
{
  int sum = 0;
  for(const auto& neighbor : neighbors)
  {
    if(puzzle[neighbor.row][neighbor.col] == '#')
    {
      sum++;
    }
  }
  return sum;
}

bool advance(std::vector<std::string>& puzzle)
{
  bool changed = false;
  std::vector<std::string> old_puzzle(puzzle);
  for(int row = 0; row < (int)puzzle.size(); row++)
  {
    for( int col = 0; col < (int)puzzle[row].size(); col++)
    {
      std::vector<Coord> adjacent = generate_neighbors(row,puzzle.size(), col, puzzle[row].size());
      int occupied = count_occupied(old_puzzle, adjacent);
      if(old_puzzle[row][col] == 'L' && occupied == 0)
      {
        puzzle[row][col] = '#';
        changed = true;
      }
      else if(old_puzzle[row][col] == '#' && occupied >= 4)
      {
        puzzle[row][col] = 'L';
        changed = true;
      }
    }
  }
  return changed;
}

int count_seats(std::vector<std::string> puzzle)
{
  int seats = 0;
  for(const auto& line : puzzle)
  {
    for(const auto& c : line)
    {
      if(c == '#')
      {
        seats++;
      }
    }
  }
  return seats;
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

  print_puzzle(inputs);
  while(advance(inputs))
  {
    print_puzzle(inputs);
  }
  print_puzzle(inputs);

  int solution = count_seats(inputs);

  printf("Solution: %d\n",solution);
  return 0;
}
