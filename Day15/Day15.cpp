#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <array>

using namespace std;

bool gPrintInput = false;
bool gDebugEnabled = false;

enum class Operation : char
{
	Unknown,
	Removal,
	Addition
};

struct Lens
{
	string label{};
	int focalLength{};
	
	bool operator==(const Lens& l) const
	{
		return ((this->label.compare(l.label) == 0));
	}
};

struct Box
{
	int number{};
	std::vector<Lens> lenses{};
};

std::vector<string> getInitializationSequence(std::ifstream& inputFile);
unsigned char getStringHash(const string s);
int computeSum(const std::vector<string> initSequences);
std::array<Box, 256> populateBoxes(const std::vector<string> initSequences);
int computeFocusingPower(const std::array<Box, 256> boxes);

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

		const auto seqs = getInitializationSequence(inputFile);
		r1 = computeSum(seqs);
		const auto boxes = populateBoxes(seqs);
		r2 = computeFocusingPower(boxes);

		const auto b = populateBoxes(seqs);
	}
	else
	{
		std::cout << "Could not open input file!" << std::endl;
	}

	std::cout << "Result for Trial #1 is " << r1 << std::endl;
	std::cout << "Result for Trial #2 is " << r2 << std::endl;

	return 0;
}

std::vector<string> getInitializationSequence(std::ifstream& inputFile)
{
	string inputString{ "" };
	std::vector<string> sequences;

	while (!inputFile.eof())
	{
		std::getline(inputFile, inputString, ',');
		sequences.push_back(inputString);
	}

	if (gDebugEnabled)
	{
		std::cout << "Found a total of " << sequences.size() << " sequences" << std::endl;
	}

	return sequences;
}

unsigned char getStringHash(const string s)
{
	unsigned char out = 0;
	int tmp = 0;
	for (const auto& c : s)
	{
		tmp = (int)out + (int)c;
		tmp = (tmp * 17);
		out = static_cast<unsigned char>((tmp % 256));
	}

	return out;
}

int computeSum(const std::vector<string> initSequences)
{
	int out = 0;

	for (const auto& s : initSequences)
	{
		const auto h = getStringHash(s);
		out += h;
		if (gDebugEnabled)
		{
			std::cout << "String:" << s << " returns hash:" << static_cast<unsigned int>(h) << std::endl;
		}
	}

	return out;
}

std::array<Box, 256> populateBoxes(const std::vector<string> initSequences)
{
	std::array<Box, 256> boxes{};
	auto cntBoxes = 0;
	for (auto& b : boxes)
	{
		b.number = cntBoxes;
		cntBoxes++;
	}

	for (const auto& s : initSequences)
	{
		const auto p1 = s.find_last_of('=');
		const auto p2 = s.find_last_of('-');
		auto p = p2;
		Operation o = Operation::Removal;
		if (p1 != string::npos)
		{
			p = p1;
			o = Operation::Addition;
		}
		
		auto label = (s.substr(0, p));
		const auto boxNumber = getStringHash(label);
		
		auto box = &boxes.at(boxNumber);
		switch (o)
		{
		case Operation::Addition:
		{
			const auto focalLength = s.at(p + 1) - '0';
			const Lens lens{ label, focalLength };
			const auto it = find(box->lenses.begin(), box->lenses.end(), lens);
			if (it != box->lenses.end())
			{
				if (gDebugEnabled)
				{
					std::cout << "Box #" << static_cast<int>(boxNumber) << " replacing lens label:" << it->label << " FL:" << it->focalLength << "->" << focalLength << std::endl;
				}
				it->focalLength = focalLength;
			}
			else
			{
				if (gDebugEnabled)
				{
					std::cout << "Box #" << static_cast<int>(boxNumber) << " adding lens label:" << label << " FL:" << focalLength << std::endl;
				}
				box->lenses.push_back(lens);
			}
		}
		break;
		case Operation::Removal:
		{
			const Lens lens{label, 0};
			const auto it = find(box->lenses.begin(), box->lenses.end(), lens);
			if (it != box->lenses.end())
			{
				if (gDebugEnabled)
				{
					std::cout << "Box #" << static_cast<int>(boxNumber) << " removing lens label:" << it->label << " FL:" << it->focalLength << std::endl;
				}
				box->lenses.erase(it);
			}
			else
			{
				if (gDebugEnabled)
				{
					std::cout << "Box #" << static_cast<int>(boxNumber) << " removing lens label:" << label << " lens not found" << std::endl;
				}
			}
		}
		break;
		default:
			break;
		}
	}
	
	return boxes;
}

int computeFocusingPower(const std::array<Box, 256> boxes)
{
	int power = 0;

	for (const auto& b : boxes)
	{
		if (b.lenses.size() > 0)
		{
			if (gDebugEnabled)
			{
				std::cout << "Box #:" << b.number << " has " << b.lenses.size() << " lenses: ";
			}
			auto cntLenses = 0;
			for (const auto& s : b.lenses)
			{
				if (gDebugEnabled)
				{
					std::cout << s.label << " FL:" << s.focalLength << " | ";
				}
				power += (b.number + 1) * (cntLenses + 1) * s.focalLength;
				cntLenses++;
			}
			if (gDebugEnabled)
			{
				std::cout << std::endl;
			}
		}
	}

	return power;
}
