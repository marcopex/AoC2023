#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>

using namespace std;

bool gPrintInput = false;
bool gDebugEnabled = false;

std::vector<string> getExtractionString(const string& inputString, const char* delimiters);
std::unordered_map<string, long> getLargestPresence(const std::vector<string>& singleExtractionString);
bool isExtractionPossible(const std::unordered_map<string, long>& extractionPresences);
int computePower(const std::unordered_map<string, long>& extractionPresences);

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
			cout << "Input file content is\n";
			cout << "**********************\n";
			while (!inputFile.eof())
			{
				std::getline(inputFile, inputString, '\n');
				cout << inputString << std::endl;
			}
			cout << "**********************\n";
			inputFile.clear();
			inputFile.seekg(0);
		}

		while (!inputFile.eof())
		{
			std::getline(inputFile, inputString, '\n');
			const auto extractionString = getExtractionString(inputString, ":;");
			const auto presences = getLargestPresence(extractionString);
			if (isExtractionPossible(presences))
			{
				cumSumTrial1 += presences.find("id")->second;
			}
			cumSumTrial2 += computePower(presences);
		}
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

std::vector<string> getExtractionString(const string& inputString, const char* delimiters)
{
	char localString[256] = "";
	strcpy_s(localString, inputString.c_str());

	std::vector<string> extractionStrings;

	if (gDebugEnabled)
	{
		std::cout << inputString << std::endl;
	}

	char* nextToken{ nullptr };
	auto token = strtok_s(localString, delimiters, &nextToken);
	while (token)
	{
		extractionStrings.emplace_back(token);
		token = strtok_s(nullptr, delimiters, &nextToken);
	}

	return extractionStrings;
}

std::unordered_map<string, long> getLargestPresence(const std::vector<string>& singleExtractionString)
{
	const std::vector<string> colors = { "red", "green", "blue" };
	char localString[256] = "";
	std::vector<string> substrings;
	uint16_t extractionCounter = 0;
	char* nextToken{ nullptr };
	char* token{ nullptr };
	char* pEnd;

	std::unordered_map<string, long> presences{ {"id", 0}, {"red", 0}, {"green", 0}, {"blue", 0} };

	if (gDebugEnabled)
	{
		for (const auto& string : singleExtractionString)
		{
			cout << string << " | ";
		}
		cout << std::endl;
	}

	for (const auto& string : singleExtractionString)
	{
		if (extractionCounter == 0)
		{
			// parse game number
			substrings.clear();
			nextToken = nullptr;
			strcpy_s(localString, string.c_str());
			token = strtok_s(localString, " ", &nextToken);
			while (token)
			{
				substrings.emplace_back(token);
				token = strtok_s(nullptr, " ", &nextToken);
			}
			presences.find("id")->second = strtol(substrings.at(1).c_str(), &pEnd, 0);;
			if (gDebugEnabled)
			{
				std::cout << "id: " << substrings.at(1) << " | ";
			}
		}
		else
		{
			// parse 1st extraction
			substrings.clear();
			nextToken = nullptr;
			strcpy_s(localString, string.c_str());
			token = strtok_s(localString, ", ", &nextToken);
			while (token)
			{
				substrings.emplace_back(token);
				token = strtok_s(nullptr, ", ", &nextToken);

				if (token != nullptr)
				{
					for (const auto& color : colors)
					{
						if (strcmp(token, color.c_str()) == 0)
						{
							const auto value = strtol(substrings.back().c_str(), &pEnd, 0);
							
							if (presences.find(color)->second < value)
							{
								presences.find(color)->second = value;
								if (gDebugEnabled)
								{
									std::cout << color << "->" << value << " ";
								}
							}
						}
					}
				}
			}
			if (gDebugEnabled)
			{
				std::cout << " | ";
			}
		}
		extractionCounter++;
	}
	if (gDebugEnabled)
	{
		std::cout << " final " << " red: " << presences.find("red")->second << " green: " << presences.find("green")->second << " blue: " << presences.find("blue")->second;
		std::cout << "\n";
	}
	return presences;
}

int computePower(const std::unordered_map<string, long>& extractionPresences)
{
	const std::vector<string> colors = { "red", "green", "blue" };
	int power = 1;
	
	if (gDebugEnabled)
	{
		std::cout << "power: " << power << " ";
	}
	for (const auto& color : colors)
	{
		if (extractionPresences.find(color) != extractionPresences.end())
		{
			if (gDebugEnabled)
			{
				std::cout << "*" << extractionPresences.find(color)->second << " = " << power * extractionPresences.find(color)->second << " | ";
			}
			power *= extractionPresences.find(color)->second;
		}
	}
	if (gDebugEnabled)
	{
		std::cout << std::endl;
	}

	return power;
}

bool isExtractionPossible(const std::unordered_map<string, long>& extractionPresences)
{
	const std::vector<string> colors = { "red", "green", "blue" };
	std::unordered_map<string, long> limits{ {"red", 12}, {"green", 13}, {"blue", 14} };

	bool isExtractionPossible = true;

	for (const auto& color : colors)
	{
		if (extractionPresences.find(color)->second > limits.find(color)->second)
		{
			isExtractionPossible = false;
		}
	}

	if (gDebugEnabled)
	{
		if (isExtractionPossible)
		{
			std::cout << "Extraction is possible!" << std::endl;
		}
		else
		{
			std::cout << "Extraction is NOT possible!" << std::endl;
		}
	}

	return isExtractionPossible;
}
