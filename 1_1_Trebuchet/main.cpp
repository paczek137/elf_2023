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
#include <boost/range/adaptor/transformed.hpp>
#include <boost/range/adaptors.hpp>
#include <chrono>
#include <thread>
#include <compare>
#include <initializer_list>
#include <ctime>
#include <concepts>

using namespace std;



int main()
{
    setlocale(LC_ALL, "");
    srand(time(0));
    
    std::filesystem::path cwd = std::filesystem::current_path().filename();
    cout << "Hello World: " << cwd << endl;

    //string filename("example_input");
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
        std::vector<int> v{};
        for (auto const &c : line)
        {
            if (not isdigit(c))
            {
                continue;
            }
            //cout << "found: " << c << endl;
            v.push_back(static_cast<int>(c - '0'));
        }

        if (v.empty())
        {
            return EXIT_FAILURE;
        }

        long num = v.front() * 10 + v.back();
        cout << num << endl;
        sum += num;
        v_all.push_back(num);
    }

    cout << "sum: " << sum << endl;
    cout << "sum2: " << std::accumulate(v_all.cbegin(), v_all.cend(), 0) << endl;


    return EXIT_SUCCESS;
}
