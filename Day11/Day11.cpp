#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <regex>
#include <algorithm>

using namespace std;

bool gPrintInput = false;
bool gDebugEnabled = false;

struct Galaxy
{
	int id;
	long long x{};
	long long y{};
	long long deltax{};
	long long deltay{};
};

using GalaxyMap = std::vector<string>;

GalaxyMap readGalaxyMap(std::ifstream& inputFile);
std::vector<Galaxy> getGalaxies(const GalaxyMap& map);
std::vector<Galaxy> expandGalaxies(GalaxyMap& map, std::vector<Galaxy>& galaxies, const long long expFactor);
long long computeDistance(const Galaxy g1, const Galaxy g2);
long long computeShortestPath(const std::vector<Galaxy> galaxiesToCompare);

int main()
{
	string inputString{ "empty" };

	std::ifstream inputFile("input_test_1.txt");
	//std::ifstream inputFile("input_real_1.txt");

	long long r1 = 0;
	long long r2 = 0;

	std::cout << "Script starts here " << std::endl;

	if (inputFile.is_open())
	{
		if (gPrintInput)
		{
			std::cout << "Input file content is" << std::endl;
			std::cout << "**********************" << std::endl;
			while (!inputFile.eof())
			{
				std::getline(inputFile, inputString, '\n');
				std::cout << inputString << std::endl;
			}
			std::cout << "**********************" << std::endl;
			inputFile.clear();
			inputFile.seekg(0);
		}

		auto map = readGalaxyMap(inputFile);
		auto inGalaxies = getGalaxies(map);
		std::cout << "Found " << inGalaxies.size() << " galaxies" << std::endl;
		
		auto galExp2 = expandGalaxies(map, inGalaxies, 2);
		r1 = computeShortestPath(galExp2);
		
		auto galExpMil = expandGalaxies(map, inGalaxies, 1000000l);
		r2 = computeShortestPath(galExpMil);

	}
	else
	{
		std::cout << "Could not open input file!" << std::endl;
	}

	std::cout << "Result for Trial #1 is " << r1 << std::endl;
	std::cout << "Result for Trial #2 is " << r2 << std::endl;

	return 0;
}

string getStringFromGalaxy(const Galaxy g)
{
	string out{};
	out += "Galaxy #";
	out += std::to_string(g.id);
	out += "[";
	out += std::to_string(g.x);
	out += "|";
	out += std::to_string(g.y);
	out += "]";

	return out;
}

GalaxyMap readGalaxyMap(std::ifstream& inputFile)
{
	string inputString{ "" };
	GalaxyMap map;
	auto rowCnt = 0;

	while (!inputFile.eof())
	{
		std::getline(inputFile, inputString, '\n');
		map.push_back(inputString);
	}

	return map;
}

std::vector<Galaxy> expandGalaxies(GalaxyMap& map, std::vector<Galaxy>& inGalaxies, const long long expFactor)
{
	// find rows to expand
	std::vector<long long> expRows{};
	for (auto r = 0; r < map.size(); r++)
	{
		auto galaxyPresent = false;
		for (auto c = 0; c < map.at(0).size(); c++)
		{
			if (map.at(r).at(c) == '#')
			{
				galaxyPresent = true;
			}
		}
		if (not(galaxyPresent))
		{
			expRows.push_back(r);
		}
	}

	// find cols to expand
	std::vector<long long> expCols{};
	for (auto c = 0; c < map.at(0).size(); c++)
	{
		auto galaxyPresent = false;
		for (auto r = 0; r < map.size(); r++)
		{
			if (map.at(r).at(c) == '#')
			{
				galaxyPresent = true;
			}
		}
		if (not(galaxyPresent))
		{
			expCols.push_back(c);
		}
	}

	// add deltas (i.e. expand)
	for (auto& g : inGalaxies)
	{
		for (const auto r : expRows)
		{
			if (g.x > r)
			{
				g.deltax += expFactor - 1;
			}
		}
		for (const auto c : expCols)
		{
			if (g.y > c)
			{
				g.deltay += expFactor - 1;
			}
		}
	}

	std::vector<Galaxy> expGalaxies;
	for (const auto& g : inGalaxies)
	{
		expGalaxies.push_back({ g.id, g.x + g.deltax, g.y + g.deltay, 0, 0 });
	}

	return expGalaxies;
}


std::vector<Galaxy> getGalaxies(const GalaxyMap& map)
{
	std::vector<Galaxy> galaxies{};
	
	auto cntGalaxies = 0;
	auto cntRow = 0;
	for (const auto r : map)
	{
		auto cntCol = 0;
		for (const auto c : r)
		{
			if (c == '#')
			{
				cntGalaxies++;
				galaxies.push_back({ cntGalaxies, cntRow, cntCol });
			}
			cntCol++;
		}
		cntRow++;
	}

	return galaxies;
}

long long computeDistance(const Galaxy g1, const Galaxy g2)
{
	long long res = 0;
	long long f1 = g1.y - g2.y;
	long long f2 = g1.x - g2.x;
	res = abs(f1) + abs(f2);

	if (gDebugEnabled)
	{
		std::cout << "Computing distance between " << getStringFromGalaxy(g1) << " and " << getStringFromGalaxy(g2) << std::endl;
		std::cout << "f1:" << f1 << " f2:" << f2 << " res:" << res << std::endl;
	}

	return res;
}

long long computeShortestPath(const std::vector<Galaxy> galaxiesToCompare)
{
	std::vector<Galaxy> galaxiesForComparison = galaxiesToCompare;
	long long cntPath = 0;
	long long cntCmp = 0;
	for (const auto& g1 : galaxiesToCompare)
	{
		for (const auto& g2 : galaxiesForComparison)
		{
			if ((g1.x != g2.x) or (g1.y != g2.y))
			{
				cntCmp++;
				cntPath += computeDistance(g1, g2);
			}
		}
		galaxiesForComparison.erase(galaxiesForComparison.begin());
	}

	return cntPath;
}