#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <numeric>
#include <ranges>
#include <sstream>
#include <string>
#include <vector>
#include <list>
#include <filesystem>
#include <memory>
#include <map>
#include <set>
#include <boost/algorithm/string/classification.hpp> // Include boost::for is_any_of
#include <boost/algorithm/string/split.hpp> // Include for boost::split
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/range/adaptor/transformed.hpp>
#include <boost/range/adaptors.hpp>
#include <chrono>
#include <thread>
#include <compare>
#include <initializer_list>
#include <ctime>
#include <concepts>

using namespace std;

constexpr int MAX_RED_CUBES = 12;
constexpr int MAX_GREEN_CUBES = 13;
constexpr int MAX_BLUE_CUBES = 14;

struct Cube
{
    int red;
    int green;
    int blue;
};

bool getColor(const string &color, const string &colorIndication, int &ammount)
{
    if (color.find(colorIndication) == std::string::npos)
    {
        return false;
    }

    ammount = std::stoi(color.substr(0, color.size() - color.find(" ")));
    return true;
}

void parseSet(const string &set, Cube &cube)
{
    vector<string> colors{};
    boost::split(colors, set, boost::is_any_of(","));
    for (const auto &color : colors)
    {
        int ammount{};

        if (getColor(color, "red", ammount))
        {
            cube.red = ammount;
        }
        if (getColor(color, "green", ammount))
        {
            cube.green = ammount;
        }
        if (getColor(color, "blue", ammount))
        {
            cube.blue = ammount;
        }
    }
}

void checkLine(const string &s, long &sum)
{
    string gameIdString = s.substr(s.find(" ") + 1, s.find(":") - s.find(" ") - 1);
    int gameId = std::stoi(gameIdString);
    // cout << std::quoted(gameIdString) << ": " << gameId << endl;
    string games = s.substr(s.find(":") + 1);
    // cout << std::quoted(games) << endl;

    vector<string> sets{};
    boost::split(sets, games, boost::is_any_of(";"));

    vector<Cube> cubes{};
    for (const auto &set : sets)
    {
        Cube cube{};
        parseSet(set, cube);
        cubes.push_back(cube);
    }

    // for (const auto &cube: cubes)
    // {
    //     cout << "R: " << cube.red << " G: " << cube.green << " B: " << cube.blue << endl;
    // }

    auto minRed = std::max_element(cubes.cbegin(), cubes.cend(), [] (const Cube &Lcube, const Cube &Rcube)
    {
        return (Lcube.red < Rcube.red);
    });
    auto minGreen = std::max_element(cubes.cbegin(), cubes.cend(), [] (const Cube &Lcube, const Cube &Rcube)
    {
        return (Lcube.green < Rcube.green);
    });
    auto minBlue = std::max_element(cubes.cbegin(), cubes.cend(), [] (const Cube &Lcube, const Cube &Rcube)
    {
        return (Lcube.blue < Rcube.blue);
    });

    // cout << "Red: " << (*minRed).red << endl;
    // cout << "Green: " << (*minGreen).green << endl;
    // cout << "Blue: " << (*minBlue).blue << endl;

    sum += (*minRed).red * (*minGreen).green * (*minBlue).blue;
}

int main()
{
    setlocale(LC_ALL, "");
    srand(time(0));
    
    std::filesystem::path cwd = std::filesystem::current_path().filename();
    cout << "Hello World: " << cwd << endl;

    // string filename("example_input"); // 2286
    string filename("input");
    ifstream input_file(filename);

    string line{};
 
    if (not input_file.is_open())
    {
        cout << "Could not open the file: " << filename << endl;
        return EXIT_FAILURE;
    }

    std::vector<int> v_all{};
    long sum {};

    while (getline(input_file, line, '\n')) 
    {
        long num{};
        checkLine(line, num);

        cout << num << endl;
        sum += num;
        v_all.push_back(num);
    }

    cout << "sum: " << sum << endl;
    cout << "sum2: " << std::accumulate(v_all.cbegin(), v_all.cend(), 0) << endl;


    return EXIT_SUCCESS;
}
