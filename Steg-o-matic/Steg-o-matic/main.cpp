// We have not yet produced the test driver main routine for you.

#include "provided.h"
#include "HashTable.h"
#include "http.h"
#include <iostream>
#include <cassert>
using namespace std;

void testHashTable()
{
	HashTable<string, int> table(1, 0);
	assert(table.isFull());
	assert(!table.set("Joe", 3));
	assert(!table.set("Billy", 0));

	HashTable<string, unsigned short> table2(1, 2);
	assert(!table2.isFull());
	assert(table2.set("Joe", 3));
	assert(table2.set("Billy", 0));
	assert(!table2.set("Madeline", 7));
	assert(table2.isFull());

	HashTable<string, unsigned short> table3(2, 2);
	assert(table3.set("hello", 2, true));
	assert(!table3.touch("hello"));
	assert(table3.set("you", 5, true));
	assert(!table3.set("hi", 0));
	assert(!table3.set("Hello", 2));
	assert(!table3.set("YOU", 3));
	assert(table3.set("hello", 1));
	assert(table3.set("you", 0));
	assert(!table3.set("hi", 0));
	assert(!table3.set("Hello", 2));
	assert(!table3.set("YOU", 3));

	unsigned short value = 999;
	assert(!table3.get("hi", value) && value == 999);
	assert(!table3.get("Hello", value) && value == 999);
	assert(!table3.get("YOU", value) && value == 999);

	assert(table3.get("hello", value) && value == 1);
	assert(table3.get("you", value) && value == 0);
	assert(!table3.get("welcome", value) && value == 0);

	assert(!table3.touch("hello"));
	assert(!table3.touch("you"));
	assert(!table3.touch("greetings"));
	assert(!table3.touch("Hello"));

	HashTable<string, unsigned short> table4(10, 5);
	value = 999;
	unsigned short discardedV = 999;
	string discardedS = "";

	assert(!table4.isFull());
	assert(!table4.get("hello", value) && value == 999);
	assert(!table4.get("welcome", value) && value == 999);
	assert(table4.set("welcome", 3));
	assert(table4.set("hi", 2));
	assert(!table4.isFull());
	assert(!table4.get("Welcome", value) && value == 999);
	assert(table4.get("welcome", value) && value == 3);
	assert(table4.get("hi", value) && value == 2);
	assert(table4.set("welcome", 1, true));
	assert(table4.get("welcome", value) && value == 1);

	assert(table4.discard(discardedS, discardedV) && discardedS == "hi" && discardedV == 2);
	assert(table4.discard(discardedS, discardedV) && discardedS == "welcome" && discardedV == 1);
	assert(!table4.discard(discardedS, discardedV) && discardedS == "welcome" && discardedV == 1);
	assert(!table4.discard(discardedS, discardedV) && discardedS == "welcome" && discardedV == 1);
	assert(!table4.get("welcome", value));
	assert(!table4.get("hi", value));

	assert(table4.set("permanent", 3, true));
	assert(table4.set("perm2", 2, true));
	assert(table4.set("perm3", 1, true));
	assert(!table4.isFull());
	assert(table4.set("permanent", 5, false));
	assert(table4.set("perm2", 3, true));
	assert(table4.set("perm3", 0, false));
	assert(!table4.discard(discardedS, discardedV));
	assert(table4.get("permanent", value) && value == 5);
	assert(table4.get("perm2", value) && value == 3);
	assert(table4.get("perm3", value) && value == 0);
	assert(!table4.discard(discardedS, discardedV));
	assert(!table4.touch("permanent"));
	assert(!table4.touch("perm2"));
	assert(!table4.touch("perm3"));
	assert(table4.get("permanent", value) && value == 5);
	assert(table4.get("perm2", value) && value == 3);
	assert(table4.get("perm3", value) && value == 0);
	assert(!table4.discard(discardedS, discardedV));
	assert(!table4.isFull());

	assert(table4.set("non-perm", 4));
	assert(table4.set("np2", 5));
	assert(!table4.set("extra room?", 1));
	assert(!table4.set("really no room?", 3));
	assert(table4.isFull());

	assert(table4.touch("non-perm"));
	assert(table4.discard(discardedS, discardedV) && discardedS == "np2" && discardedV == 5);

	assert(table4.set("gets deleted first", 10));
	assert(table4.touch("gets deleted first"));
	assert(table4.touch("non-perm"));
	assert(table4.discard(discardedS, discardedV) && discardedS == "gets deleted first" && discardedV == 10);
}

