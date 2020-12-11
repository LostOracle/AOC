#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>


void print_vec(std::vector<int> vec)
{
  for(auto &v : vec)
  {
    printf("%d,",v);
  }
  printf("\n");
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
  // Find the two values that multiply together to get 2020
  // Sort the input first
  std::sort(inputs.begin(), inputs.end());

  // For each number in the list
  for(const auto & item : inputs)
  {
    // The companion number must be 2020-item
    int search_val = 2020 - item;
    // Check for this item in the sorted list
    if(std::binary_search(inputs.begin(), inputs.end(), search_val))
    {
      // We found a match, the solution is item * search_val
      printf("Solution: %d\n",item*search_val);
      return 0;
    }
  }
  // failed
  printf("No solution found\n");
  return 0;
}
