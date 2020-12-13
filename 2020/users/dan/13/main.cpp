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
}

int main()
{
   string line;
   vector<int> input;
   vector<int> input2;
   uint64_t timestamp = 0;

   getline(cin, line);
   timestamp = stoll(line);
   while (getline(cin, line, ','))
   {
      if ("x" == line)
         input2.push_back(1);
      else
      {
         const auto busId = stoi(line);
         input.push_back(busId);
         input2.push_back(busId);
      }
   }

   // Input check
   //for (auto item : input)
   //{
   //   cout << item << endl;
   //}

   // Part 1
   // Find the earliest bus we can take
   const auto busWaits = input
      | views::transform([&](auto i){ return i - lldiv(timestamp, i).rem; });

   const auto minWait = ranges::min_element(busWaits);
   cout << "Part 1: " << *minWait * (input[distance(begin(busWaits), minWait)]) << endl;

   // Part 2
   // Find the sequence where bus IDs proceed in the input order, one minute after another
   const auto maxBus = &input2[0];

   const auto roughStart = uint64_t{ timestamp };
   const auto startPoint = lldiv(roughStart, *maxBus).quot * *maxBus;

   // Returns the offset between timestamp and the next bus
   const auto busOffset = [&](uint64_t busId, uint64_t timestamp) {
      return lldiv(timestamp, busId).rem;
   };

   // Since all the inputs are relatively prime, the LCM is the two #s multiplied together
   // So once we find a matching offset for two busses, we know that offset will repeat at each LCM
   // Therefore, we can use that advancement as our new value to check
   auto matchTime = startPoint;
   auto currBus = begin(input2)+1;
   auto advanceVal = static_cast<uint64_t>(input2[0]);
   while (currBus != end(input2))
   {
      matchTime += advanceVal;
      auto dist = distance(begin(input2), currBus);
      if (busOffset(*currBus, matchTime + dist) == 0)
      {
         advanceVal *= *currBus;
         do
         {
            ++currBus; // Go to the next bus
         } while (currBus != end(input2)
            && *currBus == 1); // Skip 'x' busses
      }
   }

   cout << "Part 2: " << matchTime << endl;

   return 0;
}


