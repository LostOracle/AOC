#include <algorithm>
#include <iostream>
#include <sstream>
#include <numeric>
#include <map>
#include <string>
#include <vector>
#include <ranges>
#include <optional>
#include <span>

using namespace std;
namespace
{
   using SpanIt = span<uint64_t>;

   bool validNext(SpanIt mySpan, uint64_t num)
   {
      bool valid = false;
      for (auto i = 0; i < mySpan.size(); ++i)
      {
         const auto firstNum = mySpan[i];
         const auto target = num - firstNum;
         const auto subSpan = mySpan.subspan(i);

         const auto secondNum = find(begin(subSpan), end(subSpan), target);

         if (secondNum != end(subSpan)
            && *secondNum != firstNum)
         {
            valid = true;
            break;
         }
      }

      return valid;
   }

   using OptNum = optional<uint64_t>;

   OptNum checkWindow(SpanIt mySpan, uint64_t num, int windowSize)
   {
      OptNum valid;
      auto start = begin(mySpan);
      auto window = SpanIt{ start, windowSize };

      while (end(window) != end(mySpan))
      {
         const auto sum = accumulate(begin(window), end(window), uint64_t{ 0 });
         if (sum == num)
         {
            const auto maxVal = *max_element(begin(window), end(window));
            const auto minVal = *min_element(begin(window), end(window));
            valid = OptNum(minVal + maxVal);
            break;
         }
         ++start;
         window = SpanIt{ start, windowSize };
      }

      return valid;
   }
}

int main()
{
   string line;
   vector<uint64_t> input;

   while (getline(cin, line))
   {
      input.push_back(stoull(line));
   }

   // Input check
   //for (auto item : input)
   //{
   //   cout << item << endl;
   //}

   // Part 1
   uint64_t matchFail = 0;
   for (int i = 25-1; i < input.size(); ++i)
   {
      if (!validNext(span<uint64_t>(&input[i-25], 25), input[i]))
      {
         matchFail = input[i];
         break;
      }
   }
   cout << "Part 1: " << matchFail << endl;

   // Part 2
   // Find the sequence which matches 1721308972
   const auto spanIt = SpanIt{ begin(input), input.size() };
   OptNum op;
   const auto target = uint64_t{ 1721308972 };
   for (int windowSize = 2; windowSize < input.size() && !op; ++windowSize)
   {
      op = checkWindow(spanIt, target, windowSize);
   }
   if (op)
   {
      cout << "Part 2: " << *op << endl;
   }
   else
   {
      cout << "Part 2 not found" << endl;
   }

   return 0;
}


