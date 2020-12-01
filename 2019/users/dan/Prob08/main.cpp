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
    struct Layer
    {
        vector<vector<int>> rows;
        int zeroCount = 0;
        int oneCount = 0;
        int twoCount = 0;
    };

    // Convert input to layers
    vector<Layer> layers(string input, int wide, int tall)
    {
        auto out = vector<Layer>();

        auto idx = 0U;

        // Continue until we have no string left
        while (idx < input.size() - 1)
        {
            auto l = Layer{};
            for(int i = 0; i < tall; ++i) // Rows
            {
                auto row = vector<int>();

                for (int j = 0; j < wide; ++j) // Columns
                {
                    const auto val = input[idx] - '0';
                    row.push_back(val);
                    ++idx;

                    // Count digits in the "image"
                    if (val == 0)
                        ++l.zeroCount;
                    else if (val == 1)
                        ++l.oneCount;
                    else if (val == 2)
                        ++l.twoCount;
                }
                l.rows.push_back(row);
            }
            out.push_back(l);
        }

        return out;
    }

    // Overlay rules: 2 is transparent, 1 is black, 0 is white
    // Overlay images to find out what each pixel value is
    Layer overlay(vector<Layer> layList)
    {
        Layer finalImage;

        for (auto row = 0U; row < layList[0].rows.size(); ++row)
        {
            auto newRow = vector<int>();
            for (auto idx = 0U; idx < layList[0].rows[row].size(); ++idx)
            {
                // Look for the first visible pixel (top to bottom)
                auto layNum = 0;
                auto visPix = 2;
                do
                {
                    visPix = layList[layNum].rows[row][idx];
                    ++layNum;
                } while (visPix == 2);
                newRow.push_back(visPix);
            }
            finalImage.rows.push_back(newRow);
        }

        return finalImage;
    }

    void print(Layer lay)
    {
        for(auto row : lay.rows)
        {
            for (auto i : row)
                cout << i;
            cout << endl;
        }
    }

    void printSparse(Layer lay)
    {
        for(auto row : lay.rows)
        {
            for (auto i : row)
            {
                if (i == 0)
                    cout << ' ';
                else
                    cout << i;
            }
            cout << endl;
        }
    }

    void test()
    {
        const auto w = 3;
        const auto t = 2;
        const auto input = string("123456789012");

        const auto l = layers(input, w, t);

        for (auto lay : l)
        {
            print(lay);
        }
    }
}

int main()
{
    //test();

    string line;
    getline(cin, line);

    // Divide input into layers
    auto l = layers(line, 25, 6);

    // Find fewest 0 digits in layer
    auto laySorted = l;
    sort(begin(laySorted), end(laySorted),
        [](const Layer& a, const Layer& b)
        {
            return a.zeroCount < b.zeroCount;
        });

    // Count 1s & 2s and multiply by each other
    cout << laySorted[0].oneCount * laySorted[0].twoCount << endl;

    // Part 2: Overlay images
    auto finalImage = overlay(l);
    printSparse(finalImage);

    return 0;
}
