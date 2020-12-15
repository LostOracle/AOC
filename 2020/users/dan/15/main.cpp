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
   using Int = uint32_t;

   struct Game
   {
      Int turn = 0;
      vector<Int> history;
      map<Int, Int> histMap;
   };

   // Returns the next number in the sequence
   //Int turn(const vector<Int>& history)
   //{
   //   const auto last = history.back();
   //   auto next = Int{ 0 };

   //   // Find second occurrences of the last #
   //   const auto second = ranges::find(rbegin(history)+1, rend(history), last);

   //   // If the last # is unique, say "0"
   //   // Otherwise, calculate distance between previous utterances of this number
   //   if (second != rend(history))
   //   {
   //      //const auto secondIt = second.base() - 1; // Convert to forward iterator
   //      next = distance(rbegin(history), second);
   //   }

   //   return next;
   //}

   Int fastTurn(Int last, map<Int, Int>& histMap, Int currTurn)
   {
      auto next = Int{ 0 };

      const auto prevUtterance = histMap.find(last);
      if (prevUtterance != end(histMap))
      {
         next = currTurn - prevUtterance->second - 1; // -1 is due to pre-increment
      }

      // We insert LAST, not NEXT - we don't want to consider this int while executing next turn
      histMap[last] = currTurn-1; // Note - we want to overwrite on insert

      return next;
   }
}

int main()
{
   string line;
   vector<Int> input;

   while (getline(cin, line, ','))
   {
      input.push_back(stoull(line));
   }

   // Input check
   for (auto item : input)
   {
      cout << item << endl;
   }

   const auto runGame = [&input](Game g, Int duration)
   {
      // Initialize our map with the input
      Int last = input[0];
      for (int i = 1; i < input.size(); ++i)
      {
         g.histMap[last] = i;
         last = input[i];
      }
      g.turn = input.size();

      // Run until duration of turns is satisfied
      while (g.turn < Int{ duration })
      {
         ++g.turn;
         last = fastTurn(last, g.histMap, g.turn);
      }

      return last;
   };

   // Part 1
   Game g1;
   cout << "Part 1: " << runGame(g1, Int{ 2020 }) << endl;;

   // Part 2
   // Execute game until turn 30000000
   Game g2;
   cout << "Part 2: " << runGame(g2, Int{ 30000000 }) << endl;

   return 0;
}


