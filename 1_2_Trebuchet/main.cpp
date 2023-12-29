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

bool checkForNumbers(const string &s, int &n)
{
    bool ret = false;
    if (s.find("one") != std::string::npos)
    {
        n = 1;
        ret = true;
    }
    else if (s.find("two") != std::string::npos)
    {
        n = 2;
        ret = true;
    }
    else if (s.find("three") != std::string::npos)
    {
        n = 3;
        ret = true;
    }
    else if (s.find("four") != std::string::npos)
    {
        n = 4;
        ret = true;
    }
    else if (s.find("five") != std::string::npos)
    {
        n = 5;
        ret = true;
    }
    else if (s.find("six") != std::string::npos)
    {
        n = 6;
        ret = true;
    }
    else if (s.find("seven") != std::string::npos)
    {
        n = 7;
        ret = true;
    }
    else if (s.find("eight") != std::string::npos)
    {
        n = 8;
        ret = true;
    }
    else if (s.find("nine") != std::string::npos)
    {
        n = 9;
        ret = true;
    }
    return ret;
}

void checkLine(const string &s, long &sum)
{
    string tmp{};
    int num{};
    vector<int> v{};
    for (auto const &c : s)
    {
        if (isdigit(c))
        {
           // cout << "found: " << c << endl;
           v.push_back(static_cast<int>(c - '0'));
           tmp = {};
           continue;
        }

        tmp += c;
        if (checkForNumbers(tmp, num))
        {
            // cout << "found: " << num << endl;
            v.push_back(num);
            tmp = {};
        }

    }

    if (v.empty())
    {
        cout << "Empty vector..." << endl;
        return;
    }

    sum = v.front() * 10 + v.back();
}

void replaceString(string &s, const string &find, const string &replace)
{
    if (s.find(find) == std::string::npos)
    {
        return;
    }
    boost::algorithm::replace_all(s, find, replace);
}

void checkLine2(const string &s, long &sum)
{
    string temp = s;
    replaceString(temp, "one", "one1one");
    replaceString(temp, "two", "two2two");
    replaceString(temp, "three", "three3three");
    replaceString(temp, "four", "four4four");
    replaceString(temp, "five", "five5five");
    replaceString(temp, "six", "six6six");
    replaceString(temp, "seven", "seven7seven");
    replaceString(temp, "eight", "eight8eight");
    replaceString(temp, "nine", "nine9nine");

    // cout << s << endl << temp << endl;

    vector<int> v{};
    for (auto const &c : temp)
    {
        if (isdigit(c))
        {
           // cout << "found: " << c << endl;
           v.push_back(static_cast<int>(c - '0'));
        }
    }

    if (v.empty())
    {
        cout << "Empty vector..." << endl;
        return;
    }

    sum = v.front() * 10 + v.back();

}

int main()
{
    setlocale(LC_ALL, "");
    srand(time(0));
    
    std::filesystem::path cwd = std::filesystem::current_path().filename();
    cout << "Hello World: " << cwd << endl;

    // string filename("example_input"); // 281
    string filename("input");
    ifstream input_file(filename);
    // 1148pdtcl1eight5oneights -> tricky one

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
        // checkLine(line, num);
        checkLine2(line, num); // 54208

        cout << num << endl;
        sum += num;
        v_all.push_back(num);
    }

    cout << "sum: " << sum << endl;
    cout << "sum2: " << std::accumulate(v_all.cbegin(), v_all.cend(), 0) << endl;


    return EXIT_SUCCESS;
}
