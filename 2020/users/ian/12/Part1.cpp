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
#include <cstdlib>

enum class Direction { N, E, S, W };
enum class Rotation { L, R };

struct Ship
{
  Ship() : north(0),east(0),bearing(90){}
  int north;
  int east;
  int bearing;

  void processMove(Direction h, int val)
  {
    switch(h)
    {
      case Direction::N:
      {
        north += val;
        break;
      }
      case Direction::S:
      {
        north -= val;
        break;
      }
      case Direction::E:
      {
        east += val;
        break;
      }
      case Direction::W:
      {
        east -= val;
        break;
      }
      default:
      {
        printf("FAILURE 5\n");
        throw(1);
        break;
      }
    }
  }

  void print()
  {
    printf("North/South: %d\n",north);
    printf("East/West:   %d\n",east);
    printf("Bearing:     %d\n",bearing);
  }

  void processRotate(Rotation r, int val)
  {
    if( Rotation::L == r)
    {
      bearing = bearing - val;
    }
    else if( Rotation::R == r)
    {
      bearing = bearing + val;
    }
    else
    {
      printf("The gods have forsaken us\n");
        throw(1);
    }
    if(bearing < 0)
      bearing += 360;
    if(bearing >= 360)
      bearing -= 360;
  }


  void moveForward(int val)
  {
    switch(bearing)
    {
      case 0:
        north += val;
        break;
      case 90:
        east += val;
        break;
      case 180:
        north -= val;
        break;
      case 270:
        east -= val;
        break;
      default:
        printf("Oh the huge manatee\n");
        throw(1);
        break;
    }
  }

  void processCommand(std::string line)
  {
    int val;
    sscanf(line.c_str()+1,"%d",&val);
    switch(line[0])
    {
      case 'N':
        processMove(Direction::N, val);
        break;
      case 'S':
        processMove(Direction::S, val);
        break;
      case 'E':
        processMove(Direction::E, val);
        break;
      case 'W':
        processMove(Direction::W, val);
        break;
      case 'R':
        processRotate(Rotation::R, val);
        break;
      case 'L':
        processRotate(Rotation::L, val);
        break;
      case 'F':
        moveForward(val);
        break;
      default:
        printf("THE OTHER\n");
        break;
    }
  }
};

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
  Ship s;
  for(const auto& line : inputs)
  {
    s.processCommand(line);
    s.print();
  }

  int solution = std::abs(s.north) + std::abs(s.east);

  printf("Solution: %d\n",solution);
  return 0;
}
