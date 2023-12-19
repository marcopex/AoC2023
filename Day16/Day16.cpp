#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

bool gPrintInput = false;
bool gDebugEnabled = false;

struct Element
{
	char symbol;
	bool energizedGoingNorth;
	bool energizedGoingSouth;
	bool energizedGoingWest;
	bool energizedGoingEast;
};

struct Layout
{
	std::vector<std::vector<Element>> rows;
	std::vector<std::vector<Element>> cols;
};

enum class Direction : char
{
	Unknown,
	North,
	South,
	West,
	East
};

Layout getLayout(std::ifstream& inputFile);
void travelLayout(Layout& l, Direction dir, int x, int y, int recursionCnt);
int countEnergizedElements(const Layout& l);
int findBestStartingPoint(const Layout& l);

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

	const auto baseLayout = getLayout(inputFile);

	const auto startDir = Direction::East;
	auto tmpLayout = baseLayout;
	travelLayout(tmpLayout, startDir, 0, 0, 0);
	r1 = countEnergizedElements(tmpLayout);

	r2 = findBestStartingPoint(baseLayout);

	std::cout << "Result for Trial #1 is " << r1 << std::endl;
	std::cout << "Result for Trial #2 is " << r2 << std::endl;

	return 0;
}

string getDirectionString(const Direction& dir)
{
	string out{};

	switch (dir)
	{
		case Direction::North:
			out = "North";
			break;
		case Direction::South:
			out = "South";
			break;
		case Direction::East:
			out = "East";
			break;
		case Direction::West:
			out = "West";
			break;
		default:
			out = "Unknown";
			break;
	}
	return out;
}

std::vector<std::vector<Element>> getColumns(std::vector<std::vector<Element>>& rows)
{
	std::vector<std::vector<Element>> cols{};

	for (auto j = 0; j < rows.at(0).size(); j++)
	{
		std::vector<Element> elements;
		for (auto i = 0; i < rows.size(); i++)
		{
			const Element e{ rows.at(i).at(j).symbol, false };
			elements.push_back(e);
		}
		cols.push_back(elements);
	}
	return cols;
}

Layout getLayout(std::ifstream& inputFile)
{
	string inputString{ "" };
	Layout l;

	while (!inputFile.eof())
	{
		std::getline(inputFile, inputString, '\n');
		std::vector<Element> elements;
		for (const auto& c : inputString)
		{
			const Element e{ c, false };
			elements.push_back(e);
		}
		l.rows.push_back(elements);
	}
	if (l.rows.size() > 0)
	{
		l.cols = getColumns(l.rows);
	}

	return l;
}

bool canMove(const Layout& l, const Direction& dir, int x, int y)
{
	bool movePossible = true;
	auto x2 = x;
	auto y2 = y;
	switch (dir)
	{
	case Direction::North:
		if (x < 0)
		{
			movePossible = false;
		}
		x2--;
		break;
	case Direction::South:
		if (x >= (l.rows.size() - 1))
		{
			movePossible = false;
		}
		x2++;
		break;
	case Direction::West:
		if (y < 0)
		{
			movePossible = false;
		}
		y2--;
		break;
	case Direction::East:
		if (y >= (l.cols.size() - 1))
		{
			movePossible = false;
		}
		y2++;
		break;
	default:
		movePossible = false;
		break;
	}

	return movePossible;
}

void performMove(Layout& l, const Direction& dir, int& x, int& y)
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
	x = x2;
	y = y2;
}

Direction rotateDirection(const Direction dir, const char mirrorType)
{
	auto out = Direction::Unknown;

	switch (mirrorType)
	{
	case '/':
		switch (dir)
		{
		case Direction::North:
			out = Direction::East;
			break;
		case Direction::South:
			out = Direction::West;
			break;
		case Direction::West:
			out = Direction::South;
			break;
		case Direction::East:
			out = Direction::North;
			break;
		default:
			break;
		}
		break;
	case '\\':
		switch (dir)
		{
		case Direction::North:
			out = Direction::West;
			break;
		case Direction::South:
			out = Direction::East;
			break;
		case Direction::West:
			out = Direction::North;
			break;
		case Direction::East:
			out = Direction::South;
			break;
		default:
			break;
		}
		break;
	default:
		std::cout << "Error! Unknown mirrorType:" << mirrorType << std::endl;
		break;
	}
	return out;
}

