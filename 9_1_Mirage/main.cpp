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

using MeasuredData = vector<long>;
using MeasuredDataHistory = vector<MeasuredData>;

struct Oasis
{
    vector<MeasuredDataHistory> data;

    Oasis() = default;
    ~Oasis() = default;

    void parseData()
    {
        for (auto &measuredDataHistory : data)
        {
            while (true)
            {
                if (std::all_of(measuredDataHistory.at(measuredDataHistory.size() - 1).cbegin(), 
                                measuredDataHistory.at(measuredDataHistory.size() - 1).cend(),
                                [] (const long &val) {return (0 == val);}))
                {
                    break;
                }

                MeasuredData measuredData{};
                for (size_t i=0; i<measuredDataHistory.at(measuredDataHistory.size() - 1).size() - 1; i++)
                {
                    measuredData.push_back(
                        measuredDataHistory.at(measuredDataHistory.size() - 1).at(i+1) -
                         measuredDataHistory.at(measuredDataHistory.size() - 1).at(i));
                    // cout << measuredDataHistory.at(measuredDataHistory.size() - 1).at(i+1) - measuredDataHistory.at(measuredDataHistory.size() - 1).at(i) << endl;
                }
                measuredDataHistory.push_back(measuredData);
            }
        }
    }

    void extrapolateData()
    {
        for (auto &measuredDataHistory : data)
        {
            for (int i=measuredDataHistory.size()-1; i>=0; i--)
            {
                if ((measuredDataHistory.size()-1) == i)
                {
                    measuredDataHistory.at(i).push_back(0);
                    continue;
                }
                measuredDataHistory.at(i).push_back(measuredDataHistory.at(i).at(measuredDataHistory.at(i).size()-1) + 
                                                    measuredDataHistory.at(i+1).at(measuredDataHistory.at(i+1).size()-1));
            }
        }
    }

    long getSumOfLastExtrapolateValues()
    {
        return std::accumulate(data.cbegin(), data.cend(), 0, [] (long sum, const MeasuredDataHistory &measuredDataHistory)
        {
            return sum + measuredDataHistory.at(0).at(measuredDataHistory.at(0).size()-1);
        });

    }

    string const to_str_complete() const
    {
        return boost::algorithm::join(data |
            boost::adaptors::transformed([](const MeasuredDataHistory &measuredDataHistory)
                {
                    return boost::algorithm::join(measuredDataHistory |
                        boost::adaptors::transformed([](const MeasuredData &measuredData)
                        {
                            return boost::algorithm::join(measuredData |
                                        boost::adaptors::transformed(static_cast<std::string(*)(long)>(std::to_string)),
                                            " ");
                        }), "\n");
                }), "\n\n");
    }

    string const to_str() const
    {
        return boost::algorithm::join(data |
                boost::adaptors::transformed([](const MeasuredDataHistory &measuredDataHistory)
                    {
                        return boost::algorithm::join(measuredDataHistory.at(0) |
                                    boost::adaptors::transformed(static_cast<std::string(*)(long)>(std::to_string)),
                                        " ");
                    }), "\n");
        // return boost::algorithm::join(data.at(0) |
        //                                 boost::adaptors::transformed(static_cast<std::string(*)(long)>(std::to_string)),
        //                                 " ") + "\n";
    }

    friend std::ostream & operator<<(std::ostream &os, const Oasis &oasis)
    {
        return os << oasis.to_str_complete();
    } 
};

void parseOasis(const vector<string> &lines, Oasis &oasis)
{
    // std::regex reg("\\D+"); // inversed logic - anything that is NOT a digit
    std::regex reg(" ");

    for (const auto &line : lines)
    {
        vector<string> result{};
        std::copy(std::sregex_token_iterator(line.cbegin(), line.cend(), reg, -1),
                    std::sregex_token_iterator(),
                    std::back_inserter(result));

        if (result.empty())
        {
            cout << "Wrong input!" << endl;
            return;
        }

        MeasuredData measuredData{};
        for (const auto &el : result)
        {
            // cout << el << endl;
            measuredData.push_back(std::stol(el));
        }
        MeasuredDataHistory measuredDataHistory{};
        measuredDataHistory.push_back(measuredData);
        oasis.data.push_back(measuredDataHistory);
    }
}

int main()
{
    setlocale(LC_ALL, "");
    srand(time(0));
    
    std::filesystem::path cwd = std::filesystem::current_path().filename();
    cout << "Hello World: " << cwd << endl;

    // string filename("example_input"); // 114
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

    Oasis oasis{};
    parseOasis(lines, oasis);
    cout << oasis << endl;
    oasis.parseData();
    cout << endl << "After parsing" << endl;
    cout << oasis << endl;
    oasis.extrapolateData();
    cout << endl << "After extrapolate" << endl;
    cout << oasis << endl;
    cout << "getSumOfLastExtrapolateValues: " << oasis.getSumOfLastExtrapolateValues() << endl;


    return EXIT_SUCCESS;
}
