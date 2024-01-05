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
   std::basic_string<Char, Traits, Allocator> tmp = s;
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

    static string const to_str(const Pixel &pixel)
    {
        return string(1, pixel.c);
    }

    friend std::ostream & operator<<(std::ostream &os, const Pixel &pixel)
    {
        return os << Pixel::to_str(pixel);
    }

    friend bool operator==(const Pixel &l, const Pixel &r)
    {
        return (l.x == r.x) and (l.y == r.y);
    }

};

struct Image
{
    std::vector<Pixel> pixels;
    size_t pixelsInRow;
    size_t pixelsInColumn;

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
        for (size_t x=0; x<pixelsInRow; x++)
        {
            pixels.at(row * pixelsInRow + x).setSizeX(scale);
        }
    }

    void checkGravitationalEffects()
    {
        // for (size_t x=0; x<pixelsInRow; x++)
        // {
        //     for (size_t y=0; y<pixelsInColumn; y++)
        //     {
        //         if ( == pixels.at(y).at(x).symbol)
        //     }
        // }
        // std::all_of(pixels.cbegin())

        for (size_t y=0; y<pixelsInColumn; y++)
        {
            if (std::all_of(pixels.cbegin() + (y*pixelsInRow), pixels.cbegin() + (y*pixelsInRow) + pixelsInRow, 
                [](const Pixel &pixel){return Pixel::PixelSymbol::EMPTY_SPACE == pixel.symbol;}))
            {
                cout << "found gravitational effect at " << y << " row" << endl;
                this->markGravitationalEffectInRow(y, 2);
            }
        }

        for (size_t x=0; x<pixelsInRow; x++)
        {

        }
    }

    static string const to_str(const Image &image)
    {
        return boost::algorithm::join(image.pixels |
                boost::adaptors::transformed([&](const Pixel &pixel)
                    {
                        return Pixel::to_str(pixel) + ((pixel.x < image.pixelsInRow-1) ? "" : "\n");
                    }), "");
    }

    static string const to_str2(const Image &image)
    {
        // string s{};
        // for (size_t y=0; y<image.pixelsInColumn; y++)
        // {
        //     string scaleX{};
        //     for (size_t x=0; x<image.pixelsInRow; x++)
        //     {
        //         if (image.pixels.at(y * image.pixelsInRow + x).sizeX() != 1)
        //         {
        //             scaleX += image.pixels.at(y * image.pixelsInRow + x).to_str();
        //         }
        //         s += image.pixels.at(y * image.pixelsInRow + x).to_str();
        //     }
        // }
    }

    friend std::ostream & operator<<(std::ostream &os, const Image &image)
    {
        return os << Image::to_str(image);
    }
};

int main()
{
    setlocale(LC_ALL, "");
    srand(time(0));
    
    // std::filesystem::path cwd = std::filesystem::current_path();
    std::filesystem::path cwd = getExePath();
    cout << "Hello World: " << cwd.filename() << endl;

    string filename(cwd.parent_path()/"example_input"); // 374
    // string filename(cwd.parent_path()/"input");
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

    return EXIT_SUCCESS;
}
