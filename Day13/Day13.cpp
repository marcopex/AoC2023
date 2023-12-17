#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

bool gPrintInput = false;
bool gDebugEnabled = false;

struct Pattern
{
	std::vector<string> rows;
	std::vector<string> cols;
	std::vector<size_t> rowHashes;
	std::vector<size_t> colHashes;
};

enum class SymmetryType : char
{
	Unknown,
	Vertical,
	Horizontal,
	NotFound
};

struct SymmetryInfo
{
	SymmetryType type;
	std::pair<int, int> coordinates;

	bool operator!=(const SymmetryInfo& s) const
	{
		return ((this->type != s.type) or (this->coordinates.first != s.coordinates.first) or (this->coordinates.second != s.coordinates.second));
	}
};

SymmetryInfo getSymmetry(const Pattern p, const SymmetryInfo& origSym);
std::vector<Pattern> getPatterns(std::ifstream& inputFile);
SymmetryInfo getAlternateSymmetry(const Pattern& p, const SymmetryInfo& origSym);

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

		const auto patterns = getPatterns(inputFile);
		for (const auto& p : patterns)
		{
			SymmetryInfo origSym{ SymmetryType::Unknown, {0, 0} };
			const auto sym = getSymmetry(p, origSym);
			switch (sym.type)
			{
			case SymmetryType::Horizontal:
				r1 += sym.coordinates.first;
				break;
			case SymmetryType::Vertical:
				r1 += sym.coordinates.first * 100;
				break;
			default:
				break;
			}

			const auto altSym = getAlternateSymmetry(p, sym);
			switch (altSym.type)
			{
			case SymmetryType::Horizontal:
				r2 += altSym.coordinates.first;
				break;
			case SymmetryType::Vertical:
				r2 += altSym.coordinates.first * 100;
				break;
			default:
				break;
			}
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

void computeHashes(Pattern &p)
{
	hash<string> hasher;
	
	p.rowHashes.clear();
	p.colHashes.clear();
	for (auto i = 0; i < p.rows.size(); i++)
	{
		p.rowHashes.push_back(hasher(p.rows.at(i)));
	}
	for (auto i = 0; i < p.cols.size(); i++)
	{
		p.colHashes.push_back(hasher(p.cols.at(i)));
	}
}

std::vector<string> getColumns(std::vector<string>& rows)
{
	std::vector<string> cols{};

	for (auto j = 0; j < rows.at(0).size(); j++)
	{
		string col{};
		for (auto i = 0; i < rows.size(); i++)
		{
			col.push_back(rows.at(i).at(j));
		}
		cols.push_back(col);
	}
	return cols;
}

std::vector<Pattern> getPatterns(std::ifstream& inputFile)
{
	string inputString{ "" };
	std::vector<Pattern> patterns;

	Pattern pattern{};
	while (!inputFile.eof())
	{
		std::getline(inputFile, inputString, '\n');
		if (inputString.size() == 0)
		{
			pattern.cols = getColumns(pattern.rows);
			computeHashes(pattern);
			patterns.push_back(pattern);
			
			pattern.rows.clear();
			pattern.rowHashes.clear();
			pattern.cols.clear();
			pattern.colHashes.clear();
		}
		else
		{
			pattern.rows.push_back(inputString);
		}
	}
	if (pattern.rows.size() > 0)
	{
		pattern.cols = getColumns(pattern.rows);
		computeHashes(pattern);
		patterns.push_back(pattern);
	}

	std::cout << "Found a total of " << patterns.size() << " patterns" << std::endl;

	return patterns;
}

SymmetryInfo getAlternateSymmetry(const Pattern& p, const SymmetryInfo& origSym)
{
	auto i = 0;
	auto j = 0;
	SymmetryInfo sOut{ SymmetryType::Unknown, {0, 0} };
	auto altFound = false;
	auto cntAlt = 0;
	while ((i < p.rows.size()) and not(altFound))
	{
		j = 0;
		while ((j < p.cols.size()) and not(altFound))
		{
			auto pAlt = p;
			if (pAlt.rows.at(i).at(j) == '#')
			{
				pAlt.rows.at(i).at(j) = '.';
				pAlt.cols.at(j).at(i) = '.';
			}else if (pAlt.rows.at(i).at(j) == '.')
			{
				pAlt.rows.at(i).at(j) = '#';
				pAlt.cols.at(j).at(i) = '#';
			}
			computeHashes(pAlt);
			if (gDebugEnabled)
			{
				std::cout << "cntAlt:" << cntAlt << " for i:" << i << " j:" << j << std::endl;
			}
			const auto sAlt = getSymmetry(pAlt, origSym);
			if (sAlt.type != SymmetryType::NotFound)
			{
				sOut = sAlt;
				altFound = true;
			}
			j++;
			cntAlt++;
		}
		i++;
	}

	if (not(altFound))
	{
		std::cout << "ERROR! No alternate symmetry found" << std::endl;
	}

	return sOut;
}

SymmetryInfo getSymmetry(const Pattern p, const SymmetryInfo& origSym)
{
	SymmetryInfo symInfo{ SymmetryType::NotFound, {0, 0}};

	// find 2 consecutive rows with the same hash
	auto i = 1;
	auto symFound = false;
	while ((i < p.rowHashes.size()) and not(symFound))
	{
		if (p.rowHashes.at(i) == p.rowHashes.at(i - 1))
		{
			// possible symmetry, check other rows
			const int cntReq1 = p.rowHashes.size() - i - 1;
			const int cntReq2 = i - 1;
			int cntReq = 0;
			if (cntReq1 < cntReq2)
			{
				cntReq = cntReq1;
			}
			else
			{
				cntReq = cntReq2;
			}
			if (gDebugEnabled)
			{
				std::cout << "i:" << i << " i-1:" << i - 1 << " size:" << p.rowHashes.size() << " cntReq1:" << cntReq1 << " cntReq2:" << cntReq2 << " cntReq:" << cntReq << std::endl;
			}

			auto allReflecting = true;
			for (auto j = 1; j <= cntReq; j++)
			{
				if (p.rowHashes.at(i + j) != p.rowHashes.at(i - 1 - j))
				{
					allReflecting = false;
				}
			}
			symInfo.type = SymmetryType::Vertical;
			symInfo.coordinates.first = i;
			symInfo.coordinates.second = i + 1;
			if (allReflecting and (symInfo != origSym))
			{
				if (gDebugEnabled)
				{
					std::cout << "Vertical symmetry found between rows " << i << " and " << i - 1 << " (cntReq:" << cntReq << ")" << std::endl;
				}
				symFound = true;
			}
		}
		i++;
	}
	if (symFound)
	{
		return symInfo;
	}

	// find 2 consecutive columns with the same hash
	i = 1;
	while ((i < p.colHashes.size()) and not(symFound))
	{
		if (p.colHashes.at(i) == p.colHashes.at(i - 1))
		{
			// possible symmetry, check other cols
			const int cntReq1 = p.colHashes.size() - i - 1;
			const int cntReq2 = i - 1;
			auto cntReq = 0;
			if (cntReq1 < cntReq2)
			{
				cntReq = cntReq1;
			}
			else
			{
				cntReq = cntReq2;
			}
			if (gDebugEnabled)
			{
				std::cout << "i:" << i << " i-1:" << i - 1 << " size:" << p.colHashes.size() << " cntReq1:" << cntReq1 << " cntReq2:" << cntReq2 << " cntReq:" << cntReq << std::endl;

			}
			auto allReflecting = true;
			for (auto j = 1; j <= cntReq; j++)
			{
				if (p.colHashes.at(i + j) != p.colHashes.at(i - 1 - j))
				{
					allReflecting = false;
				}
			}
			symInfo.type = SymmetryType::Horizontal;
			symInfo.coordinates.first = i;
			symInfo.coordinates.second = i + 1;
			if (allReflecting and (symInfo != origSym))
			{
				if (gDebugEnabled)
				{
					std::cout << "Horizontal symmetry found between cols " << i << " and " << i - 1 << " (cntReq:" << cntReq << ")" << std::endl;
				}
				symFound = true;
			}
		}
		i++;
	}
	if (symFound)
	{
		return symInfo;
	}
	
	if (gDebugEnabled)
	{
		std::cout << "No symmetry found" << std::endl;
	}
	return SymmetryInfo{ SymmetryType::NotFound, {0, 0} };
}