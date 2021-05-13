#include "index.h"
int ORDER = 100;

Node::Node(){
	for(int i = 0;i < ORDER + 1;i++){
    	ptr[i]=NULL;
  	}
}

Index::Index(int num_rows, vector<int>& key, vector<int>& value) {
	root = NULL;
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

void Index::range_query(vector<pair<int,int>>& r){
	int maxValue;
	Node *cursor;
	ofstream output;
	output.open("key_query_out.txt");

	for(int i = 0;i < r.size();i++){
		cursor = searchLeaf(r[i].first);
		maxValue = -1000000001;
		while(true){
			for(int j = 0;j < cursor->size;j++){
				if(cursor->key[j] >= r[i].first && cursor->key[j] <= r[i].second && cursor->value[j] > maxValue){
					maxValue = cursor->value[j];
				}
			}
			if(r[i].second <= cursor->key[cursor->size-1])
				break;
			
			else{
				cursor = cursor->ptr[cursor->size];
				if(cursor == NULL)
					break;
			}
		}
		if(maxValue != -1000000001) {
			output << maxValue << endl;
		}
		else {
			output << -1 << endl;
		}
	}
	output.close();
}

Node *Index::searchLeaf(int x){
	if(root==NULL){
		return NULL;
	}
	else{
		Node *cursor = root;
		while(cursor->IS_LEAF == false){
			for(int i = 0;i < cursor->size;i++){
				if (x < cursor->key[i]){
					cursor = cursor->ptr[i];
					break;
				}
				if (i == cursor->size-1){
					cursor = cursor->ptr[i+1];
					break;
				}
			}
		}
		return cursor;
	}
}

int Index::search(int x) {
	// If tree is empty
	if (root == NULL) {
		return -1;
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
		stack<Node*> p;

		// 往下搜索到leaf層
		while (cursor->IS_LEAF == false) {
            //用來紀錄leaf層的上一層
			p.push(cursor);

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
		if (cursor->size < ORDER) {
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
			int virtualKeyArray[ORDER + 1];
			int virtualValueArray[ORDER + 1];
			

			// 複製原陣列到運算用陣列
			for (int i = 0; i < ORDER; i++) {
				virtualKeyArray[i] = cursor->key[i];
				virtualValueArray[i] = cursor->value[i];
			}

			int i = 0, j;

			// Traverse to find where the new
			// node is to be inserted
			while (key > virtualKeyArray[i] && i < ORDER) {
				i++;
			}

			// 運算用陣列進行插入
			for (j = ORDER;j > i; j--) {
                virtualKeyArray[j] = virtualKeyArray[j - 1];
				virtualValueArray[j] = virtualValueArray[j - 1];
			}

			virtualKeyArray[i] = key;
			virtualValueArray[i] = value;
			newLeaf->IS_LEAF = true;

            //切割陣列
			cursor->size = (ORDER + 1) / 2;
			newLeaf->size = ORDER + 1 - (ORDER + 1) / 2;

			newLeaf->ptr[newLeaf->size] = cursor->ptr[ORDER];
			cursor->ptr[cursor->size] = newLeaf;
			cursor->ptr[ORDER] = NULL;

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
				newRoot->ptr[0] = cursor;
				newRoot->ptr[1] = newLeaf;
				newRoot->IS_LEAF = false;
				newRoot->size = 1;
				root = newRoot;
			}
			else {

				// 如果現在這裡不是root => 上面有parent
				insertInternal(newLeaf->key[0], p, newLeaf);
			}
		}
	}
}

void Index::insertInternal(int key, stack<Node*> p, Node* child){
	Node* cursor = p.top();
	p.pop();
	// parent 還沒放滿
	if (cursor->size < ORDER) {
		int i = 0;

		//找到插入點
		while (key > cursor->key[i] && i < cursor->size) 
			i++;

		// 調整插入後的key跟value
		for (int j = cursor->size;j > i; j--) {
			cursor->key[j] = cursor->key[j - 1];
		}
		

		// 調整插入後的指標
		for (int j = cursor->size + 1;j > i + 1; j--) 
			cursor->ptr[j] = cursor->ptr[j - 1];
		
		//最後將值插入
		cursor->key[i] = key;
		cursor->size++;
		cursor->ptr[i + 1] = child;
	}

	// parent已經放滿了
	else {

		// 建立一個更上面新parent
		Node* newInternal = new Node;
		int virtualKeyArray[ORDER + 1];
		Node* virtualPtr[ORDER + 2];

		// 將key跟value放入運算用的array
		for (int i = 0; i < ORDER; i++) {
			virtualKeyArray[i] = cursor->key[i];
		}
		
		for (int i = 0; i < ORDER + 1; i++) 
			virtualPtr[i] = cursor->ptr[i];
		
		int i = 0, j;

		// 找到合適的插入點
		while (key > virtualKeyArray[i] && i < ORDER) 
			i++;
		

		//調整插入後的key
		for (int j = ORDER;j > i; j--) {
			virtualKeyArray[j] = virtualKeyArray[j - 1];
		}
		virtualKeyArray[i] = key;


		// 調整插入後的pointer
		for (int j = ORDER + 1;j > i + 1; j--) 
			virtualPtr[j] = virtualPtr[j - 1];
		virtualPtr[i + 1] = child;
		newInternal->IS_LEAF = false;

		//切割
		cursor->size = (ORDER + 1) / 2;

		newInternal->size = ORDER - (ORDER + 1) / 2;

		// 運算完的放入新的node
		for(i = 0,j = 0;i < cursor->size;i++, j++){
			cursor->key[i]=virtualKeyArray[j];
		}
		for(i = 0;i < ORDER + 1;i++){
			cursor->ptr[i]=NULL;
		}
		for(i = 0,j = 0;i < cursor->size+1;i++, j++){
			cursor->ptr[i] = virtualPtr[j];
		}
		for (i=0, j=cursor->size + 1;i < newInternal->size;i++, j++){
			newInternal->key[i] = virtualKeyArray[j];
		}
		for(i = 0,j = cursor->size+1;i < newInternal->size+1;i++, j++){
			newInternal->ptr[i] = virtualPtr[j];
		}
		

		// 如果已經推到頂是root了
		if (cursor == root) {

			// Create a new root node
			Node* newRoot = new Node;

			// Update key value
			newRoot->key[0] = virtualKeyArray[cursor->size];

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
				virtualKeyArray[cursor->size],
				p,
				newInternal);
		
		}
	}
}


