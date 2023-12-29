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

struct Pipe
{
    enum class PipeSymbol
    {
        NORTH_TO_SOUTH, //  |
        WEST_TO_EAST,   //  -
        NORTH_TO_EAST,  //  L
        NORTH_TO_WEST,  //  J
        SOUTH_TO_WEST,  //  7
        SOUTH_TO_EAST,  //  F
        GROUND,         //  .
        START           //  S
    };

    enum class PipeDirection
    {
        LEFT = 0,
        RIGHT,
        TOP,
        BOTTOM,
        UNKNOWN
    };

    inline static const vector<Pipe::PipeSymbol> pipesStartList = 
            { Pipe::PipeSymbol::NORTH_TO_SOUTH, Pipe::PipeSymbol::WEST_TO_EAST, Pipe::PipeSymbol::NORTH_TO_EAST,
                Pipe::PipeSymbol::NORTH_TO_WEST, Pipe::PipeSymbol::SOUTH_TO_WEST, Pipe::PipeSymbol::NORTH_TO_EAST};
    inline static const vector<Pipe::PipeSymbol> pipesTopList = 
            { Pipe::PipeSymbol::NORTH_TO_SOUTH, Pipe::PipeSymbol::SOUTH_TO_WEST, Pipe::PipeSymbol::SOUTH_TO_EAST};
    inline static const vector<Pipe::PipeSymbol> pipesBottomList = 
            { Pipe::PipeSymbol::NORTH_TO_SOUTH, Pipe::PipeSymbol::NORTH_TO_EAST, Pipe::PipeSymbol::NORTH_TO_WEST};
    inline static const vector<Pipe::PipeSymbol> pipesLeftList = 
            { Pipe::PipeSymbol::WEST_TO_EAST, Pipe::PipeSymbol::NORTH_TO_EAST, Pipe::PipeSymbol::SOUTH_TO_EAST};
    inline static const vector<Pipe::PipeSymbol> pipesRightList = 
            { Pipe::PipeSymbol::WEST_TO_EAST, Pipe::PipeSymbol::SOUTH_TO_WEST, Pipe::PipeSymbol::NORTH_TO_WEST};

    char symbol;
    PipeSymbol pipeSymbol;
    long steps;
    PipeDirection direction;
    Pipe *left;
    Pipe *right;
    Pipe *top;
    Pipe *bottom;
    Pipe *from;
    Pipe *to;

    Pipe() = default;
    ~Pipe() = default;
    Pipe(const Pipe &pipe) = delete;
    Pipe(Pipe &&pipe) = default;
    Pipe(const char c) : symbol(c), pipeSymbol(Pipe::getPipeSymbol(c)), steps(0), direction(PipeDirection::UNKNOWN),
                            left(nullptr), right(nullptr), top(nullptr), bottom(nullptr), from(nullptr), to(nullptr) {};

    static PipeSymbol getPipeSymbol(const char c)
    {
        if ('|' == c) return PipeSymbol::NORTH_TO_SOUTH;
        if ('-' == c) return PipeSymbol::WEST_TO_EAST;
        if ('L' == c) return PipeSymbol::NORTH_TO_EAST;
        if ('J' == c) return PipeSymbol::NORTH_TO_WEST;
        if ('7' == c) return PipeSymbol::SOUTH_TO_WEST;
        if ('F' == c) return PipeSymbol::SOUTH_TO_EAST;
        if ('.' == c) return PipeSymbol::GROUND;
        if ('S' == c) return PipeSymbol::START;
        else
        {
            cout << "Wrong symbol!" << endl;
            return PipeSymbol::GROUND;
        }
    }

    Pipe *getNextPipeBasedOnDirection(const vector<Pipe::PipeSymbol> &pipeList, const PipeDirection dir)
    {
        // cout << "dir: " << static_cast<int>(dir) << endl;
        Pipe *destinationPipe = nullptr;
        if (PipeDirection::TOP == dir) destinationPipe = top;
        if (PipeDirection::BOTTOM == dir) destinationPipe = bottom;
        if (PipeDirection::LEFT == dir) destinationPipe = left;
        if (PipeDirection::RIGHT == dir) destinationPipe = right;

        // if (destinationPipe == nullptr) cout << "nullptr" << endl;
        if ((destinationPipe != nullptr) and 
            (pipeList.cend() != std::find(pipeList.cbegin(), pipeList.cend(), destinationPipe->pipeSymbol)) or
            (PipeSymbol::START == destinationPipe->pipeSymbol))
        {
            to = destinationPipe;
            destinationPipe->from = this;
            direction = dir;
            return destinationPipe;
        }

        cout << "Wrong pipe end!!" << endl;
        return this;
    }