void testBinaryConverter()
{
	vector<unsigned short> numbers;

	numbers.push_back(0);
	assert(BinaryConverter::encode(numbers) == "                ");
	numbers.clear();
	numbers.push_back(1);
	assert(BinaryConverter::encode(numbers) == "               \t");
	numbers.clear();
	numbers.push_back(5);
	assert(BinaryConverter::encode(numbers) == "             \t \t");
	numbers.clear();
	numbers.push_back(19);
	assert(BinaryConverter::encode(numbers) == "           \t  \t\t");
	numbers.clear();
	numbers.push_back(18);
	assert(BinaryConverter::encode(numbers) == "           \t  \t ");
	numbers.push_back(19);
	assert(BinaryConverter::encode(numbers) == "           \t  \t            \t  \t\t");
	numbers.push_back(1);
	assert(BinaryConverter::encode(numbers) == "           \t  \t            \t  \t\t               \t");
	numbers.push_back(5);
	assert(BinaryConverter::encode(numbers) == "           \t  \t            \t  \t\t               \t             \t \t");
	numbers.clear();
	string msg;

	msg = "          \t    -";
	assert(!BinaryConverter::decode(msg, numbers) && numbers.size() == 0);
	numbers.clear();
	msg = "          \t _   ";
	assert(!BinaryConverter::decode(msg, numbers) && numbers.size() == 0);
	numbers.clear();
	msg = "          \t _  ";
	assert(!BinaryConverter::decode(msg, numbers) && numbers.size() == 0);
	numbers.clear();
	msg = "    \t   ";
	assert(!BinaryConverter::decode(msg, numbers) && numbers.size() == 0);
	numbers.clear();
	/*msg = "";
	assert(!BinaryConverter::decode(msg, numbers) && numbers.size() == 0);
	numbers.clear();*/
	msg = "0000100100111001";
	assert(!BinaryConverter::decode(msg, numbers) && numbers.size() == 0);
	numbers.clear();

	msg = "               \t";
	assert(BinaryConverter::decode(msg, numbers) && numbers.size() == 1 && numbers[0] == 1);
	numbers.clear();
	msg = "             \t \t";
	assert(BinaryConverter::decode(msg, numbers) && numbers.size() == 1 && numbers[0] == 5);
	numbers.clear();
	msg = "               \t             \t \t";
	assert(BinaryConverter::decode(msg, numbers) && numbers.size() == 2 && numbers[0] == 1
		&& numbers[1] == 5);
	numbers.clear();
	msg = "               \t             \t \t           \t  \t\t           \t  \t ";
	assert(BinaryConverter::decode(msg, numbers) && numbers.size() == 4 && numbers[0] == 1
		&& numbers[1] == 5 && numbers[2] == 19 && numbers[3] == 18);
	numbers.clear();
}

void testCompressor()
{
	string input;
	vector<unsigned short> numbers;

	input = "AAAAAAAAAB";
	Compressor::compress(input, numbers);
	assert(numbers.size() == 8 && numbers[0] == 65 && numbers[1] == 65 && numbers[2] == 256
		&& numbers[3] == 65 && numbers[4] == 257 && numbers[5] == 65 && numbers[6] == 66
		&& numbers[7] == 517);
	input = "";
	assert(Compressor::decompress(numbers, input) && input == "AAAAAAAAAB");
	numbers.clear();
	numbers.push_back(517); // Note numbers contains just a capacity here.
	assert(Compressor::decompress(numbers, input) && input == "");
	numbers.clear();
	input = "XYZ";
	assert(!Compressor::decompress(numbers, input) && input == "XYZ");
}


int main()
{
	/testHashTable();
	testBinaryConverter();
	testCompressor();

	/*HashTable<string, string> ht(10, 10);
	ht.set("heey", "its me");
	ht.set("hey", "yup mario");
	ht.set("not", "really there, you know", true);
	ht.set("x", "xxx");

	string key = "not";
	string value;
	cout << ht.get(key, value) << key << value << endl;

	cout << ht.touch("heey") << endl;
	*/


	string text;
	if ( ! WebSteg::hideMessageInPage("http://cs.ucla.edu", "Hello there!", text))
		cout << "Error hiding!" << endl;

	HTTP().set("http://cs.ucla.edu", text);

	string msg;
	if ( ! WebSteg::revealMessageInPage("http://cs.ucla.edu", msg))
		cout << "Error revealing!" << endl;
}
