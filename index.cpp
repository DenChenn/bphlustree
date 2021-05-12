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
	vector<int> value;
	vector<Node*> ptr;
    //Node建構的時候會出使化大小
	Node(){
        key = vector<int>(MAX);
		value = vector<int>(MAX);
	    ptr = vector<Node*>(MAX + 1);
    }
};

// BP tree
class Index {
	Node* root;
	void insertInternal(int, int, Node*, Node*);
	Node* findParent(Node*, Node*);

public:
	Index();
	Index(int num_rows, vector<int>& key, vector<int>& value);
	int search(int);
	void key_query(vector<int>&);
	void range_query(vector<pair<int, int>>&);
	void insert(int, int);
	void display(Node*);
	Node* getRoot();
};

// Initialise the Index Node
Index::Index() {
	root = NULL;
}

Index::Index(int num_rows, vector<int>& key, vector<int>& value) {
	for(int i = 0;i < num_rows;i++){
		this->insert(key[i], value[i]);
	}
}

void Index::key_query(vector<int>& key) {
	ofstream output;
	output.open("key_query_out.txt");

	for(auto k : key) {
		int temp = this->search(k);
		output << temp << endl;
	}
	output.close();
}

void Index::range_query(vector<pair<int,int>>& pp){
	ofstream output;
	output.open("range_query_out.txt");

	if (root == NULL){
		output << -1 << endl;
		return;
	}
	else {
		Node* cursor = root;

		for(auto p : pp){
			// 查詢到是leaf為止 找到 lb
			while (cursor->IS_LEAF == false) {

				for (int i = 0;i < cursor->size; i++) {

					// 這個比較大的話回傳左邊的指標
					if (p.first < cursor->key[i]) {
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

			int maxValue = -1;

			// 到達leaf層之後 遍歷整個key陣列找值
			bool ubIsFound = false;
			while(cursor != NULL) {
				for (int i = 0;i < cursor->size; i++) {
					//找到了 ub
					if (cursor->key[i] > p.second) {
						ubIsFound = true;
						break;
					}
					else if(cursor->key[i] >= p.first && cursor->key[i] <= p.second){
						if(cursor->value[i] > maxValue)
							maxValue = cursor->value[i];
					}
				}
				if (ubIsFound){
					output << maxValue << endl;
					break;
				}
				else
					cursor = cursor->ptr[cursor->size];
			}
			if(!ubIsFound)
				output << -1 << endl;
		}
	}
}



int Index::search(int x) {

	// If tree is empty
	if (root == NULL) 
		return -1;


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
				return cursor->value[i];
			}
		}
		return -1;
	}
}

// Function to implement the Insert
// Operation in B+ Tree
void Index::insert(int key, int value) {

	// 如果完全沒有Node 就初始化一個 並且他自己就是一個leaf
	if (root == NULL) {
		root = new Node;
		root->key[0] = key;
		root->value[0] = value;
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
				if (key < cursor->key[i]) {
					cursor = cursor->ptr[i];
					break;
				}

				// 這個值筆全部都大 右邊新增
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
			while (key > cursor->key[i] && i < cursor->size) {
				i++;
			}

            //插入數值的過程 
			for (int j = cursor->size;j > i; j--) {
                cursor->key[j] = cursor->key[j - 1];
				cursor->value[j] = cursor->value[j - 1];
			}

			cursor->key[i] = key;
			cursor->value[i] = value;
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
			int virtualKeyArray[MAX + 1];
			int virtualValueArray[MAX + 1];
			

			// 複製原陣列到運算用陣列
			for (int i = 0; i < MAX; i++) {
				virtualKeyArray[i] = cursor->key[i];
				virtualKeyArray[i] = cursor->value[i];
			}

			int i = 0, j;

			// Traverse to find where the new
			// node is to be inserted
			while (key > virtualKeyArray[i] && i < MAX) {
				i++;
			}

			// 運算用陣列進行插入
			for (int j = MAX + 1;j > i; j--) {
                virtualKeyArray[j] = virtualKeyArray[j - 1];
				virtualValueArray[j] = virtualValueArray[j - 1];
			}

			virtualKeyArray[i] = key;
			virtualValueArray[i] = value;
			newLeaf->IS_LEAF = true;

            //切割陣列
			cursor->size = (MAX + 1) / 2;
			newLeaf->size = MAX + 1 - (MAX + 1) / 2;

			cursor->ptr[cursor->size] = newLeaf;

			newLeaf->ptr[newLeaf->size] = cursor->ptr[MAX];

			cursor->ptr[MAX] = NULL;

			// Update the current virtual
			// Node's key to its previous
			for (i = 0;i < cursor->size; i++) {
				cursor->key[i] = virtualKeyArray[i];
				cursor->value[i] = virtualValueArray[i];
			}
			

			// Update the newLeaf key to
			// virtual Node
			for (i = 0, j = cursor->size;i < newLeaf->size;i++, j++) {
				newLeaf->key[i]= virtualKeyArray[j];
				newLeaf->value[i] = virtualValueArray[j];
			}

			// If cursor is the root node
			if (cursor == root) {

				// Create a new Node
				Node* newRoot = new Node;

				// 最一開始的時候的情況
				newRoot->key[0] = newLeaf->key[0];
				newRoot->value[0] = newLeaf->value[0];
				newRoot->ptr[0] = cursor;
				newRoot->ptr[1] = newLeaf;
				newRoot->IS_LEAF = false;
				newRoot->size = 1;
				root = newRoot;
			}
			else {

				// 如果現在這裡不是root => 上面有parent
				insertInternal(newLeaf->key[0], newLeaf->value[0], parent, newLeaf);
			}
		}
	}
}

