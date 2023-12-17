#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

bool gPrintInput = false;
bool gDebugEnabled = false;

struct Platform
{
	std::vector<std::string> rows;
	std::vector<std::string> cols;
};

enum class Direction : char
{
	North,
	South,
	West,
	East
};

Platform getPlatform(std::ifstream& inputFile);
Platform tiltPlatform(const Platform& p, const Direction& dir);
int computeWeights(const Platform& p);
std::vector<int> spinCyclePlatform(const Platform& p, const long long numCycles);
long long extrapolateWeight(std::vector<int> weightSequence, const long long reqCycles);

int main()
{
	string inputString{ "empty" };

	//std::ifstream inputFile("input_test_1.txt");
	std::ifstream inputFile("input_real_1.txt");

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
	}
	else
	{
		std::cout << "Could not open input file!" << std::endl;
	}

	auto p = getPlatform(inputFile);
	auto pTilt = tiltPlatform(p, Direction::North);
	r1 = computeWeights(pTilt);

	const long long testCycles = 1000;
	const auto weightSequence = spinCyclePlatform(p, testCycles);
	const long long reqCycles = 1000000000;
	r2 = extrapolateWeight(weightSequence, reqCycles);

	std::cout << "Result for Trial #1 is " << r1 << std::endl;
	std::cout << "Result for Trial #2 is " << r2 << std::endl;

	return 0;
}

std::vector<string> getColumns(std::vector<string>& rows)
{
	std::vector<string> cols{};

	for (auto j = 0; j < rows.at(0).size(); j++)
	{
		string col{};
		for (auto i = 0; i < rows.size(); i++)
		{
			col.push_back(rows.at(i).at(j));
		}
		cols.push_back(col);
	}
	return cols;
}

Platform getPlatform(std::ifstream& inputFile)
{
	string inputString{ "" };
	Platform p;

	while (!inputFile.eof())
	{
		std::getline(inputFile, inputString, '\n');
		p.rows.push_back(inputString);
	}
	if (p.rows.size() > 0)
	{
		p.cols = getColumns(p.rows);
	}

	return p;
}

bool canMove(const Platform& p, const Direction& dir, int x, int y)
{
	if (p.rows.at(x).at(y) != 'O')
	{
		return false;
	}
	
	bool movePossible = true;
	auto x2 = x;
	auto y2 = y;
	switch (dir)
	{
	case Direction::North:
		if (x == 0)
		{
			movePossible = false;
		}
		x2--;
		break;
	case Direction::South:
		if (x == (p.rows.size() - 1))
		{
			movePossible = false;
		}
		x2++;
		break;
	case Direction::West:
		if (y == 0)
		{
			movePossible = false;
		}
		y2--;
		break;
	case Direction::East:
		if (y == (p.cols.size() - 1))
		{
			movePossible = false;
		}
		y2++;
		break;
	default:
		movePossible = false;
		break;
	}
	if (movePossible)
	{
		if (p.rows.at(x2).at(y2) != '.')
		{
			movePossible = false;
		}
	}

	return movePossible;

}

void performMove(Platform& p, const Direction& dir, int &x, int &y)
{
	auto x2 = x;
	auto y2 = y;
	switch (dir)
	{
	case Direction::North:
		x2--;
		break;
	case Direction::South:
		x2++;
		break;
	case Direction::West:
		y2--;
		break;
	case Direction::East:
		y2++;
		break;
	default:
		break;
	}
	p.rows.at(x2).at(y2) = p.rows.at(x).at(y);
	p.rows.at(x).at(y) = '.';
	x = x2;
	y = y2;
}

std::vector<int> spinCyclePlatform(const Platform& p, const long long numCycles)
{
	auto pTilt = p;
	std::vector<int> weightSequence{};

	for (long long i = 0; i < numCycles; i++)
	{
		pTilt = tiltPlatform(pTilt, Direction::North);
		pTilt = tiltPlatform(pTilt, Direction::West);
		pTilt = tiltPlatform(pTilt, Direction::South);
		pTilt = tiltPlatform(pTilt, Direction::East);
		const auto w = computeWeights(pTilt);
		weightSequence.push_back(w);
	}
	return weightSequence;
}

