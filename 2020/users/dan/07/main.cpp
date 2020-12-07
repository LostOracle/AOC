#include <algorithm>
#include <iostream>
#include <sstream>
#include <numeric>
#include <map>
#include <string>
#include <vector>
#include <ranges>
#include <optional>

using namespace std;
namespace
{
   struct BagCount { string type; int count; };
   struct BagSpec
   {
      string type;
      int count;
      vector<BagCount> bags;
   };

   using BagMap = map<string, BagSpec>;

   string readColor(stringstream& ss)
   {
      // Colors are 2 words
      string a, b;
      ss >> a >> b;
      return a + " "s + b;
   }

   ostream& operator<<(ostream& os, const BagSpec& bs)
   {
      os << bs.type << " bags contain ";
      for (const auto& item : bs.bags)
      {
         os << item.count << " " << item.type << " bag(s), ";
      }
      os << endl;
      return os;
   }

   bool hasGoldBag(const BagSpec& bs, const BagMap& bagMap)
   {
      const auto hasGold =
         ranges::any_of(bs.bags,
         [](auto inner){
            return inner.type == "shiny gold";
         })
         || ranges::any_of(bs.bags,
            [&](auto inner){
               auto innerSpec = bagMap.find(inner.type);
               return hasGoldBag(innerSpec->second, bagMap);
            });

      return hasGold;
   }

   int countBags(const BagSpec& bs, const BagMap& bagMap)
   {
      const auto sumBags = accumulate(begin(bs.bags), end(bs.bags), 0,
         [&](auto sum, auto bc){
            const auto innerSpec = bagMap.find(bc.type);
            const auto innerCount = countBags(innerSpec->second, bagMap);
            return sum + (bc.count * (innerCount+1));
         });

      return sumBags;
   }
}

int main()
{
   string line;
   BagMap input;

   while (getline(cin, line))
   {
      BagSpec item;
      stringstream ss(line);

      item.type = readColor(ss);

      ss.ignore(" bags contain"s.size());

      while (ss.good())
      {
         string a;
         ss >> a;
         if (a == "no")
         {
            ss.setstate(ios_base::failbit);
         }
         else
         {
            BagCount bc;
            bc.count = stoi(a);
            bc.type = readColor(ss);
            item.bags.push_back(bc);

            ss.ignore(numeric_limits<streamsize>::max(), ',');
         }
      }

      input.emplace(item.type, item);
   }

   // Input check
   //for (auto item : input)
   //{
   //   cout << item.second;
   //}

   // Part 1
   // Figure out what kind of bags could contain a gold bag

   const auto totalGold = ranges::count_if(input, [&](auto bs){ return hasGoldBag(bs.second, input); });
   cout << "Part 1: " << totalGold << endl;

   // Part 2
   const auto goldSpec = input.find("shiny gold");
   cout << "Part 2: " << countBags(goldSpec->second, input) << endl;

   return 0;
}


