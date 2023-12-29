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
#include <regex>

using namespace std;

struct Position
{
    int line;
    int xBegin;
    int xEnd;

    Position(const int _line, const int _xBegin, const int _xEnd) : line(_line), xBegin(_xBegin), xEnd(_xEnd) {}
};

struct Part
{
    int partNumber;
    Position position;
    vector<char> adjacentSymbols;

    Part(const int number, const Position pos) : partNumber(number), position(pos) {}
    void addAdjacentSymbol(const char c)
    {
        adjacentSymbols.push_back(c);
    }
};

void parseSchematic(const vector<string> &schematic, vector<Part> &parts)
{
    std::regex regexNumber("[0-9]+");

    for (size_t i=0; i<schematic.size(); i++)
    {
        std::smatch match{};

        auto searchStart(schematic.at(i).cbegin());
        while (regex_search(searchStart, schematic.at(i).cend(), match, regexNumber))
        {
            Position pos {static_cast<int>(i), 
                        static_cast<int>(match.position(0) + std::distance(schematic.at(i).cbegin(), searchStart)), 
                        static_cast<int>(match.position(0) + std::distance(schematic.at(i).cbegin(), searchStart) + 
                            match.length(0))};
            parts.emplace_back(std::stoi(match[0]), pos);
            // cout << match[0] << ": line: " << i << " xBegin: " << 
            //         match.position(0) + std::distance(schematic.at(i).cbegin(), searchStart) <<
            //         " xEnd: " << match.position(0) + std::distance(schematic.at(i).cbegin(), searchStart) +
            //          match.length(0) << endl;
            searchStart = match.suffix().first;
        }
        // cout << endl;
    }
}

void getAdjacentSymbols(const vector<string> &schematic, vector<Part> &parts)
{
    for (auto &p : parts)
    {
        cout << p.partNumber << ": line: " << p.position.line <<
                " xBegin: " << p.position.xBegin << " xEnd: " << p.position.xEnd << endl;
        // TOP
        if (0 != p.position.line)
        {
            for (int i=0; i<p.position.xEnd-p.position.xBegin; i++)
            {
                p.addAdjacentSymbol(schematic.at(p.position.line - 1).at(p.position.xBegin + i));
                cout << schematic.at(p.position.line - 1).at(p.position.xBegin + i);
            }
        }
        cout << endl;

        // BOTTOM
        if (p.position.line < schematic.size() - 1)
        {
            for (int i=0; i<p.position.xEnd-p.position.xBegin; i++)
            {
                p.addAdjacentSymbol(schematic.at(p.position.line + 1).at(p.position.xBegin + i));
                cout << schematic.at(p.position.line + 1).at(p.position.xBegin + i);
            }
        }
        cout << endl;

        // LEFT
        if (0 != p.position.xBegin)
        {
            if (0 != p.position.line)
            {
                p.addAdjacentSymbol(schematic.at(p.position.line - 1).at(p.position.xBegin - 1));
                cout << schematic.at(p.position.line - 1).at(p.position.xBegin - 1);    
            }
            p.addAdjacentSymbol(schematic.at(p.position.line).at(p.position.xBegin - 1));
            cout << schematic.at(p.position.line).at(p.position.xBegin - 1);
            if (p.position.line < schematic.size() - 1)
            {
                p.addAdjacentSymbol(schematic.at(p.position.line + 1).at(p.position.xBegin - 1));
                cout << schematic.at(p.position.line + 1).at(p.position.xBegin - 1);
            }
        }
        cout << endl;

        // RIGHT
        if (p.position.xEnd < schematic.at(p.position.line).size() - 1)
        {
            if (0 != p.position.line)
            {
                p.addAdjacentSymbol(schematic.at(p.position.line - 1).at(p.position.xEnd));
                cout << schematic.at(p.position.line - 1).at(p.position.xEnd);    
            }
            p.addAdjacentSymbol(schematic.at(p.position.line).at(p.position.xEnd));
            cout << schematic.at(p.position.line).at(p.position.xEnd);
            if (p.position.line < schematic.size() - 1)
            {
                p.addAdjacentSymbol(schematic.at(p.position.line + 1).at(p.position.xEnd));
                cout << schematic.at(p.position.line + 1).at(p.position.xEnd);
            }
        }
        cout << endl;
    }
}

void checkRealParts(const vector<Part> &parts, long &sum)
{
    for (auto const &part : parts)
    {
        if (std::any_of(part.adjacentSymbols.cbegin(), part.adjacentSymbols.cend(), [] (const char &s)
        {
            return '.' != s;
        }))
        {
            sum += part.partNumber;
        }
    }
}

int main()
{
    setlocale(LC_ALL, "");
    srand(time(0));
    
    std::filesystem::path cwd = std::filesystem::current_path().filename();
    cout << "Hello World: " << cwd << endl;

    // string filename("example_input"); // 4361
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

    vector<string> schematic{};
    vector<Part> parts{};

    while (getline(input_file, line, '\n')) 
    {
        schematic.push_back(line);
    }

    parseSchematic(schematic, parts);
    getAdjacentSymbols(schematic, parts);
    checkRealParts(parts, sum);

    cout << "sum: " << sum << endl;

    return EXIT_SUCCESS;
}
