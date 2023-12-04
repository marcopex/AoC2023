#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <regex>

using namespace std;

bool gPrintInput = false;
bool gDebugEnabled = false;

struct cardInfo
{
	int id;
	std::vector<int> winningNumbers;
	std::vector<int> targetNumbers;
	std::vector<int> matchingNumbers;
	int points;
	int copies;
};

std::vector<int> getNumbersFromString(const string& inputString);
int computePoints(cardInfo& cardInfo);

int main()
{
	string inputString{ "empty" };

	//std::ifstream inputFile("input_test_1.txt");
	std::ifstream inputFile("input_real_1.txt");

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

		std::vector<cardInfo> cards;

		while (!inputFile.eof())
		{
			std::getline(inputFile, inputString, '\n');
			std::size_t const npos1 = inputString.find_first_of(':');

			auto cardString = inputString.substr(0, npos1);
			auto winningString = inputString.substr(npos1 + 1, inputString.length());

			std::size_t const npos2 = winningString.find_first_of('|');
			auto targetString = winningString.substr(npos2 + 1, winningString.length());
			winningString = winningString.substr(0, npos2);

			cardInfo newCard{ 0, {}, {}, {}, 0, 1 };

			newCard.id = getNumbersFromString(cardString).at(0);
			newCard.winningNumbers = getNumbersFromString(winningString);
			newCard.targetNumbers = getNumbersFromString(targetString);
			newCard.points = computePoints(newCard);

			cards.push_back(newCard);

			cumSumTrial1 += newCard.points;
		}

		for (const auto& card : cards)
		{
			for (int idxCard = 0; idxCard < card.matchingNumbers.size(); idxCard++)
			{
				for (int idxCopy = 0; idxCopy < card.copies; idxCopy++)
				{
					const auto targetCardNumber = card.id + idxCard;
					cards.at(targetCardNumber).copies++;
					auto nowCopies = 0;

					if ((card.id + idxCard) < cards.size())
					{
						nowCopies = cards.at(card.id + idxCard).copies;
					}
					if (gDebugEnabled)
					{
						cout << "Card #" << card.id << " Copy #" << idxCopy + 1 << " -> Copies for Card #" << targetCardNumber + 1 << " are now " << nowCopies << std::endl;
					}
				}
			}
		}

		// compute total copies
		for (const auto& card : cards)
		{
			cumSumTrial2 += card.copies;
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

std::vector<int> getNumbersFromString(const string& inputString)
{
	string localString = inputString;

	std::vector<string> strings;
	std::vector<int> numbers;

	std::regex stringRegex(R"(\d+)");   // matches a sequence of digits
	std::smatch regexMatch;

	while (std::regex_search(localString, regexMatch, stringRegex))
	{
		char* pEndLocal;
		const auto number = strtol(regexMatch.str().c_str(), &pEndLocal, 0);
		strings.push_back(regexMatch.str());
		numbers.push_back(number);
		localString = regexMatch.suffix();
	}

	return numbers;
}

int computePoints(cardInfo& card)
{
	auto cardPoints = 0;
	auto isFirstMatch = true;

	for (const auto& winningNumber : card.winningNumbers)
	{
		const auto it = find_if(card.targetNumbers.begin(), card.targetNumbers.end(),
			[&winningNumber](const int& targetNumber)
			{
				return (winningNumber == targetNumber);
			});
		if (it != card.targetNumbers.end())
		{
			if (isFirstMatch)
			{
				isFirstMatch = false;
				cardPoints = 1;
			}
			else
			{
				cardPoints *= 2;
			}
			card.matchingNumbers.push_back(winningNumber);
		}
	}

	if (gDebugEnabled)
	{
		cout << "Matching numbers are : ";
		for (const auto& matchingNumber : card.matchingNumbers)
		{
			cout << matchingNumber << " | ";

		}
		cout << " Card Points: " << cardPoints;
		cout << std::endl;
	}

	return cardPoints;
}
