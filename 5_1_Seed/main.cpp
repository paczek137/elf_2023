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
#include <deque>
#include <climits>

using namespace std;

struct Seed
{
    long id;
    long soil;
    long fertilizer;
    long water;
    long light;
    long temperature;
    long humidity;
    long location;

    Seed() = default;
    Seed(const long _id) : id(_id), soil(0), fertilizer(0), water(0), light(0),
                            temperature(0), humidity(0), location(0) {};

    std::string const to_str() const
    {
        return to_string(id);
    }

    std::string const to_str_full() const
    {
        return to_string(id) + "->" + to_string(soil) + "->" + to_string(fertilizer) + "->" + 
                to_string(water) + "->" + to_string(light) + "->" + to_string(temperature) + "->" + 
                to_string(humidity) + "->" + to_string(location);
    }

    friend std::ostream & operator<<(std::ostream &os, const Seed &seed)
    {
        // return os << seed.to_str();
        return os << seed.to_str_full();
    }
};

struct CustomMap
{
    const string TOKEN;
    vector<long> sourceRangeStart;
    vector<long> destinationRangeStart;
    vector<long> rangeLength;

    CustomMap() = default;
    CustomMap(const string &_token) : TOKEN(_token) {};

    std::string const to_str() const
    {
        string s = TOKEN + " size: " + to_string(destinationRangeStart.size()) + "\n";
        for (size_t i=0; i<destinationRangeStart.size(); i++)
        {
            s += to_string(destinationRangeStart.at(i)) + " " + 
                to_string(sourceRangeStart.at(i)) + " " + 
                to_string(rangeLength.at(i)) + "\n";
        }
        return s;
    }

    friend std::ostream & operator<<(std::ostream &os, const CustomMap &customMap)
    {
        return os << customMap.to_str();
    }
};

struct Almanac
{
    inline static const string SEEDS_TOKEN = "seeds:";
    inline static const string SEEDTOSOIL_TOKEN = "seed-to-soil map:";
    inline static const string SOILTOFERTILIZER_TOKEN = "soil-to-fertilizer map:";
    inline static const string FERTILIZERTOWATER_TOKEN = "fertilizer-to-water map:";
    inline static const string WATERTOLIGHT_TOKEN = "water-to-light map:";
    inline static const string LIGHTTOTEMPERATURE_TOKEN = "light-to-temperature map:";
    inline static const string TEMPERATURETOHUMIDITY_TOKEN = "temperature-to-humidity map:";
    inline static const string HUMIDITYTOLOCATION_TOKEN = "humidity-to-location map:"; 

    vector<Seed> seeds;
    CustomMap seedToSoilMap = CustomMap(SEEDTOSOIL_TOKEN);
    CustomMap soilToFertilizerMap = CustomMap(SOILTOFERTILIZER_TOKEN);
    CustomMap fertilizerToWaterMap = CustomMap(FERTILIZERTOWATER_TOKEN);
    CustomMap waterToLightMap = CustomMap(WATERTOLIGHT_TOKEN);
    CustomMap lightToTemperatureMap = CustomMap(LIGHTTOTEMPERATURE_TOKEN);
    CustomMap temperatureToHumidityMap = CustomMap(TEMPERATURETOHUMIDITY_TOKEN);
    CustomMap humidityToLocationMap = CustomMap(HUMIDITYTOLOCATION_TOKEN);

    Almanac() = default;

    CustomMap & getCustomMap(const string &token)
    {
        if (SEEDTOSOIL_TOKEN == token)
        {
            return seedToSoilMap;
        }
        else if (SOILTOFERTILIZER_TOKEN == token)
        {
            return soilToFertilizerMap;
        }
        else if (FERTILIZERTOWATER_TOKEN == token)
        {
            return fertilizerToWaterMap;
        }
        else if (WATERTOLIGHT_TOKEN == token)
        {
            return waterToLightMap;
        }
        else if (LIGHTTOTEMPERATURE_TOKEN == token)
        {
            return lightToTemperatureMap;
        }
        else if (TEMPERATURETOHUMIDITY_TOKEN == token)
        {
            return temperatureToHumidityMap;
        }
        else
        {
            return humidityToLocationMap;
        }
    }
};

void parseSeeds(const string &line, Almanac &almanac)
{
    if (line.find(Almanac::SEEDS_TOKEN) == std::string::npos)
    {
        cout << "No seeds!" << endl;
        return;
    }
    
    vector<string> numbers{};
    boost::split(numbers, 
                line.substr(line.find(almanac.SEEDS_TOKEN) + almanac.SEEDS_TOKEN.size()),
                boost::is_any_of(" "));
    if (numbers.empty())
    {
        cout << "No seeds!" << endl;
        return;
    }    

    for (auto const &seed : numbers)
    {
        if (seed.empty())
        {
            continue;
        }

        almanac.seeds.emplace_back(static_cast<long>(stol(seed)));
    }
}

