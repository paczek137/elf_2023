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

template<typename Char, typename Traits, typename Allocator>
std::basic_string<Char, Traits, Allocator> operator *
(const std::basic_string<Char, Traits, Allocator> s, size_t n)
{
    // cout << "trick: " << n << endl;
    std::basic_string<Char, Traits, Allocator> tmp{};
    for (size_t i = 0; i < n; ++i)
    {
        tmp += s;
    }
    return tmp;
}

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

struct Pixel
{
    enum class PixelSymbol
    {
        EMPTY_SPACE,
        GALAXY
    };

    int x;
    int y;
    char c;
    PixelSymbol symbol;
    size_t scaleX = 1;
    size_t scaleY = 1;

    PixelSymbol const getSymbol(const char c) const
    {
        if ('#' == c) return PixelSymbol::GALAXY;
        else return PixelSymbol::EMPTY_SPACE;
    }

    Pixel(const char _c, const int _x, const int _y) : x(_x), y(_y), c(_c), symbol(getSymbol(_c)) {}

    size_t const sizeX() const {return static_cast<const size_t>(this->scaleX);}
    size_t const sizeY() const {return static_cast<const size_t>(this->scaleY);}

    void setSizeX(const size_t _scale) {this->scaleX = _scale;}
    void setSizeY(const size_t _scale) {this->scaleY = _scale;}

    string const to_str() const
    {
        return string(1, this->c);
    }

    friend std::ostream & operator<<(std::ostream &os, const Pixel &pixel)
    {
        return os << pixel.to_str();
    }

    friend bool operator==(const Pixel &l, const Pixel &r)
    {
        return (l.x == r.x) and (l.y == r.y);
    }

    friend bool operator<(const Pixel &l, const Pixel &r)
    {
        return tie(l.x, l.x) < tie(r.y, r.y);
    }

};

struct Image
{
    static const inline size_t GRAVITAIONAL_EFFECT_VALUE = 1000000;
    // static const inline size_t GRAVITAIONAL_EFFECT_VALUE = 100;
    std::vector<Pixel> pixels;
    size_t pixelsInRow;
    size_t pixelsInColumn;
    // std::vector<std::pair<Pixel, Pixel>> pairsOfGalaxies;
    std::vector<std::pair<std::reference_wrapper<const Pixel>, std::reference_wrapper<const Pixel>>> pairsOfGalaxies;

    void parseInput(const vector<string> &lines)
    {
        for (size_t y=0; y<lines.size(); y++)
        {
            for (size_t x=0; x<lines.at(y).size(); x++)
            {
                pixels.emplace_back(lines.at(y).at(x), x, y);
            }
        }
        pixelsInRow = lines.at(0).size();
        pixelsInColumn = lines.size();
        cout << "Pixels in row: " << pixelsInRow << endl;
        cout << "Pixels in column: " << pixelsInColumn << endl;
    }

    void markGravitationalEffectInRow(const size_t row, const size_t scale)
    {
        // cout << "marking row: " << row << endl;
        for (size_t x=0; x<pixelsInRow; x++)
        {
            pixels.at(row * pixelsInRow + x).setSizeX(scale);
        }
    }

    void markGravitationalEffectInColumn(const size_t column, const size_t scale)
    {
        // cout << "marking column: " << row << endl;
        for (size_t y=0; y<pixelsInColumn; y++)
        {
            pixels.at(y * pixelsInRow + column).setSizeY(scale);
        }
    }

    void checkGravitationalEffects()
    {
        for (size_t y=0; y<pixelsInColumn; y++)
        {
            if (std::all_of(pixels.cbegin() + (y*pixelsInRow), pixels.cbegin() + (y*pixelsInRow) + pixelsInRow, 
                [](const Pixel &pixel){return Pixel::PixelSymbol::EMPTY_SPACE == pixel.symbol;}))
            {
                cout << "found gravitational effect at " << y << " row" << endl;
                this->markGravitationalEffectInRow(y, GRAVITAIONAL_EFFECT_VALUE);
            }
        }

        for (size_t x=0; x<pixelsInRow; x++)
        {
            size_t cnt{};
            for (size_t y=0; y<pixelsInColumn; y++)
            {
                if (Pixel::PixelSymbol::EMPTY_SPACE != pixels.at(y * pixelsInRow + x).symbol)
                {
                    break;
                }
                cnt++;
            }
            if (pixelsInColumn == cnt)
            {
                cout << "found gravitational effect at " << x << " column" << endl;
                this->markGravitationalEffectInColumn(x, GRAVITAIONAL_EFFECT_VALUE);
            }
        }
    }