void Index::clear_index() {
	deleteBp(root);
}

void Index::deleteBp(Node* cursor) {
	if(cursor->IS_LEAF){
		delete(cursor);
		return;
	}

	for(int i = 0;i < cursor->size+1;i++){
		deleteBp(cursor->ptr[i]);
	}

	delete(cursor);
}

void Index::display(Node *cursor) {
  if (cursor != NULL) {
    for (int i = 0; i < cursor->size; i++) {
      cout << cursor->key[i] << " ";
    }
    cout << "\n";
    if (cursor->IS_LEAF != true) {
      for (int i = 0; i < cursor->size + 1; i++) {
        display(cursor->ptr[i]);
      }
    }
  }
}

// Function to get the root Node
Node* Index::getRoot() {
	return root;
}

// Driver Code
/*int main() {	
	vector<int> key = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	vector<int> value = {10, 20, 30, 40, 50, 40, 30, 20, 10, 1};
	Index node(10, key, value);
	node.display(node.getRoot());

	// Create B+ Tree

	// Function Call to search node
	// with value 16
	vector<int> key_list = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	node.key_query(key_list);

	pair<int, int> a = {1, 5};
	vector<pair<int, int>> range_list;

	range_list.push_back(a);
	node.range_query(range_list);

	return 0;
}*/