void findToken(const vector<string> &lines, const string &token, size_t &idx)
{
    for (; idx<lines.size(); idx++)
    {
        if (lines.at(idx).find(token) != std::string::npos)
        {
            return;
        }
    }

    cout << "Not found token: " << token << endl;
}

void parseMap(const vector<string> &lines, const string &token, size_t &idx, Almanac &almanac)
{
    findToken(lines, token, idx);
    for (idx=idx+1; idx<lines.size(); idx++)
    {
        if (lines.at(idx).empty())
        {
            break;
        }

        vector<string> numbers{};
        boost::split(numbers, lines.at(idx), boost::is_any_of(" "));
        if (3 != numbers.size())
        {
            cout << "Wrong input data" << endl;
            return;
        }

        almanac.getCustomMap(token).destinationRangeStart.push_back(stol(numbers.at(0)));
        almanac.getCustomMap(token).sourceRangeStart.push_back(stol(numbers.at(1)));
        almanac.getCustomMap(token).rangeLength.push_back(stol(numbers.at(2)));
    }
}

void parseAlmanac(const vector<string> &lines, Almanac &almanac)
{
    size_t idx = 0;
    parseSeeds(lines.at(idx), almanac);

    parseMap(lines, Almanac::SEEDTOSOIL_TOKEN, idx, almanac);
    parseMap(lines, Almanac::SOILTOFERTILIZER_TOKEN, idx, almanac);
    parseMap(lines, Almanac::FERTILIZERTOWATER_TOKEN, idx, almanac);
    parseMap(lines, Almanac::WATERTOLIGHT_TOKEN, idx, almanac);
    parseMap(lines, Almanac::LIGHTTOTEMPERATURE_TOKEN, idx, almanac);
    parseMap(lines, Almanac::TEMPERATURETOHUMIDITY_TOKEN, idx, almanac);
    parseMap(lines, Almanac::HUMIDITYTOLOCATION_TOKEN, idx, almanac);

    cout << "Seeds:\n";
    for (auto const &seed : almanac.seeds)
    {
        cout << seed << endl;
    }
    cout << endl;

    cout << almanac.seedToSoilMap << endl;
    cout << almanac.soilToFertilizerMap << endl;
    cout << almanac.fertilizerToWaterMap << endl;
    cout << almanac.waterToLightMap << endl;
    cout << almanac.lightToTemperatureMap << endl;
    cout << almanac.temperatureToHumidityMap << endl;
    cout << almanac.humidityToLocationMap << endl;
}

long getMappingValue(const string &token, const long value, Almanac &almanac)
{
    CustomMap &map = almanac.getCustomMap(token);
    for (size_t i=0; i<map.sourceRangeStart.size(); i++)
    {
        if (value >= map.sourceRangeStart.at(i) and 
            value < map.sourceRangeStart.at(i) + map.rangeLength.at(i))
        {
            return  map.destinationRangeStart.at(i) + 
                        value - map.sourceRangeStart.at(i);
        }
    }
    return value;
}

void parseSeeds(Almanac &almanac)
{
    for (Seed &seed : almanac.seeds)
    {
        seed.soil = getMappingValue(almanac.SEEDTOSOIL_TOKEN, seed.id, almanac);
        seed.fertilizer = getMappingValue(almanac.SOILTOFERTILIZER_TOKEN, seed.soil, almanac);
        seed.water = getMappingValue(almanac.FERTILIZERTOWATER_TOKEN, seed.fertilizer, almanac);
        seed.light = getMappingValue(almanac.WATERTOLIGHT_TOKEN, seed.water, almanac);
        seed.temperature = getMappingValue(almanac.LIGHTTOTEMPERATURE_TOKEN, seed.light, almanac);
        seed.humidity = getMappingValue(almanac.TEMPERATURETOHUMIDITY_TOKEN, seed.temperature, almanac);
        seed.location = getMappingValue(almanac.HUMIDITYTOLOCATION_TOKEN, seed.humidity, almanac);
    }
}

int main()
{
    setlocale(LC_ALL, "");
    srand(time(0));
    
    std::filesystem::path cwd = std::filesystem::current_path().filename();
    cout << "Hello World: " << cwd << endl;

    // string filename("example_input"); // 35
    string filename("input");
    ifstream input_file(filename);

    string line{};
 
    if (not input_file.is_open())
    {
        cout << "Could not open the file: " << filename << endl;
        return EXIT_FAILURE;
    }

    vector<string> lines{};
    Almanac almanac{};

    while (getline(input_file, line, '\n')) 
    {
        lines.push_back(line);
    }

    parseAlmanac(lines, almanac);
    parseSeeds(almanac);

    cout << "Seeds:\n";
    for (auto const &seed : almanac.seeds)
    {
        cout << seed << endl;
    }
    cout << endl;

    cout << "Minimum location: " << (*std::min_element(almanac.seeds.cbegin(), almanac.seeds.cend(),
                                     [] (const Seed &seedA, const Seed &seedB)
                                     {
                                        return seedA.location < seedB.location;
                                     })).location << endl;

    return EXIT_SUCCESS;
}
