#include <algorithm>
#include <bitset>
#include <cmath>
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
   // Maps address: value
   using Int = uint64_t;
   using MemMap = map<Int, Int>;

   using Mask = string;

   struct MemVal
   {
      Int addr = 0;
      Int val = 0;
   };

   struct MaskSet
   {
      Mask m;
      vector<MemVal> v;
   };

   ostream& operator<<(ostream& os, MaskSet ms)
   {
      os << "mask = " << ms.m << endl;//std::bitset<36>(ms.m) << endl;
      for (auto mv : ms.v)
      {
         os << "mem[" << mv.addr << "] = " << mv.val << endl;
      }
      return os;
   }
}

int main()
{
   string line;
   vector<MaskSet> input;

   auto curr = MaskSet{};
   while (getline(cin, line))
   {
      if (line.find("mask") != string::npos)
      {
         if (!curr.m.empty())
         {
            input.push_back(curr);
            curr.m = "";
            curr.v.clear();
         }

         curr.m.resize(36);
         sscanf(line.data(), "mask = %s", curr.m.data());
      }
      else
      {
         MemVal mv;

         sscanf(line.data(), "mem[%llu] = %llu", &mv.addr, &mv.val);
         curr.v.push_back(mv);
      }
   }
   input.push_back(curr);

   // Input check
   //for (auto item : input)
   //{
   //   cout << item << endl;
   //}

   // Part 1
   MemMap mm;
   for (auto entry : input)
   {
      for (auto maskSet : entry.v)
      {
         // Process # via the mask & add to our map
         auto processed = bitset<36>(maskSet.val);
         for (auto i = 0U; i < processed.size(); ++i)
         {
            const auto bitIdx = processed.size() - i - 1;
            const auto maskVal = entry.m[i];
            if (maskVal != 'X')
            {
               processed[bitIdx] = (maskVal == '0' ? 0 : 1);
            }
         }
         mm[maskSet.addr] = processed.to_ullong();
      }
   }

   const auto total = accumulate(begin(mm), end(mm), Int{ 0 },
      [](auto sum, auto mapEntry){ return sum + mapEntry.second; });
   cout << "Part 1: " << total << endl;


   auto strToNum = [](string s, unsigned permuteVal){
      // Create an integer by replacing 'X' values with a digit from the permutation
      auto newInt = s | views::reverse | views::transform([&](auto c){
         if (c == 'X')
         {
            const auto bit = permuteVal & 0x01;
            permuteVal >>= 1;
            return bit;
         }
         else
            return static_cast<unsigned>(c - '0');
      });

      // Convert the sequence of digits back to an integer
      const auto sum = accumulate(begin(newInt), end(newInt), Int{ 0 }, [](auto sum, auto digit){
         sum <<= 1;
         sum |= digit;
         return sum;
      });

      return sum;
   };

   // Part 2
   // Now it's a memory mask rather than a value mask.... >_>
   MemMap mm2;
   for (auto entry : input)
   {
      for (auto maskSet : entry.v)
      {
         // Process ADDRESS by the mask to produce a mask with Xs in it.
         // X overrides original value, as does 1. 0 keeps previous address value.
         const auto addrBits = bitset<36>(maskSet.addr);
         string mask = entry.m;
         for (auto i = 0U; i < mask.size(); ++i)
         {
            const auto bitIdx = mask.size() - i - 1;
            // 0s take the orig address value
            if (mask[i] == '0')
            {
               mask[i] = '0' + addrBits[bitIdx];
            }
         }

         // Now for the new masked address, compute each permutation for the X positions
         // Write to each of those memory locations. There are 2^(count X) combinations.
         const auto numPerms = pow(2,
            ranges::count_if(mask, [](auto c){ return c == 'X'; }));

         for (auto i = 0; i < numPerms; ++i)
         {
            const auto newAddr = strToNum(mask, i);
            mm2[newAddr] = maskSet.val;
         }
      }
   }
   const auto total2 = accumulate(begin(mm2), end(mm2), Int{ 0 },
      [](auto sum, auto mapEntry){ return sum + mapEntry.second; });
   cout << "Part 2: " << total2 << endl;

   return 0;
}


