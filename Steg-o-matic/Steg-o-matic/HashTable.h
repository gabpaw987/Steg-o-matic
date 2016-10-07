#ifndef HASHTABLE_INCLUDED
#define HASHTABLE_INCLUDED

template <typename KeyType, typename ValueType>
class HashTable
{
public:
	HashTable(unsigned int numBuckets, unsigned int capacity);
	~HashTable();

	inline bool isFull() const { return m_nItems == m_capacity; }
	bool set(const KeyType& key, const ValueType& value, bool permanent = false);
	bool get(const KeyType& key, ValueType& value) const;
	bool touch(const KeyType& key);
	bool discard(KeyType& key, ValueType& value);

private:
	//We prevent a HashTable from being copied or assigned by declaring the
	//copy constructor and assignment operator private and not implementing them.
	HashTable(const HashTable&);
	HashTable& operator=(const HashTable&);

	struct Node
	{
		Node()
		: m_next(nullptr), m_nextRecentlyWritten(nullptr), m_lastRecentlyWritten(nullptr) { }
		Node(const KeyType& key, const ValueType& value, Node* next = nullptr, Node* nextRecentlyWritten = nullptr, Node* lastRecentlyWritten = nullptr)
		: m_key(key), m_value(value), m_next(next), m_nextRecentlyWritten(nextRecentlyWritten), m_lastRecentlyWritten(lastRecentlyWritten) {}

		KeyType m_key;
		ValueType m_value;
		Node* m_next;
		Node* m_nextRecentlyWritten;
		Node* m_lastRecentlyWritten;
	};

	unsigned int determineBucketFromHash(const unsigned int hash) const;
	unsigned int getBucketFromKey(const KeyType& key) const;

private:
	unsigned int m_numBuckets;
	unsigned int m_capacity;
	unsigned int m_nItems;

	Node** m_hashTable;
	Node* m_head;

};

template <typename KeyType, typename ValueType>
HashTable<KeyType, ValueType>::HashTable(unsigned int numBuckets, unsigned int capacity)
: m_numBuckets(numBuckets), m_capacity(capacity), m_nItems(0), m_head(new Node())
{
	m_head->m_nextRecentlyWritten = m_head;
	m_head->m_lastRecentlyWritten = m_head;

	m_hashTable = new Node*[m_numBuckets];

	for (size_t i = 0; i < m_numBuckets; i++)
		m_hashTable[i] = nullptr;
}

template <typename KeyType, typename ValueType>
HashTable<KeyType, ValueType>::~HashTable()
{
	for (size_t i = 0; i < m_numBuckets; i++)
	{
		for (Node* currentNode = m_hashTable[i]; currentNode != nullptr;)
		{
			Node* tmpCurrentNode = currentNode;
			currentNode = currentNode->m_next;
			delete tmpCurrentNode;
		}
	}

	delete[] m_hashTable;

	delete m_head;
}

template <typename KeyType, typename ValueType>
bool HashTable<KeyType, ValueType>::set(const KeyType& key, const ValueType& value, bool permanent)
{
	unsigned int keyBucket = getBucketFromKey(key);
	for (Node* currentNode = m_hashTable[keyBucket]; currentNode != nullptr; currentNode = currentNode->m_next)
	{
		if (currentNode->m_key == key)
		{
			currentNode->m_value = value;
			
			if (currentNode->m_nextRecentlyWritten != nullptr && currentNode->m_lastRecentlyWritten != nullptr)
			{
				currentNode->m_lastRecentlyWritten->m_nextRecentlyWritten = currentNode->m_nextRecentlyWritten;
				currentNode->m_nextRecentlyWritten->m_lastRecentlyWritten = currentNode->m_lastRecentlyWritten;
				currentNode->m_nextRecentlyWritten = m_head->m_nextRecentlyWritten;
				currentNode->m_lastRecentlyWritten = m_head;
				m_head->m_nextRecentlyWritten = currentNode;
				currentNode->m_nextRecentlyWritten->m_lastRecentlyWritten = currentNode;
			}

			return true;
		}
	}

	if (isFull())
		return false;

	Node* newNode = new Node(key, value);

	if (!permanent)
	{
		newNode->m_nextRecentlyWritten = m_head->m_nextRecentlyWritten;
		newNode->m_lastRecentlyWritten = m_head;
		m_head->m_nextRecentlyWritten->m_lastRecentlyWritten = newNode;
		m_head->m_nextRecentlyWritten = newNode;
	}

	Node* currentNode = m_hashTable[keyBucket];
	if (currentNode != nullptr)
	{
		for (; currentNode->m_next != nullptr; currentNode = currentNode->m_next)
			;
		currentNode->m_next = newNode;
	}
	else
	{
		m_hashTable[keyBucket] = newNode;
	}
	
	m_nItems++;

	return true;
}

