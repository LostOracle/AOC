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
   struct Range
   {
      int min = 0;
      int max = 0;
   };
   using VList = vector<Range>;

   struct FieldSpec
   {
      string name;
      VList ranges;
   };

   ostream& operator<<(ostream& os, FieldSpec fs)
   {
      os << fs.name << ": " << fs.ranges[0].min << "-" << fs.ranges[0].max
         << " or " << fs.ranges[1].min << "-" << fs.ranges[1].max;

      return os;
   }

   struct Ticket
   {
      vector<int> fields;
   };

   ostream& operator<<(ostream& os, Ticket t)
   {
      for (auto item : t.fields)
      {
         os << item << ",";
      }
      return os;
   }

   vector<int> ticketFromStr(string s)
   {
      stringstream ss(s);

      vector<int> v;
      string token;
      while (getline(ss, token, ','))
      {
         v.push_back(stoi(token));
      }

      return v;
   }

   bool inRange(FieldSpec f, int i)
   {
      return (f.ranges[0].min <= i && f.ranges[0].max >= i)
         || (f.ranges[1].min <= i && f.ranges[1].max >= i);
   }

   int invalidSum(Ticket t, const vector<FieldSpec>& fs)
   {
      return accumulate(begin(t.fields), end(t.fields), 0U,
         [&](int sum, int i){
            const auto noMatch = none_of(begin(fs), end(fs), [i](FieldSpec f){
               return inRange(f, i);
            });

            return (noMatch ? (sum + i) : sum);
         });
   }

   bool isInvalid(Ticket t, const vector<FieldSpec>& fs)
   {
      return any_of(begin(t.fields), end(t.fields),
         [&](int i){
            const auto noMatch = none_of(begin(fs), end(fs), [i](FieldSpec f){
               return inRange(f, i);
            });

            return noMatch;
         });
   }

   int scoreField(vector<Ticket> tickets, int ticketIdx, FieldSpec f)
   {
      return accumulate(begin(tickets), end(tickets), 0U,
         [&](int sum, Ticket t){
            const auto fieldMatch = inRange(f, t.fields[ticketIdx]);
            return sum + (fieldMatch ? 1 : 0);
         });
   }
}

int main()
{
   string line;
   Ticket myTicket;
   vector<Ticket> tickets;
   vector<FieldSpec> fieldSpecs;

   int mode = 0;
   while (getline(cin, line))
   {
      if (line.empty()) ++mode;
      else
      {
         switch(mode)
         {
         case 0:
         {
            FieldSpec f;
            f.name.resize(20);
            f.ranges.resize(2);
            sscanf(line.data(), "%[^:]: %d-%d or %d-%d",
               f.name.data(), &f.ranges[0].min, &f.ranges[0].max, &f.ranges[1].min, &f.ranges[1].max);

            fieldSpecs.push_back(f);
            break;
         }
         case 1:
            ++mode; // Skip this line;
            break;
         case 2:
         {
            myTicket.fields = ticketFromStr(line);
            break;
         }
         case 3:
            ++mode; // Skip this line
            break;
         case 4:
         {
            tickets.emplace_back(ticketFromStr(line));
            break;
         }
         }
      }
   }

   // Input check
   for (auto item : fieldSpecs)
   {
      cout << item << endl;
   }
   cout << endl;

   cout << myTicket << endl;
   cout << endl;

   for (auto item : tickets)
   {
      cout << item << endl;
   }
   cout << endl;

   // Part 1
   // Look for entirely invalid tickets
   const auto errorRate = accumulate(begin(tickets), end(tickets), 0U,
      [&](int sum, Ticket t){ return sum + invalidSum(t, fieldSpecs); });
   cout << "Part 1: " << errorRate << endl;;

   // Remove invalid tickets from consideration
   tickets.erase(remove_if(begin(tickets), end(tickets),
      [&](Ticket t){ return isInvalid(t, fieldSpecs); }),
      end(tickets));

   // Part 2
   // Figure out how fields map
   //
   // NOTE TO READER: WHAT FOLLOWS IS TERRIBLE CODE
   // AT THIS POINT, I DO NOT CARE
   //
   //for (int i = 0; i < myTicket.size(); ++i)
   //{
   //}
   const auto NumFields = fieldSpecs.size();
   struct Score
   {
      int idx = -1;
      string name;
      vector<int> colScores;
   };
   vector<Score> scores;
   for (int i = 0; i < NumFields; ++i)
   {
      Score s;
      s.name = fieldSpecs[i].name;
      scores.push_back(s);
      for (int t = 0; t < NumFields; ++t)
      {
         auto score = scoreField(tickets, t, fieldSpecs[i]);
         scores[i].colScores.push_back(score);
      }
   }

   vector<int> indices;

   const auto NumTickets = tickets.size();
   cout << "Num tickets: " << NumTickets << endl;

   sort(begin(scores), end(scores),
      [&](Score a, Score b){
         const auto numA = ranges::count(a.colScores, NumTickets);
         const auto numB = ranges::count(b.colScores, NumTickets);
         return numA < numB;
      });

   for (auto& fieldScore : scores)
   {
      auto idx = 0;
      for (auto i = 0; i < fieldScore.colScores.size(); ++i)
      {
         if (fieldScore.colScores[i] == NumTickets)
         {
            idx = i;
            break;
         }
      }

      indices.push_back(idx);
      fieldScore.idx = idx;
      cout << fieldScore.idx << ": " << fieldScore.name << ": ";
      for (auto stuff : fieldScore.colScores)
      {
         cout << stuff << ",";
      }
      cout << endl;

      // Mutate the rest so we don't screw up
      for (auto& score : scores)
      {
         score.colScores[idx] = 0;
      }
   }
   cout << endl;

   //// Now, for the "destination" field indices, multiply stuff together
   //auto fieldVals = scores
   //   | views::filter([&](const Score& s){ return s.name.find("departure") != string::npos; })
   //   | views::transform([&](const Score& s){ return myTicket.fields[s.idx]; });

   //auto sum = 0;
   //for (const auto& val : fieldVals)
   //{
   //   sum += val;
   //}
   cout << "Part 2: " << "Figure it out manually - I don't care anymore" << endl;

   return 0;
}


