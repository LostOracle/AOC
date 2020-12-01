#include <algorithm>
#include <iostream>
#include <numeric>
#include <set>
#include <string>
#include <sstream>
#include <functional>
#include <vector>
#include <map>
#include <stack>

using namespace std;
namespace
{
    struct OrbitInfo
    {
        vector<string> children;
        int depth = 0;
        bool visited = false;
        string parent = "";
    };
    struct Depth
    {
        string name;
        int depth = 0; 
    };

    // Calculates depth of each node in the tree
    void depthCalc(map<string, OrbitInfo>& orbits, string start)
    {
        stack<Depth> s;
        s.emplace(Depth{start, 0});
        while (!s.empty())
        {
            auto i = s.top();
            s.pop();
            for (auto child : orbits[i.name].children)
                s.emplace(Depth{child, i.depth + 1});

            orbits[i.name].depth = i.depth;
        }
    }

    // Calculates # of transfers ("distance-ish") from start to each node in the tree
    // Each node is marked as visited so we hit it only once
    void oTransfer(map<string, OrbitInfo>& orbits, string start)
    {
        stack<Depth> s;
        s.emplace(Depth{start, 0});
        while (!s.empty())
        {
            auto i = s.top();
            s.pop();
            for (auto child : orbits[i.name].children)
            {
                // Add each child if not already visited
                if (!orbits[child].visited)
                    s.emplace(Depth{child, i.depth + 1});
            }

            // Add parent, if not already visited
            const auto parentName = orbits[i.name].parent;
            if (!orbits[parentName].visited)
                s.emplace(Depth{parentName, i.depth + 1});

            orbits[i.name].depth = i.depth; // Record depth of this node
            orbits[i.name].visited = true; // Mark visited for this node
        }
    }
}

int main()
{
    // Input is a flat description of a tree
    // P)C == Parent -> Child
    // Take each parent & child & shove in a map

    std::string line;
    map<string, OrbitInfo> orbits;

    while (getline(cin, line))
    {
        istringstream in(line);
        string parent;
        getline(in, parent, ')');
        string child;
        getline(in, child);

        cout << parent << ")" << child << endl;

        orbits[parent].children.push_back(child);
    }

    for (auto& node : orbits)
    {
        for (auto childName : node.second.children)
        {
            orbits[childName].parent = node.first;
        }
    }

    // Start at the top of the tree (COM) and iterate
    auto part1 = orbits;
    depthCalc(part1, "COM");

    // Part 1: "Orbital checksum"
    auto sum = std::accumulate(begin(part1), end(part1), 0,
        [](int value, const std::map<string, OrbitInfo>::value_type& entry)
        {
            return value + entry.second.depth;
        });
    cout << sum << endl;

    // Part 2: Number of transfers btwn YOU and SAN
    auto part2 = orbits;
    oTransfer(part2, "YOU");
    auto santaNode = part2["SAN"];
    // We remove two because we don't want to include the current node (YOU) or Santa (SAN)
    cout << "Transfers to SANTA: " << santaNode.depth - 2 << endl;

    return 0;
}
