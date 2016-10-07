#include "provided.h"
#include <string>
using namespace std;

void splitStringByLineBreaks(const string& stringToSplit, vector<string>& splittedString)
{
	splittedString.push_back("");
	for (size_t i = 0; i < stringToSplit.size(); i++)
	{
		if (stringToSplit[i] == '\r' && stringToSplit.size() >= i + 2 && stringToSplit[i + 1] == '\n')
			i++;

		if (stringToSplit[i] == '\n')
		{
			if (i != (stringToSplit.size() - 1))
				splittedString.push_back("");
		}
		else
			splittedString[splittedString.size() - 1] += stringToSplit[i];
	}
}

bool Steg::hide(const string& hostIn, const string& msg, string& hostOut) 
{
	if (hostIn.empty())
		return false;
	if (msg.empty())
		return true;

	vector<string> splittedHostIn;
	splitStringByLineBreaks(hostIn, splittedHostIn);

	vector<unsigned short> compressedMsg;
	Compressor::compress(msg, compressedMsg);
	string encodedString = BinaryConverter::encode(compressedMsg);

	int noOfLongerStrings = encodedString.size() % splittedHostIn.size();

	int j = 0;
	for (size_t i = 0; i < splittedHostIn.size(); i++)
	{
		int strEnd = splittedHostIn[i].find_last_not_of(" \t");
		splittedHostIn[i] = splittedHostIn[i].substr(0, strEnd + 1);

		int sizeOfThisCompressedString = (encodedString.size() / splittedHostIn.size()) + (i < noOfLongerStrings ? 1 : 0);
		int oldJ = j;
		for (; (j - oldJ) < sizeOfThisCompressedString; j++)
			splittedHostIn[i] += encodedString[j];
		hostOut += splittedHostIn[i] + "\n";
	}

	return true;
}

bool Steg::reveal(const string& hostIn, string& msg) 
{
	if (hostIn.empty())
		return false;

	vector<string> splittedHostIn;
	splitStringByLineBreaks(hostIn, splittedHostIn);

	string encodedMsg;

	int sizeOfLineContents = 0;
	for (size_t i = 0; i < splittedHostIn.size(); i++)
	{
		int strEnd = splittedHostIn[i].find_last_not_of(" \t");
		string endOfThisLine;
		if (strEnd != splittedHostIn[i].size() - 1)
			endOfThisLine = splittedHostIn[i].substr(strEnd + 1, splittedHostIn[i].size());
		if (sizeOfLineContents == 0)
			sizeOfLineContents = endOfThisLine.size();

		if (sizeOfLineContents != endOfThisLine.size() && sizeOfLineContents - 1 != endOfThisLine.size())
			return false;

		encodedMsg += endOfThisLine;
	}

	vector<unsigned short> decodedMsg;
	string decompressedMsg;
	if (!BinaryConverter::decode(encodedMsg, decodedMsg) || !Compressor::decompress(decodedMsg, decompressedMsg))
		return false;
	
	msg = decompressedMsg;

	return true;
}
