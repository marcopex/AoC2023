#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <regex>
#include <unordered_map>

using namespace std;

bool gPrintInput = false;
bool gDebugEnabled = true;

enum ParsingState
{
	parsingNothing,
	parsingSeeds,
	parsingSoils,
	parsingFertilizers,
	parsingWaters,
	parsingLights,
	parsingTemperatures,
	parsingHumidities,
	parsingLocations,
};

struct relationship
{
	long long dstRangeStart;
	long long srcRangeStart;
	long long rangeLen;
};

struct RelationshipsData
{
	std::vector<long long> seeds{};
	std::vector<relationship> seedsToSoils{};
	std::vector<relationship> soilsToFertilizers{};
	std::vector<relationship> fertilizersToWaters{};
	std::vector<relationship> watersToLights{};
	std::vector<relationship> lightsToTemperatures{};
	std::vector<relationship> temperaturesToHumidities{};
	std::vector<relationship> humiditiesToLocations{};
	std::vector<relationship> soilsToSeeds;
	std::vector<relationship> fertilizersToSoils{};
	std::vector<relationship> watersToFertilizers{};
	std::vector<relationship> lightsToWaters{};
	std::vector<relationship> temperaturesToLights{};
	std::vector<relationship> humidititesToTemperatures{};
	std::vector<relationship> locationsToHumidities{};
};

struct SeedRange
{
	long long rangeStart;
	long long rangeStop;
};

std::vector<long long> getNumbersFromString(const string& inputString);
RelationshipsData getRelationships(std::ifstream& inputFile);
long long getMinimumLocationFromSeeds(const RelationshipsData& relationships);
long long getMinimumLocationFromExpandedSeeds(const RelationshipsData& relationships);
long long getMapping(std::vector<relationship> relationships, const long long reqNumber);
relationship swapDirection(const relationship& rel);
long long getLocationFromSeed(const RelationshipsData& relationships, const long long seed);
long long getSeedFromLocation(const RelationshipsData& relationships, const long long location);

int main()
{
	string inputString{ "empty" };

	//std::ifstream inputFile("input_test_1.txt");
	std::ifstream inputFile("input_real_1.txt");

	long long r1 = 0;
	long long r2 = 0;

	cout << "Script starts here " << std::endl;

	if (inputFile.is_open())
	{
		if (gPrintInput)
		{
			cout << "Input file content is" << std::endl;
			cout << "**********************" << std::endl;
			while (!inputFile.eof())
			{
				std::getline(inputFile, inputString, '\n');
				cout << inputString << std::endl;
			}
			cout << "**********************" << std::endl;
			inputFile.clear();
			inputFile.seekg(0);
		}

		const auto relationships = getRelationships(inputFile);
		r1 = getMinimumLocationFromSeeds(relationships);
		r2 = getMinimumLocationFromExpandedSeeds(relationships);


		inputFile.close();
	}
	else
	{
		cout << "Could not open input file!" << std::endl;
	}

	cout << "Result for Trial #1 is " << r1 << std::endl;
	cout << "Result for Trial #2 is " << r2 << std::endl;

	return 0;
}

constexpr unsigned long long str2int(const char* str, long long h = 0)
{
	return !str[h] ? 5381 : (str2int(str, h + 1) * 33) ^ str[h];
}

long long getLocationFromSeed(const RelationshipsData& relationships, const long long seed)
{
	const auto soil = getMapping(relationships.seedsToSoils, seed);
	const auto fertilizer = getMapping(relationships.soilsToFertilizers, soil);
	const auto water = getMapping(relationships.fertilizersToWaters, fertilizer);
	const auto light = getMapping(relationships.watersToLights, water);
	const auto temperature = getMapping(relationships.lightsToTemperatures, light);
	const auto humidity = getMapping(relationships.temperaturesToHumidities, temperature);
	const auto location = getMapping(relationships.humiditiesToLocations, humidity);

	//if (gDebugEnabled)
	//{
	//	std::cout << "Seed:" << seed << " --> " << std::endl;
	//	std::cout << "Soil:" << soil << " --> " << std::endl;
	//	std::cout << "Fertilizer:" << fertilizer << " --> " << std::endl;
	//	std::cout << "Water:" << water << " --> " << std::endl;
	//	std::cout << "Light:" << light << " --> " << std::endl;
	//	std::cout << "Temperature:" << temperature << " --> " << std::endl;
	//	std::cout << "Humidity:" << humidity << " --> " << std::endl;
	//	std::cout << "Location:" << location << " --> " << std::endl;
	//}
	
	return location;
}

