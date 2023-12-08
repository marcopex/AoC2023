#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iostream>
#include <numeric>

using namespace std;

bool gPrintInput = false;
bool gDebugEnabled = false;

enum class Direction : char
{
	None,
	Left,
	Right
};

struct Path
{
	int id{};
	string src{};
	string dstLeft{};
	string dstRight{};
};

struct CycleInfo
{
	int id{};
	int periodicity{};
};

std::vector<Direction> getDirections(std::ifstream& inputFile);
std::vector<Path> getPaths(std::ifstream& inputFile);
int computeSteps(const std::vector<Path>& paths, const std::vector<Direction>& directions, const string srcString, const string dstString);
long long computeStepsMultiple(const std::vector<Path>& paths, const std::vector<Direction>& directions, const string srcString, const string dstString);

constexpr int placeStringLength = 3;

constexpr auto lcm(auto x, auto... xs)
{
	return ((x = std::lcm(x, xs)), ...);
}

string getStr(Direction direction)
{
	string out{};

	switch (direction)
	{
	case Direction::Left:
		out = "Left";
		break;
	case Direction::Right:
		out = "Right";
		break;
	case Direction::None:
		out = "None";
		break;
	default:
		out = "Unknown";
		break;
	}
	return out;
}

int main()
{
	string inputString{ "empty" };

	//std::ifstream inputFile("input_test_1.txt");
	//std::ifstream inputFile("input_test_2.txt");
	//std::ifstream inputFile("input_real_1.txt");
	//std::ifstream inputFile("input_test_3.txt");
	std::ifstream inputFile("input_real_2.txt");

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

		const auto directions = getDirections(inputFile);
		const auto paths = getPaths(inputFile);

		r1 = computeSteps(paths, directions, "AAA", "ZZZ");
		r2 = computeStepsMultiple(paths, directions, "AAA", "ZZZ");

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

std::vector<Direction> getDirections(std::ifstream& inputFile)
{
	string inputString{ "empty" };
	std::vector<Direction> directions{};

	auto readDirections = true;

	while (!inputFile.eof() and readDirections)
	{
		std::getline(inputFile, inputString, '\n');
		
		int idxString = 0;
		while (idxString < inputString.length())
		{
			Direction direction = Direction::None;
			switch (inputString.at(idxString))
			{
			case 'L':
				direction = Direction::Left;
				break;
			case 'R':
				direction = Direction::Right;
				break;
			default:
				break;
			}
			idxString++;
			directions.push_back(direction);
		}
		if (directions.size() > 0)
		{
			readDirections = false;
		}
	}
	return directions;
}

std::vector<Path> getPaths(std::ifstream& inputFile)
{
	string inputString{ "empty" };
	std::vector<Path> paths{};

	int cntLines = 0;
	while (!inputFile.eof())
	{
		std::getline(inputFile, inputString, '\n');
		
		std::size_t const npos1 = inputString.find_first_of('=');
		std::size_t const npos2 = inputString.find_first_of('(');
		std::size_t const npos3 = inputString.find_first_of(',');
		std::size_t const npos4 = inputString.find_first_of(')');

		if ((npos1 == string::npos) or (npos2 == string::npos) or (npos3 == string::npos) or (npos4 == string::npos))
		{
			continue;
		}
		
		auto srcString = inputString.substr(0, placeStringLength);
		auto dstLeftString = inputString.substr(npos2 + 1, placeStringLength);
		auto dstRightString = inputString.substr(npos3 + 2, placeStringLength);

		paths.push_back({ cntLines, srcString, dstLeftString, dstRightString });
		
		cntLines++;
	}
	return paths;
}

Direction getNextDirection(const std::vector<Direction>& directions, int& idx)
{
	auto out = Direction::None;
	out = directions.at(idx);
	idx = (idx + 1) % directions.size();
	return out;
}

bool getNextPath(const std::vector<Path>& paths, const Path& currPath, const Direction dir, Path& nextPath)
{
	string dst{};

	switch (dir)
	{
	case Direction::Left:
		dst = currPath.dstLeft;
		break;
	case Direction::Right:
		dst = currPath.dstRight;
		break;
	default:
		dst = "";
		break;
	}
	const auto it = find_if(paths.begin(), paths.end(),
		[dst](const Path p)
		{ return (dst.compare(p.src) == 0); });
	
	if (it != paths.end())
	{
		nextPath = *it;
		return true;
	}
	else
	{
		return false;
	}
}

int computeSteps(const std::vector<Path>& paths, const std::vector<Direction>& directions, const string srcString, const string dstString)
{
	// find source
	const auto it = find_if(paths.begin(), paths.end(),
		[srcString](const Path p)
		{ return (srcString.compare(p.src) == 0); });
	if (it == paths.end())
	{
		return 0;
	}

	// navigate
	auto dstFound = false;
	auto idxDir = 0;
	auto currPath = *it;
	auto nextPath = *it;
	auto cntSteps = 0;
	while (not(dstFound))
	{
		const auto nextDir = getNextDirection(directions, idxDir);
		if (getNextPath(paths, currPath, nextDir, nextPath))
		{
			cntSteps++;
			if (nextPath.src.compare(dstString) == 0)
			{
				dstFound = true;
			}
			else
			{
				currPath = nextPath;
			}
		}
		else
		{
			std::cout << "Error!" << std::endl;
		}
	}

	return cntSteps;
}

long long computeStepsMultiple(const std::vector<Path>& paths, const std::vector<Direction>& directions, const string srcString, const string dstString)
{
	// find sources
	std::vector<Path> multiplePaths{};
	std::vector<CycleInfo> cycles{};
	auto idxCnt = 0;
	for (const auto& path : paths)
	{
		if (path.src.at(2) == 'A')
		{
			multiplePaths.push_back(path);
			cycles.push_back({ idxCnt, 0 });
			idxCnt++;
		}
	}

	if (gDebugEnabled)
	{
		for (const auto& src : multiplePaths)
		{
			std::cout << "Multiple Paths - Source found at " << src.src << std::endl;
		}
	}

	//// navigate
	auto cyclesAcquired = false;
	auto idxDir = 0;
	auto cntSteps = 0;
	int idxPath = 0;
	while (not(cyclesAcquired))
	{
		const auto nextDir = getNextDirection(directions, idxDir);
		int idxPath = 0;
		for (auto& currPath : multiplePaths)
		{
			Path nextPath;
			if (getNextPath(paths, currPath, nextDir, nextPath))
			{
				if (nextPath.src.at(2) != 'Z')
				{
					multiplePaths.at(idxPath) = nextPath;
				}
				else
				{
					if (gDebugEnabled)
					{
						std::cout << "Idx:" << idxPath << " Dir:" << getStr(nextDir) << " Steps:" << cntSteps << " | ";
						std::cout << "From:" << currPath.src << " To:" << nextPath.src << "[Path:" << currPath.src << "->[" << currPath.dstLeft << "|" << currPath.dstRight << "]" << std::endl;
					}
					multiplePaths.at(idxPath) = nextPath;
					if (cycles.at(idxPath).periodicity == 0)
					{
						cycles.at(idxPath).periodicity = cntSteps + 1;
						if (gDebugEnabled)
						{
							std::cout << "Cycle #" << idxPath << " Periodicity:" << cycles.at(idxPath).periodicity << std::endl;
						}
					}
				}
			}
			else
			{
				std::cout << "Error!" << std::endl;
			}
			idxPath++;
		}
		cntSteps++;

		if (gDebugEnabled)
		{
			if ((cntSteps % 1000) == 0)
			{
				std::cout << "Steps so far: " << cntSteps << std::endl;
			}
		}

		cyclesAcquired = true;
		for (const auto& cycle : cycles)
		{
			if (cycle.periodicity == 0)
			{
				cyclesAcquired = false;
			}
		}
	}

	if (gDebugEnabled)
	{
		for (const auto& cycle : cycles)
		{
			std::cout << "Cycle #" << cycle.id << " Periodicity:" << cycle.periodicity << std::endl;
		}
	}

	long long totSteps = 1;
	for (const auto& cycle : cycles)
	{
		totSteps = std::lcm(totSteps, cycle.periodicity);
	}

	return totSteps;
}
