#include "provided.h"
#include "HashTable.h"
#include <string>
#include <vector>
using namespace std;

void Compressor::compress(const string& s, vector<unsigned short>& numbers)
{
	int capacity = (s.size() / 2) + 512;
	if (capacity > 16384)
		capacity = 16384;
	
	HashTable<string, unsigned short> hashTable(capacity * 2, capacity);

	for (unsigned short j = 0; j < 256; j++)
		hashTable.set(string(1, static_cast<char>(j)), j, true);

	unsigned short nextFreeID = 256;
	string runSoFar;
	numbers = vector<unsigned short>();

	for (size_t i = 0; i < s.size(); i++)
	{
		char c = s[i];

		string expandedRun = runSoFar + c;
		unsigned short discardedValue;
		if (hashTable.get(expandedRun, discardedValue))
		{
			runSoFar = expandedRun;
			continue;
		}

		unsigned short x;
		hashTable.get(runSoFar, x);
		numbers.push_back(x);
		hashTable.touch(runSoFar);
		runSoFar = "";

		unsigned short cv;
		hashTable.get(string(1, c), cv);
		numbers.push_back(cv);

		string garbageString;

		if (hashTable.set(expandedRun, nextFreeID))
			nextFreeID++;
		else if (hashTable.discard(garbageString, discardedValue))
			hashTable.set(expandedRun, discardedValue);
	}

	if (!runSoFar.empty())
	{
		unsigned short x;
		hashTable.get(runSoFar, x);
		numbers.push_back(x);
	}

	numbers.push_back(capacity);
}

bool Compressor::decompress(const vector<unsigned short>& numbers, string& s)
{
	if (numbers.empty())
		return false;

	int capacity = numbers[numbers.size() - 1];
	HashTable<unsigned short, string> hashTable(capacity * 2, capacity);

	for (unsigned short j = 0; j < 256; j++)
		hashTable.set(j, string(1, static_cast<char>(j)), true);

	unsigned short nextFreeID = 256;
	string runSoFar;
	string output;

	for (size_t i = 0; i < numbers.size() - 1; i++)
	{
		unsigned short us = numbers[i];

		string S;

		if (us <= 255)
		{
			hashTable.get(us, S);
			output += S;

			if (runSoFar.empty())
			{
				runSoFar = S;
				continue;
			}

			string expandedRun = runSoFar + S;
			unsigned short discardedValue;
			string garbageString;
			if (hashTable.set(nextFreeID, expandedRun))
				nextFreeID++;
			else if (hashTable.discard(discardedValue, garbageString))
				hashTable.set(discardedValue, expandedRun);

			runSoFar = "";
			continue;
		}

		if (!hashTable.get(us, S))
			return false;
		hashTable.touch(us);

		output += S;
		runSoFar = S;
	}

	s = output;

	return true;
}

unsigned int computeHash(string key)
{
	unsigned int hash = 2166136261U;

	for (size_t i = 0; i < key.size(); i++)
	{
		hash += key[i];
		hash *= 16777619;
	}

	return hash;
}

unsigned int computeHash(unsigned short key)
{
	return key;
}