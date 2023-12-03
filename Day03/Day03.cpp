#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <regex>

using namespace std;
using matrix = std::vector<string>;

struct numberInfo
{
	int value;
	int row;
	int colStart;
	int colStop;
	bool isGearNearby;
	int rowGear;
	int colGear;
	bool isGearAccountedFor;

	bool operator!=(const numberInfo& n) const
	{
		return ((this->colStart != n.colStart) or (this->colStop != n.colStop) or (this->row != n.row));
	}
};


enum class SymbolType { SYMBOL_NOT_VALID, SYMBOL_VALID, SYMBOL_GEAR };

bool gPrintInput = false;
bool gDebugEnabled = false;

void createMatrix(const string& inputString, matrix& inputMatrix);
int parseValidNumbersInRow(const matrix& inputMatrix, const int rowNumber, std::vector<numberInfo>& numbersWithGears);
SymbolType checkSymbolType(const matrix& inputMatrix, const int rowNum, const int columnNum);
int checkGears(std::vector<numberInfo>& numbersWithGears);
bool CheckRowBelow(const matrix& inputMatrix, const int rowNumber, numberInfo& number);
bool CheckRowAbove(const matrix& inputMatrix, const int rowNumber, numberInfo& number);
bool CheckSameRow(const matrix& inputMatrix, const int rowNumber, numberInfo& number);

int main()
{
	string inputString{ "empty" };

	//std::ifstream inputFile("input_test_1.txt");
	//std::ifstream inputFile("input_real_1.txt");
	//std::ifstream inputFile("input_test_2.txt");
	std::ifstream inputFile("input_real_2.txt");

	int cumSumTrial1 = 0;
	int cumSumTrial2 = 0;

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

		matrix inputMatrix;
		int rowCnt = 0;
		std::vector<numberInfo> numbersWithGears;
		while (!inputFile.eof())
		{
			std::getline(inputFile, inputString, '\n');
			createMatrix(inputString, inputMatrix);
			rowCnt++;
		}
		inputFile.clear();
		inputFile.seekg(0);
		rowCnt = 0;
		while (!inputFile.eof())
		{
			std::getline(inputFile, inputString, '\n');
			cumSumTrial1 += parseValidNumbersInRow(inputMatrix, rowCnt, numbersWithGears);
			rowCnt++;
		}

		if (gDebugEnabled)
		{
			std::cout << "Numbers with Gears are " << numbersWithGears.size() << std::endl;
		}
		cumSumTrial2 = checkGears(numbersWithGears);

		inputFile.close();
	}
	else
	{
		cout << "Could not open input file!\n";
	}

	cout << "Result for Trial #1 is " << cumSumTrial1 << std::endl;
	cout << "Result for Trial #2 is " << cumSumTrial2 << std::endl;

	return 0;
}

void createMatrix(const string& inputString, matrix& inputMatrix)
{
	inputMatrix.push_back(inputString);
}

bool CheckSameRow(const matrix& inputMatrix, const int rowNumber, numberInfo& number)
{
	auto isSymbolAdjacent = false;

	if (checkSymbolType(inputMatrix, rowNumber, number.colStart - 1) == SymbolType::SYMBOL_VALID)
	{
		isSymbolAdjacent = true;
	}
	else if (checkSymbolType(inputMatrix, rowNumber, number.colStart - 1) == SymbolType::SYMBOL_GEAR)
	{
		isSymbolAdjacent = true;
		number.isGearNearby = true;
		number.rowGear = rowNumber;
		number.colGear = number.colStart - 1;
	}
	if (checkSymbolType(inputMatrix, rowNumber, number.colStop + 1) == SymbolType::SYMBOL_VALID)
	{
		isSymbolAdjacent = true;
	}
	else if (checkSymbolType(inputMatrix, rowNumber, number.colStop + 1) == SymbolType::SYMBOL_GEAR)
	{
		isSymbolAdjacent = true;
		number.isGearNearby = true;
		number.rowGear = rowNumber;
		number.colGear = number.colStop + 1;
	}
	return isSymbolAdjacent;
}

bool CheckRowAbove(const matrix& inputMatrix, const int rowNumber, numberInfo& number)
{
	auto isSymbolAdjacent = false;

	auto columnStart = 0;
	auto columnStop = 0;
	auto i = 0;

	columnStart = number.colStart - 1;
	columnStop = number.colStop + 1;
	i = columnStart;
	while (i <= columnStop)
	{
		if (checkSymbolType(inputMatrix, rowNumber - 1, i) == SymbolType::SYMBOL_VALID)
		{
			isSymbolAdjacent = true;
		}
		else if (checkSymbolType(inputMatrix, rowNumber - 1, i) == SymbolType::SYMBOL_GEAR)
		{
			isSymbolAdjacent = true;
			number.isGearNearby = true;
			number.rowGear = rowNumber - 1;
			number.colGear = i;
		}
		i++;
	}

	return isSymbolAdjacent;
}

