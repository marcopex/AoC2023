#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <regex>
#include <algorithm>

using namespace std;

bool gPrintInput = false;
bool gDebugEnabled = false;

enum class InfoType : char
{
	Unknown,
	Operational,
	Damaged,
	Missing
};

using InfoTypeSequence = std::vector <InfoType>;
using DamagedSpringsSequence = std::vector<int>;

struct RecordLine
{
	int lineNumber;
	InfoTypeSequence springsInfos;
	DamagedSpringsSequence damageInfos;
};

using TextRecord = std::vector<string>;
using SymbRecord = std::vector<RecordLine>;

TextRecord getTextRecord(std::ifstream& inputFile);
SymbRecord getSymbRecord(const TextRecord& txtRec);
InfoType getInfoType(const char c);
DamagedSpringsSequence getContiguousDamagedSprings(const RecordLine& recLine);

int main()
{
	string inputString{ "empty" };

	std::ifstream inputFile("input_test_1.txt");
	//std::ifstream inputFile("input_test_2.txt");

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

		const auto txtRec = getTextRecord(inputFile);
		const auto symRec = getSymbRecord(txtRec);

		for (const auto& r : symRec)
		{
			const auto s = getContiguousDamagedSprings(r);
		}
	}
	else
	{
		std::cout << "Could not open input file!" << std::endl;
	}

	std::cout << "Result for Trial #1 is " << r1 << std::endl;
	std::cout << "Result for Trial #2 is " << r2 << std::endl;

	return 0;
}

//string getStringFromGalaxy(const Galaxy g)
//{
//	string out{};
//	out += "Galaxy #";
//	out += std::to_string(g.id);
//	out += "[";
//	out += std::to_string(g.x);
//	out += "|";
//	out += std::to_string(g.y);
//	out += "]";
//
//	return out;
//}

TextRecord getTextRecord(std::ifstream& inputFile)
{
	string inputString{ "" };
	TextRecord txtRec;
	
	while (!inputFile.eof())
	{
		std::getline(inputFile, inputString, '\n');
		txtRec.push_back(inputString);
	}

	return txtRec;
}

std::vector<int> getNumbersFromString(const string& inputString)
{
	string localString = inputString;

	std::vector<int> numbers;

	std::regex stringRegex(R"(\d+)");   // matches a sequence of digits
	std::smatch regexMatch;

	while (std::regex_search(localString, regexMatch, stringRegex))
	{
		char* pEndLocal;
		const auto number = strtol(regexMatch.str().c_str(), &pEndLocal, 0);
		numbers.push_back(number);
		localString = regexMatch.suffix();
	}

	return numbers;
}

SymbRecord getSymbRecord(const TextRecord& txtRec)
{
	SymbRecord symRec{};
	auto cntRecs = 0;
	for (const auto& r : txtRec)
	{
		const auto p = r.find_first_of(' ');

		const auto springsString = r.substr(0, p);
		const auto damageString = r.substr(p + 1);

		std::cout << springsString << " | " << damageString << std::endl;

		std::vector <InfoType> infoTypes{};

		for (const auto& c : springsString)
		{
			const auto infoType = getInfoType(c);
			infoTypes.push_back(infoType);
		}
		symRec.push_back({cntRecs, infoTypes, getNumbersFromString(damageString) });
		cntRecs++;
	}
	
	return symRec;
}

InfoType getInfoType(const char c)
{
	auto out = InfoType::Unknown;
	switch (c)
	{
	case '.':
		out = InfoType::Operational;
		break;
	case '#':
		out = InfoType::Damaged;
		break;
	case '?':
		out = InfoType::Missing;
		break;
	default:
		out = InfoType::Missing;
		break;
	}
	return out;
}

DamagedSpringsSequence getContiguousDamagedSprings(const RecordLine& recLine)
{
	DamagedSpringsSequence seq{};

	auto cntOpe = 0;
	auto cntDam = 0;

	for (const auto s : recLine.springsInfos)
	{
		auto changeDir = false;

		if (s == InfoType::Damaged)
		{
			if (cntOpe > 0)
			{
				cntOpe = 0;
			}
			cntDam++;
		}
		else if (s == InfoType::Operational)
		{
			if (cntDam > 0)
			{
				changeDir = true;
			}
			else
			{
				cntOpe++;
			}
		}
		else
		{
			std::cout << "Unexpected InfoType:" << static_cast<int>(s) << std::endl;
		}

		if (changeDir)
		{
			seq.push_back(cntDam);
			cntDam = 0;
		}
	}

	if (cntDam > 0)
	{ // final
		seq.push_back(cntDam);
	}

	std::cout << "Damaged Sequence is: ";
	for (const auto& s : seq)
	{
		std::cout << s << " ";
	}
	std::cout << std::endl;

	return seq;
}

int checkPossibleCombinations(const RecordLine& recLine)
{
	// detect and memorize possible variables
	std::vector<int> missingSpots{};

	auto cntInfos = 0;
	for (const auto& s : recLine.springsInfos)
	{
		if (s == InfoType::Missing)
		{
			missingSpots.push_back(cntInfos);
		}
		cntInfos++;
	}

	for (const auto& m : missingSpots)
	{

	}


}