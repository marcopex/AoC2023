#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

bool gPrintInput = false;
bool gDebugEnabled = false;

int getCalibrationValueFromString(const string& inputString);
int getCalibrationsValueFromStringButReadingNumbersAsWords(const string& inputString);

int main()
{
	string inputString{ "empty" };
	
	//std::ifstream inputFile("input_test_1.txt");
	//std::ifstream inputFile("input_real_1.txt");
	//std::ifstream inputFile("input_test_2.txt");
	std::ifstream inputFile("input_real_2.txt");

	int cumSumTrial1 = 0;
	int cumSumTrial2 = 0;

	cout << "Script starts here" << std::endl;
	
	if (inputFile.is_open())
	{
		if (gPrintInput)
		{
			cout << "Input file content is" << std::endl;
			cout << "**********************" << std::endl;
			while (!inputFile.eof())
			{
				inputFile >> inputString;
				cout << inputString << std::endl;
			}
			cout << "**********************" << std::endl;
			inputFile.clear();
			inputFile.seekg(0);
		}
		while (!inputFile.eof())
		{
			inputFile >> inputString;
			cumSumTrial1 += getCalibrationValueFromString(inputString);
			cumSumTrial2 += getCalibrationsValueFromStringButReadingNumbersAsWords(inputString);
		}
		inputFile.close();
	}
	else
	{
		cout << "Could not open input file!" << std::endl;
	}

	cout << "Result for Trial #1 is " << cumSumTrial1 << std::endl;
	cout << "Result for Trial #2 is " << cumSumTrial2 << std::endl;

	return 0;
}

int getCalibrationValueFromString(const string& inputString)
{
	const auto firstDigit = inputString.find_first_of("0123456789");
	const auto lastDigit = inputString.find_last_of("0123456789");

	if ((firstDigit != string::npos) and (lastDigit != string::npos))
	{
		const auto d1 = inputString[firstDigit];
		const auto d2 = inputString[lastDigit];
		const auto s = string(1, d1) + string(1, d2);
		char* pEnd;
		const auto value = strtol(s.c_str(), &pEnd, 0);

		if (gDebugEnabled)
		{
			cout << "Parsing: " << inputString << " | ";
			cout << "Indexes: " << firstDigit << " and " << lastDigit << " | ";
			cout << "Strings: " << inputString[firstDigit] << " and " << inputString[lastDigit] << " | ";
			cout << "Value: " << value << std::endl;
		}
		return value;
	}
	else
	{
		cout << "Digits not found!";
		return 0;
	}
}

int getCalibrationsValueFromStringButReadingNumbersAsWords(const string& inputString)
{
	const std::vector<string> goalStrings{ "one", "two", "three", "four", "five", "six", "seven", "eight", "nine", "1", "2", "3", "4", "5", "6", "7", "8", "9" };

	string firstString{ "" };
	string lastString{ "" };

	size_t firstCnt{ 0 };
	size_t lastCnt{ 0 };

	auto stringCnt = 0;

	// look for the first string
	size_t earlyIndex = 0xFFFF;
	for (const auto& goalString : goalStrings)
	{
		auto index = inputString.find(goalString);
		if ((index != std::string::npos) and (index < earlyIndex))
		{
			earlyIndex = index;
			firstString = goalString;
			firstCnt = stringCnt;
		}
		stringCnt++;
	}

	// look for the last string
	size_t lateIndex = 0;
	stringCnt = 0;
	for (const auto& goalString : goalStrings)
	{
		size_t index = 0;
		size_t upToHere = 0;
		while (index != std::string::npos)
		{
			index = inputString.find(goalString, upToHere);
			if (index == std::string::npos)
			{
				// do nothing				
			}
			else
			{
				upToHere = index + 1;
				if (index > lateIndex)
				{
					lateIndex = index;
					lastString = goalString;
					lastCnt = stringCnt;
				}
			}

		}
		stringCnt++;
	}

	if (lateIndex == 0)
	{ // not found, use firstCnt
		lastCnt = firstCnt;
	}

	if ((earlyIndex == 0xFFFF))
	{
		cout << "Digits not found!" << std::endl;
		return 0;
	}

	auto d1 = static_cast<uint32_t>(((firstCnt % 9) + 1) * 10);
	auto d2 = static_cast<uint32_t>(((lastCnt % 9) + 1));
	auto value = d1 + d2;

	if (gDebugEnabled)
	{
		cout << "Parsing: " << inputString << " | ";
		cout << "Indexes: " << earlyIndex << " and " << lateIndex << " | ";
		cout << "Strings: " << goalStrings[firstCnt] << " and " << goalStrings[lastCnt] << " | ";
		cout << "Numbers: " << d1 << " and " << d2 << " | ";
		cout << "Value: " << value << std::endl;
	}

	return value;
}
