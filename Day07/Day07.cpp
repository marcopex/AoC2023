#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <regex>
#include <array>
#include <unordered_map>
#include <map>

using namespace std;

bool gPrintInput = false;
bool gDebugEnabled = false;

enum class CardType : char
{
	Ace,
	King,
	Queen,
	Jack,
	Ten,
	Nine,
	Eight,
	Seven,
	Six,
	Five,
	Four,
	Three,
	Two,
	Joker,
	Unknown,
};

enum class HandType : char
{
	FiveOfAKind,
	FourOfAKind,
	FullHouse,
	ThreeOfAKind,
	TwoPairs,
	OnePair,
	HighCard,
	Nothing
};

struct Hand
{
	string cardsString;
	std::array<CardType, 5> cards;
	int bet;
	HandType type;
	int rank;

	bool operator < (const Hand& h) const
	{
		if (this->type == h.type)
		{
			for (auto i = 0; i < cards.size(); i++)
			{
				if (this->cards.at(i) != h.cards.at(i))
				{
					return (this->cards.at(i) < h.cards.at(i));
				}
			}
		}
		return this->type < h.type;
	}

	bool operator > (const Hand& h) const
	{
		if (this->type == h.type)
		{
			for (auto i = 0; i < cards.size(); i++)
			{
				if (this->cards.at(i) != h.cards.at(i))
				{
					return (this->cards.at(i) > h.cards.at(i));
				}
			}
		}
		return this->type > h.type;
	}
};

std::vector<Hand> loadHands(std::ifstream& inputFile, const bool jokersPresent);
int computeTotalWinnings(std::vector<Hand> hands);