bool isEnergized(const Element e, const Direction dir)
{
	switch (dir)
	{
	case Direction::North:
		return e.energizedGoingNorth;
	case Direction::South:
		return e.energizedGoingSouth;
	case Direction::West:
		return e.energizedGoingWest;
	case Direction::East:
		return e.energizedGoingEast;
	default:
		return false;
	}
}

void setEnergized(Element& e, const Direction dir, bool status)
{
	switch (dir)
	{
	case Direction::North:
		e.energizedGoingNorth = status;
		break;
	case Direction::South:
		e.energizedGoingSouth = status;
		break;
	case Direction::West:
		e.energizedGoingWest = status;
		break;
	case Direction::East:
		e.energizedGoingEast = status;
		break;
	default:
		return;
	}
}

void travelLayout(Layout& l, Direction dir, int x, int y, int recursionCnt)
{
	recursionCnt++;
	
	auto keepGoing = true;
	auto keepDirection = true;
	while (keepGoing)
	{
		if (gDebugEnabled)
		{
			std::cout << "[" << recursionCnt << "]Dir:" << getDirectionString(dir) << " on x:" << x << " y:" << y << " | ";
		}

		if ((x < 0) or (x >= l.rows.size()) or (y < 0) or (y >= l.cols.size()))
		{
			// exit condition
			if (gDebugEnabled)
			{
				std::cout << "Exit condition 1!" << std::endl;
			}
			keepDirection = false;
			keepGoing = false;
			break;
		}

		if (isEnergized(l.rows.at(x).at(y), dir))
		{
			// exit condition
			if (gDebugEnabled)
			{
				std::cout << "Exit condition 2!" << std::endl;
			}
			keepDirection = false;
			keepGoing = false;
			break;
		}

		//if (not(canMove(l, dir, x, y)))
		//{
		//	// exit condition
		//	setEnergized(l.rows.at(x).at(y), dir, true);
		//	std::cout << "Exit condition 3!" << std::endl;
		//	keepDirection = false;
		//	keepGoing = false;
		//	break;
		//}

		auto e = &l.rows.at(x).at(y);
		if (gDebugEnabled)
		{
			std::cout << ":{" << e->symbol << "} Flags:" <<
				e->energizedGoingNorth << "|" <<
				e->energizedGoingSouth << "|" <<
				e->energizedGoingWest << "|" <<
				e->energizedGoingEast << "|" <<
				std::endl;
		}

		setEnergized(l.rows.at(x).at(y), dir, true);

		switch (l.rows.at(x).at(y).symbol)
		{
		case '.':
		{
			// continue in the same direction
		}
		break;
		case '/':
		case '\\':
		{
			// 90 degrees rotation
			dir = rotateDirection(dir, l.rows.at(x).at(y).symbol);
		}
		break;
		case '|':
		{
			if ((dir == Direction::North) or (dir == Direction::South))
			{ // straight, same direction
			}
			else
			{ // split
				if (gDebugEnabled)
				{
					std::cout << "Splitting North-South" << std::endl;
				}
				auto x1 = x;
				auto y1 = y;
				performMove(l, Direction::North, x1, y1);
				travelLayout(l, Direction::North, x1, y1, recursionCnt);
				auto x2 = x;
				auto y2 = y;
				performMove(l, Direction::South, x2, y2);
				travelLayout(l, Direction::South, x2, y2, recursionCnt);
				if (gDebugEnabled)
				{
					std::cout << "Exit condition 4a!" << std::endl;
				}
				keepDirection = false;
				keepGoing = false;
			}
		}
		break;
		case '-':
		{
			if ((dir == Direction::West) or (dir == Direction::East))
			{ // straight, same direction
			}
			else
			{ // split
				if (gDebugEnabled)
				{
					std::cout << "Splitting West-East" << std::endl;
				}
				auto x1 = x;
				auto y1 = y;
				performMove(l, Direction::West, x1, y1);
				travelLayout(l, Direction::West, x1, y1, recursionCnt);
				auto x2 = x;
				auto y2 = y;
				performMove(l, Direction::East, x2, y2);
				travelLayout(l, Direction::East, x2, y2, recursionCnt);
				if (gDebugEnabled)
				{
					std::cout << "Exit condition 4b!" << std::endl;
				}
				keepDirection = false;
				keepGoing = false;
			}
		}
		break;
		default:
			break;
		}

		if (keepDirection)
		{
			performMove(l, dir, x, y);
		}
	}
}

