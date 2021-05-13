#ifndef INDEX_H_
#define INDEX_H_
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <stack>
using namespace std;
// BP node
struct Node {
	bool IS_LEAF;
	int size;
    vector<int> key;
	vector<int> value;
	vector<Node*> ptr;
    //Node建構的時候會出使化大小
	Node();
};

// BP tree
class Index {
	Node* root;
	void insertInternal(int, stack<Node*>, Node*);
	Node* findParent(Node*, Node*);

public:
	Index();
	Index(int num_rows, vector<int>& key, vector<int>& value);
	int search(int);
	void key_query(vector<int>&);
	void range_query(vector<pair<int, int>>&);
	void insert(int, int);
	Node* getRoot();
	void clear_index();
	void deleteBp(Node*);
	Node* searchLeaf(int);
};
#endif