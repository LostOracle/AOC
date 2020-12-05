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

class Passport
{
  public:
  Passport()
  {
    for(const auto & f : RequiredFields)
    {
      fields[f]="";
    }
    for(const auto & f : OptionalFields)
    {
      fields[f]="";
    }
  }

  Passport(const Passport &p2)
  {
    fields = p2.fields;
  }

  Passport& operator=(const Passport &p2)
  {
    fields = p2.fields;
    return *this;
  }

  void print()
  {
    for( auto it = fields.begin(); it != fields.end(); ++it)
    {
      printf("%s:%s\n",it->first.c_str(),it->second.c_str());
    }
  }

  bool isValid()
  {
    for(const auto & f : RequiredFields)
    {
      if( fields[f] == "" )
      {
        return false;
      }
      if(!validationFuncs.at(f)(fields[f]) )
      {
        printf("FAiled validation function for: %s\n",f.c_str());
        return false;
      }
    }
    return true;
  }

  void populate(std::string key, std::string data)
  {
    fields[key] = data;
  }

  static bool validate_byr(const std::string byr)
  {
    return validate_num_range(byr,1920,2002);
  }

  static bool validate_iyr(const std::string iyr)
  {
    return validate_num_range(iyr,2010,2020);
  }

  static bool validate_eyr(const std::string eyr)
  {
    return validate_num_range(eyr,2020,2030);
  }

  static bool validate_hgt(const std::string hgt)
  {
    char units[3];
    int num = 0;
    int parsed = sscanf(hgt.c_str(),"%d%2s",&num,units);
    if(parsed != 2)
    {
      return false;
    }
    if(strncmp(units,"cm",2) == 0)
    {
      return validate_num_range(num,150,193);
    }
    else if(strncmp(units,"in",2) == 0)
    {
      return validate_num_range(num,59,76);
    }
    else
    {
      return false;
    }
  }

  static bool validate_num_range(const int num, int min, int max)
  {
    if( min <= num && num <= max )
    {
      return true;
    }
    return false;
  }

  static bool validate_num_range(const std::string str, int min, int max)
  {
    int num = 0;
    int parsed = sscanf(str.c_str(),"%d",&num);
    if( parsed != 1)
    {
      return false;
    }
    return validate_num_range(num,min,max);
  }

  static bool pass(const std::string)
  {
    return true;
  }

  static bool validate_hcl(const std::string str)
  {
    // exactly 7 characters, # then 0-f
    if(str.length() != 7)
    {
      return false;
    }
    if(str[0] != '#')
    {
      return false;
    }
    for(const auto& c : str.substr(1,6))
    {
      if(!isxdigit(c))
      {
        return false;
      }
    }
    return true;
  }

  static bool validate_ecl(const std::string str)
  {
    const std::vector<std::string> valid_strs = { "amb","blu","brn","gry","grn","hzl","oth" };
    for(const auto& v_str : valid_strs)
    {
      if( str == v_str)
      {
        return true;
      }
    }
    return false;
  }

  static bool validate_pid(const std::string str)
  {
    // Nine digits, all 0-9
    if(str.length() != 9)
    {
      return false;
    }
    for(const auto& c : str)
    {
      if(!isdigit(c))
      {
        return false;
      }
    }
    return true;
  }

  // Expected Fields
  const std::vector<std::string> RequiredFields = {"byr","iyr","eyr","hgt","hcl","ecl","pid"};
  const std::vector<std::string> OptionalFields = {"cid"};

  // Validation functions
  using validationFuncT = std::function<bool(const std::string)>;
  using validationFuncPair = std::pair<std::string,validationFuncT>;

  const std::map<std::string,validationFuncT> validationFuncs = {
    validationFuncPair("byr",validate_byr),
    validationFuncPair("iyr",validate_iyr),
    validationFuncPair("eyr",validate_eyr),
    validationFuncPair("hgt",validate_hgt),
    validationFuncPair("hcl",validate_hcl),
    validationFuncPair("ecl",validate_ecl),
    validationFuncPair("pid",validate_pid)
  };


  std::map<std::string,std::string> fields;
};

std::tuple<bool, Passport> processLine(std::string line)
{
  static Passport passInProgress;
  // If the line doesn't contain any key:value pairs
  if( line.find(":") == std::string::npos)
  {
    // Then we've "finished" parsing this Passport, return it
    // Make a copy of the passport in progress and return it
    Passport finishedPass = passInProgress;
    passInProgress = Passport();
    return std::tuple<bool,Passport>(true, finishedPass);
  }

  // Otherwise, parse the key:values out of the line until we can't find any more
  int offset = 0;
  while(true)
  {
    char buffer[100];

    // Get the next block of characters
    int bytesRead = 0;
    int parsed = sscanf(line.c_str() + offset,"%s%n", buffer, &bytesRead);

    // update the offset for the next read call - we'll want to skip to the
    // end of what we've already read
    offset += bytesRead;

    // If we failed to parse out an item, we're done with this line, move on
    if( parsed != 1)
    {
      break;
    }

    // If we found something, we need to break it into key and value
    char * key = std::strtok(buffer,": \n");
    std::string keyStr = key;
    char * val = strtok(NULL,": \n");
    std::string valStr = val;

    passInProgress.populate(keyStr,valStr);
  }
  return std::tuple<bool,Passport>(false,Passport());
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
  inputs.push_back("\n");

  int valid_passports = 0;
  for( const auto & line : inputs )
  {
    auto [finished,pass] = processLine(line);
    if(finished)
    {
      pass.print();
      if(pass.isValid())
      {
        valid_passports++;
      }
    }
  }
  printf("Solution: %d\n",valid_passports);
  return 0;
}
