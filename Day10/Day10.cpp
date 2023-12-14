#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <regex>
#include <algorithm>

using namespace std;

bool gPrintInput = false;
bool gDebugEnabled = false;

enum class Symbol : char
{
	Erased,
	Unknown,
	Nothing,
	PipeHor,
	PipeVer,
	AngTopLeft,
	AngTopRight,
	AngBottomLeft,
	AngBottomRight,
	Source
};

struct Element
{
	Symbol symbol;
	int weight;
	bool onPath;
	bool inside;
};

using Matrix = std::vector<std::vector<Element>>;

const int DEFAULT_WEIGHT = 0xFFFF;

enum class Direction : char
{
	Unknown,
	Left,
	Up,
	Right,
	Down,
};

struct Coordinate
{
	int row{};
	int col{};

	bool operator==(const Coordinate& c) const
	{
		return ((this->row == c.row) and (this->col == c.col));
	}
};

Matrix createMatrix(std::ifstream& inputFile);
Coordinate findStartingPoint(const Matrix& matrix);
void parseMatrix(const Matrix& matrix);
int getFarthestPosition(const Matrix& matrix);
int walkPaths(const Coordinate start, Matrix& matrix);
int countAreaEnclosedByLoop(Matrix& matrix);
void cleanMatrix(Matrix& matrix);

int main()
{
	string inputString{ "empty" };

	//std::ifstream inputFile("input_test_1.txt");
	//std::ifstream inputFile("input_test_2.txt");
	//std::ifstream inputFile("input_test_3.txt");
	//std::ifstream inputFile("input_test_4.txt");
	//std::ifstream inputFile("input_test_5.txt");
	//std::ifstream inputFile("input_test_6.txt");
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

		auto matrix = createMatrix(inputFile);
		const auto s = findStartingPoint(matrix);
		
		if (gDebugEnabled)
		{
			std::cout << "Starting point at [" << s.row << "-" << s.col << "]" << std::endl;
		}
		
		r1 = walkPaths(s, matrix);
		cleanMatrix(matrix);
		r2 = countAreaEnclosedByLoop(matrix);
	}
	else
	{
		std::cout << "Could not open input file!" << std::endl;
	}

	std::cout << "Result for Trial #1 is " << r1 << std::endl;
	std::cout << "Result for Trial #2 is " << r2 << std::endl;

	return 0;
}

Symbol getSymbol(const char inputChar)
{
	Symbol out = Symbol::Unknown;
	switch (inputChar)
	{
	case '-':
		out = Symbol::PipeHor;
		break;
	case '|':
		out = Symbol::PipeVer;
		break;
	case 'F':
		out = Symbol::AngTopLeft;
		break;
	case '7':
		out = Symbol::AngTopRight;
		break;
	case 'L':
		out = Symbol::AngBottomLeft;
		break;
	case 'J':
		out = Symbol::AngBottomRight;
		break;
	case 'S':
		out = Symbol::Source;
		break;
	default:
		out = Symbol::Nothing;
		break;
	}
	return out;
}

char getChar(const Symbol sym)
{
	char out = ' ';
	switch (sym)
	{
	case Symbol::Erased:
		out = 'e';
		break;
	case Symbol::PipeHor:
		out = '-';
		break;
	case Symbol::PipeVer:
		out = '|';
		break;
	case Symbol::AngTopLeft:
		out = 'F';
		break;
	case Symbol::AngTopRight:
		out = '7';
		break;
	case Symbol::AngBottomLeft:
		out = 'L';
		break;
	case Symbol::AngBottomRight:
		out = 'J';
		break;
	case Symbol::Source:
		out = 'S';
		break;
	case Symbol::Nothing:
		out = '.';
		break;
	default:
		out = '?';
		break;
	}
	
	return out;
}

Matrix createMatrix(std::ifstream& inputFile)
{
	string inputString{ "" };
	Matrix matrix;
	auto rowCnt = 0;

	while (!inputFile.eof())
	{
		std::vector<Element> symbols{};

		std::getline(inputFile, inputString, '\n');
		for (const auto& c : inputString)
		{
			auto sym = getSymbol(c);
			symbols.push_back({ sym, DEFAULT_WEIGHT });
		}
		matrix.push_back(symbols);
		rowCnt++;
	}

	return matrix;
}

