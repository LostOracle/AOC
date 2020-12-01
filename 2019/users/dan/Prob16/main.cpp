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
using namespace std::placeholders;

namespace
{
   using FFT = vector<int>;

   // Generates the "FFT" pattern for us
   class Pat
   {
   public:
      Pat(int eN_): elemNum(eN_) {}

      int next()
      {
         static const auto pat = vector<int>{ 0, 1, 0, -1 };
         ++curr;
         curr %= (4*elemNum);
         auto idx = curr / elemNum;
         return pat[idx];
      }

   private:
      int elemNum = 0;
      int curr = 0;
   };

   void phase(FFT& f, int offset = 0)
   {
      const auto fCopy = f;
      const int max = fCopy.size();
      if (offset < max / 2) // Check for cheat-y case
      {
         for (int i = offset; i < max; ++i)
         {
            auto& c = f[i];
            // Generate pattern
            Pat p(i+1);

            // Mult & sum
            c = accumulate(begin(fCopy)+offset, end(fCopy), 0,
               [&](int sum, int a) { return sum + p.next()*a; });
            c = abs(c) % 10; // Keep only the ones digit
            //cout << " = " << c << endl;
         }
      }
      else
      {
         // Yippee, we get to cheat
         // The pattern has a whole bunch of leading zeroes at high offsets
         // If our offset is more than half the vector, we never need to subtract - only add
         auto curr = accumulate(begin(fCopy)+offset, end(fCopy), 0); 

         for (int i = offset; i < max; ++i)
         {
            auto& c = f[i];
            c = abs(curr) % 10;
            curr -= fCopy[i];
         }
      }
   }

   FFT doPhases(FFT in, int count, int offset = 0)
   {
      auto temp = in;
      for (int i = 0; i < count; ++i)
      {
         phase(temp, offset);
      }

      return temp;
   }

   void print(FFT out, int offset = 0)
   {
      for (auto i = offset; i < offset+8; ++i)
         cout << out[i];
      cout << endl;
   }
}

int main()
{
   string fileName;
   cin >> fileName;
   ifstream in(fileName);

   FFT seq;
   std::string line;
   getline(in, line);
   for (auto c : line)
      seq.push_back(c - '0');

   // Part 1: Calc fft over input pattern
   {
      auto out = doPhases(seq, 100);
      print(out);
   }

   // Part 2: Calc fft over "real input" - 10000x what we were given
   {
      // Separate the offset
      auto offset = 0;
      for (auto i = 0; i < 7; ++i)
         offset = offset*10 + seq[i];

      if (offset < (int)(seq.size()*10000))
      {
         // Duplicate pattern 10000 and do it again
         FFT s2;
         for (auto i = 0; i < 10000; ++i)
         {
            copy(begin(seq), end(seq), back_inserter(s2));
         }
         cout << "Done preparing input for pt 2." << endl;
         cout << "Size: " << s2.size() << "; Offset: " << offset << endl;

         auto out = doPhases(s2, 100, offset);
         print(out, offset);
      }
   }

   return 0;
}