string getPlatformString(const Platform& p)
{
	string out{};
	for (const auto& r : p.rows)
	{
		for (const auto& c : r)
		{
			out += c;
		}
		out += '\n';
	}
	return out;
}

Platform tiltPlatformNorth(const Platform& p)
{
	auto pTilt = p;
	// start from North
	for (int i = 0; i < pTilt.rows.size(); i++)
	{
		for (int j = 0; j < pTilt.cols.size(); j++)
		{
			auto i2 = i;
			auto j2 = j;
			while (canMove(pTilt, Direction::North, i2, j2))
			{
				performMove(pTilt, Direction::North, i2, j2);
			}
		}
	}
	
	return pTilt;
}

Platform tiltPlatformSouth(const Platform& p)
{
	auto pTilt = p;
	// start from South
	for (int i = static_cast<int>(pTilt.rows.size()) - 1; i >= 0; i--)
	{
		for (int j = 0; j < pTilt.cols.size(); j++)
		{
			auto i2 = i;
			auto j2 = j;
			while (canMove(pTilt, Direction::South, i2, j2))
			{
				performMove(pTilt, Direction::South, i2, j2);
			}
		}
	}

	return pTilt;
}

Platform tiltPlatformWest(const Platform& p)
{
	auto pTilt = p;
	// start from West
	for (int j = 0; j < pTilt.cols.size(); j++)
	{
		for (int i = 0; i < pTilt.rows.size(); i++)
		{
			auto i2 = i;
			auto j2 = j;
			while (canMove(pTilt, Direction::West, i2, j2))
			{
				performMove(pTilt, Direction::West, i2, j2);
			}
		}
	}

	return pTilt;
}

Platform tiltPlatformEast(const Platform& p)
{
	auto pTilt = p;
	// start from East
	for (int j = static_cast<int>(pTilt.cols.size()) - 1; j >= 0; j--)
	{
		for (int i = 0; i < pTilt.rows.size(); i++)
		{
			auto i2 = i;
			auto j2 = j;
			while (canMove(pTilt, Direction::East, i2, j2))
			{
				performMove(pTilt, Direction::East, i2, j2);
			}
		}
	}

	return pTilt;
}

Platform tiltPlatform(const Platform& p, const Direction& dir)
{
	Platform pTilt{};

	switch (dir)
	{
	case Direction::North:
		pTilt = tiltPlatformNorth(p);
		break;
	case Direction::South:
		pTilt = tiltPlatformSouth(p);
		break;
	case Direction::West:
		pTilt = tiltPlatformWest(p);
		break;
	case Direction::East:
		pTilt = tiltPlatformEast(p);
		break;
	default:
		break;
	}
	return pTilt;
}

int computeWeights(const Platform& p)
{
	auto sum = 0;
	for (auto i = 0; i < p.rows.size(); i++)
	{
		const auto r = p.rows.at(i);
		const auto rocks = std::count(r.begin(), r.end(), 'O');
		sum += static_cast<int>(rocks) * (static_cast<int>(p.rows.size()) - i);
	}

	return sum;
}

long long extrapolateWeight(std::vector<int> weightSequence, const long long reqCycles)
{
	/* after a transitory the weights sequence is periodic, so from the end of the sequence, 
	we count down until the first repetition and then extrapolate which index would contain the
	weight for the cycle required */
	
	const auto refWeight = weightSequence.back();
	auto idxWeight = weightSequence.size() - 2;
	auto weightFound = false;

	while (not(weightFound) and (idxWeight > 0))
	{
		if (weightSequence.at(idxWeight) == refWeight)
		{
			weightFound = true;
		}
		else
		{
			idxWeight--;
		}
	}

	const auto periodicity = weightSequence.size() - 1 - idxWeight;
	const auto div = static_cast<double>(reqCycles) / static_cast<double>(periodicity);
	const auto fdiv = static_cast<int>(floor(div) - 2.0f);
	const auto targetIdx = reqCycles - periodicity * fdiv - 1;

	return weightSequence.at(targetIdx);
}