long long getSeedFromLocation(const RelationshipsData& relationships, const long long location)
{
	const auto humidity = getMapping(relationships.locationsToHumidities, location);
	const auto temperature = getMapping(relationships.humidititesToTemperatures, humidity);
	const auto light = getMapping(relationships.temperaturesToLights, temperature);
	const auto water = getMapping(relationships.lightsToWaters, light);
	const auto fertilizer = getMapping(relationships.watersToFertilizers, water);
	const auto soil = getMapping(relationships.fertilizersToSoils, fertilizer);
	const auto seed = getMapping(relationships.soilsToSeeds, soil);
	
	//if (gDebugEnabled)
	//{
	//	std::cout << "Location:" << location << " --> " << std::endl;
	//	std::cout << "Humidity:" << humidity << " --> " << std::endl;
	//	std::cout << "Temperature:" << temperature << " --> " << std::endl;
	//	std::cout << "Light:" << light << " --> " << std::endl;
	//	std::cout << "Water:" << water << " --> " << std::endl;
	//	std::cout << "Fertilizer:" << fertilizer << " --> " << std::endl;
	//	std::cout << "Soil:" << soil << " --> " << std::endl;
	//	std::cout << "Seed:" << seed << " --> " << std::endl;
	//}

	return seed;
}


relationship swapDirection(const relationship& rel)
{
	relationship swappedRelationship{ rel.srcRangeStart, rel.dstRangeStart, rel.rangeLen };
	return swappedRelationship;
}

long long getMapping(std::vector<relationship> relationships, const long long reqNumber)
{
	auto mappingFound = false;
	long long mapNumber = 0;
	int relCount = 0;

	for (const auto& relationship : relationships)
	{
		const long long srcStart = relationship.srcRangeStart;
		const long long srcStop = srcStart + relationship.rangeLen;

		if ((reqNumber >= srcStart) && (reqNumber < srcStop))
		{
			if (mappingFound)
			{
				std::cout << "WARNING - Double Mapping!" << std::endl;
			}
			mappingFound = true;
			mapNumber = (reqNumber - srcStart) + relationship.dstRangeStart;
		}
		relCount++;
	}

	if (!mappingFound)
	{
		mapNumber = reqNumber;
	}

	return mapNumber;
}

std::vector<long long> getNumbersFromString(const string& inputString)
{
	string localString = inputString;

	std::vector<string> strings;
	std::vector<long long> numbers;

	std::regex stringRegex(R"(\d+)");   // matches a sequence of digits
	std::smatch regexMatch;

	while (std::regex_search(localString, regexMatch, stringRegex))
	{
		char* pEndLocal;
		const auto number = strtoll(regexMatch.str().c_str(), &pEndLocal, 0);
		strings.push_back(regexMatch.str());
		numbers.push_back(number);
		localString = regexMatch.suffix();
	}

	return numbers;
}

