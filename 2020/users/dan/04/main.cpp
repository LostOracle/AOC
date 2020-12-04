#include <algorithm>
#include <iostream>
#include <sstream>
#include <numeric>
#include <set>
#include <string>
#include <vector>
#include <ranges>
#include <optional>

using namespace std;
namespace
{
   struct Passport
   {
      optional<int> byr;
      optional<int> iyr;
      optional<int> eyr;
      optional<string> hgt;
      optional<string> hcl;
      optional<string> ecl;
      optional<string> pid;
      optional<uint64_t> cid;
   };

   ostream& operator<<(ostream& os, const Passport& p)
   {
      if (p.byr) os << "byr:" << *p.byr << " ";
      if (p.iyr) os << "iyr:" << *p.iyr << " ";
      if (p.eyr) os << "eyr:" << *p.eyr << " ";
      if (p.hgt) os << "hgt:" << *p.hgt << " ";
      if (p.hcl) os << "hcl:" << *p.hcl << " ";
      if (p.ecl) os << "ecl:" << *p.ecl << " ";
      if (p.pid) os << "pid:" << *p.pid << " ";
      if (p.cid) os << "cid:" << *p.cid << " ";

      return os;
   }

   // Validates each field of the struct
   // Destroys invalid fields
   void validate(Passport& p)
   {
      // Each check below verifies a particular field
      // If the field fails, we reset it, putting the optional back to its uninitialized form

      if (p.byr && !(1920 <= *p.byr && *p.byr <= 2002))
         p.byr.reset();
      if (p.iyr && !(2010 <= *p.iyr && *p.iyr <= 2020))
         p.iyr.reset();
      if (p.eyr && !(2020 <= *p.eyr && *p.eyr <= 2030))
         p.eyr.reset();
      if (p.hgt)
      {
         int v = 0;
         string unit;
         unit.resize(10);
         sscanf((*p.hgt).data(), "%d%s", &v, unit.data());

         if (!((unit.starts_with("cm") && 150 <= v && v <= 193)
               || (unit.starts_with("in") && 59 <= 59 && v <= 76)))
            p.hgt.reset();
      }
      if (p.hcl)
      {
         const auto val = *p.hcl;
         if (!(val.size() == 7
            && val[0] == '#'
            && all_of(begin(val)+1, end(val),
               [](char c){
                  return ('0' <= c && c <= '9')
                     || ('a' <= c && c <= 'f');
               })))
            p.hcl.reset();
      }
      if (p.ecl)
      {
         static const auto eyeColors = { "amb", "blu", "brn", "gry", "grn", "hzl", "oth" };
         if (none_of(begin(eyeColors), end(eyeColors), [&](string s){ return s == *p.ecl; }))
            p.ecl.reset();
      }
      if (p.pid)
      {
         const auto val = *p.pid;
         if (!(val.size() == 9
            && all_of(begin(val), end(val), [](char c){ return '0' <= c && c <= '9'; })))
         p.pid.reset();
      }
   }

   // Maps fields to the struct. No validation of data.
   void parse(string s, Passport& p)
   {
      stringstream ss(s);
      // Figure out which field we have
      string tag;
      string val;
      getline(ss, tag, ':');
      ss >> val;

      if (tag == "byr")      p.byr = stoi(val);
      else if (tag == "iyr") p.iyr = stoi(val);
      else if (tag == "eyr") p.eyr = stoi(val);
      else if (tag == "hgt") p.hgt = val;
      else if (tag == "hcl") p.hcl = val;
      else if (tag == "ecl") p.ecl = val;
      else if (tag == "pid") p.pid = val;
      else if (tag == "cid") p.cid = stoull(val);
      else {
         cout << "Invalid input: " << tag << ":" << val << endl;
         throw range_error("Invalid Input");
      }
      
   }
}

int main()
{
   string line;
   vector<Passport> input;

   Passport p;
   while (getline(cin, line))
   {
      stringstream ss(line);
      cout << line << endl;
      if (line != "")
      {
         string s;
         while (ss >> s)
         {
            parse(s, p);
         }
      }
      else
      {
         input.push_back(p);
         p = Passport();
      }
   }
   // The final passport might not have an extra line
   input.push_back(p);

   // Input check
   //for (auto p : input)
   //{
   //   cout << p << endl;
   //}

   const auto isValid = [](Passport p) {
      return p.byr && p.iyr && p.eyr && p.hgt && p.hcl && p.ecl && p.pid;
   };

   // Part 1
   // Count the # of valid passports.
   // Valid passports have all fields, except cid which is truly optional
   const auto numValid = ranges::count_if(input, isValid);
   cout << "Part 1: " << numValid << endl;

   // Part 2
   // Actually validate passport field values
   ranges::for_each(input, validate);
   const auto twoValid = ranges::count_if(input, isValid);
   cout << "Part 2: " << twoValid << endl;

   return 0;
}