Coordinate findStartingPoint(const Matrix& matrix)
{
	Coordinate c{ 0xFFFF, 0xFFFF };

	auto cntRow = 0;
	auto cntCol = 0;

	for (const auto row : matrix)
	{
		cntCol = 0;
		for (const auto col : row)
		{
			if (col.symbol == Symbol::Source)
			{
				c.row = cntRow;
				c.col = cntCol;
			}
			cntCol++;
		}
		cntRow++;
	}
	return c;
}

string getCoordinateString(const Coordinate c)
{
	string out{};
	out += "[";
	out += std::to_string(c.row);
	out += "-";
	out += std::to_string(c.col);
	out += "]";

	return out;
}

string getDirectionString(const Direction dir)
{
	string out{};

	switch (dir)
	{
	case Direction::Up:
		out = "UP";
		break;
	case Direction::Right:
		out = "RIGHT";
		break;
	case Direction::Down:
		out = "DOWN";
		break;
	case Direction::Left:
		out = "LEFT";
		break;
	case Direction::Unknown:
		out = "UNKNOWN";
		break;
	default:
		out = "ERROR";
		break;
	}
	return out;
}

string getSymbolString(const Symbol sym)
{
	string out{};

	switch (sym)
	{
	case Symbol::Unknown:
		out = '?';
		break;
	case Symbol::Nothing:
		out = '.';
		break;
	case Symbol::PipeHor:
		out = '-';
		break;
	case Symbol::PipeVer:
		out = '|';
		break;
	case Symbol::AngTopLeft:
		out = 'F';
		break;
	case Symbol::AngTopRight:
		out = '7';
		break;
	case Symbol::AngBottomLeft:
		out = 'L';
		break;
	case Symbol::AngBottomRight:
		out = 'J';
		break;
	case Symbol::Source:
		out = 'S';
		break;
	default:
		out = "O";
		break;
	}
	return out;
}

bool canGoUp(const Coordinate c, const Matrix& matrix)
{
	if (c.row == 0)
	{
		return false;
	}
	const auto nowx = c.row;
	const auto nowy = c.col;
	const auto futx = nowx - 1;
	const auto futy = nowy;
	auto out = false;

	const auto nowSym = matrix.at(nowx).at(nowy).symbol;
	const auto futSym = matrix.at(futx).at(futy).symbol;

	if (((futSym == Symbol::PipeVer) or (futSym == Symbol::AngTopLeft) or (futSym == Symbol::AngTopRight))
		and ((nowSym == Symbol::PipeVer) or (nowSym == Symbol::AngBottomLeft) or (nowSym == Symbol::AngBottomRight) or (nowSym == Symbol::Source)))
	{
		out = true;
	}
	else if ((futSym == Symbol::Source) and (
		(nowSym == Symbol::PipeVer) or
		(nowSym == Symbol::AngBottomLeft) or
		(nowSym == Symbol::AngBottomRight)))
	{
		out = true;
	}
	else
	{
		out = false;
	}

	return out;
}

bool canGoDown(const Coordinate c, const Matrix& matrix)
{
	if (c.row == matrix.size() - 1)
	{
		return false;
	}
	const auto nowx = c.row;
	const auto nowy = c.col;
	const auto futx = nowx + 1;
	const auto futy = nowy;
	auto out = false;
	
	const auto nowSym = matrix.at(nowx).at(nowy).symbol;
	const auto futSym = matrix.at(futx).at(futy).symbol;
	
	if (((futSym == Symbol::PipeVer) or (futSym == Symbol::AngBottomLeft) or (futSym == Symbol::AngBottomRight))
		and ((nowSym == Symbol::PipeVer) or (nowSym == Symbol::AngTopLeft) or (nowSym == Symbol::AngTopRight) or (nowSym == Symbol::Source)))
	{
		out = true;
	}
	else if ((futSym == Symbol::Source) and (
		(nowSym == Symbol::PipeVer) or
		(nowSym == Symbol::AngTopLeft) or
		(nowSym == Symbol::AngTopRight)))
	{
		out = true;
	}
	else
	{
		out = false;
	}
	
	return out;
}

