#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <regex>
#include <algorithm>

using namespace std;

bool gPrintInput = false;
bool gDebugEnabled = false;

struct Sequence
{
	int id{};
	std::vector<long long> numbers{};
};

std::vector<Sequence> getSequences(std::ifstream& inputFile, bool reverseNumbersOrder);
long long extrapolateNextSequenceNumber(const Sequence sequence);

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
		const auto sequences = getSequences(inputFile, false);
		for (auto seq : sequences)
		{
			r1 += extrapolateNextSequenceNumber(seq);
		}

		inputFile.clear();
		inputFile.seekg(0);
		const auto revSequences = getSequences(inputFile, true);
		for (auto seq : revSequences)
		{
			r2 += extrapolateNextSequenceNumber(seq);
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

std::vector<long long> getNumbersFromString(const string& inputString)
{
	string localString = inputString;

	std::vector<string> strings;
	std::vector<long long> numbers;

	std::regex stringRegex(R"(-?\d+)");   // matches a sequence of digits
	std::smatch regexMatch;

	while (std::regex_search(localString, regexMatch, stringRegex))
	{
		char* pEndLocal;
		const auto number = strtoll(regexMatch.str().c_str(), &pEndLocal, 0);
		strings.push_back(regexMatch.str());
		numbers.push_back(number);
		localString = regexMatch.suffix();
	}

	return numbers;
}

std::vector<Sequence> getSequences(std::ifstream& inputFile, bool reverseNumbersOrder)
{
	string inputString{ "empty" };
	std::vector<Sequence> sequences;

	auto cntSequences = 0;
	while (!inputFile.eof())
	{
		std::getline(inputFile, inputString, '\n');
		auto numbers = getNumbersFromString(inputString);
		if (reverseNumbersOrder)
		{
			std::reverse(numbers.begin(), numbers.end());
		}
		sequences.push_back({ cntSequences, numbers });
		
		cntSequences++;
	}

	return sequences;
}

bool isSequenceAllZeroes(const std::vector<long long> numbers)
{
	return count(numbers.begin(), numbers.end(), 0) == numbers.size();
}

Sequence computeDeltas(const Sequence& sequence)
{
	Sequence deltas{};
	auto isDiffAlwaysZero = true;
	for (auto i = 1; i < sequence.numbers.size(); i++)
	{
		const auto diff = sequence.numbers.at(i) - sequence.numbers.at(i - 1);
		deltas.numbers.push_back(diff);
	}
	return deltas;
}

long long extrapolateNextSequenceNumber(const Sequence sequence)
{
	if (gDebugEnabled)
	{
		std::cout << "Sequence#" << sequence.id << ":" << std::endl;
		for (const auto num : sequence.numbers)
		{
			std::cout << num << " ";
		}
		std::cout << std::endl;
	}

	std::vector<Sequence> localSequences{};
	// get first string
	localSequences.push_back(sequence);

	// stop until zeroes
	auto deltas = sequence;
	auto cntLocalSequences = 0;
	auto isSequenceDeriveNull = false;
	while (not(isSequenceDeriveNull))
	{
		deltas = computeDeltas(deltas);
		localSequences.push_back(deltas);
		isSequenceDeriveNull = isSequenceAllZeroes(deltas.numbers);

		if (gDebugEnabled)
		{
			std::cout << "#" << cntLocalSequences << ": ";
			for (const auto num : deltas.numbers)
			{
				std::cout << num << " ";
			}
			std::cout << std::endl;
		}

		cntLocalSequences++;
	}


	// extrapolate
	auto idxSequences = cntLocalSequences;
	while (idxSequences > 0)
	{
		auto& seqDown = localSequences.at(idxSequences);
		auto& seqUp = localSequences.at(idxSequences - 1);
		const auto ex = seqUp.numbers.back() + seqDown.numbers.back();
		seqUp.numbers.push_back(ex);
		idxSequences--;
	}

	if (gDebugEnabled)
	{
		idxSequences = 0;
		for (const auto& seq : localSequences)
		{
			std::cout << "#" << idxSequences << ": ";
			for (const auto num : seq.numbers)
			{
				std::cout << num << " ";
			}
			std::cout << std::endl;
			idxSequences++;
		}
		std::cout << "Last number is " << localSequences.at(0).numbers.back() << std::endl;
	}

	return localSequences.at(0).numbers.back();
}
