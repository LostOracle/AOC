#include <algorithm>
#include <iostream>
#include <numeric>
#include <set>
#include <string>
#include <sstream>
#include <vector>

using namespace std;
namespace
{
    const int RED = 1;
    const int BLUE = 2;

    struct Coord
    {
        int x;
        int y;
        int currLen = 0;
    };

    struct Segment
    {
        Coord start;
        Coord stop;
        int color;
        int currLen = 0;
    };


    Segment segment(Coord init, char dir, int amt, int color)
    {
        if (dir == 'R')
        {
            return {init, Coord{init.x + amt, init.y}, color};
        }
        else if (dir == 'D')
        {
            return {init, Coord{init.x, init.y - amt}, color};
        }
        else if (dir == 'L')
        {
            return {init, Coord{init.x - amt, init.y}, color};
        }
        else if (dir == 'U')
        {
            return {init, Coord{init.x, init.y + amt}, color};
        }
        else
        {
            throw std::runtime_error(string("Invalid direction: ") + dir);
        }
    }

    void printSeg(Segment seg)
    {
        std::cout << seg.start.x << ", " << seg.start.y << " -> "
            << seg.stop.x << ", " << seg.stop.y << ": "
            << (seg.color == RED ? "RED" : "BLUE") << std::endl;
    }

    bool vert(Segment s)
    {
        // If x is the same, the segment is vertical
        return s.start.x == s.stop.x;
    }

    bool inSeg(int val, int start, int stop)
    {
        int a = start;
        int b = stop;
        if (stop < start)
        {
            a = stop;
            b = start;
        }

        return (val >= a && val <= b);
    }

    Coord intersect(Segment a, Segment b)
    {
        // 1. Are they orthogonal?
        const auto vA = vert(a);
        const auto vB = vert(b);

        if (vA == vB)
        {
            return { 0, 0 };
        }

        // 2. Calculate intersection point
        Coord inter{ 0, 0 };
        bool onSeg = false;
        if(vB)
        {
            inter.x = b.start.x;
            inter.y = a.start.y;
            inter.currLen = a.currLen + b.currLen
                + abs(a.start.x - inter.x)
                + abs(b.start.y - inter.y);
            onSeg = inSeg(inter.x, a.start.x, a.stop.x)
                && inSeg(inter.y, b.start.y, b.stop.y);
        }
        else
        {
            inter.x = a.start.x;
            inter.y = b.start.y;
            inter.currLen = a.currLen + b.currLen
                + abs(a.start.y - inter.y)
                + abs(b.start.x - inter.x);
            onSeg = inSeg(inter.x, b.start.x, b.stop.x)
                && inSeg(inter.y, a.start.y, a.stop.y);
        }

        if(onSeg)
        {
            printSeg(a);
            printSeg(b);
            std::cout << inter.x << ", " << inter.y << std::endl;
        }

        return onSeg ? inter : Coord{ 0, 0 };
    }
}

int main()
{
    auto parseWire = [&](Coord init, int color)
    {
        vector<Segment> segments;

        string w;
        getline(cin, w);
        stringstream in(w);

        int currLen = 0;
        string val;
        while(getline(in, val, ','))
        {
            char dir;
            int amt;
            sscanf(val.data(), "%c%d", &dir, &amt);
            auto seg = segment(init, dir, amt, color);
            init = seg.stop;
            seg.currLen = currLen;
            currLen += amt;
            segments.push_back(seg);
        }

        return segments;
    };

    // Wire A
    const auto segA = parseWire( Coord{0, 0}, RED );
    // Wire B
    const auto segB = parseWire( Coord{0, 0}, BLUE );

    // for(const auto& seg : segments)
    // {
    //     std::cout << seg.start.x << ", " << seg.start.y << " -> "
    //         << seg.stop.x << ", " << seg.stop.y << ": "
    //         << (seg.color == RED ? "RED" : "BLUE") << std::endl;
    // }

    // For each segment, figure out if there is an intersection
    vector<Coord> inter;
    for (const auto sA : segA)
    {
        for (const auto sB : segB)
        {
            auto i = intersect(sA, sB);
            if (i.x != 0
                && i.y != 0)
            {
                inter.push_back(i);
            }
        }
    }

    std::sort(begin(inter), end(inter),
        [](Coord a, Coord b)
        {
            return (abs(a.x) + abs(a.y)) < (abs(b.x) + abs(b.y));
        });

    for (const auto& pt : inter)
    {
        std::cout << pt.x << ", " << pt.y << std::endl;
    }

    std::cout << "Min dist: " << abs(inter[0].x) + abs(inter[0].y) << std::endl;

    std::sort(begin(inter), end(inter),
        [](Coord a, Coord b)
        {
            return (a.currLen < b.currLen);
        });

    std::cout << "Min sig dist: " << inter[0].currLen << std::endl;


    return 0;
}