bool canGoLeft(const Coordinate c, const Matrix& matrix)
{
	if (c.col == 0)
	{
		return false;
	}
	const auto nowx = c.row;
	const auto nowy = c.col;
	const auto futx = nowx;
	const auto futy = nowy - 1;
	auto out = false;

	const auto nowSym = matrix.at(nowx).at(nowy).symbol;
	const auto futSym = matrix.at(futx).at(futy).symbol;

	if (((futSym == Symbol::PipeHor) or (futSym == Symbol::AngBottomLeft) or (futSym == Symbol::AngTopLeft))
		and ((nowSym == Symbol::PipeHor) or (nowSym == Symbol::AngBottomRight) or (nowSym == Symbol::AngTopRight) or (nowSym == Symbol::Source)))
	{
		out = true;
	}
	else if ((futSym == Symbol::Source) and (
		(nowSym == Symbol::PipeHor) or
		(nowSym == Symbol::AngBottomRight) or
		(nowSym == Symbol::AngTopRight)))
	{
		out = true;
	}
	else
	{
		out = false;
	}

	return out;
}

bool canGoRight(const Coordinate c, const Matrix& matrix)
{
	if (c.col == matrix.at(0).size() - 1)
	{
		return false;
	}
	const auto nowx = c.row;
	const auto nowy = c.col;
	const auto futx = nowx;
	const auto futy = nowy + 1;
	auto out = false;
	
	const auto nowSym = matrix.at(nowx).at(nowy).symbol;
	const auto futSym = matrix.at(futx).at(futy).symbol;

	if (((futSym == Symbol::PipeHor) or (futSym == Symbol::AngBottomRight) or (futSym == Symbol::AngTopRight))
		and ((nowSym == Symbol::PipeHor) or (nowSym == Symbol::AngTopLeft) or (nowSym == Symbol::AngBottomLeft) or (nowSym == Symbol::Source)))
	{
		out = true;
	}
	else if ((futSym == Symbol::Source) and (
		(nowSym == Symbol::PipeHor) or
		(nowSym == Symbol::AngTopLeft) or
		(nowSym == Symbol::AngBottomLeft)))
	{
		out = true;
	}
	else
	{
		out = false;
	}

	return out;
}

int getFarthestPosition(const Matrix& matrix)
{
	int maxDistance = 0;
	auto cntRow = 0;
	auto cntCol = 0;
	for (const auto row : matrix)
	{
		cntCol = 0;
		for (const auto col : row)
		{
			const int weight = matrix.at(cntRow).at(cntCol).weight;

			if ((weight > maxDistance) and (weight != DEFAULT_WEIGHT))
			{
				maxDistance = weight;
			}
			cntCol++;
		}
		cntRow++;
	}
	
	return maxDistance;
}

void parseMatrix(const Matrix& matrix)
{
	auto cntRow = 0;
	auto cntCol = 0;
	for (const auto row : matrix)
	{
		cntCol = 0;
		for (const auto col : row)
		{
			const Coordinate c{ cntRow, cntCol };
			if (gDebugEnabled)
			{
				std::cout << "[" << cntRow << "-" << cntCol << "]"
					<< " Left?" << canGoLeft(c, matrix)
					<< " Up?" << canGoUp(c, matrix)
					<< " Right?" << canGoRight(c, matrix)
					<< " Down?" << canGoDown(c, matrix)
					<< std::endl;
				cntCol++;
			}
		}
		cntRow++;
	}
}

Direction determineNextDirection(const Coordinate now, const Direction prevDir, const Matrix& matrix)
{
	// clockwise is preferential
	Direction nextDir = Direction::Unknown;
	
	if (canGoUp(now, matrix) and (prevDir != Direction::Down))
	{
		nextDir = Direction::Up;
	}
	else if (canGoRight(now, matrix) and (prevDir != Direction::Left))
	{
		nextDir = Direction::Right;
	}
	else if (canGoDown(now, matrix) and (prevDir != Direction::Up))
	{
		nextDir = Direction::Down;
	}
	else if (canGoLeft(now, matrix) and (prevDir != Direction::Right))
	{
		nextDir = Direction::Left;
	}

	return nextDir;
}