bool CheckRowBelow(const matrix& inputMatrix, const int rowNumber, numberInfo& number)
{
	auto isSymbolAdjacent = false;

	auto columnStart = 0;
	auto columnStop = 0;
	auto i = 0;

	columnStart = number.colStart - 1;
	columnStop = number.colStop + 1;
	i = columnStart;
	while (i <= columnStop)
	{
		if (checkSymbolType(inputMatrix, rowNumber + 1, i) == SymbolType::SYMBOL_VALID)
		{
			isSymbolAdjacent = true;
		}
		else if (checkSymbolType(inputMatrix, rowNumber + 1, i) == SymbolType::SYMBOL_GEAR)
		{
			isSymbolAdjacent = true;
			number.isGearNearby = true;
			number.rowGear = rowNumber + 1;
			number.colGear = i;
		}
		i++;
	}

	return isSymbolAdjacent;
}

int parseValidNumbersInRow(const matrix& inputMatrix, const int rowNumber, std::vector<numberInfo>& numbersWithGears)
{
	// get numbers in row
	const auto& parsingString = inputMatrix.at(rowNumber);
	auto targetString = inputMatrix.at(rowNumber);
	std::vector<numberInfo> numbers;
	const std::regex regexExp(R"(\d+)");   // matches a sequence of digits
	std::smatch matchString;

	size_t charCnt = 0;
	while (std::regex_search(targetString, matchString, regexExp))
	{
		const auto tempNumber = std::stoi(matchString.str());
		const auto colStart = charCnt + targetString.find_first_of(matchString.str());
		const auto colStop = colStart + matchString.str().length() - 1;

		targetString = matchString.suffix();
		charCnt = colStop + 1;

		const numberInfo numInfo{ tempNumber, rowNumber, static_cast<int>(colStart), static_cast<int>(colStop), false, 0, 0, false };
		numbers.emplace_back(numInfo);
	}

	if (gDebugEnabled)
	{
		std::cout << "Found " << numbers.size() << " numbers: ";
		for (const auto& number : numbers)
		{
			std::cout << number.value << " [" << number.colStart << "|" << number.colStop << "] | ";
		}
		std::cout << std::endl;
	}

	// for each number, check if it has valid characters nearby
	int totalSum = 0;
	for (auto& number : numbers)
	{
		// check on same row
		bool isSameRowAdjacent = CheckSameRow(inputMatrix, rowNumber, number);

		// check on row above
		bool isAboveRowAdjacent = CheckRowAbove(inputMatrix, rowNumber, number);

		// check on row above
		bool isBelowRowAdjacent = CheckRowBelow(inputMatrix, rowNumber, number);

		if (number.isGearNearby)
		{
			numbersWithGears.push_back(number);
		}

		if (gDebugEnabled)
		{
			if (isSameRowAdjacent or isAboveRowAdjacent or isBelowRowAdjacent)
			{
				std::cout << "Number " << number.value << " has an adjacent symbol!" << std::endl;
			}
			else
			{
				std::cout << "Number " << number.value << " has NO adjacent symbol!" << std::endl;
			}

			if (number.isGearNearby)
			{
				std::cout << "Number " << number.value << " has a nearby gear at [" << number.rowGear << "|" << number.colGear << "]" << std::endl;
			}

		}

		if (isSameRowAdjacent or isAboveRowAdjacent or isBelowRowAdjacent)
		{
			totalSum += number.value;
		}
	}

	// return sum of valid numbers
	return totalSum;
}

SymbolType checkSymbolType(const matrix& inputMatrix, const int rowNum, const int columnNum)
{
	if ((rowNum < 0) or (rowNum >= inputMatrix.at(0).length()))
	{
		return SymbolType::SYMBOL_NOT_VALID;
	}
	if ((columnNum < 0) or (columnNum >= inputMatrix.size()))
	{
		return SymbolType::SYMBOL_NOT_VALID;
	}

	const auto targetChar(inputMatrix.at(rowNum).at(columnNum));

	if (targetChar == '.')
	{
		return SymbolType::SYMBOL_NOT_VALID;
	}
	else if (targetChar == '*')
	{
		return SymbolType::SYMBOL_GEAR;
	}
	else
	{
		return SymbolType::SYMBOL_VALID;
	}
}

int checkGears(std::vector<numberInfo>& numbersWithGears)
{
	auto gearRatio = 0;

	for (auto& number : numbersWithGears)
	{
		if (not(number.isGearAccountedFor))
		{
			if (gDebugEnabled)
			{
				std::cout << "Gear check for n1 value:" << number.value << " row:" << number.rowGear << " col:" << number.colGear << std::endl;
			}
			const auto it = find_if(numbersWithGears.begin(), numbersWithGears.end(),
				[&number](const numberInfo n)
				{ return (
					(n.rowGear == number.rowGear) and
					(n.colGear == number.colGear) and
					not(n.isGearAccountedFor) and
					(n != number)); });
			if (it != numbersWithGears.end())
			{
				number.isGearAccountedFor = true;
				it->isGearAccountedFor = true;
				gearRatio += number.value * it->value;
				if (gDebugEnabled)
				{
					std::cout << "Gear match! n1 value:" << number.value << " row:" << number.rowGear << " col:" << number.colGear << " | n2 value:" << it->value << " row :" << it->rowGear << " col:" << it->colGear << std::endl;
				}
			}
			else
			{
				if (gDebugEnabled)
				{
					std::cout << "Gear not found ! n1 value:" << number.value << " row:" << number.rowGear << " col:" << number.colGear << std::endl;
				}
			}
		}
	}
	return gearRatio;
}
