// C++ program for implementing B+ Tree
#include <climits>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
using namespace std;
int MAX = 3;

// BP node
struct Node {
	bool IS_LEAF;
	int size;
    vector<int> key;
	vector<Node*> ptr;
    //Node建構的時候會出使化大小
	Node(){
        key = vector<int>(MAX);
	    ptr = vector<Node*>(MAX + 1);
    }
};

// BP tree
class BPTree {
	Node* root;
	void insertInternal(int, Node*, Node*);
	Node* findParent(Node*, Node*);

public:
	BPTree();
	void search(int);
	void insert(int);
	void display(Node*);
	Node* getRoot();
};

// Initialise the BPTree Node
BPTree::BPTree() {
	root = NULL;
}

// Function to find any element
// in B+ Tree
void BPTree::search(int x) {

	// If tree is empty
	if (root == NULL) {
		cout << "Tree is empty\n";
	}

	// Traverse to find the value
	else {

		Node* cursor = root;

		// 查詢到是leaf為止
		while (cursor->IS_LEAF == false) {

			for (int i = 0;i < cursor->size; i++) {

				// 這個比較大的話回傳左邊的指標
				if (x < cursor->key[i]) {
					cursor = cursor->ptr[i];
					break;
				}

				// I找完整個比全部都大 回傳最右邊的
				if (i == cursor->size - 1) {
					cursor = cursor->ptr[i + 1];
					break;
				}
			}
		}

		// 到達leaf層之後 遍歷整個key陣列找值
		for (int i = 0;i < cursor->size; i++) {
			if (cursor->key[i] == x) {
				cout << "Found\n";
				return;
			}
		}
		cout << "Not found\n";
	}
}

// Function to implement the Insert
// Operation in B+ Tree
void BPTree::insert(int x) {

	// 如果完全沒有Node 就初始化一個 並且他自己就是一個leaf
	if (root == NULL) {
		root = new Node;
		root->key[0] = x;
		root->IS_LEAF = true;
        //size是紀錄現在實際上有幾個值在vector裡面
		root->size = 1;
	}

	// Traverse the B+ Tree
	else {
		Node* cursor = root;
		Node* parent;

		// 往下搜索到leaf層
		while (cursor->IS_LEAF == false) {
            //用來紀錄leaf層的上一層
			parent = cursor;

			for (int i = 0;i < cursor->size;i++) {
				// 找到可以insert的位置
				if (x < cursor->key[i]) {
					cursor = cursor->ptr[i];
					break;
				}

				// 這個執筆全部都大 右邊新增
				if (i == cursor->size - 1) {
					cursor = cursor->ptr[i + 1];
					break;
				}
			}
		}
    
        //若是cursor裡面的元素還沒有放滿
		if (cursor->size < MAX) {
			int i = 0;
            //找到合適的position
			while (x > cursor->key[i] && i < cursor->size) {
				i++;
			}

            //插入數值的過程 很慢
			for (int j = cursor->size;j > i; j--) {
                cursor->key[j] = cursor->key[j - 1];
			}

			cursor->key[i] = x;
			cursor->size++;

            //重新調整pointer指向 主要調整最右邊那個
			cursor->ptr[cursor->size] = cursor->ptr[cursor->size - 1];
			cursor->ptr[cursor->size - 1] = NULL;
		}

        //overflow的情況
		else {
			// Create a newLeaf node
			Node* newLeaf = new Node;

            //建立一個運算用的array
			int virtualNode[MAX + 1];

			// 複製原陣列到運算用陣列
			for (int i = 0; i < MAX; i++) {
				virtualNode[i] = cursor->key[i];
			}
			int i = 0, j;

			// Traverse to find where the new
			// node is to be inserted
			while (x > virtualNode[i] && i < MAX) {
				i++;
			}

			// 運算用陣列進行插入
			for (int j = MAX + 1;j > i; j--) {
                virtualNode[j] = virtualNode[j - 1];
			}

			virtualNode[i] = x;
			newLeaf->IS_LEAF = true;

            //切割陣列
			cursor->size = (MAX + 1) / 2;
			newLeaf->size = MAX + 1 - (MAX + 1) / 2;

			cursor->ptr[cursor->size] = newLeaf;

			newLeaf->ptr[newLeaf->size] = cursor->ptr[MAX];

			cursor->ptr[MAX] = NULL;

			// Update the current virtual
			// Node's key to its previous
			for (i = 0;i < cursor->size; i++) 
				cursor->key[i] = virtualNode[i];
			

			// Update the newLeaf key to
			// virtual Node
			for (i = 0, j = cursor->size;i < newLeaf->size;i++, j++) 
				newLeaf->key[i]= virtualNode[j];
			

			// If cursor is the root node
			if (cursor == root) {

				// Create a new Node
				Node* newRoot = new Node;

				// Update rest field of
				// B+ Tree Node
				newRoot->key[0] = newLeaf->key[0];
				newRoot->ptr[0] = cursor;
				newRoot->ptr[1] = newLeaf;
				newRoot->IS_LEAF = false;
				newRoot->size = 1;
				root = newRoot;
			}
			else {

				// Recursive Call for
				// insert in internal
				insertInternal(newLeaf->key[0], parent, newLeaf);
			}
		}
	}
}

