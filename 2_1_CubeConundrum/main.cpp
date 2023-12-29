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

bool getColor(const string &color, const string &colorIndication, const long MAX_OF_THE_COLOR)
{
    if (color.find(colorIndication) == std::string::npos)
    {
        return true;
    }

    int ammountOfColors = std::stoi(color.substr(0, color.size() - color.find(" ")));
    // cout << ammountOfColors << endl;
    return ammountOfColors <= MAX_OF_THE_COLOR ? true : false;
}

bool parseSet(const string &set)
{
    vector<string> colors{};
    boost::split(colors, set, boost::is_any_of(","));
    for (const auto &color : colors)
    {
        if (not getColor(color, "red", MAX_RED_CUBES))
        {
            return false;
        }
        if (not getColor(color, "green", MAX_GREEN_CUBES))
        {
            return false;
        }
        if (not getColor(color, "blue", MAX_BLUE_CUBES))
        {
            return false;
        }
    }
    return true;
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

    if (std::any_of(sets.cbegin(), sets.cend(), [] (const string &s)
        {
            return not parseSet(s);
        }))
    {
        return;
    }

    sum += gameId;
    cout << "Game: " << gameId << " possible" << endl;
}

int main()
{
    setlocale(LC_ALL, "");
    srand(time(0));
    
    std::filesystem::path cwd = std::filesystem::current_path().filename();
    cout << "Hello World: " << cwd << endl;

    string filename("example_input"); // 8
    // string filename("input");
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

        // cout << num << endl;
        sum += num;
        v_all.push_back(num);
    }

    cout << "sum: " << sum << endl;
    cout << "sum2: " << std::accumulate(v_all.cbegin(), v_all.cend(), 0) << endl;


    return EXIT_SUCCESS;
}
