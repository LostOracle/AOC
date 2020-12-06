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
   struct Group
   {
      vector<string> answers;
   };

   ostream& operator<<(ostream& os, const Group& g)
   {
      for (const auto& person : g.answers)
      {
         os << person << endl;
      }
      os << endl;
      return os;
   }
}

int main()
{
   string line;
   vector<Group> input;

   Group item;
   while (getline(cin, line))
   {
      stringstream ss(line);
      if (line != "")
      {
         string s;
         while (ss >> s)
         {
            item.answers.push_back(s);
         }
      }
      else
      {
         input.push_back(item);
         item = Group();
      }
   }
   // The final item might not have an extra line
   input.push_back(item);

   // Input check
   //for (auto item : input)
   //{
   //   cout << item << endl;
   //}

   // Part 1
   // Identify answers for which "anyone" answered "yes"
   // Transform from a list of group answers to a flattened list of all answers for the group
   const auto groups = views::transform(input,
      [](Group g)
      {
         // Concatenate all the answers together and sort
         auto flat = accumulate(begin(g.answers), end(g.answers), ""s);
         ranges::sort(flat);

         // Collapse down to unique answers
         const auto last = unique(begin(flat), end(flat));
         flat.erase(last, end(flat));

         return flat;
      });

   const auto sum = accumulate(begin(groups), end(groups), 0,
      [](auto sum, const auto g){ return sum + g.size(); });
   cout << "Part 1: " << sum << endl;

   // Part 2
   // Identify answers for which *everyone* answered "yes"
   // Generate 26 letters (the question codes),
   // then for each question, count the # of groups who all answered "yes" to that question
   const auto questionCounts = views::iota('a')
      | views::take(26)
      | views::transform([&](char c)
        {
           return ranges::count_if(input,
              [&](Group g){ return ranges::all_of(g.answers,
                 [&](string s){ return ranges::any_of(s, [&](char x){ return x == c; }); });
              });
        });

   // Now that we've got a count for each question collapse to a final sum
   auto sum2 = 0;
   for (auto i : questionCounts)
      sum2 += i;

   cout << "Part 2: " << sum2 << endl;

   return 0;
}