// Function to implement the Insert
// Internal Operation in B+ Tree
void BPTree::insertInternal(int x, Node* cursor, Node* child){

	// If we doesn't have overflow
	if (cursor->size < MAX) {
		int i = 0;

		// Traverse the child node
		// for current cursor node
		while (x > cursor->key[i] && i < cursor->size) 
			i++;

		// Traverse the cursor node
		// and update the current key
		// to its previous node key
		for (int j = cursor->size;j > i; j--) 
			cursor->key[j] = cursor->key[j - 1];
		

		// Traverse the cursor node
		// and update the current ptr
		// to its previous node ptr
		for (int j = cursor->size + 1;j > i + 1; j--) 
			cursor->ptr[j] = cursor->ptr[j - 1];
		
		cursor->key[i] = x;
		cursor->size++;
		cursor->ptr[i + 1] = child;
	}

	// For overflow, break the node
	else {

		// For new Interval
		Node* newInternal = new Node;
		int virtualKey[MAX + 1];
		Node* virtualPtr[MAX + 2];

		// Insert the current list key
		// of cursor node to virtualKey
		for (int i = 0; i < MAX; i++) 
			virtualKey[i] = cursor->key[i];
		

		// Insert the current list ptr
		// of cursor node to virtualPtr
		for (int i = 0; i < MAX + 1; i++) 
			virtualPtr[i] = cursor->ptr[i];
		
		int i = 0, j;

		// Traverse to find where the new
		// node is to be inserted
		while (x > virtualKey[i] && i < MAX) 
			i++;
		

		// Traverse the virtualKey node
		// and update the current key
		// to its previous node key
		for (int j = MAX + 1;j > i; j--) 
			virtualKey[j] = virtualKey[j - 1];
		

		virtualKey[i] = x;

		// Traverse the virtualKey node
		// and update the current ptr
		// to its previous node ptr
		for (int j = MAX + 2;j > i + 1; j--) 
			virtualPtr[j] = virtualPtr[j - 1];
		

		virtualPtr[i + 1] = child;
		newInternal->IS_LEAF = false;

		cursor->size = (MAX + 1) / 2;

		newInternal->size = MAX - (MAX + 1) / 2;

		// Insert new node as an
		// internal node
		for (i = 0, j = cursor->size + 1;i < newInternal->size;i++, j++) 
			newInternal->key[i] = virtualKey[j];
		

		for (i = 0, j = cursor->size + 1;i < newInternal->size + 1;i++, j++) 
			newInternal->ptr[i] = virtualPtr[j];
		

		// If cursor is the root node
		if (cursor == root) {

			// Create a new root node
			Node* newRoot = new Node;

			// Update key value
			newRoot->key[0] = cursor->key[cursor->size];

			// Update rest field of
			// B+ Tree Node
			newRoot->ptr[0] = cursor;
			newRoot->ptr[1] = newInternal;
			newRoot->IS_LEAF = false;
			newRoot->size = 1;
			root = newRoot;
		}

		else {

			// Recursive Call to insert
			// the data
			insertInternal(cursor->key[cursor->size],
				findParent(root,cursor),
				newInternal);
		}
	}
}

// Function to find the parent node
Node* BPTree::findParent(Node* cursor,Node* child) {
	Node* parent;

	// If cursor reaches the end of Tree
	if (cursor->IS_LEAF || (cursor->ptr[0])->IS_LEAF) 
		return NULL;

	// Traverse the current node with
	// all its child
	for (int i = 0;i < cursor->size + 1; i++) {

		// Update the parent for the
		// child Node
		if (cursor->ptr[i] == child) {
			parent = cursor;
			return parent;
		}

		// Else recursively traverse to
		// find child node
		else {
			parent = findParent(cursor->ptr[i],
							child);

			// If parent is found, then
			// return that parent node
			if (parent != NULL)
				return parent;
		}
	}

	// Return parent node
	return parent;
}

// Function to get the root Node
Node* BPTree::getRoot()
{
	return root;
}

// Driver Code
int main()
{
	BPTree node;

	// Create B+ Tree
	node.insert(6);
	node.insert(16);
	node.insert(26);
	node.insert(36);
	node.insert(46);

	// Function Call to search node
	// with value 16
	node.search(16);

	return 0;
}
