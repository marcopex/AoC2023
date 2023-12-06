#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <regex>

using namespace std;

bool gPrintInput = false;
bool gDebugEnabled = false;

struct RaceInfo
{
	long long raceDuration;
	long long recordDistance;
};

std::vector<long long> getNumbersFromString(const string& inputString);
std::vector<string> getStringsFromString(const string& inputString);
std::vector<RaceInfo> getRaces(std::ifstream &inputFile);
RaceInfo getCumulativeRace(std::ifstream &inputFile);
long long computePossibleWinningDistances(const RaceInfo& race);
long long estimateWinningDistance(const long long time, const long long distance);

int main()
{
	string inputString{ "empty" };

	//std::ifstream inputFile("input_test_1.txt");
	std::ifstream inputFile("input_real_1.txt");

	long long cumMultTrial1 = 1;
	long long cumMultTrial2 = 0;

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

		const auto races = getRaces(inputFile);
		
		inputFile.clear();
		inputFile.seekg(0);
		const auto cumRace = getCumulativeRace(inputFile);

		for (const auto& race : races)
		{
			cumMultTrial1 *= computePossibleWinningDistances(race);
		}

		cumMultTrial2 = estimateWinningDistance(cumRace.raceDuration, cumRace.recordDistance);

		inputFile.close();
	}
	else
	{
		cout << "Could not open input file!" << std::endl;
	}

	cout << "Result for Trial #1 is " << cumMultTrial1 << std::endl;
	cout << "Result for Trial #2 is " << cumMultTrial2 << std::endl;

	return 0;
}

RaceInfo getCumulativeRace(std::ifstream &inputFile)
{
	string inputString{ "empty" };
	std::vector<RaceInfo> races;
	string cumTimeString{ "" };
	string cumDistString{ "" };

	while (!inputFile.eof())
	{
		std::getline(inputFile, inputString, '\n');
		std::size_t const npos1 = inputString.find_first_of(':');

		auto headerString = inputString.substr(0, npos1);
		auto valuesString = inputString.substr(npos1 + 1, inputString.length());

		const auto strings = getStringsFromString(headerString);
		const auto numbers = getNumbersFromString(valuesString);

		std::vector<long long> values{};
		for (const auto& n : numbers)
		{
			values.push_back(n);
		}

		if (headerString.compare("Time") == 0)
		{
			for (const auto& val : values)
			{
				const RaceInfo race{ val, 0 };
				races.push_back(race);
				cumTimeString.append(std::to_string(val));
			}
		}
		else if (headerString.compare("Distance") == 0)
		{
			for (auto i = 0; i < values.size(); i++)
			{
				races.at(i).recordDistance = values.at(i);
				cumDistString.append(std::to_string(values.at(i)));
			}
		}
	}

	char* pEndLocal;
	const auto cumTime = strtoll(cumTimeString.c_str(), &pEndLocal, 0);
	const auto cumDist = strtoll(cumDistString.c_str(), &pEndLocal, 0);
	
	const RaceInfo cumRace{ cumTime, cumDist };

	return cumRace;
}

std::vector<RaceInfo> getRaces(std::ifstream &inputFile)
{
	string inputString{ "empty" };
	std::vector<RaceInfo> races;

	while (!inputFile.eof())
	{
		std::getline(inputFile, inputString, '\n');
		std::size_t const npos1 = inputString.find_first_of(':');

		auto headerString = inputString.substr(0, npos1);
		auto valuesString = inputString.substr(npos1 + 1, inputString.length());

		const auto strings = getStringsFromString(headerString);
		const auto numbers = getNumbersFromString(valuesString);

		std::vector<long long> values{};
		for (const auto& n : numbers)
		{
			values.push_back(n);
		}

		if (headerString.compare("Time") == 0)
		{
			for (const auto& val : values)
			{
				const RaceInfo race{ val, 0 };
				races.push_back(race);
			}
		}
		else if (headerString.compare("Distance") == 0)
		{
			for (auto i = 0; i < values.size(); i++)
			{
				races.at(i).recordDistance = values.at(i);
			}
		}
	}

	return races;
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

std::vector<string> getStringsFromString(const string& inputString)
{
	char localString[256] = "";
	strcpy_s(localString, inputString.c_str());
	std::vector<string> strings;

	char* nextToken{ nullptr };
	auto token = strtok_s(localString, " ", &nextToken);
	while (token)
	{
		strings.emplace_back(token);
		token = strtok_s(nullptr, " ", &nextToken);
	}

	return strings;
}

long long computePossibleWinningDistances(const RaceInfo& race)
{
	long long cntWinningDistances = 0;
	long long maxDistanceTravelled = 0;

	for (auto i = 0; i <= race.raceDuration; i++)
	{
		const auto travellingSpeed = i;
		const auto remainingDistance = race.raceDuration - i;
		const auto distanceTravelled = travellingSpeed * remainingDistance;
		if (distanceTravelled > maxDistanceTravelled)
		{
			maxDistanceTravelled = distanceTravelled;
		}
		if (distanceTravelled > race.recordDistance)
		{
			cntWinningDistances++;
		}
		if (gDebugEnabled)
		{
			std::cout << "Travelling Speed:" << travellingSpeed << " Remaining distance:" << remainingDistance << " Distance Travelled:" << distanceTravelled << std::endl;
		}
	}
	if (gDebugEnabled)
	{
		std::cout << "Max Distance Travelled: " << maxDistanceTravelled << std::endl;
		std::cout << "Winning Distances: " << cntWinningDistances << std::endl;
	}

	return cntWinningDistances;
}

long long estimateWinningDistance(const long long raceDuration, const long long raceDistance)
{

	if (gDebugEnabled)
	{
		std::cout << "Cumulative Race Duration:" << raceDuration << " [ms]" << std::endl;
		std::cout << "Cumulative Race Distance:" << raceDistance << " [m]" << std::endl;
	}

	int cntLosingDistances = 0;
	auto belowAcceptableDistance = true;
	for (auto i = 0; (i <= raceDuration) && belowAcceptableDistance; i++)
	{
		const long long travellingSpeed = i;
		const long long remainingDistance = raceDuration - i;
		const long long distanceTravelled = travellingSpeed * remainingDistance;

		if (distanceTravelled >= raceDistance)
		{
			belowAcceptableDistance = false;
		}
		else
		{
			cntLosingDistances++;
		}
	}

	const auto cntWinningDistances = raceDuration - cntLosingDistances * 2 + 1;
	
	if (gDebugEnabled)
	{
		std::cout << "Losing Distances: " << cntLosingDistances << std::endl;
		std::cout << "Winning Distances: " << cntWinningDistances << std::endl;
	}
	
	return cntWinningDistances;
}
