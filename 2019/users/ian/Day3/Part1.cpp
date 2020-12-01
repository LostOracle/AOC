#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <exception>
#include <map>
#include <cstring>

enum Direction { up, down, left, right };

class Coordinate
{
  public:
    Coordinate(int up, int right):
      up(up),right(right)
  {
  }
    bool operator<(const Coordinate& other) const
    {
      if(up == other.up && right == other.right)
      {
        return false;
      }
      else
      {
        return up < other.up || right < other.right;
      }
    }
    void step(Direction d)
    {
      if(Direction::up == d)
      {
        up += 1;
      }
      else if(Direction::down == d)
      {
        up -= 1;
      }
      else if(Direction::right == d)
      {
        right += 1;
      }
      else if(Direction::left == d)
      {
        right -= 1;
      }
      else
      {
        std::string err = "Invalid direction: " + std::to_string((int)d);
        throw std::runtime_error(err);
      }
    }
    std::string to_string() const
    {
      return std::string("<") + std::to_string(up) + "," + std::to_string(right) + ">";
    }

    int up;
    int right;
};

struct Wires
{
  bool red;
  bool blue;
  Wires(bool red,bool blue):
    red(red),blue(blue){};

};

class GridPoint
{
  public:
    GridPoint() :
      wires(Wires(false,false))
  {}
    void layRed()
    {
      wires.red = true;
    }
    void layBlue()
    {
      wires.blue = true;
    }
    bool red() const
    {
      return wires.red;
    }
    bool blue() const
    {
      return wires.blue;
    }
    std::string to_string() const
    {
      return std::string("red: ") + (wires.red ? "true" : "false") + " blue: " + (wires.blue ? "true" : "false");
    }
    Wires wires;
};

Direction get_direction(std::string in_str)
{
  // validate length non-zero
  if(in_str.length() <= 0)
  {
    throw std::runtime_error("Parse failure.");
  }
  if(in_str[0] == 'U')
  {
    return Direction::up;
  }
  else if (in_str[0] == 'D')
  {
    return Direction::down;
  }
  else if (in_str[0] == 'L')
  {
    return Direction::left;
  }
  else if (in_str[0] == 'R')
  {
    return Direction::right;
  }
  else
  {
    throw std::runtime_error("Direction parse failure");
  }
}

int get_distance(std::string in_str)
{
  return atoi(in_str.c_str()+1);
}

void path_wire(std::string wire, std::map<Coordinate,GridPoint> &grid, bool red, bool blue)
{
  Coordinate c(0,0);

  // Get a stringstream equivalent of the line so we can use getline
  std::stringstream ss(wire);
  std::string token;

  // For each , separated value...
  while(std::getline(ss,token,','))
  {
    // Parse out the direction...
    Direction d = get_direction(token);
    // ... and distance
    int dist = get_distance(token);

    // And lay wire
    for(int i = 0; i < dist; i++)
    {
      c.step(d);
      if(red && blue)
      {
        throw std::runtime_error("red and blue can't both be true");
      }
      else if(!red && !blue)
      {
        throw std::runtime_error("red and blue can't both be false");
      }
      else if(red && !blue)
      {
        grid[c].layRed();
      }
      else if(blue && !red)
      {
        grid[c].layBlue();
      }
      else
      {
        throw std::runtime_error("Unreachable code reached.");
      }
    }
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

  // Map to hold all the coordinates at which wires are laid
  std::map<Coordinate, GridPoint> grid;

  std::string red;
  std::getline(fin,red);
  path_wire(red, grid, true, false);

  std::string blue;
  std::getline(fin,blue);
  path_wire(blue, grid, false, true);

  std::vector<Coordinate> matches;
  // Find all points that cross
  for( const auto& [coord,grid_point] : grid )
  {
    if ( grid_point.red() &&
         grid_point.blue() &&
         ( ! (coord.up == 0 && coord.right == 0) )
         )
    {
      matches.push_back(coord);
    }
  }
  if(matches.size() == 0)
    std::runtime_error("No matches found");

  Coordinate best = matches[0];
  int best_dist = abs(matches[0].up) + abs(matches[0].right);
  // Find manhatten distance
  for ( const auto& coord : matches )
  {
    if(abs(coord.up) + abs(coord.right) < best_dist)
    {
      best = coord;
      best_dist = abs(coord.up) + abs(coord.right);
    }
  }
  std::cout << "best: " << std::to_string(best_dist) << std::endl;
  return 0;
}

