#include <iostream>
#include <vector>
#include <utility>
#include <climits>
#include <queue>
#include <stdio.h>
#include "index.h"
using namespace std;

Node::Node(){
  for(int i=0;i<MAX+1;i++){
    ptr[i]=NULL;
  }
}
Index::Index(int n,vector<int> key,vector<int> value){
  root=NULL;
  for(int i=0;i<n;i++){
    insert(key[i],value[i]);
  }
}
void Index::key_query(vector<int> k){
  FILE *fp;
  fp=fopen("key_query_out.txt","w");
  for(int i=0;i<k.size();i++){
    fprintf(fp,"%d\n",search(k[i]));
  }
  fclose(fp);
}
void Index::range_query(vector<pair<int,int> > r){
  FILE *fp;
  int max;
  Node *cursor;
  fp=fopen("range_query_out.txt","w");
  for(int i=0;i<r.size();i++){
    cursor=searchR(r[i].first);
    max=INT_MIN;
    while(true){
      for(int j=0;j<cursor->size;j++){
        if(cursor->key[j]>=r[i].first&&cursor->key[j]<=r[i].second&&cursor->value[j]>max){
          max=cursor->value[j];
        }
      }
      if(r[i].second<=cursor->key[cursor->size-1]){
        break;
      }
      else{
        cursor=cursor->ptr[cursor->size];
        if(cursor==NULL){
          break;
        }
      }
    }
    if(max!=INT_MIN){
      fprintf(fp,"%d\n",max);
    }
    else{
      fprintf(fp,"-1\n");
    }
  }
  fclose(fp);
}
void Index::clear_index(){
  DFS(root);
  delete root;
}
void Index::insert(int x,int v){
  if(root==NULL){
    root = new Node;
    root->key[0]=x;
    root->value[0]=v;
    root->IS_LEAF=true;
    root->size=1;
  }
  else{
    Node *cursor=root;
    stack<Node*> p;
    while(cursor->IS_LEAF==false){
      p.push(cursor);
      for(int i=0;i<cursor->size;i++){
        if(x<cursor->key[i]){
          cursor=cursor->ptr[i];
          break;
        }
        if(i==cursor->size-1){
          cursor=cursor->ptr[i+1];
          break;
        }
      }
    }
    if(cursor->size<MAX){
      int i=0;
      while(x>cursor->key[i]&&i<cursor->size){
        i++;
      }
      for(int j=cursor->size;j>i;j--){
        cursor->key[j]=cursor->key[j-1];
        cursor->value[j]=cursor->value[j-1];
      }
      cursor->key[i]=x;
      cursor->value[i]=v;
      cursor->size++;
      cursor->ptr[cursor->size]=cursor->ptr[cursor->size-1];
      cursor->ptr[cursor->size-1]=NULL;
    } else {
      Node *newLeaf=new Node;
      int virtualNode[MAX+1];
      int virtualValue[MAX+1];
      for (int i=0;i<MAX;i++){
        virtualNode[i]=cursor->key[i];
        virtualValue[i]=cursor->value[i];
      }
      int i=0,j;
      while(x>virtualNode[i]&&i<MAX){
        i++;
      }
      for(int j=MAX;j>i;j--){
        virtualNode[j]=virtualNode[j-1];
        virtualValue[j]=virtualValue[j-1];
      }
      virtualNode[i]=x;
      virtualValue[i]=v;
      newLeaf->IS_LEAF=true;
      cursor->size=(MAX+1)/2;
      newLeaf->size=MAX+1-(MAX+1)/2;
      newLeaf->ptr[newLeaf->size]=cursor->ptr[MAX];
      cursor->ptr[cursor->size]=newLeaf;
      cursor->ptr[MAX]=NULL;
      for(i=0;i<cursor->size;i++){
        cursor->key[i]=virtualNode[i];
        cursor->value[i]=virtualValue[i];
      }
      for (i=0,j=cursor->size;i<newLeaf->size;i++,j++) {
        newLeaf->key[i]=virtualNode[j];
        newLeaf->value[i]=virtualValue[j];
      }
      if(cursor==root){
        Node *newRoot=new Node;
        newRoot->key[0]=newLeaf->key[0];
        newRoot->ptr[0]=cursor;
        newRoot->ptr[1]=newLeaf;
        newRoot->IS_LEAF=false;
        newRoot->size=1;
        root=newRoot;
      } else{
        insertInternal(newLeaf->key[0],p,newLeaf);
      }
    }
  }
}
void Index::insertInternal(int x,stack<Node*> p,Node *child) {
  Node *cursor=p.top();
  p.pop();
  if (cursor->size< MAX){
    int i=0;
    while(x>cursor->key[i]&&i<cursor->size){
      i++;
    }
    for(int j=cursor->size;j>i;j--){
      cursor->key[j]=cursor->key[j-1];
    }
    for(int j=cursor->size+1;j>i+1;j--){
      cursor->ptr[j]=cursor->ptr[j-1];
    }
    cursor->key[i]=x;
    cursor->size++;
    cursor->ptr[i+1]=child;
  }
  else {
    Node *newInternal=new Node;
    int virtualKey[MAX+1];
    Node *virtualPtr[MAX+2];
    for(int i=0;i<MAX;i++){
      virtualKey[i]=cursor->key[i];
    }
    for (int i=0;i<MAX+1;i++){
      virtualPtr[i]=cursor->ptr[i];
    }
    int i=0,j;
    while(x>virtualKey[i]&&i<MAX){
      i++;
    }
    for(int j=MAX;j>i;j--){
      virtualKey[j]=virtualKey[j-1];
    }
    virtualKey[i]=x;
    for(int j=MAX+1;j>i+1;j--){
      virtualPtr[j]=virtualPtr[j-1];
    }
    virtualPtr[i+1]=child;
    newInternal->IS_LEAF=false;
    cursor->size=(MAX+1)/2;
    newInternal->size=MAX-(MAX+1)/2;
    for(i=0,j=0;i<cursor->size;i++,j++){
      cursor->key[i]=virtualKey[j];
    }
    for(i=0;i<MAX+1;i++){
      cursor->ptr[i]=NULL;
    }
    for(i=0,j=0;i<cursor->size+1;i++,j++){
      cursor->ptr[i]=virtualPtr[j];
    }
    for (i=0,j=cursor->size+1;i<newInternal->size;i++,j++){
      newInternal->key[i]=virtualKey[j];
    }
    for(i=0,j=cursor->size+1;i<newInternal->size+1;i++,j++){
      newInternal->ptr[i]=virtualPtr[j];
    }
    if(cursor==root){
      Node *newRoot=new Node;
      newRoot->key[0]=virtualKey[cursor->size];
      newRoot->ptr[0]=cursor;
      newRoot->ptr[1]=newInternal;
      newRoot->IS_LEAF=false;
      newRoot->size=1;
      root=newRoot;
    }
    else{
      insertInternal(virtualKey[cursor->size],p,newInternal);
    }
  }
}
int Index::search(int x){
  if(root==NULL){
    return -1;
  }
  else{
    Node *cursor=root;
    while(cursor->IS_LEAF==false){
      for(int i=0;i<cursor->size;i++){
        if(x<cursor->key[i]){
          cursor=cursor->ptr[i];
          break;
        }
        if(i==cursor->size-1){
          cursor=cursor->ptr[i+1];
          break;
        }
      }
    }
    for (int i=0;i<cursor->size;i++){
      if(cursor->key[i]==x){
        return cursor->value[i];
      }
    }
    return -1;
  }
}
Node *Index::searchR(int x){
  if(root==NULL){
    return NULL;
  }
  else{
    Node *cursor=root;
    while(cursor->IS_LEAF==false){
      for(int i=0;i<cursor->size;i++){
        if (x<cursor->key[i]){
          cursor=cursor->ptr[i];
          break;
        }
        if (i==cursor->size-1){
          cursor=cursor->ptr[i+1];
          break;
        }
      }
    }
    return cursor;
  }
}
void Index::DFS(Node *cursor){
  if(cursor->IS_LEAF){
    for(int i=0;i<MAX+1;i++){
      cursor->ptr[i]=NULL;
      delete cursor->ptr[i];
    }
    return;
  }
  for(int i=0;i<MAX+1;i++){
    if(cursor->ptr[i]==NULL){
      delete cursor->ptr[i];
      continue;
    }
    DFS(cursor->ptr[i]);
    delete cursor->ptr[i];
  }
}
