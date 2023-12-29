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

struct Node
{
    string name;
    string left;
    string right;

    Node() = default;
    ~Node() = default;
    Node(const Node &node) = default;
    Node(const string &_name, const string &_left, const string &_right) : 
            name(_name), left(_left), right(_right) {};

    string const to_str() const
    {
        return name + " = (" + left + ", " + right + ")";
    }

    friend std::ostream & operator<<(std::ostream &os, const Node &node)
    {
        return os << node.to_str();
    }

    friend bool operator==(const Node &l, const Node &r)
    {
        return (l.name == r.name) && (l.left == r.left) && (l.right == r.right);
    }
};

struct Network
{
    enum class NetworkInstruction
    {
        LEFT,
        RIGHT
    };

    vector<NetworkInstruction> networkInstructions;
    vector<Node> nodes;

    Network() = default;
    ~Network() = default;

    Node getNodeByName(const string &name) const
    {
        if (auto it = std::find_if(nodes.cbegin(), nodes.cend(), 
            [&name] (const Node &node) {return (node.name == name);}); it != nodes.cend())
        {
            return *it;
        }
        else
        {
            cout << "Not found Node by name: " << name << "!" << endl;
            return *(nodes.cbegin());
        }
    }

    void traverseNetwork()
    {
        string entryNode = "AAA";
        string destinationNode = "ZZZ";
        long steps = 0;
        Node currentNode = getNodeByName(entryNode);

        cout << "instructions size: " << networkInstructions.size() << endl;

        for (size_t i=0 ; ;)
        {
            if (networkInstructions.size() == i)
            {
                cout << "Reset" << endl;
                i = 0;
            }

            if (networkInstructions.at(i) == NetworkInstruction::LEFT)
            {
                currentNode = getNodeByName(currentNode.left);
                cout << "going left to: " << currentNode << endl;
            }
            else if (networkInstructions.at(i) == NetworkInstruction::RIGHT)
            {
                currentNode = getNodeByName(currentNode.right);
                cout << "going right to: " << currentNode << endl;
            }

            steps++;

            if (currentNode.name == destinationNode)
            {
                break;
            }
            i++;
        }

        cout << "Traversed " << steps << endl;
    }

    string const to_str_network_instruction(const NetworkInstruction &ins) const
    {
        if (NetworkInstruction::LEFT == ins)
        {
            return string(1, 'L');
        }
        else return string(1, 'R');
    }

    string const to_str() const
    {
        string s;
        for (const auto &el : networkInstructions)
        {
            s += to_str_network_instruction(el);
        }
        s += "\n";
        for (const auto &el : nodes)
        {
            s += "\n" + el.to_str();
        }
        return s;
    }

    friend std::ostream & operator<<(std::ostream &os, const Network &network)
    {
        return os << network.to_str();
    }
};

void parseNetwork(const vector<string> &lines, Network &network)
{
    for (const auto &c : lines.at(0))
    {
        if ('L' == c)
        {
            network.networkInstructions.push_back(Network::NetworkInstruction::LEFT);
        }
        else if ('R' == c)
        {
            network.networkInstructions.push_back(Network::NetworkInstruction::RIGHT);
        }
    }

    std::regex reg("[A-Z]{3}");

    for (size_t i=1; i<lines.size(); i++)
    {
        if (lines.at(i).empty())
        {
            continue;
        }

        std::smatch match{};
        auto searchStart(lines.at(i).cbegin());
        vector<string> result{};

        while (regex_search(searchStart, lines.at(i).cend(), match, reg))
        {
            result.push_back(match[0]);
            // cout << match[0] << endl;
            searchStart = match.suffix().first;
        }
        if (result.size() != 3)
        {
            cout << "Wrong node at index: " << i << "!" << endl;
            return;
        }

        network.nodes.emplace_back(result.at(0), result.at(1), result.at(2));
    }
}

int main()
{
    setlocale(LC_ALL, "");
    srand(time(0));
    
    std::filesystem::path cwd = std::filesystem::current_path().filename();
    cout << "Hello World: " << cwd << endl;

    // string filename("example_input"); // 6
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

    Network network{};
    parseNetwork(lines, network);
    cout << network << endl;
    network.traverseNetwork();

    return EXIT_SUCCESS;
}
