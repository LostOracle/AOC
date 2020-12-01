#include <algorithm>
#include <fstream>
#include <iostream>
#include <numeric>
#include <set>
#include <map>
#include <string>
#include <vector>
#include <sstream>
#include <stack>
#include <cmath>


using namespace std;

namespace
{
   struct Chemical
   {
      string name;
      int64_t count;
   };

   struct Reaction
   {
      Chemical product;
      vector<Chemical> reagents;
   };

   // Parses a line of the reaction list into a struct
   Reaction parseLine(string line)
   {
      replace(begin(line), end(line), ',', ' ');
      auto inS = istringstream{line};

      Reaction r;

      // Parse out all the reagents
      do
      {
         Chemical c;
         inS >> c.count;
         inS >> c.name;
         r.reagents.push_back(c);
         inS >> std::ws;
      } while (inS.peek() != '='); 

      // Parse out the product
      inS.ignore(5, '>');
      inS >> r.product.count;
      inS >> r.product.name;

      return r;
   }
}

int main()
{
   string fileName;
   cin >> fileName;
   ifstream in(fileName);

   map<string, Reaction> rs;
   
   std::string line;
   while(getline(in, line))
   {
      auto r = parseLine(line);
      rs.insert({r.product.name, r});
   }
   for (auto val : rs)
   {
      cout << val.second.product.count << " " << val.second.product.name << ": ";
      for (auto rea : val.second.reagents)
      {
         cout << rea.count << " " << rea.name << ", ";
      }
      cout << endl;
   }

   map<string, int64_t> leftovers;
   stack<Chemical> s;

   // This function calculates reagents needed to produce chem "C"
   // and pushes those onto the stack
   auto push_reagents = [&](Chemical c)
   {
      auto reaction = rs[c.name];

      // Check for leftovers first
      auto productNeeded = c.count;
      const auto stored = leftovers[reaction.product.name];
      const auto storedReacts = static_cast<int64_t>(
         std::ceil(
            static_cast<double>(productNeeded - stored) / reaction.product.count));

      const auto totalProduct = (storedReacts * reaction.product.count) + stored;
      const auto extra = totalProduct - productNeeded;
      leftovers[reaction.product.name] = extra;
      if (storedReacts > 0)
      {
         //cout << "For " << productNeeded << " " << c.name << ", need ";
         for (auto r : reaction.reagents)
         {
            s.push(Chemical{ r.name, r.count * storedReacts});
            //cout << r.count * storedReacts << " " << r.name << ", ";
         }
         //cout << " with " << extra << " " << c.name << " left over." << endl;
      }
   };

   // This function counts amt of ore required for X fuel
   auto count_ore = [&](int64_t numFuel)
   {
      auto fuel_target = rs["FUEL"].product;
      fuel_target.count = numFuel;
      push_reagents(fuel_target);

      int64_t oreCount = 0;
      while (!s.empty())
      {
         auto c = s.top();
         s.pop();
         if (c.name == "ORE")
            oreCount += c.count;
         else
            push_reagents(c);
      }

      cout << "FUEL: " << numFuel << ", Total ore count: " << oreCount << endl;
      return oreCount;
   };

   // Part 1: Count ore for 1 fuel
   count_ore(1);

   // Part 2: Find fuel produced by 1 tril. ore

   // First, locate our upper and lower bounds for a binary search
   auto currOre = int64_t{ 0 };
   auto currFuel = int64_t{ 1 };
   auto prevFuel = currFuel;
   const auto trill = int64_t{ 1000000000000 };
   while (currOre < trill)
   {
      prevFuel = currFuel;
      currFuel *= 2;
      currOre = count_ore(currFuel);
   }

   // Now, taking previous and current ore, binary search 
   auto upper = currFuel;
   auto lower = prevFuel;
   auto curr = upper;
   auto ore = int64_t{ 0 };
   auto failsafe = 0;
   do
   {
      curr = lower + (int64_t(upper - lower) / 2);
      cout << "Lower: " << lower << " < " << curr << " < " << upper << endl;
      ore = count_ore(curr);
      if (ore < trill)
         lower = curr;
      else
         upper = curr;
      cout << "Ore: " << ore << ", Fuel: " << curr << endl;
      ++failsafe;
   } while(ore != trill && failsafe < 200);

   return 0;
}


