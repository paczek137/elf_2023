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
#include <codecvt>


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

    friend PipeDirection const operator!(const PipeDirection &pipeDirection)
    {
        if (PipeDirection::LEFT == pipeDirection) return PipeDirection::RIGHT;
        if (PipeDirection::RIGHT == pipeDirection) return PipeDirection::LEFT;
        if (PipeDirection::TOP == pipeDirection) return PipeDirection::BOTTOM;
        if (PipeDirection::BOTTOM == pipeDirection) return PipeDirection::TOP;
        return pipeDirection;
    }

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
    inline static long counter = 0;

    char symbol;
    char32_t symbolUnicode;
    PipeSymbol pipeSymbol;
    long steps;
    PipeDirection direction;
    bool isMainPipe;
    long id;
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
    Pipe(const char c) : symbol(c), symbolUnicode(Pipe::getUnicode(c)), pipeSymbol(Pipe::getPipeSymbol(c)),
                            steps(0), direction(PipeDirection::UNKNOWN), isMainPipe(false), id(counter++),
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

    void markInner()
    {
        symbolUnicode = U'1';
    }

    void markBold()
    {
        if ('|' == symbol) symbolUnicode = U'\u2503';
        if ('-' == symbol) symbolUnicode = U'\u2501';
        if ('L' == symbol) symbolUnicode = U'\u2517';
        if ('J' == symbol) symbolUnicode = U'\u251B';
        if ('7' == symbol) symbolUnicode = U'\u2513';
        if ('F' == symbol) symbolUnicode = U'\u250F';
        // if ('.' == symbol) symbolUnicode = U'.';
        // if ('S' == symbol) symbolUnicode = U'S';
    }

    void markNewStart()
    {
        bool l{}, r{}, t{}, b{};
        if ((nullptr != left) and ((left == from) or (left == to))) l = true;
        if ((nullptr != right) and ((right == from) or (right == to))) r = true;
        if ((nullptr != top) and ((top == from) or (top == to))) t = true;
        if ((nullptr != bottom) and ((bottom == from) or (bottom == to))) b = true;

        if (t and b) symbol = '|';
        if (l and r) symbol = '-';
        if (t and r) symbol = 'L';
        if (t and l) symbol = 'J';
        if (b and l) symbol = '7';
        if (b and r) symbol = 'F';

        symbolUnicode = Pipe::getUnicode(symbol);
        pipeSymbol = Pipe::getPipeSymbol(symbol);
        this->markBold();
    }

    static char32_t getUnicode(const char c)
    {
        if ('|' == c) return U'\u2502';
        if ('-' == c) return U'\u2500';
        if ('L' == c) return U'\u2514';
        if ('J' == c) return U'\u2518';
        if ('7' == c) return U'\u2510';
        if ('F' == c) return U'\u250C';
        if ('.' == c) return U'\u2022';
        if ('S' == c) return U'S';
        else
        {
            cout << "Wrong symbol!" << endl;
            return U'.';
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
            this->markBold();
            isMainPipe = true;
            return destinationPipe;
        }

        cout << "Wrong pipe end!!" << endl;
        return this;
    }

    Pipe *getNextPipeFromStart()
    {
        if ((left != nullptr) and (Pipe::pipesLeftList.cend() != std::find(Pipe::pipesLeftList.cbegin(), Pipe::pipesLeftList.cend(), left->pipeSymbol)))
        {
            cout << "left" << endl;
            to = left;
            left->from = this;
            direction = PipeDirection::LEFT;
            isMainPipe = true;
            return left;
        }
        if ((right != nullptr) and (Pipe::pipesRightList.cend() != std::find(Pipe::pipesRightList.cbegin(), Pipe::pipesRightList.cend(), (*right).pipeSymbol)))
        {
            cout << "right" << endl;
            to = right;
            (*right).from = this;
            direction = PipeDirection::RIGHT;
            isMainPipe = true;
            return right;
        }
        if ((top != nullptr) and (Pipe::pipesTopList.cend() != std::find(Pipe::pipesTopList.cbegin(), Pipe::pipesTopList.cend(), (*top).pipeSymbol)))
        {
            cout << "top" << endl;
            to = top;
            (*top).from = this;
            direction = PipeDirection::TOP;
            isMainPipe = true;
            return top;
        }
        if ((bottom != nullptr) and (Pipe::pipesBottomList.cend() != std::find(Pipe::pipesBottomList.cbegin(), Pipe::pipesBottomList.cend(), (*bottom).pipeSymbol)))
        {
            cout << "bottom" << endl;
            to = bottom;
            (*bottom).from = this;
            direction = PipeDirection::BOTTOM;
            isMainPipe = true;
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
                return getNextPipeFromStart();
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

    Pipe *getFirstTileOnLeft()
    {
        if (left != nullptr) return left->getFirstTileOnLeft();
        else return this;
    }

    Pipe *getNextTile()
    {
        if (nullptr != right) return right;
        if (nullptr != bottom) return bottom->getFirstTileOnLeft();
        else return this; 
    }

    Pipe *getNextTileInDirection(const PipeDirection dir)
    {
        if (PipeDirection::LEFT == dir) return this->left;
        if (PipeDirection::RIGHT == dir) return this->right;
        if (PipeDirection::TOP == dir) return this->top;
        if (PipeDirection::BOTTOM == dir) return this->bottom;
        return this;
    }

    bool const isCommonDirection(Pipe *pipe) const
    {
        return (this->direction == pipe->direction) and (this != pipe);
    }

    bool const isDifferentDirection(const PipeDirection dir) const
    {
        return (this->direction != dir) and (this->direction != not dir);
    }

    vector<Pipe::PipeSymbol> const &getPipesLisInDirection(const PipeDirection dir) const
    {
        if (PipeDirection::LEFT == dir) return Pipe::pipesLeftList;
        if (PipeDirection::RIGHT == dir) return Pipe::pipesRightList;
        if (PipeDirection::TOP == dir) return Pipe::pipesTopList;
        if (PipeDirection::BOTTOM == dir) return Pipe::pipesBottomList;
        return Pipe::pipesStartList;
    }

    long getCrossesParityInDirection(const PipeDirection dir)
    {
        Pipe *tile = this;
        Pipe *lastPipe = nullptr;
        long crosses{};
        bool alongPipe = false;
        PipeSymbol symbolForCounting{};
        vector<Pipe::PipeSymbol> pipesList = getPipesLisInDirection(not dir);

        if ((PipeDirection::LEFT == dir) or (PipeDirection::RIGHT == dir)) symbolForCounting = PipeSymbol::NORTH_TO_SOUTH;
        if ((PipeDirection::TOP == dir) or (PipeDirection::BOTTOM == dir)) symbolForCounting = PipeSymbol::WEST_TO_EAST;

        while (nullptr != tile->getNextTileInDirection(dir))
        {
            tile = tile->getNextTileInDirection(dir);
            if (not tile->isMainPipe) continue;
            if (symbolForCounting == tile->pipeSymbol)
            {
                crosses++;
                alongPipe = false;
                continue;
            }
            if (pipesList.cend() != std::find(pipesList.cbegin(), pipesList.cend(), tile->pipeSymbol))
            {
                alongPipe = true;
                if (nullptr == lastPipe) lastPipe = tile;
                continue;
            }
            else if (((lastPipe->from->isCommonDirection(tile) and lastPipe->from->isDifferentDirection(dir)) or
                (lastPipe->isCommonDirection(tile) and lastPipe->isDifferentDirection(dir)) or
                (lastPipe->isCommonDirection(tile->from) and lastPipe->isDifferentDirection(dir))))
            // else if ((((lastPipe->from->direction == tile->direction) and (lastPipe->from != tile) and (tile->direction != dir) and (tile->direction != not dir)) or
            //     ((lastPipe->direction == tile->direction) and (lastPipe != tile) and (tile->direction != dir) and (tile->direction != not dir)) or
            //     ((lastPipe->direction == tile->from->direction) and (lastPipe != tile->from) and (tile->from->direction != dir) and (tile->from->direction != not dir))))
            {
                // cout << tile->id << endl;
                crosses++;
            }
            alongPipe = false;
            lastPipe = nullptr;
        }
        return crosses;
    }

    bool getCrossesParity()
    {
        cout << getCrossesParityInDirection(PipeDirection::LEFT) << " crosses on LEFT" << endl;
        cout << getCrossesParityInDirection(PipeDirection::RIGHT) << " crosses on RIGHT" << endl;
        cout << getCrossesParityInDirection(PipeDirection::TOP) << " crosses on TOP" << endl;
        cout << getCrossesParityInDirection(PipeDirection::BOTTOM) << " crosses on BOTTOM" << endl;
        return static_cast<bool>(getCrossesParityInDirection(PipeDirection::LEFT) % 2 == 0) and
                static_cast<bool>(getCrossesParityInDirection(PipeDirection::RIGHT) % 2 == 0) and
                static_cast<bool>(getCrossesParityInDirection(PipeDirection::TOP) % 2 == 0) and
                static_cast<bool>(getCrossesParityInDirection(PipeDirection::BOTTOM) % 2 == 0);
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

    static wstring const to_str_unicode(const Pipe &pipe)
    {
        return wstring(1, pipe.symbolUnicode);
    }

    friend std::ostream & operator<<(std::ostream &os, const Pipe &pipe)
    {
        return os << Pipe::to_str(pipe);
    }

    friend bool operator==(const Pipe &l, const Pipe &r)
    {
        return (l.id == r.id);
    }
};

// Pipe::PipeDirection operator!(const Pipe::PipeDirection &pipeDirection)
// {
//     if (Pipe::PipeDirection::LEFT == pipeDirection) return Pipe::PipeDirection::RIGHT;
//     if (Pipe::PipeDirection::RIGHT == pipeDirection) return Pipe::PipeDirection::LEFT;
//     if (Pipe::PipeDirection::TOP == pipeDirection) return Pipe::PipeDirection::BOTTOM;
//     if (Pipe::PipeDirection::BOTTOM == pipeDirection) return Pipe::PipeDirection::TOP;
//     return pipeDirection;
// }

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
        pipe->markNewStart();
        cout << "Distance: " << distance/2 << endl;
    }

    void markAllInnerTiles()
    {
        Pipe *tile = &pipeMatrix.at(0).at(0);
        // Pipe *tile = &pipeMatrix.at(2).at(3); // outer
        // Pipe *tile = &pipeMatrix.at(3).at(14); // inner
        Pipe *lastTile = &pipeMatrix.at(pipeMatrix.size()-1).at(pipeMatrix.at(0).size()-1);
        long innerTiles{};

        if (not tile->isMainPipe)
        {
            if (not tile->getCrossesParity())
            {
                tile->markInner();
                innerTiles++;    
            }
        }

        while (true)
        {
            tile = tile->getNextTile();

            if (not tile->isMainPipe)
            {
                if (not tile->getCrossesParity())
                {
                    tile->markInner();
                    innerTiles++;    
                }
            }

            if (tile == lastTile) break;
        }
        cout << "innerTiles: " << innerTiles << endl;
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

    wstring const to_str_unicode() const
    {
        return boost::algorithm::join(pipeMatrix |
                boost::adaptors::transformed([](const vector<Pipe> &vec)
                    {
                        return boost::algorithm::join(vec |
                                    boost::adaptors::transformed(Pipe::to_str_unicode),
                                        "");
                    }), "\n");
    }

    friend std::ostream & operator<<(std::ostream &os, const Maze &maze)
    {
        return os << maze.to_str();
    }
};

std::string getStringFromWstring(const wstring &wstr)
{
    static wstring_convert<codecvt_utf8<wchar_t>> cvu32{};
    return cvu32.to_bytes(wstr);
}

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

    // string filename(cwd.parent_path()/"example_input"); // 10
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

    // wstring w = L"Krążek";
    // std::wcout << w << std::endl;
    // cout << "Krążek" << endl;
    // cout << "日曜日" << endl;
    // cout << getStringFromWstring(w) << endl;
    // cout << getStringFromWstring(Pipe::to_str_unicode(maze.pipeMatrix.at(0).at(2))) << endl;
    cout << getStringFromWstring(maze.to_str_unicode()) << endl;
    // cout << maze << endl;
    maze.slide();
    cout << getStringFromWstring(maze.to_str_unicode()) << endl;
    maze.markAllInnerTiles();
    cout << endl << getStringFromWstring(maze.to_str_unicode()) << endl;

    // Pipe::PipeDirection pp = Pipe::PipeDirection::TOP;
    // cout << static_cast<int>(not pp) << endl;

    return EXIT_SUCCESS;
}