    Pipe *getNextPipeFromStart(const vector<Pipe::PipeSymbol> &pipeList)
    {
        if ((left != nullptr) and (pipeList.cend() != std::find(pipeList.cbegin(), pipeList.cend(), left->pipeSymbol)))
        {
            cout << "left" << endl;
            to = left;
            left->from = this;
            direction = PipeDirection::LEFT;
            return left;
        }
        if ((right != nullptr) and (pipeList.cend() != std::find(pipeList.cbegin(), pipeList.cend(), (*right).pipeSymbol)))
        {
            cout << "right" << endl;
            to = right;
            (*right).from = this;
            direction = PipeDirection::RIGHT;
            return right;
        }
        if ((top != nullptr) and (pipeList.cend() != std::find(pipeList.cbegin(), pipeList.cend(), (*top).pipeSymbol)))
        {
            cout << "top" << endl;
            to = top;
            (*top).from = this;
            direction = PipeDirection::TOP;
            return top;
        }
        if ((bottom != nullptr) and (pipeList.cend() != std::find(pipeList.cbegin(), pipeList.cend(), (*bottom).pipeSymbol)))
        {
            cout << "bottom" << endl;
            to = bottom;
            (*bottom).from = this;
            direction = PipeDirection::BOTTOM;
            return bottom;
        }
        cout << "Wrong pipe end at start!" << endl;
        return this;
    }

    Pipe *getNextPipe()
    {
        if ((from == nullptr) and (PipeSymbol::START != pipeSymbol)) return this;

        cout << *this << endl;
        switch (pipeSymbol)
        {
        case PipeSymbol::START:
            {
                return getNextPipeFromStart(Pipe::pipesStartList);
            }
        case PipeSymbol::NORTH_TO_SOUTH:
            {
                if (PipeDirection::TOP == from->direction) return getNextPipeBasedOnDirection(Pipe::pipesTopList, PipeDirection::TOP);
                else return getNextPipeBasedOnDirection(Pipe::pipesBottomList, PipeDirection::BOTTOM);
            }
        case PipeSymbol::WEST_TO_EAST:
            {
                if (PipeDirection::RIGHT == from->direction) return getNextPipeBasedOnDirection(Pipe::pipesRightList, PipeDirection::RIGHT);
                else return getNextPipeBasedOnDirection(Pipe::pipesLeftList, PipeDirection::LEFT);
            }
        case PipeSymbol::NORTH_TO_EAST:
            {
                // cout << "L: direction: " << static_cast<int>(from->direction) << endl;
                if (PipeDirection::BOTTOM == from->direction) return getNextPipeBasedOnDirection(Pipe::pipesRightList, PipeDirection::RIGHT);
                else return getNextPipeBasedOnDirection(Pipe::pipesTopList, PipeDirection::TOP);
            }
        case PipeSymbol::NORTH_TO_WEST:
            {
                // cout << "J: direction: " << static_cast<int>(from->direction) << endl;
                if (PipeDirection::BOTTOM == from->direction) return getNextPipeBasedOnDirection(Pipe::pipesLeftList, PipeDirection::LEFT);
                else return getNextPipeBasedOnDirection(Pipe::pipesTopList, PipeDirection::TOP);
            }
        case PipeSymbol::SOUTH_TO_WEST:
            {
                if (PipeDirection::TOP == from->direction) return getNextPipeBasedOnDirection(Pipe::pipesLeftList, PipeDirection::LEFT);
                else return getNextPipeBasedOnDirection(Pipe::pipesBottomList, PipeDirection::BOTTOM);
            }
        case PipeSymbol::SOUTH_TO_EAST:
            {
                if (PipeDirection::TOP == from->direction) return getNextPipeBasedOnDirection(Pipe::pipesRightList, PipeDirection::RIGHT);
                else return getNextPipeBasedOnDirection(Pipe::pipesBottomList, PipeDirection::BOTTOM);
            }
        case PipeSymbol::GROUND:
        default:
            {
                cout << "Wrong pipe end!" << endl;
                break;
            }
        }
        return this;
    }

