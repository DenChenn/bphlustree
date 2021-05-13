#ifndef INDEX_H_
#define INDEX_H_

#include <vector>
#include <utility>
#include <stack>
using namespace std;

const int MAX=99;

class Node{
private:
  bool IS_LEAF;
  int key[MAX],value[MAX],size;
  Node *ptr[MAX+1];
  friend class Index;
public:
  Node();
};

class Index{
private:
  Node *root;
  void insertInternal(int,stack<Node*>,Node*);
public:
  Index(int,vector<int>,vector<int>);
  void key_query(vector<int>);
  void range_query(vector<pair<int,int> >);
  void clear_index();

  void insert(int,int);
  int search(int);
  Node *searchR(int);
  void DFS(Node*);
};

#endif