RelationshipsData getRelationships(std::ifstream& inputFile)
{
	const std::vector<string> headers = { "seeds:",
										"seed-to-soil map:",
										"soil-to-fertilizer map:",
										"fertilizer-to-water map:",
										"water-to-light map:",
										"light-to-temperature map:",
										"temperature-to-humidity map:",
										"humidity-to-location map:" };


	RelationshipsData relationships;

	auto state = ParsingState::parsingNothing;
	auto headerNow = 0;
	while (!inputFile.eof())
	{
		string inputString;
		std::getline(inputFile, inputString, '\n');

		if (inputString.empty())
		{ // skip empty strings
			continue;
		}

		string headerString{ "" };
		auto pos = inputString.find_first_of(':');
		auto isHeaderString = false;
		if (pos != string::npos)
		{
			isHeaderString = true;
			headerString = inputString.substr(0, pos + 1);
		}

		auto headerCnt = 1;
		for (const auto& header : headers)
		{
			if ((str2int(header.c_str()) == str2int(headerString.c_str())) && (headerCnt > headerNow))
			{
				headerNow = headerCnt;
				state = (ParsingState)headerNow;
				break;
			}
			headerCnt++;
		}

		if (isHeaderString && (state != ParsingState::parsingSeeds))
		{
			continue;
		}

		switch (state)
		{
		case ParsingState::parsingSeeds:
			relationships.seeds = getNumbersFromString(inputString.substr(pos + 2));
			break;

		case ParsingState::parsingSoils:
		{
			auto values = getNumbersFromString(inputString);
			const relationship rel{ values.at(0), values.at(1), values.at(2) };
			relationships.seedsToSoils.push_back(rel);
		}
		break;

		case ParsingState::parsingFertilizers:
		{
			auto values = getNumbersFromString(inputString);
			const relationship rel{ values.at(0), values.at(1), values.at(2) };
			relationships.soilsToFertilizers.push_back(rel);
		}
		break;

		case ParsingState::parsingWaters:
		{
			auto values = getNumbersFromString(inputString);
			const relationship rel{ values.at(0), values.at(1), values.at(2) };
			relationships.fertilizersToWaters.push_back(rel);
		}
		break;

		case ParsingState::parsingLights:
		{
			auto values = getNumbersFromString(inputString);
			const relationship rel{ values.at(0), values.at(1), values.at(2) };
			relationships.watersToLights.push_back(rel);
		}
		break;

		case ParsingState::parsingTemperatures:
		{
			auto values = getNumbersFromString(inputString);
			const relationship rel{ values.at(0), values.at(1), values.at(2) };
			relationships.lightsToTemperatures.push_back(rel);
		}
		break;

		case ParsingState::parsingHumidities:
		{
			auto values = getNumbersFromString(inputString);
			const relationship rel{ values.at(0), values.at(1), values.at(2) };
			relationships.temperaturesToHumidities.push_back(rel);
		}
		break;

		case ParsingState::parsingLocations:
		{
			auto values = getNumbersFromString(inputString);
			const relationship rel{ values.at(0), values.at(1), values.at(2) };
			relationships.humiditiesToLocations.push_back(rel);
		}
		break;

		default:
			break;
		}
	}

	for (const auto& temp : relationships.seedsToSoils)
	{
		relationships.soilsToSeeds.push_back(swapDirection(temp));
	}
	for (const auto& temp : relationships.soilsToFertilizers)
	{
		relationships.fertilizersToSoils.push_back(swapDirection(temp));
	}
	for (const auto& temp : relationships.fertilizersToWaters)
	{
		relationships.watersToFertilizers.push_back(swapDirection(temp));
	}
	for (const auto& temp : relationships.watersToLights)
	{
		relationships.lightsToWaters.push_back(swapDirection(temp));
	}
	for (const auto& temp : relationships.lightsToTemperatures)
	{
		relationships.temperaturesToLights.push_back(swapDirection(temp));
	}
	for (const auto& temp : relationships.temperaturesToHumidities)
	{
		relationships.humidititesToTemperatures.push_back(swapDirection(temp));
	}
	for (const auto& temp : relationships.humiditiesToLocations)
	{
		relationships.locationsToHumidities.push_back(swapDirection(temp));
	}

	return relationships;
}

long long getMinimumLocationFromSeeds(const RelationshipsData& relationships)
{
	long long minLocation = 0xFFFFFFFFFF;
	for (const auto& seed : relationships.seeds)
	{
		const auto location = getLocationFromSeed(relationships, seed);
		if (gDebugEnabled)
		{
			std::cout << "Seed: " << seed << " returns Location:" << location << std::endl;
		}

		if (location < minLocation)
		{
			minLocation = location;
		}
	}
	return minLocation;
}

long long getMinimumLocationFromExpandedSeeds(const RelationshipsData& relationships)
{
	std::vector<SeedRange> seedRanges{};

	for (auto i = 0; i < relationships.seeds.size(); i += 2)
	{
		const auto rangeStart = relationships.seeds.at(i);
		const auto rangeStop = rangeStart + relationships.seeds.at(i + 1) - 1;
		seedRanges.push_back({ rangeStart, rangeStop });
	}

	long long location = 0;
	long long bestLocation = 0;
	auto searchingSeedRange = true;

	while (searchingSeedRange)
	{
		const auto seed = getSeedFromLocation(relationships, location);
		
		for (const auto& range : seedRanges)
		{
			if ((seed < range.rangeStart) or (seed > range.rangeStop))
			{
				if (gDebugEnabled)
				{
					if ((location % 100000) == 0)
					{
						std::cout << "Range Check for location: " << location << " -> seed:" << seed << " between [" << range.rangeStart << "-" << range.rangeStop << "]" << std::endl;
					}
				}
				// just skip location
			}
			else
			{
				if (gDebugEnabled)
				{
					std::cout << "Range Check for location: " << location << " -> seed:" << seed << " between [" << range.rangeStart << "-" << range.rangeStop << "]" << std::endl;
					std::cout << "Found location:" << location << " for seed:" << seed << std::endl;
				}
				bestLocation = location;
				searchingSeedRange = false;
			}
		}
		location++;
	}
	
	return bestLocation;
}
