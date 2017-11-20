// MyMap.h

// Skeleton for the MyMap class template.  You must implement the first six
// member functions.
// TURN IN



#include <iostream>
template<typename KeyType, typename ValueType>
class MyMap
{
public:
	MyMap();
	~MyMap();
	void clear();
	int size() const;
	void associate(const KeyType& key, const ValueType& value);

	const ValueType* find(const KeyType& key) const;

	  // for a modifiable map, return a pointer to modifiable ValueType
	ValueType* find(const KeyType& key)
	{
		return const_cast<ValueType*>(const_cast<const MyMap*>(this)->find(key));
	}

	  // C++11 syntax for preventing copying and assignment
	MyMap(const MyMap&) = delete;
	MyMap& operator=(const MyMap&) = delete;


private:
	struct Node {
		Node(KeyType k, ValueType v, Node* r, Node* l, Node* p) 
			: key(k), value(v), right(r), left(l), parent(p){}
		KeyType key;
		ValueType value;
		Node* right;
		Node* left;
		Node* parent;
	};
	
	// helper functions
	void deleteNode(Node* n) {
		if (n == nullptr)
			return;
		if (n->right != nullptr) {
			deleteNode(n->right);
			delete n->right;
		}
		if (n->left != nullptr) {
			deleteNode(n->left);
			delete n->left;
		}
	}

	Node* checkForNode(const KeyType key, Node* node) const {
		if (node == nullptr)
			return node;

		if (key == node->key)
			return node;
		else if (node->key < key)
			return checkForNode(key, node->right);
		else
			return checkForNode(key, node->left);
	}

	void insert(const KeyType& key, Node* node, const ValueType& value) {
		if (node == nullptr) {
			m_head = new Node(key, value, nullptr, nullptr, nullptr);
			nodeCount++;
			return;
		}

		if (node->key < key) {
			if (node->right == nullptr) {
				node->right = new Node(key, value, nullptr, nullptr, node);
				nodeCount++;
				return;
			}
			else
				insert(key, node->right, value);
		}
		else if (node->key > key) {
			if (node->left == nullptr) {
				node->left = new Node(key, value, nullptr, nullptr, node);
				nodeCount++;
				return;
			}
			else
				insert(key, node->left, value);
		}
	}

	//private member variables
	Node* m_head;
	int nodeCount = 0;
};


template<typename KeyType, typename ValueType>
MyMap< KeyType,  ValueType>::MyMap() : m_head(nullptr) {}

template<typename KeyType, typename ValueType>
MyMap< KeyType,  ValueType>::~MyMap() {
	clear();
}

template<typename KeyType, typename ValueType>
void MyMap< KeyType,  ValueType>::clear() { 
	deleteNode(m_head);
	delete m_head;
	m_head = nullptr;
}

template<typename KeyType, typename ValueType>
int MyMap< KeyType,  ValueType>::size() const {return nodeCount;}

//	The	associate	method	associates	one	item	(key)	with	another	(value).
//	If	no	association	currently	exists	with	that	key,	this	method	inserts
//	a	new	association	into	the	tree	with	that	key/value	pair.		If	there	is
//	already	an	association	with	that	key	in	the	tree,	then	the	item
//	associated	with	that	key	is	replaced	by	the	second	parameter	(value).
//	Thus,	the	tree	contains	no	duplicate	keys.
template<typename KeyType, typename ValueType>
void MyMap< KeyType,  ValueType>::associate(const KeyType& key, const ValueType& value) {
	Node* node = checkForNode(key, m_head);
	if (node == nullptr) {
		insert(key, m_head, value);
	}
	else
		node->value = value;
}

// for a map that can't be modified, return a pointer to const ValueType
//	If	no	association	exists	with	the	given	key,	return	nullptr;	otherwise,
//	return	a	pointer	to	the	value	associated	with	that	key.		This	pointer	can	be
//	used	to	examine	that	value,	and	if	the	map	is	allowed	to	be	modified,	to
//	modify	that	value	directly	within	the	map (the	second	overload	enables
//	this).		Using	a	little	C++	magic,	we	have	implemented	it	in terms	of	the
//	first	overload,	which	you	must	implement.
template<typename KeyType, typename ValueType>
const ValueType* MyMap< KeyType,  ValueType>::find(const KeyType& key) const {
	Node* node = checkForNode(key, m_head);
	if (node == nullptr)
		return nullptr;
	else
		return &(node->value);
}