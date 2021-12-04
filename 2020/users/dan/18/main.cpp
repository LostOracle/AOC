#include <algorithm>
#include <iostream>
#include <sstream>
#include <numeric>
#include <map>
#include <string>
#include <vector>
#include <ranges>
#include <optional>
#include <stack>

using namespace std;
namespace
{
   using Int = uint64_t;

   Int handleOp(Int a, char op, Int b)
   {
      if (op == '+')
         return a + b;
      else
         return a * b;
   }

   // Performs the calculation for part 1
   Int process(string s)
   {
      stack<Int> savedV;
      stack<char> ops;

      char op = '+';
      Int v = 0;
      for (auto c : s)
      {
         if (c == '(')
         {
            savedV.push(v);
            ops.push(op);

            v = 0;
            op = '+';
         }
         else if (c == ')')
         {
            const Int parens = v;
            v = savedV.top();
            op = ops.top();
            v = handleOp(v, op, parens);

            savedV.pop();
            ops.pop();
         }
         else if (c == '+'
            || c == '*')
         {
            op = c;
         }
         else if (c == ' ')
         {
            // Skip
         }
         else
         {
            const Int temp = c - '0';
            v = handleOp(v, op, temp);
         }
      }

      return v;
   }

   // Performs the calculation for part 2
   Int process2(string s)
   {
      stack<Int> savedV; // This stack is for saving values
      stack<char> ops; // This stack saves operators
      stack<Int> opCount; // This stack saves the # of lower-precedence operations we pushed
      opCount.push(0);

      char op = '+';
      Int v = 0;
      for (auto c : s)
      {
         if (c == '(')
         {
            opCount.push(0);
            savedV.push(v);
            ops.push(op);

            v = 0;
            op = '+';
         }
         else if (c == ')')
         {
            // Resolve operations pushed onto the stack due to order of op
            for (int i = 0; i < opCount.top(); ++i)
            {
               const Int parens = v;
               v = savedV.top();
               op = ops.top();
               v = handleOp(v, op, parens);

               savedV.pop();
               ops.pop();
            }
            opCount.pop();

            // Now complete the operation we saved due to parens
            const Int parens = v;
            v = savedV.top();
            op = ops.top();
            v = handleOp(v, op, parens);

            savedV.pop();
            ops.pop();
         }
         else if (c == '*')
         {
            op = c;
            savedV.push(v);
            ops.push(op);
            opCount.top() += 1;

            v = 0;
            op = '+';
         }
         else if (c == '+')
         {
            op = c;
         }
         else if (c == ' ')
         {
            // Skip
         }
         else
         {
            const Int temp = c - '0';
            v = handleOp(v, op, temp);
         }
      }

      // Due to our order of op, we need to keep processing
      // until everything saved is done
      while (!savedV.empty())
      {
         v = handleOp(v, ops.top(), savedV.top());
         ops.pop();
         savedV.pop();
      }

      return v;
   }
}

int main()
{
   string line;
   vector<Int> values;
   vector<Int> values2;

   while (getline(cin, line))
   {
      values.push_back(process(line));
      values2.push_back(process2(line));
   }

   // Input check
   for (auto item : values)
   {
      cout << item << endl;
   }

   // Part 1
   const auto sum = accumulate(begin(values), end(values), Int{ 0 });
   cout << "Part 1: " << sum << endl;

   // Part 2
   for (auto item : values2)
   {
      cout << item << endl;
   }
   const auto sum2 = accumulate(begin(values2), end(values2), Int{ 0 });
   cout << "Part 2: " << sum2 << endl;

   return 0;
}


