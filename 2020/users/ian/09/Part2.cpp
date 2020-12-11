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
#include <climits>

enum class CompRes{ LESS, EQUAL, MORE };

std::vector<unsigned long long int> convert_input(std::vector<std::string> inputs)
{
  std::vector<unsigned long long> numbers;
  for(const auto& str : inputs)
  {
    numbers.push_back(std::atoi(str.c_str()));
  }
  return numbers;
}

CompRes checkRange(std::vector<unsigned long long int> data, int left, int right, const unsigned long long target)
{
  unsigned long long int sum = 0;
  for(int i = left; i <= right; i++)
  {
    sum += data[i];
  }
  if(sum < target)
    return CompRes::LESS;
  else if(sum == target)
    return CompRes::EQUAL;
  else
    return CompRes::MORE;
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

  std::vector<unsigned long long> numbers = convert_input(inputs);

  //Find a contiguous set of at least 2 numbers which adds up to 15690279
  // Use a sliding window
  const unsigned long long ANSWER = 15690279;
  int left = 0;
  int right = 1;

  while(right < (int)numbers.size())
  {
    CompRes res = checkRange(numbers,left,right,ANSWER);
    if(res == CompRes::LESS)
    {
      // not enough numbers, grow to the right
      right++;
    }
    else if(res == CompRes::EQUAL)
    {
      // This is the correct range, break out and go to answer processing
      break;
    }
    else
    {
      // We've gone over, drop from the left until we're equal or under again
      left++;
    }
    // Check if the current windows sums to our answer
  }

  if( right >= (int)numbers.size())
  {
    printf("No solution\n");
    return 0;
  }

  // Find the min and max values in the range
  unsigned long long min = ULLONG_MAX;
  unsigned long long max = 0;
  for(int i = left; i<= right; i++)
  {
    if(numbers[i] < min)
      min = numbers[i];
    if(numbers[i] > max)
      max = numbers[i];
  }
  unsigned long long solution = max + min;

  printf("Solution: %llu\n",solution);
  return 0;
}