    string const showConnections() const
    {
        string s{};
        if (nullptr != top)
        {
            s += "Top: " + Pipe::to_str(*top) + " ";
        }
        if (nullptr != bottom)
        {
            s += "Bottom: " + Pipe::to_str(*bottom) + " ";
        }
        if (nullptr != left)
        {
            s += "Left: " + Pipe::to_str(*left) + " ";
        }
        if (nullptr != right)
        {
            s += "Right: " + Pipe::to_str(*right) + " ";
        }
        return s + "\n";
    }

    static string const to_str(const Pipe &pipe)
    {
        return string(1, pipe.symbol);
    }

    friend std::ostream & operator<<(std::ostream &os, const Pipe &pipe)
    {
        return os << Pipe::to_str(pipe);
    }
};

using PipeMatrix = vector<vector<Pipe>>;

struct Maze
{
    PipeMatrix pipeMatrix;
    Pipe *startPipe;

    Maze() = default;
    ~Maze() = default;
    // Maze(const Maze &maze) = delete;
    // Maze(Maze &&maze) = default;

    void checkConnections()
    {
        for (size_t i=0; i<pipeMatrix.size(); i++)
        {
            for (size_t j=0; j<pipeMatrix.at(i).size(); j++)
            {
                if (i>0)
                {
                    pipeMatrix.at(i).at(j).top = &pipeMatrix.at(i-1).at(j);
                }
                if (i<pipeMatrix.size()-1)
                {
                    pipeMatrix.at(i).at(j).bottom = &pipeMatrix.at(i+1).at(j);
                }
                if (j>0)
                {
                    pipeMatrix.at(i).at(j).left = &pipeMatrix.at(i).at(j-1);
                }
                if (j<pipeMatrix.at(i).size()-1)
                {
                    pipeMatrix.at(i).at(j).right = &pipeMatrix.at(i).at(j+1);
                }
            }
        }
    }

    void slide()
    {
        for (auto &linePipes : pipeMatrix)
        {
            for (auto &pipe: linePipes)
            {
                if (Pipe::getPipeSymbol(pipe.symbol) == Pipe::PipeSymbol::START)
                {
                    cout << "Found start pipe" << endl;
                    startPipe = &pipe;
                }
            }
        }
        
        long distance = 1;
        Pipe *pipe = startPipe;
        pipe = pipe->getNextPipe();
        // for (int i=0; i<15; i++)
        // {
        //     pipe = pipe->getNextPipe();
        // }
        while (Pipe::PipeSymbol::START != pipe->pipeSymbol)
        {
            distance++;
            pipe = pipe->getNextPipe();
        }
        cout << "Distance: " << distance/2 << endl;
    }

    void parseInput(const vector<string> &lines)
    {
        for (const auto &line : lines)
        {
            vector<Pipe> pipes{};
            for (const auto &c : line)
            {
                pipes.emplace_back(c);
            }

            if (pipes.empty())
            {
                cout << "Wrong input!" << endl;
                return;
            }

            pipeMatrix.push_back(std::move(pipes));
        }
        checkConnections();
        // cout << Pipe::to_str(pipeMatrix.at(2).at(0)) << ": " << pipeMatrix.at(2).at(0).showConnections() << endl;
        // slide();
    }

    string const to_str() const
    {
        return boost::algorithm::join(pipeMatrix |
                boost::adaptors::transformed([](const vector<Pipe> &vec)
                    {
                        return boost::algorithm::join(vec |
                                    boost::adaptors::transformed(Pipe::to_str),
                                        "");
                    }), "\n");
    }

    friend std::ostream & operator<<(std::ostream &os, const Maze &maze)
    {
        return os << maze.to_str();
    }
};

std::string getExePath()
{
    char result[PATH_MAX];
    ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
    return std::string(result, (count > 0) ? count : 0);
}

int main()
{
    setlocale(LC_ALL, "");
    srand(time(0));
    
    // std::filesystem::path cwd = std::filesystem::current_path();
    std::filesystem::path cwd = getExePath();
    cout << "Hello World: " << cwd.filename() << endl;

    // string filename(cwd.parent_path()/"example_input"); // 8
    string filename(cwd.parent_path()/"input");
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

    Maze maze{};
    maze.parseInput(lines);
    cout << maze << endl;
    maze.slide();

    return EXIT_SUCCESS;
}