int main()
{
	string inputString{ "empty" };

	//std::ifstream inputFile("input_test_1.txt");
	std::ifstream inputFile("input_real_1.txt");

	long long r1 = 1;
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

		const auto handsWithoutJokers = loadHands(inputFile, false);
		r1 = computeTotalWinnings(handsWithoutJokers);

		inputFile.clear();
		inputFile.seekg(0);

		const auto handsWithJokers = loadHands(inputFile, true);
		r2 = computeTotalWinnings(handsWithJokers);

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

CardType getCardType(const char cardSymbol, const bool jokersPresent)
{
	auto type = CardType::Unknown;
	
	switch (cardSymbol)
	{
		case 'A':
			type = CardType::Ace;
		break;
		case 'K':
			type = CardType::King;
			break;
		case 'Q':
			type = CardType::Queen;
			break;
		case 'J':
			if (jokersPresent)
			{
				type = CardType::Joker;
			}
			else
			{
				type = CardType::Jack;
			}
			break;
		case 'T':
			type = CardType::Ten;
			break;
		case '9':
			type = CardType::Nine;
			break;
		case '8':
			type = CardType::Eight;
			break;
		case '7':
			type = CardType::Seven;
			break;
		case '6':
			type = CardType::Six;
			break;
		case '5':
			type = CardType::Five;
			break;
		case '4':
			type = CardType::Four;
			break;
		case '3':
			type = CardType::Three;
			break;
		case '2':
			type = CardType::Two;
			break;
		default:
			type = CardType::Unknown;
	}
	return type;
}

string getCardString(const CardType type)
{
	string cardString{};

	switch (type)
	{
	case CardType::Ace:
		cardString = "Ace";
		break;
	case CardType::King:
		cardString = "King";
		break;
	case CardType::Queen:
		cardString = "Queen";
		break;
	case CardType::Jack:
		cardString = "Jack";
		break;
	case CardType::Ten:
		cardString = "Ten";
		break;
	case CardType::Nine:
		cardString = "Nine";
		break;
	case CardType::Eight:
		cardString = "Eight";
		break;
	case CardType::Seven:
		cardString = "Seven";
		break;
	case CardType::Six:
		cardString = "Six";
		break;
	case CardType::Five:
		cardString = "Five";
		break;
	case CardType::Four:
		cardString = "Four";
		break;
	case CardType::Three:
		cardString = "Three";
		break;
	case CardType::Two:
		cardString = "Two";
		break;
	case CardType::Joker:
		cardString = "Joker";
		break;
	default:
		cardString = "Unknown";
	}
	return cardString;
}

string getHandString(const HandType type)
{
	string cardString{};

	switch (type)
	{
	case HandType::FiveOfAKind:
		cardString = "Five";
		break;
	case HandType::FourOfAKind:
		cardString = "Four";
		break;
	case HandType::FullHouse:
		cardString = "Full";
		break;
	case HandType::ThreeOfAKind:
		cardString = "Three";
		break;
	case HandType::TwoPairs:
		cardString = "TwoP";
		break;
	case HandType::OnePair:
		cardString = "OneP";
		break;
	case HandType::HighCard:
		cardString = "High";
		break;
	default:
		cardString = "Nothing";
	}
	return cardString;
}

typedef std::map<CardType, int> mapCardType;
typedef std::map<CardType, int>::value_type mapValueType;

mapCardType counter(const std::array<CardType, 5>& handCards)
{
	std::map<CardType, int> cntMap;

	for (auto card : handCards)
	{
		cntMap[card]++;
	}

	return cntMap;
}

bool isFiveOfAKind(const mapCardType cardMap, const int cntJokers)
{
	if (cntJokers == 5)
	{
		return true;
	}

	const auto it5 = find_if(cardMap.begin(), cardMap.end(),
		[cntJokers](const mapValueType m)
		{ return ((m.second == 5) and (m.first != CardType::Joker)); });
	if (it5 != cardMap.end())
	{
		return true;
	}
	
	const auto it4 = find_if(cardMap.begin(), cardMap.end(),
		[cntJokers](const mapValueType m)
		{ return ((m.second == 4) and (m.first != CardType::Joker)); });
	if (it4 != cardMap.end() and (cntJokers == 1))
	{
		return true;
	}

	const auto it3 = find_if(cardMap.begin(), cardMap.end(),
		[cntJokers](const mapValueType m)
		{ return ((m.second == 3) and (m.first != CardType::Joker)); });
	if (it3 != cardMap.end() and (cntJokers == 2))
	{
		return true;
	}
	
	const auto it2 = find_if(cardMap.begin(), cardMap.end(),
		[cntJokers](const mapValueType m)
		{ return ((m.second == 2) and (m.first != CardType::Joker)); });
	if (it2 != cardMap.end() and (cntJokers == 3))
	{
		return true;
	}
	
	if (cntJokers == 4)
	{
		return true;
	}

	return false;
}

bool isFourOfAKind(const mapCardType cardMap, const int cntJokers)
{
	const auto it4 = find_if(cardMap.begin(), cardMap.end(),
		[cntJokers](const mapValueType m)
		{ return ((m.second == 4) and (m.first != CardType::Joker)); });
	if (it4 != cardMap.end())
	{
		return true;
	}

	const auto it3 = find_if(cardMap.begin(), cardMap.end(),
		[cntJokers](const mapValueType m)
		{ return ((m.second == 3) and (m.first != CardType::Joker)); });
	if (it3 != cardMap.end() and (cntJokers == 1))
	{
		return true;
	}

	const auto it2 = find_if(cardMap.begin(), cardMap.end(),
		[cntJokers](const mapValueType m)
		{ return ((m.second == 2) and (m.first != CardType::Joker)); });
	if (it2 != cardMap.end() and (cntJokers == 2))
	{
		return true;
	}

	if (cntJokers == 3)
	{
		return true;
	}

	return false;
}

bool isFullHouse(const mapCardType cardMap, const int cntJokers)
{
	auto isValid = false;


	switch (cntJokers)
	{
		case 5:
		// should never happen
		isValid = false;
		break;

		case 4:
		// should never happen
		isValid = false;
		break;

		case 3:
		// automatic success
		isValid = true;
		break;

		case 2:
		{
			// need one pair
			const auto it2 = find_if(cardMap.begin(), cardMap.end(),
				[cntJokers](const mapValueType m)
				{ return ((m.second == 2) and (m.first != CardType::Joker)); });
			if (it2 != cardMap.end())
			{
				isValid = true;
			}
			else
			{
				isValid = false;
			}
		}
		break;

		case 1:
		{
			// need two pairs
			const auto it2 = find_if(cardMap.begin(), cardMap.end(),
				[cntJokers](const mapValueType m)
				{ return ((m.second == 2) and (m.first != CardType::Joker)); });

			if (it2 != cardMap.end())
			{
				auto localMap = cardMap;
				localMap.erase(it2->first);
				const auto it22 = find_if(localMap.begin(), localMap.end(),
					[](const mapValueType m)
					{ return (m.second == 2); });

				if (it22 != localMap.end())
				{
					isValid = true;
				}
				else
				{
					isValid = false;
				}
			}
			else
			{
				isValid = false;
			}
		}
		break;
		
		case 0:
		{
			// need 3 + 2
			const auto it3 = find_if(cardMap.begin(), cardMap.end(),
				[cntJokers](const mapValueType m)
				{ return ((m.second == 3) and (m.first != CardType::Joker)); });
			if (it3 != cardMap.end())
			{
				auto localMap = cardMap;
				localMap.erase(it3->first);
				const auto it2 = find_if(localMap.begin(), localMap.end(),
					[](const mapValueType m)
					{ return (m.second == 2); });
				if (it2 != localMap.end())
				{
					isValid = true;
				}
				else
				{
					isValid = false;
				}
			}
			else
			{
				isValid = false;
			}
		}
		break;

		default:
		isValid = false;
		break;
	}
	return isValid;
}

bool isThreeOfAKind(const mapCardType cardMap, const int cntJokers)
{
	const auto it3 = find_if(cardMap.begin(), cardMap.end(),
		[cntJokers](const mapValueType m)
		{ return ((m.second == 3) and (m.first != CardType::Joker)); });
	if (it3 != cardMap.end())
	{
		return true;
	}

	const auto it2 = find_if(cardMap.begin(), cardMap.end(),
		[cntJokers](const mapValueType m)
		{ return ((m.second == 2) and (m.first != CardType::Joker)); });
	if (it2 != cardMap.end() and (cntJokers == 1))
	{
		return true;
	}

	if (cntJokers == 2)
	{
		return true;
	}

	return false;
}

bool isTwoPairs(const mapCardType cardMap, const int cntJokers)
{
	auto isValid = false;

	switch (cntJokers)
	{
	case 5:
	{
		// should never happen
		isValid = false;
	}
	break;
	case 4:
	{
		// should never happen
		isValid = false;
	}
	break;
	case 3:
	{
		// should never happen
		isValid = false;
	}
	break;
	case 2:
	{
		// need another pair
		const auto it2 = find_if(cardMap.begin(), cardMap.end(),
			[cntJokers](const mapValueType m)
			{ return ((m.second == 2) and (m.first != CardType::Joker)); });
		if (it2 != cardMap.end())
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	break;
	case 1:
	{
		// need another pair
		const auto it2 = find_if(cardMap.begin(), cardMap.end(),
			[cntJokers](const mapValueType m)
			{ return ((m.second == 2) and (m.first != CardType::Joker)); });
		if (it2 != cardMap.end())
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	break;
	case 0:
	{
		// need two pairs
		const auto it2 = find_if(cardMap.begin(), cardMap.end(),
			[cntJokers](const mapValueType m)
			{ return ((m.second == 2) and (m.first != CardType::Joker)); });
		if (it2 != cardMap.end())
		{
			auto localMap = cardMap;
			localMap.erase(it2->first);
			const auto it22 = find_if(localMap.begin(), localMap.end(),
				[](const mapValueType m)
				{ return (m.second == 2); });
			if (it22 != localMap.end())
			{
				isValid = true;
			}
			else
			{
				isValid = false;
			}
		}
		else
		{
			isValid = false;
		}
	}
	break;
	default:
	{
		isValid = false;
	}
	break;
	}
	
	return isValid;
}

bool isOnePair(const mapCardType cardMap, const int cntJokers)
{
	auto isValid = false;

	switch (cntJokers)
	{
	case 5:
	{
		// should never happen
		isValid = false;
	}
	break;
	case 4:
	{
		// should never happen
		isValid = false;
	}
	break;
	case 3:
	{
		// should never happen
		isValid = false;
	}
	break;
	case 2:
	{
		// should never happen
		isValid = false;
	}
	case 1:
	{
		// automatic success
		return true;
	}
	break;
	case 0:
	{
		// need a pair
		const auto it2 = find_if(cardMap.begin(), cardMap.end(),
			[cntJokers](const mapValueType m)
			{ return ((m.second == 2) and (m.first != CardType::Joker)); });
		if (it2 != cardMap.end())
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	break;
	default:
	{
		isValid = false;
	}
	break;
	}
	return isValid;
}

bool isHighCard(const mapCardType cardMap, const int cntJokers)
{
	if (cardMap.size() == 5)
	{
		return true;
	}
	else
	{
		return false;
	}
}

int countJokers(const mapCardType cardMap)
{
	const auto it = find_if(cardMap.begin(), cardMap.end(),
		[](const mapValueType m)
		{ return (m.first == CardType::Joker); });
	if (it == cardMap.end())
	{
		return 0;
	}
	else
	{
		return it->second;
	}
}

HandType getHandType(const std::array<CardType, 5> handCards, const bool jokersPresent)
{
	auto handType = HandType::Nothing;
	
	auto cntJokers = 0;
	if (jokersPresent)
	{
		cntJokers = countJokers(counter(handCards));
	}
	
	const auto isFive = isFiveOfAKind(counter(handCards), cntJokers);
	const auto isFour = isFourOfAKind(counter(handCards), cntJokers);
	const auto isFull = isFullHouse(counter(handCards), cntJokers);
	const auto isThree = isThreeOfAKind(counter(handCards), cntJokers);
	const auto isTwoP = isTwoPairs(counter(handCards), cntJokers);
	const auto isOneP = isOnePair(counter(handCards), cntJokers);
	const auto isHighC = isHighCard(counter(handCards), cntJokers);
	
	if (isFive)
	{
		return HandType::FiveOfAKind;
	}
	else if (isFour)
	{
		return HandType::FourOfAKind;
	}
	else if (isFull)
	{
		return HandType::FullHouse;
	}
	else if (isThree)
	{
		return HandType::ThreeOfAKind;
	}
	else if (isTwoP)
	{
		return HandType::TwoPairs;
	}
	else if (isOneP)
	{
		return HandType::OnePair;
	}
	else if (isHighC)
	{
		return HandType::HighCard;
	}
	else
	{
		return HandType::Nothing;
	}
}

std::array<CardType, 5> getCards(const std::string cardString, const bool jokersPresent)
{
	std::array<CardType, 5> cards{};

	cards.at(0) = getCardType(cardString.c_str()[0], jokersPresent);
	cards.at(1) = getCardType(cardString.c_str()[1], jokersPresent);
	cards.at(2) = getCardType(cardString.c_str()[2], jokersPresent);
	cards.at(3) = getCardType(cardString.c_str()[3], jokersPresent);
	cards.at(4) = getCardType(cardString.c_str()[4], jokersPresent);

	return cards;
}

std::vector<Hand> loadHands(std::ifstream& inputFile, const bool jokersPresent)
{
	string inputString{ "empty" };
	std::vector<Hand> hands;

	while (!inputFile.eof())
	{
		std::getline(inputFile, inputString, '\n');
		std::size_t const npos1 = inputString.find_first_of(' ');

		auto cardsString = inputString.substr(0, npos1);
		auto betString = inputString.substr(npos1 + 1, inputString.length());

		char* pEndLocal;
		const auto bet = strtol(betString.c_str(), &pEndLocal, 0);

		Hand hand{ cardsString, getCards(cardsString, jokersPresent), bet, getHandType(getCards(cardsString, jokersPresent), jokersPresent) };

		hands.push_back(hand);
	}

	std::sort(hands.begin(), hands.end());

	return hands;
}

int computeTotalWinnings(std::vector<Hand> hands)
{
	int rank = static_cast<int>(hands.size());
	auto totalWinnings = 0;

	for (auto& hand : hands)
	{
		hand.rank = rank;
		rank--;
	}
	for (const auto& hand : hands)
	{
		totalWinnings += hand.bet * hand.rank;
		if (gDebugEnabled)
		{
			std::cout << "SORTED - Hand is " << hand.cardsString << " type:" << getHandString(hand.type) << " bet:" << hand.bet
				<< " rank:" << hand.rank << " totalWinnings:" << totalWinnings << std::endl;
		}
	}

	return totalWinnings;
}