    void findPairsOfGalaxies()
    {
        std::vector<Pixel> galaxies;
        for (auto const &pixel : pixels)
        {
            if (Pixel::PixelSymbol::GALAXY == pixel.symbol)
            {
                galaxies.push_back(pixel);
            }
        }
        cout << "found " << galaxies.size() << " galaxies" << endl;

        for (size_t i=0; i<galaxies.size(); i++)
        {
            for (size_t ii=i+1; ii<galaxies.size(); ii++)
            {
                // pairsOfGalaxies.push_back(std::make_pair(galaxies.at(i), galaxies.at(ii)));
                pairsOfGalaxies.emplace_back(galaxies.at(i), galaxies.at(ii));
            }
        }
        cout << "found " << pairsOfGalaxies.size() << " pairs of galaxies" << endl;
    }

    Pixel & getPixel(const int x, const int y)
    {
        return pixels.at(y * pixelsInColumn + x);
    }

    size_t getPathLengthOfPixels(const Pixel &p1, const Pixel &p2)
    {
        size_t length{};
        ssize_t xDiff = p2.x - p1.x;
        ssize_t yDiff = p2.y - p1.y;

        for (size_t i=1; i<=abs(xDiff); i++)
        {
            if (xDiff > 0)
            {
                length += this->getPixel(p1.x+i, p1.y).sizeY();
            }
            else
            {
                length += this->getPixel(p1.x-i, p1.y).sizeY();
            }
        }

        for (size_t i=1; i<=abs(yDiff); i++)
        {
            if (yDiff > 0)
            {
                length += this->getPixel(p1.x, p1.y+i).sizeX();
            }
            else
            {
                length += this->getPixel(p1.x, p1.y-i).sizeX();
            }
        }
        // cout << "+ " << length << endl;
        // cout << "pair x1: " << p1.x << ", y1: " << p1.y << " x2: " << p2.x << ", y2: " << p2.y << 
        //         ", xDiff: " << xDiff << ", yDiff: " << yDiff << ", length: " << length << endl;
        return length;
    }

    size_t getPathLengthOfPixels(const std::pair<Pixel, Pixel> &pair)
    {
        return getPathLengthOfPixels(std::get<0>(pair), std::get<1>(pair));
    }

    void countLengthOfPaths()
    {
        size_t length = std::accumulate(pairsOfGalaxies.cbegin(), pairsOfGalaxies.cend(), static_cast<size_t>(0), // WOW!!!
                        [&](size_t length, const std::pair<Pixel, Pixel> &pair) -> size_t
                        {
                            // cout << "l: " << length << ", sizeof: " << sizeof(length) << ", is signed: " << std::is_signed<decltype(length)>::value << endl;
                            return length + getPathLengthOfPixels(std::get<0>(pair), std::get<1>(pair));
                        });
        cout << "total length: " << length << endl;
        Pixel p = pixels.at(5);
        // cout << "Xcale of x: " << p.x << ": " << p.sizeY() << endl;
    }

    static string const to_str(const Image &image)
    {
        return boost::algorithm::join(image.pixels |
                boost::adaptors::transformed([&](const Pixel &pixel)
                    {
                        return pixel.to_str() + ((pixel.x < image.pixelsInRow-1) ? "" : "\n");
                        // return Pixel::to_str(pixel) + ((pixel.x < image.pixelsInRow-1) ? "" : "\n");
                    }), "");
    }

    static string const to_str2(const Image &image)
    {
        string s{};
        for (size_t y=0; y<image.pixelsInColumn; y++)
        {
            string row{};
            for (size_t x=0; x<image.pixelsInRow; x++)
            {
                row += image.pixels.at(y * image.pixelsInRow + x).to_str() *
                        image.pixels.at(y * image.pixelsInRow + x).sizeY();
            }
            s += (row + "\n") * image.pixels.at(y * image.pixelsInRow).sizeX();
            // row += "\n";
            // s += row * image.pixels.at(y * image.pixelsInRow).sizeX();
        }
        return s;
    }

    friend std::ostream & operator<<(std::ostream &os, const Image &image)
    {
        return os << Image::to_str2(image);
    }
};

int main()
{
    setlocale(LC_ALL, "");
    srand(time(0));
    
    // std::filesystem::path cwd = std::filesystem::current_path();
    std::filesystem::path cwd = getExePath();
    cout << "Hello World: " << cwd.filename() << endl;

    // string filename(cwd.parent_path()/"example_input"); // 82000210
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

    Image image{};
    image.parseInput(lines);
    cout << image << endl;
    image.checkGravitationalEffects();
    // cout << image << endl;
    image.findPairsOfGalaxies();
    image.countLengthOfPaths();

    // cout << "sizeof(size_t): " << sizeof(size_t) << endl;
    // cout << "sizeof(long): " << sizeof(long) << endl;

    // size_t v{};
    // v = 2136032315;
    // cout << v << endl;
    // v += 14000216;
    // cout << v << endl;
    // cout << "max of size_t: " << std::numeric_limits<size_t>::max() << endl;

    return EXIT_SUCCESS;
}
