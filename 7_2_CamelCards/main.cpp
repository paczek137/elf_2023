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

struct Card
{
    inline static const vector<char> LABELS = {'A', 'K', 'Q', 'J', 'T', '9', '8', '7', '6', '5', '4', '3', '2'};
    inline static const map<char, int> labelsToValueMap = { {'A', 14}, {'K', 13}, {'Q', 12}, {'J', 1}, {'T', 10} };
    char label;
    int value;

    Card() = default;
    Card(const char c) : label(c), value(labelToValue(c)) {};
    Card(const Card &card) = default;
    ~Card() = default;
    
    int labelToValue(const char c) const
    {
        return isalpha(c) ? labelsToValueMap.at(c) : static_cast<int>(c - '0');
    }

    string const to_str() const
    {
        return string(1, label);
    }

    friend std::ostream & operator<<(std::ostream &os, const Card &card)
    {
        return os << card.to_str();
    }

    friend bool operator<(const Card &l, const Card &r)
    {
        return l.value < r.value;
    }

    friend bool operator==(const Card &l, const Card &r)
    {
        return (l.label == r.label);
    }
};

struct Hand
{
    enum class HandType
    {
        HIGH_CARD = 0,
        ONE_PAIR,
        TWO_PAIRS,
        THREE_OF_A_KIND,
        FULL_HOUSE,
        FOUR_OF_A_KIND,
        FIVE_OF_A_KIND
    };

    vector<Card> cards;
    HandType handType;
    int idx;

    inline static int counter = 0;

    Hand() = default;
    Hand(const vector<Card> _cards) : cards(_cards), handType(getHandType(_cards)), idx(++counter) {};
    Hand(const Hand &hand) = default;
    ~Hand() = default;

    HandType getHandTypeWithJoker(const vector<Card> &cards)
    {
        if ((std::set<Card>(cards.cbegin(), cards.cend()).size() == 1) or
            (std::set<Card>(cards.cbegin(), cards.cend()).size() == 2))
        {
            return HandType::FIVE_OF_A_KIND;
        }
        if (std::set<Card>(cards.cbegin(), cards.cend()).size() == 3)
        {
            if ((std::count(cards.cbegin(), cards.cend(), Card(cards.at(0))) == 3) or
                (std::count(cards.cbegin(), cards.cend(), Card(cards.at(1))) == 3) or
                (std::count(cards.cbegin(), cards.cend(), Card(cards.at(2))) == 3) or
                (std::count(cards.cbegin(), cards.cend(), Card('J')) == 2))
            {
                return HandType::FOUR_OF_A_KIND;    
            }
            else return HandType::FULL_HOUSE;
        }
        if (std::set<Card>(cards.cbegin(), cards.cend()).size() == 4)
        {
            return HandType::THREE_OF_A_KIND;
        }
        else return HandType::ONE_PAIR;
    }

    HandType getHandType(const vector<Card> &cards)
    {
        if (std::any_of(cards.cbegin(), cards.cend(), [] (const Card &card) {return ('J' == card.label);}))
        {
            return getHandTypeWithJoker(cards);
        }

        if (std::set<Card>(cards.cbegin(), cards.cend()).size() == 1)
        {
            return HandType::FIVE_OF_A_KIND;
        }
        if (std::set<Card>(cards.cbegin(), cards.cend()).size() == 2)
        {
            if ((std::count(cards.cbegin(), cards.cend(), Card(cards.at(0))) == 1) or
                (std::count(cards.cbegin(), cards.cend(), Card(cards.at(0))) == 4))
            {
                return HandType::FOUR_OF_A_KIND;
            }
            else return HandType::FULL_HOUSE;
        }
        if (std::set<Card>(cards.cbegin(), cards.cend()).size() == 3)
        {
            if ((std::count(cards.cbegin(), cards.cend(), Card(cards.at(0))) == 3) or
                (std::count(cards.cbegin(), cards.cend(), Card(cards.at(1))) == 3) or
                (std::count(cards.cbegin(), cards.cend(), Card(cards.at(2))) == 3))
            {
                return HandType::THREE_OF_A_KIND;
            }
            else return HandType::TWO_PAIRS;

        }
        if (std::set<Card>(cards.cbegin(), cards.cend()).size() == 4)
        {
            return HandType::ONE_PAIR;
        }
        else return HandType::HIGH_CARD;
    }

    string const to_str() const
    {
        return boost::algorithm::join(cards | 
                                    boost::adaptors::transformed([](const Card &card)
                                        {
                                            return card.to_str();
                                        }), "") + " -> " + std::to_string(static_cast<int>(handType)) +
                                            " id: " + std::to_string(idx);
    }

    friend std::ostream & operator<<(std::ostream &os, const Hand &hand)
    {
        return os << hand.to_str();
    }

    friend bool operator<(const Hand &l, const Hand &r)
    {
        return (l.handType != r.handType) ? (l.handType < r.handType) : (std::tie(l.cards) < std::tie(r.cards));
    }

    friend bool operator==(const Hand &l, const Hand &r)
    {
        return (l.cards == r.cards);
    }
};

// int Hand::counter = 0;

using Hands = vector<Hand>;
using Bid = int;
using Bids = vector<Bid>;
using Rank = int;
using Ranks = vector<Rank>;

struct Camel
{
    Hands hands;
    Bids bids;
    Ranks ranks;

    Camel() = default;

    void calculateRanks()
    {
        std::set<Hand> handsSet = std::set<Hand>(hands.cbegin(), hands.cend());
        int newRank = 1;
        for (auto const &set : handsSet)
        {
            ranks.at(set.idx-1) = newRank++;
        }
    }

    long getTotalWinning()
    {
        long total = 0;
        for (size_t i=0; i<hands.size(); i++)
        {
            total += ranks.at(i) * bids.at(i);
        }

        return total;
    }

    string const to_str() const
    {
        string s{};
        for (size_t i=0; i<hands.size(); i++)
        {
            s += hands.at(i).to_str() +  " " + std::to_string(bids.at(i));
            if (not ranks.empty())
            {
                s += " rank: " + std::to_string(ranks.at(i)) + "\n";
            }
            else s += "\n";
        }
        return s;
    }

    friend std::ostream & operator<<(std::ostream &os, const Camel &camel)
    {
        return os << camel.to_str();
    }
};

void parseCamel(vector<string> &lines, Camel &camel)
{
    int rank = 1;
    for (auto const &line : lines)
    {
        vector<string> splittedCamel{};
        boost::split(splittedCamel, 
                    line,
                    boost::is_any_of(" "));
        if (splittedCamel.empty())
        {
            cout << "Not found entries for camel!" << endl;
            return;
        }    

        vector<Card> cards{};
        for (size_t i=0; i<splittedCamel.at(0).size(); i++)
        {
            // cout << splittedCamel.at(0).at(i) << " ";
            // hand.emplace_back(splittedCamel.at(0).at(i));
            cards.emplace_back(splittedCamel.at(0).at(i));
        }
        camel.hands.push_back(cards);
        camel.bids.push_back(stoi(splittedCamel.at(1)));
        camel.ranks.push_back(rank++);
    }
}

int main()
{
    setlocale(LC_ALL, "");
    srand(time(0));
    
    std::filesystem::path cwd = std::filesystem::current_path().filename();
    cout << "Hello World: " << cwd << endl;

    // string filename("example_input"); // 5905
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

    Camel camel{};
    parseCamel(lines, camel);
    camel.calculateRanks();
    cout << camel << endl;
    cout << "Total winning: " << camel.getTotalWinning() << endl;


    return EXIT_SUCCESS;
}
