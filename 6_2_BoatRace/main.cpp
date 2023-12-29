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
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/range/adaptor/tokenized.hpp>
#include <boost/range/adaptor/transformed.hpp>
#include <boost/range/adaptors.hpp>
#include <boost/range/algorithm/copy.hpp>
#include <chrono>
#include <thread>
#include <compare>
#include <initializer_list>
#include <ctime>
#include <concepts>
#include <regex>
#include <deque>
#include <climits>
#include <thread>

using namespace std;

struct BoatTimeTable
{
    inline static const string TIME_TOKEN = "Time:";
    inline static const string DISTANCE_TOKEN = "Distance:";

    vector<int> time;
    vector<int> distance;

    long realTime;
    long realDistance;

    BoatTimeTable() = default;

    string const to_str() const
    {
        string s = "Time:\t";
        for (const auto &el : time)
        {
            s += "\t" + std::to_string(el);
        }
        s += "\nDistance:";
        for (const auto &el : distance)
        {
            s += "\t" + std::to_string(el);
        }
        return s;
    }

    string const to_str2() const
    {
        return "Time:\t\t" + std::to_string(realTime) + 
                "\nDistance:\t" + std::to_string(realDistance);
    }

    friend std::ostream & operator<<(std::ostream &os, const BoatTimeTable &boatTimeTable)
    {
        return os << boatTimeTable.to_str2();
    }
};

void collectNumbers(const string &line, const string &token, vector<int> &container)
{
    if (line.find(token) == std::string::npos)
    {
        cout << "Not found: " << token << "!" << endl;
        return;
    }
    
    vector<string> numbers{};
    boost::split(numbers, 
                line.substr(line.find(token) + token.size()),
                boost::is_any_of(" "));
    if (numbers.empty())
    {
        cout << "Not found entries for: " << token << "!" << endl;
        return;
    }    

    for (size_t i=0; i<numbers.size(); i++)
    {
        if (numbers.at(i).empty())
        {
            continue;
        }

        container.push_back(stoi(numbers.at(i)));
    }
}

void collectNumbers(const string &line, const string &token, long &dest)
{
    if (line.find(token) == std::string::npos)
    {
        cout << "Not found: " << token << "!" << endl;
        return;
    }

    string tmp = line.substr(line.find(token) + token.size());
    tmp.erase(std::remove(tmp.begin(), tmp.end(), ' '), tmp.end());
    // boost::erase_all(tmp, " ");

    dest = stol(tmp);
}

void parseBoatTimeTable(const vector<string> &lines, BoatTimeTable &boatTimeTable)
{
    collectNumbers(lines.at(0), boatTimeTable.TIME_TOKEN, boatTimeTable.time);
    collectNumbers(lines.at(1), boatTimeTable.DISTANCE_TOKEN, boatTimeTable.distance);

    collectNumbers(lines.at(0), boatTimeTable.TIME_TOKEN, boatTimeTable.realTime);
    collectNumbers(lines.at(1), boatTimeTable.DISTANCE_TOKEN, boatTimeTable.realDistance);
}

void calculateStrategy(BoatTimeTable &boatTimeTable)
{
    vector<int> bestRaceRecords{};
    for (size_t i=0; i<boatTimeTable.time.size(); i++)
    {
        int numbersOfRecords = 0;
        for (int stopTime=0; stopTime<boatTimeTable.time.at(i); stopTime++)
        {
            int dist = stopTime * (boatTimeTable.time.at(i) - stopTime);
            // cout << "Distance traveled: " << dist << endl;
            if (dist > boatTimeTable.distance.at(i))
            {
                numbersOfRecords++;
            }
        }
        bestRaceRecords.push_back(numbersOfRecords);
    }

    cout << "Best race records:\n";
    for (const auto &el : bestRaceRecords)
    {
        cout << el << " ";
    }
    cout << endl;

    cout << "Result: " << std::accumulate(bestRaceRecords.cbegin(), bestRaceRecords.cend(), 1, std::multiplies<int>())
         << endl;
}

void calculateStrategy2(BoatTimeTable &boatTimeTable)
{
    long numbersOfRecords = 0;
    for (long stopTime=0; stopTime<boatTimeTable.realTime; stopTime++)
    {
        long dist = stopTime * (boatTimeTable.realTime - stopTime);
        // cout << "Distance traveled: " << dist << endl;
        if (dist > boatTimeTable.realDistance)
        {
            numbersOfRecords++;
        }
    }
    cout << "Total: " << numbersOfRecords << endl;
}

int main()
{
    setlocale(LC_ALL, "");
    srand(time(0));
    
    std::filesystem::path cwd = std::filesystem::current_path().filename();
    cout << "Hello World: " << cwd << endl;

    // string filename("example_input"); // 71503
    string filename("input");
    ifstream input_file(filename);

    string line{};
 
    if (not input_file.is_open())
    {
        cout << "Could not open the file: " << filename << endl;
        return EXIT_FAILURE;
    }

    vector<string> lines{};

    while (getline(input_file, line, '\n')) 
    {
        lines.push_back(line);
    }

    BoatTimeTable boatTimeTable{};
    parseBoatTimeTable(lines, boatTimeTable);

    cout << boatTimeTable << endl;

    // calculateStrategy(boatTimeTable);
    calculateStrategy2(boatTimeTable);

    return EXIT_SUCCESS;
}