Coordinate moveInDirection(Coordinate now, const Matrix& matrix, Direction dir)
{
	Coordinate then{ 0, 0 };
	then = now;
	switch (dir)
	{
	case Direction::Up:
		then.row--;
		break;
	case Direction::Right:
		then.col++;
		break;
	case Direction::Down:
		then.row++;
		break;
	case Direction::Left:
		then.col--;
		break;
	default:
		break;
	}
	return then;
}

void navigateMatrix(const Coordinate start, const Direction startDir, Matrix& matrix)
{
	auto reachedStop = false;
	auto cntSteps = 0;
	const Coordinate stop = start;
	Coordinate now = start;
	Coordinate next = start;
	auto dir = startDir;

	matrix.at(start.row).at(start.col).weight = 0;
	matrix.at(start.row).at(start.col).onPath = true;

	while (not(reachedStop))
	{
		if (cntSteps > 0)
		{
			dir = determineNextDirection(now, dir, matrix);
		}
		if (dir == Direction::Unknown)
		{
			if (gDebugEnabled)
			{
				std::cout << "Stopping! Can't go anywhere" << std::endl;
			}
			reachedStop = true;
		}
		else
		{
			next = moveInDirection(now, matrix, dir);
			if (gDebugEnabled)
			{
				std::cout << "Moving from " << getCoordinateString(now) << " to " << getCoordinateString(next) << " Direction: " << getDirectionString(dir) << std::endl;
			}
			now = next;
			matrix.at(now.row).at(now.col).onPath = true;
			cntSteps++;

			if (cntSteps < matrix.at(now.row).at(now.col).weight)
			{
				matrix.at(now.row).at(now.col).weight = cntSteps;
			}

			if ((cntSteps > 0) and (now == stop))
			{
				if (gDebugEnabled)
				{
					std::cout << "Stopping! Destination reached in steps:" << cntSteps << std::endl;
				}
				reachedStop = true;
			}
		}
	}
}

int walkPaths(const Coordinate start, Matrix& matrix)
{
	if (canGoUp(start, matrix))
	{
		navigateMatrix(start, Direction::Up, matrix);
	}
	
	if (canGoRight(start, matrix))
	{
		navigateMatrix(start, Direction::Right, matrix);
	}
	
	if (canGoDown(start, matrix))
	{
		navigateMatrix(start, Direction::Down, matrix);
	}
	
	if (canGoLeft(start, matrix))
	{
		navigateMatrix(start, Direction::Left, matrix);
	}

	return getFarthestPosition(matrix);
}

void cleanMatrix(Matrix& matrix)
{
	for (auto& row : matrix)
	{
		for (auto& elem : row)
		{
			if (not(elem.onPath))
			{
				elem.inside = false;
				elem.symbol = Symbol::Erased;
				elem.weight = 0;
			}
		}
	}
}

int countAreaEnclosedByLoop(Matrix& matrix)
{
	auto cntArea = 0;

	// set flags
	for (auto& row : matrix)
	{
		auto cntWalls = 0;
		for (auto& elem : row)
		{
			if ((elem.symbol == Symbol::PipeVer) or (elem.symbol == Symbol::AngTopRight) or (elem.symbol == Symbol::AngTopLeft) or (elem.symbol == Symbol::Source))
			{
				cntWalls++;
			}

			if ((elem.symbol == Symbol::Nothing) or (elem.symbol == Symbol::Erased))
			{
				if ((cntWalls % 2) == 0)
				{ // even
					elem.inside = false;
				}
				else
				{ // odd
					elem.inside = true;
				}
			}
		}
	}

	// count elements
	for (auto& row : matrix)
	{
		for (auto& elem : row)
		{
			if (elem.inside)
			{
				cntArea++;
			}
			
			if (gDebugEnabled)
			{
				if (elem.onPath)
				{
					std::cout << '*';
				}
				else if (elem.inside)
				{
					std::cout << 'I';
				}
				else
				{
					std::cout << getChar(elem.symbol);
				}
			}
		}
		if (gDebugEnabled)
		{
			std::cout << std::endl;
		}
	}

	return cntArea;
}