void Index::insertInternal(int key, int value, Node* cursor, Node* child){

	// parent 還沒放滿
	if (cursor->size < MAX) {
		int i = 0;

		//找到插入點
		while (key > cursor->key[i] && i < cursor->size) 
			i++;

		// 調整插入後的key跟value
		for (int j = cursor->size;j > i; j--) {
			cursor->key[j] = cursor->key[j - 1];
			cursor->value[j] = cursor->value[j - 1];
		}
		

		// 調整插入後的指標
		for (int j = cursor->size + 1;j > i + 1; j--) 
			cursor->ptr[j] = cursor->ptr[j - 1];
		
		//最後將值插入
		cursor->key[i] = key;
		cursor->value[i] = value;
		cursor->size++;
		cursor->ptr[i + 1] = child;
	}

	// parent已經放滿了
	else {

		// 建立一個更上面新parent
		Node* newInternal = new Node;
		int virtualKeyArray[MAX + 1];
		int virtualValueArray[MAX + 1];
		Node* virtualPtr[MAX + 2];

		// 將key跟pointer放入運算用的array
		for (int i = 0; i < MAX; i++) {
			virtualKeyArray[i] = cursor->key[i];
			virtualValueArray[i] = cursor->value[i];
		}
		
		for (int i = 0; i < MAX + 1; i++) 
			virtualPtr[i] = cursor->ptr[i];
		
		int i = 0, j;

		// 找到合適的插入點
		while (key > virtualKeyArray[i] && i < MAX) 
			i++;
		

		//調整插入後的key
		for (int j = MAX + 1;j > i; j--) {
			virtualKeyArray[j] = virtualKeyArray[j - 1];
			virtualValueArray[j] = virtualValueArray[j - 1];
		}
		virtualKeyArray[i] = key;


		// 調整插入後的pointer
		for (int j = MAX + 2;j > i + 1; j--) 
			virtualPtr[j] = virtualPtr[j - 1];
		virtualPtr[i + 1] = child;
		newInternal->IS_LEAF = false;

		//切割
		cursor->size = (MAX + 1) / 2;

		newInternal->size = MAX - (MAX + 1) / 2;

		// 運算完的放入新的node
		for (i = 0, j = cursor->size + 1;i < newInternal->size;i++, j++) {
			newInternal->key[i] = virtualKeyArray[j];
			newInternal->value[i] =virtualValueArray[j];
		}
		

		for (i = 0, j = cursor->size + 1;i < newInternal->size + 1;i++, j++) 
			newInternal->ptr[i] = virtualPtr[j];
		

		// 如果已經推到頂是root了
		if (cursor == root) {

			// Create a new root node
			Node* newRoot = new Node;

			// Update key value
			newRoot->key[0] = cursor->key[cursor->size];
			newRoot->value[0] = cursor->value[cursor->size];

			// Update rest field of
			// B+ Tree Node
			newRoot->ptr[0] = cursor;
			newRoot->ptr[1] = newInternal;
			newRoot->IS_LEAF = false;
			newRoot->size = 1;
			root = newRoot;
		}

		//沒有的話再往上推 到沒有滿的parent或是抵達root為止
		else {

			// Recursive Call to insert
			// the data
			insertInternal(
				cursor->key[cursor->size],
				cursor->value[cursor->size],
				findParent(root,cursor),
				newInternal);
		}
	}
}

// Function to find the parent node
Node* Index::findParent(Node* cursor,Node* child) {
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
Node* Index::getRoot()
{
	return root;
}

// Driver Code
int main() {	
	vector<int> key = {6, 16, 26, 36, 46};
	vector<int> value = {5, 4, 3, 2, 1};
	Index node(5, key, value);

	// Create B+ Tree

	// Function Call to search node
	// with value 16
	vector<int> key_list = {16, 26};
	node.key_query(key_list);

	return 0;
}