int countEnergizedElements(const Layout& l)
{
	int cntEnergized = 0;

	for (const auto r : l.rows)
	{
		for (const auto e : r)
		{
			if (e.energizedGoingNorth or e.energizedGoingSouth or e.energizedGoingWest or e.energizedGoingEast)
			{
				cntEnergized++;
				if (gDebugEnabled)
				{
					std::cout << '#';
				}
			}
			else
			{
				if (gDebugEnabled)
				{
					std::cout << '.';
				}
			}
		}
		if (gDebugEnabled)
		{
			std::cout << std::endl;
		}
	}

	return cntEnergized;
}

int findBestStartingPoint(const Layout& l)
{
	auto maxEnergy = 0;

	// start from any point of the perimeter of the layout and find max energy
	const auto xstart = 0;
	const auto xstop = static_cast<int>(l.rows.size());
	const auto ystart = 0;
	const auto ystop = static_cast<int>(l.cols.size());
	
	auto tmpLayout = l;

	auto tmpEnergy = 0;
	auto dir = Direction::South;
	auto x = xstart;
	for (auto y = ystart; y < ystop; y++)
	{
		tmpLayout = l;
		travelLayout(tmpLayout, dir, x, y, 0);
		tmpEnergy = countEnergizedElements(tmpLayout);
		if (gDebugEnabled)
		{
			std::cout << "Dir:" << getDirectionString(dir) << " from:[" << x << "|" << y << "] energy:" << tmpEnergy << std::endl;
		}
		if (tmpEnergy > maxEnergy)
		{
			maxEnergy = tmpEnergy;
		}
	}

	tmpEnergy = 0;
	dir = Direction::North;
	x = xstop;
	for (auto y = ystart; y < ystop; y++)
	{
		tmpLayout = l;
		travelLayout(tmpLayout, Direction::North, x, y, 0);
		tmpEnergy = countEnergizedElements(tmpLayout);
		if (gDebugEnabled)
		{
			std::cout << "Dir:" << getDirectionString(dir) << " from:[" << x << "|" << y << "] energy:" << tmpEnergy << std::endl;
		}
		if (tmpEnergy > maxEnergy)
		{
			maxEnergy = tmpEnergy;
		}
	}

	tmpEnergy = 0;
	dir = Direction::East;
	auto y = ystart;
	for (auto x = xstart; x < xstop; x++)
	{
		tmpLayout = l;
		travelLayout(tmpLayout, dir, x, y, 0);
		tmpEnergy = countEnergizedElements(tmpLayout);
		if (gDebugEnabled)
		{
			std::cout << "Dir:" << getDirectionString(dir) << " from:[" << x << "|" << y << "] energy:" << tmpEnergy << std::endl;
		}
		if (tmpEnergy > maxEnergy)
		{
			maxEnergy = tmpEnergy;
		}
	}

	tmpEnergy = 0;
	dir = Direction::West;
	y = ystop;
	for (auto x = xstart; x < xstop; x++)
	{
		tmpLayout = l;
		travelLayout(tmpLayout, dir, x, y, 0);
		tmpEnergy = countEnergizedElements(tmpLayout);
		if (gDebugEnabled)
		{
			std::cout << "Dir:" << getDirectionString(dir) << " from:[" << x << "|" << y << "] energy:" << tmpEnergy << std::endl;
		}
		if (tmpEnergy > maxEnergy)
		{
			maxEnergy = tmpEnergy;
		}
	}
	
	if (gDebugEnabled)
	{
		std::cout << "MaxEnergy:" << maxEnergy << std::endl;
	}

	return maxEnergy;
}
