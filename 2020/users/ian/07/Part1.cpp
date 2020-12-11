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
#include <regex>
#include <assert.h>

#include "regex_error.h"

struct ContainedItem
{
  ContainedItem() : number(0), color("colorless"){};
  ContainedItem(int number, std::string color) : number(number), color(color){};
  int number;
  std::string color;
};

using rule_t = std::vector<ContainedItem>;
using rules_t = std::map<std::string, rule_t>;

rules_t rules;

std::string get_containing_color(std::string in_str);
rule_t get_contained_items(std::string in_str);

void print_rule(rule_t rule)
{
  printf("Rule: ");
  for(const auto& cItem : rule)
  {
    printf("%d %s, ",cItem.number, cItem.color.c_str());
  }
  printf("\n");
}

bool parse_line(std::string in_str, rules_t & /*rules*/)
{
  // Search for the (color)(" bags contain ") part of the string and give back only the color
 // std::string containing_color = get_containing_color(in_str);
  //printf("Containing: %s\n",containing_color.c_str());

  std::string containing_color = get_containing_color(in_str);
  // Ensure we got an answer
  if(containing_color.length() == 0)
  {
    return false;
  }

  printf("Containing bag: %s\n",containing_color.c_str());

  rule_t rule = get_contained_items(in_str);
  print_rule(rule);

  // populate map with this new rule
  rules.insert(std::pair<std::string, rule_t>(containing_color,rule));
  return true;
}

rule_t get_contained_items(std::string in_str)
{
  std::regex re = std::regex("\\d.*?(?= bag)");

  std::regex_iterator<std::string::iterator> rit(in_str.begin(), in_str.end(), re);
  std::regex_iterator<std::string::iterator> rend;

  std::vector<std::string> results;
  while(rit != rend)
  {
    results.push_back(rit->str());
    ++rit;
  }

  // Now we have strings of the form "# color" where color may be multiple words
  // There may be 0 matches if there are no contained bags
  // Separate each result string into parts
  
  re = std::regex("(\\d) (.*$)");
  std::smatch string_match;

  rule_t rule;
  for(const auto& item : results)
  {
    std::regex_match(item.begin(), item.end(), string_match,re);
    // There should be 3 matches - the full match, and two submatches
    assert(string_match.size() == 3);

    rule.push_back(ContainedItem(std::atoi(string_match[1].str().c_str()),string_match[2].str()));
  }
  return rule;
}

std::string get_containing_color(std::string in_str)
{
  // look for each thing of the form # <color> bag
  // ^ - beginning of line
  // . - match any character
  // * - 0 or more times
  // (?=<stuff>) - positive lookahead, must be followed by <stuff>, but don't include <stuff> in the match
  // | - sepates possible match options
  // \\ - escaped backslash
  // \d - any decimal digit

  // Example: light red bags contain 1 bright white bag, 2 muted yellow bags.
  // Matches <light red( bags contain)> with the first clause
  // Matches <1 bright white( bag)> and <2 muted yellow( bag)> with the second clause
  std::regex re = std::regex("^.*(?= bags contain)");
//|
  std::regex_iterator<std::string::iterator> rit(in_str.begin(), in_str.end(), re);
  std::regex_iterator<std::string::iterator> rend;

  std::vector<std::string> results;
  while(rit != rend)
  {
    results.push_back(rit->str());
    ++rit;
  }

  if(results.size() != 1)
  {
    return "";
  }
  return results[0];
}

std::vector<std::string> crawl_for(rules_t rules, std::string search_color)
{
  // Search for any bags which can contain the current bag of interest


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


  for( const auto & line : inputs )
  {
    parse_line(line,rules);
  }

  std::vector<std::string> possible_bags = crawl_for(rules, "shiny gold");
  printf("Solution: %d\n",0);
  return 0;
}
