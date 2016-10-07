#include "provided.h"
#include <string>
#include <vector>
using namespace std;

const size_t BITS_PER_UNSIGNED_SHORT = 16;

string convertNumberToBitString(unsigned short number);
bool convertBitStringToNumber(string bitString, unsigned short& number);

string BinaryConverter::encode(const vector<unsigned short>& numbers)
{
	string encodedNumbers;

	for (int i = 0; i < numbers.size(); i++)
	{
		string bitString = convertNumberToBitString(numbers[i]);
		for (int j = 0; j < bitString.size(); j++)
			if (bitString[j] == '1')
				encodedNumbers += '\t';
			else
				encodedNumbers += ' ';
	}

	return encodedNumbers;
}

bool BinaryConverter::decode(const string& bitString,
							 vector<unsigned short>& numbers)
{
	numbers.clear();

	if (bitString.size() % 16 != 0)
		return false;

	string singleNumberBitString;
	for (int j = 0; j < bitString.size(); j++)
	{
		if (bitString[j] == '\t')
			singleNumberBitString += '1';
		else if (bitString[j] == ' ')
			singleNumberBitString += '0';
		else
			return false;

		if (singleNumberBitString.size() == 16)
		{
			unsigned short decodedNumber;
			if (convertBitStringToNumber(singleNumberBitString, decodedNumber))
				numbers.push_back(decodedNumber);
			else
				return false;

			singleNumberBitString = "";
		}
	}

	return true;
}

string convertNumberToBitString(unsigned short number)
{
	string result(BITS_PER_UNSIGNED_SHORT, '0');
	for (size_t k = BITS_PER_UNSIGNED_SHORT; number != 0; number /= 2)
	{
		k--;
		if (number % 2 == 1)
			result[k] = '1';
	}
	return result;
}

bool convertBitStringToNumber(string bitString, unsigned short& number)
{
	if (bitString.size() != BITS_PER_UNSIGNED_SHORT)
		return false;
	number = 0;
	for (size_t k = 0; k < bitString.size(); k++)
	{
		number *= 2;
		if (bitString[k] == '1')
			number++;
		else if (bitString[k] != '0')
			return false;
	}
	return true;
}