template <typename KeyType, typename ValueType>
bool HashTable<KeyType, ValueType>::get(const KeyType& key, ValueType& value) const
{
	unsigned int keyBucket = getBucketFromKey(key);
	for (Node* currentNode = m_hashTable[keyBucket]; currentNode != nullptr; currentNode = currentNode->m_next)
	{
		if (currentNode->m_key == key)
		{
			value = currentNode->m_value;
			return true;
		}
	}

	return false;
}


template <typename KeyType, typename ValueType>
bool HashTable<KeyType, ValueType>::touch(const KeyType& key)
{
	unsigned int keyBucket = getBucketFromKey(key);
	for (Node* currentNode = m_hashTable[keyBucket]; currentNode != nullptr; currentNode = currentNode->m_next)
	{
		if (currentNode->m_key == key && currentNode->m_nextRecentlyWritten != nullptr && currentNode->m_lastRecentlyWritten != nullptr)
		{
			currentNode->m_lastRecentlyWritten->m_nextRecentlyWritten = currentNode->m_nextRecentlyWritten;
			currentNode->m_nextRecentlyWritten->m_lastRecentlyWritten = currentNode->m_lastRecentlyWritten;
			currentNode->m_nextRecentlyWritten = m_head->m_nextRecentlyWritten;
			currentNode->m_lastRecentlyWritten = m_head;
			m_head->m_nextRecentlyWritten = currentNode;
			currentNode->m_nextRecentlyWritten->m_lastRecentlyWritten = currentNode;
			return true;
		}
	}
	return false;
}

template <typename KeyType, typename ValueType>
bool HashTable<KeyType, ValueType>::discard(KeyType& key, ValueType& value)
{
	Node* nodeToDiscard = m_head->m_lastRecentlyWritten;

	if (nodeToDiscard == m_head)
		return false;

	key = nodeToDiscard->m_key;
	value = nodeToDiscard->m_value;
	nodeToDiscard->m_lastRecentlyWritten->m_nextRecentlyWritten = m_head;
	m_head->m_lastRecentlyWritten = nodeToDiscard->m_lastRecentlyWritten;

	unsigned int keyBucket = getBucketFromKey(key);

	Node* currentNode = m_hashTable[keyBucket];
	if (currentNode->m_next != nullptr)
	{
		for (; currentNode->m_next != nullptr; currentNode = currentNode->m_next)
		{
			if (currentNode->m_next->m_key == key)
			{
				currentNode->m_next = nodeToDiscard->m_next;
				break;
			}
		}
	}
	else
	{
		m_hashTable[keyBucket] = nullptr;
	}

	delete nodeToDiscard;
	m_nItems--;

	return true;
}

template <typename KeyType, typename ValueType>
unsigned int HashTable<KeyType, ValueType>::determineBucketFromHash(const unsigned int hash) const
{
	return hash % m_numBuckets;
}

template <typename KeyType, typename ValueType>
unsigned int HashTable<KeyType, ValueType>::getBucketFromKey(const KeyType& key) const
{
	unsigned int computeHash(KeyType);
	return determineBucketFromHash(computeHash(key));
}

#endif // HASHTABLE_INCLUDED