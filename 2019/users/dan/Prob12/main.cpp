#include <algorithm>
#include <iostream>
#include <iomanip>
#include <numeric>
#include <string>
#include <sstream>
#include <functional>
#include <fstream>
#include <vector>
#include <map>

using namespace std;
namespace
{
   struct Moon
   {
      int id = 0;
      int x = 0;
      int y = 0;
      int z = 0;
      int vX = 0;
      int vY = 0;
      int vZ = 0;
   };

   void print(Moon m)
   {
      const auto doInt = [](ostream& os, string n, int i) { os << n << setw(4) << i; };
      cout << "pos=<";
      doInt(cout, "x=", m.x);
      doInt(cout, ", y=", m.y);
      doInt(cout, ", z=", m.z);
      cout << "> vel=<";
      doInt(cout, "x=", m.vX);
      doInt(cout, ", y=", m.vY);
      doInt(cout, ", z=", m.vZ);
      cout << ">" << endl;
   }

   void calcGrav(int p1, int& v1, int p2, int& v2)
   {
      if (p1 < p2)
      {
         v1 += 1;
         v2 -= 1;
      }
      else if (p1 > p2)
      {
         v1 -= 1;
         v2 += 1;
      }
   }

   void doGravity(vector<Moon>& moons)
   {
      vector<pair<int, int>> handled;
      for (auto& m : moons)
      {
         for (auto& n : moons)
         {
            auto pairM = find_if(begin(handled), end(handled),
               [&](pair<int, int> p){ 
                  return (p.first == m.id || p.first == n.id)
                  && (p.second == m.id || p.second == n.id);
               });
            if (m.id != n.id
               && pairM == end(handled))
            {
               calcGrav(m.x, m.vX, n.x, n.vX);
               calcGrav(m.y, m.vY, n.y, n.vY);
               calcGrav(m.z, m.vZ, n.z, n.vZ);
               handled.push_back({m.id, n.id});
            }
         }
      }
   }

   void doVelocity(vector<Moon>& moons)
   {
      for (auto& m : moons)
      {
         m.x += m.vX;
         m.y += m.vY;
         m.z += m.vZ;
      }
   }

   // Moves a single step in time
   void stepTime(vector<Moon>& moons)
   {
      doGravity(moons);
      doVelocity(moons);
   }

   int totEnergy(vector<Moon>& moons)
   {
      // Potential energy
      auto potE = [](Moon m){ return abs(m.x) + abs(m.y) + abs(m.z); };
      // Kinetic energy
      auto kinE = [](Moon m){ return abs(m.vX) + abs(m.vY) + abs(m.vZ); };

      return std::accumulate(begin(moons), end(moons), 0,
         [&](int tot, Moon m){ return tot + (potE(m) * kinE(m)); });
   }

   int64_t checkRepeat(auto getPos, auto getVel, vector<Moon>& moons)
   {
      vector<pair<int, int>> posVel;
      for (auto m : moons)
      {
         posVel.emplace_back(pair<int, int>{ getPos(m), getVel(m) });
      }

      auto check = [&]()
      {
         int i = 0;
         return all_of(begin(moons), end(moons),
            [&](Moon m)
            {
               int idx = i++;
               return (getPos(m) == posVel[idx].first
                  && getVel(m) == posVel[idx].second);
            }
         );
      };

      int64_t count = 0;
      do
      {
         stepTime(moons);
         ++count;
      } while(!check());

      return count;
   }
}

int main()
{
   cout << "Enter filename: " << endl;
   string fileName;
   cin >> fileName;
   ifstream inF(fileName);

   vector<Moon> moons;
   string line;
   int id = 0;
   while(getline(inF, line))
   {
      istringstream in(line);
      Moon m { id++ };
      in.ignore(5, '=');
      in >> m.x;
      in.ignore(5, '=');
      in >> m.y;
      in.ignore(5, '=');
      in >> m.z;
      moons.push_back(m);
   }

   for (auto m : moons)
      print(m);

   // Part 1: Calc total energy
   {
      auto mCopy = moons;
      vector<int> energies;
      cout << "Enter # of steps to perform: " << endl;
      int64_t steps;
      cin >> steps;
      for (int64_t i = 0; i < steps; ++i)
      {
         stepTime(mCopy);
         //cout << "After " << i+1 << " steps" << endl;
         //for (auto m : moons) print(m);
         energies.push_back(totEnergy(mCopy));
      }

      for (auto m: mCopy) print(m);
      auto energy = totEnergy(mCopy);
      cout << "Total energy: " << energy << endl;
   }

   // Part 2: Find repeat
   {
      auto mCopy = moons;

      auto repeatX = checkRepeat([](Moon m){ return m.x; }, [](Moon m){ return m.vX; }, mCopy);
      cout << "Repeat X: " << repeatX << endl;
      auto repeatY = checkRepeat([](Moon m){ return m.y; }, [](Moon m){ return m.vY; }, mCopy);
      cout << "Repeat Y: " << repeatY << endl;
      auto repeatZ = checkRepeat([](Moon m){ return m.z; }, [](Moon m){ return m.vZ; }, mCopy);
      cout << "Repeat Z: " << repeatZ << endl;

      auto initLcm = lcm(repeatX, repeatY);
      auto finLcm = lcm(initLcm, repeatZ);
      cout << "Final LCM: " << finLcm << endl;
   }

   return 0;
}
