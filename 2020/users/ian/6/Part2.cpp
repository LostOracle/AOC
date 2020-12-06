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

class Answers
{
  public:
  std::string answered;
  bool first_set;
  Answers()
  {
    first_set = true;
  }

  void add_answers(std::string ans)
  {
    // If we haven't gotten any answers yet, just take this line wholesale
    if(first_set)
    {
      answered = ans;
      first_set = false;
    }
    // Otherwise, our new answer is a logical AND of the old and the input
    std::string new_ans;
    for(const auto& c : ans)
    {
      if( std::find(answered.begin(), answered.end(), c) != answered.end() )
      {
        new_ans.push_back(c);
      }
    }
    answered = new_ans;
  }

  int count_answers()
  {
    return answered.size();
  }

  void print_answers()
  {
    printf("Answers: ");
    for(const auto& c : answered)
    {
      printf("%c,",c);
    }
    printf("\n");
  }
};

std::tuple<bool, Answers> processLine(std::string line)
{
  static Answers ansInProgress = Answers();
  // If the line is empty
  if( line.length() == 0 )
  {
    // Then we've "finished" parsing this set of answers, return it
    // Make a copy of the answers in progress and return it
    Answers finishedAns = ansInProgress;
    ansInProgress = Answers();
    return std::tuple<bool,Answers>(true, finishedAns);
  }

  // Otherwise, collect the answered questions
  ansInProgress.add_answers(line);
  return std::tuple<bool,Answers>(false,Answers());
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

  // We need a newline at the end to make sure we get the "last" passport
  // so add one in, just in case
  inputs.push_back("");

  int valid_answers = 0;
  for( const auto & line : inputs )
  {
    auto [finished,ans] = processLine(line);
    if(finished)
    {
      //ans.print_answers();
      valid_answers += ans.count_answers();
    }
  }
  printf("Solution: %d\n",valid_answers);
  return 0;
}
