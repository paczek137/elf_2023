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

using namespace std;

struct Card
{
    int id;
    vector<int> winningNumbers;
    vector<int> toBeCheckedNumbers;
    int points;
};

void parseCards(const vector<string> &lines, vector<Card> &cards)
{
    std::regex reg{};

    for (size_t i=0; i<lines.size(); i++)
    {
        Card card{};
        std::smatch match{};
        reg = "[0-9]+:";

        auto searchStart(lines.at(i).cbegin());
        while (regex_search(searchStart, lines.at(i).cend(), match, reg))
        {
            card.id = stoi(match[0]);
            cout << card.id << endl;
            searchStart = match.suffix().first;
        }

        // cout << string(searchStart, lines.at(i).cend()) << endl;
        vector<string> allSplittedNumbers{};
        boost::split(allSplittedNumbers, string(searchStart, lines.at(i).cend()), boost::is_any_of("|"));
        if (2 != allSplittedNumbers.size())
        {
            cout << "something went wrong" << endl;
            return;
        }

        vector<string> numbersString{};
        boost::split(numbersString, allSplittedNumbers.at(0), boost::is_any_of(" "));
        for (const auto &s : numbersString)
        {
            if (s.empty())
            {
                continue;
            }
            cout << std::stoi(s) << " ";
            card.winningNumbers.push_back(std::stoi(s));
        }
        cout << endl;

        numbersString.clear();
        boost::split(numbersString, allSplittedNumbers.at(1), boost::is_any_of(" "));
        for (const auto &s : numbersString)
        {
            if (s.empty())
            {
                continue;
            }
            cout << std::stoi(s) << " ";
            card.toBeCheckedNumbers.push_back(std::stoi(s));
        }
        cout << endl;

        std::sort(card.winningNumbers.begin(), card.winningNumbers.end());
        std::sort(card.toBeCheckedNumbers.begin(), card.toBeCheckedNumbers.end());
        std::vector<int> v{};
        std::set_intersection(card.toBeCheckedNumbers.cbegin(), card.toBeCheckedNumbers.cend(),
                                card.winningNumbers.cbegin(), card.winningNumbers.cend(),
                                std::back_inserter(v));
        if (not v.empty())
        {
            card.points = pow(2, v.size() - 1);    
        }
        cout << "points: " << card.points << endl;

        cards.push_back(card);
        

        // boost::regex expr{"[0-9]+"};
        // boost::copy(boost::adaptors::tokenize(splittedNumbersString.at(0), expr, 0,
        // boost::regex_constants::match_default),
        // std::ostream_iterator<std::string>{std::cout, ","});
        // linking issue with boost regex...

        // reg = "[0-9]+";
        // for (const auto &token : splittedNumbersString.at(0) 
        //                 | boost::adaptors::tokenized(boost::regex("[0-9]+"), 0))
        // {
        //     cout << token << endl;
        // }

        // auto view = splittedNumbersString.at(0)
        //             | ranges::views::split(' ')
        //             | ranges::views::transform([](const string &s)
        //             {
        //                 return stoi(s);
        //             });

        // auto view = s
        //             | ranges::views::split(' ')
        //             | ranges::views::transform([](auto &&rng) {
        //                     return std::string_view(&*rng.begin(), ranges::distance(rng));
        //         });

        // std::ranges::copy(line | std::views::filter([](unsigned char c) {
        //                 return std::isalpha(c);
        //               }) | std::views::transform([](unsigned char c) {
        //                 return static_cast<char>(c);
        //               }),
        //               std::back_inserter(items));
    }
}

int main()
{
    setlocale(LC_ALL, "");
    srand(time(0));
    
    std::filesystem::path cwd = std::filesystem::current_path().filename();
    cout << "Hello World: " << cwd << endl;

    // string filename("example_input"); // 13
    string filename("input");
    ifstream input_file(filename);

    string line{};
 
    if (not input_file.is_open())
    {
        cout << "Could not open the file: " << filename << endl;
        return EXIT_FAILURE;
    }

    vector<string> lines{};
    vector<Card> cards{};

    while (getline(input_file, line, '\n')) 
    {
        lines.push_back(line);
    }

    parseCards(lines, cards);

    cout << "sum: " << std::accumulate(cards.cbegin(), cards.cend(), 0, [] (int sum, const Card &card)
    {
        return sum + card.points;
    }) << endl;

    return EXIT_SUCCESS;
}
