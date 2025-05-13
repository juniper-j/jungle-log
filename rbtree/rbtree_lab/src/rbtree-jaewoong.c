#include "rbtree.h"
#include <stdlib.h>
#include <stdio.h>

// RB트리 생성 //
////////////////////////////////////////////////////////////////////////////////////////////////
rbtree *new_rbtree(void) {  
  rbtree *t = calloc(1, sizeof(rbtree)); // 트리에 메모리 할당
  t->nil = calloc(1, sizeof(node_t)); // 꼬리에 메모리 할당
  t->nil->color = RBTREE_BLACK; // 꼬리 컬러 지정  
  t->root = t->nil; // 트리의 빈 루트에 꼬리 지정
  return t;
}


// 트리의 모든 노드 삭제 (후위순회) //
////////////////////////////////////////////////////////////////////////////////////////////////
void traverseNodeFree(const rbtree *t, node_t *n)
{
  if(n == t->nil) return;      

  traverseNodeFree(t, n->left);
  traverseNodeFree(t, n->right);

  free(n);
  n = NULL;

  return;
}


// RB트리 삭제 //
////////////////////////////////////////////////////////////////////////////////////////////////
void delete_rbtree(rbtree *t) 
{    
  traverseNodeFree(t, t->root);
  free(t->nil);  
  free(t);  
  t = NULL;
}


// 왼쪽 회전 //
////////////////////////////////////////////////////////////////////////////////////////////////
void rotate_left(rbtree *t, node_t *x) 
{  
  node_t *y = x->right; // y: x의 오른쪽 자식 → 회전 후 x의 부모가 될 노드

  x->right = y->left; // y의 왼쪽 자식을 x의 오른쪽 자식으로 설정 (중간 서브트리 이동)

  if (y->left != t->nil)
    y->left->parent = x; // y의 왼쪽 자식이 nil이 아니면 그 부모를 x로 갱신

  y->parent = x->parent; // y가 x의 위치를 차지하므로 y의 부모를 x의 부모로 설정

  if (x->parent == t->nil)
    t->root = y; // x가 루트였으면 이제 y가 루트가 됨

  else if (x == x->parent->left)
    x->parent->left = y; // x가 부모의 왼쪽 자식이었다면 y를 왼쪽으로 연결

  else
    x->parent->right = y;  // x가 부모의 오른쪽 자식이었다면 y를 오른쪽으로 연결

  y->left = x; // x는 이제 y의 왼쪽 자식이 됨
  x->parent = y; // x의 부모는 이제 y가 됨
}

// 오른쪽 회전 //
////////////////////////////////////////////////////////////////////////////////////////////////
void rotate_right(rbtree *t, node_t *x) 
{  
  node_t *y = x->left; // y: x의 왼쪽 자식

  x->left = y->right; // x의 왼쪽 자식을 y의 오른쪽 자식으로 대체 (서브트리 이동)

  if (y->right != t->nil)
    y->right->parent = x; // y의 오른쪽 자식이 nil이 아니면 그 부모를 x로 설정

  y->parent = x->parent; // y가 위로 올라가므로 y의 부모는 x의 부모가 됨

  if (x->parent == t->nil)
    t->root = y; // x가 루트였으면, 이제 y가 루트

  else if (x == x->parent->right)
    x->parent->right = y; // x가 오른쪽 자식이었으면, y를 오른쪽 자식으로 연결

  else
    x->parent->left = y; // x가 왼쪽 자식이었으면, y를 왼쪽 자식으로 연결

  y->right = x; // y의 오른쪽 자식으로 x를 연결
  x->parent = y; // x의 부모를 y로 갱신
}

// 노드 삽입 //
////////////////////////////////////////////////////////////////////////////////////////////////
node_t *node_insert(rbtree *t, node_t *root, const key_t key) {
  node_t *cur = root;
  node_t *parent = t->nil;

  // 현재 주목 포인터가 꼬리를 만나기 전까지 반복 수행
  while (cur != t->nil) 
  {
    parent = cur;

    // 입력받은 키의 크기에 따라 이진트리를 따라 적절할 지점까지 이동
    if (key < cur->key)
      cur = cur->left;
    else
      cur = cur->right;
  }

  node_t *newNode = calloc(1, sizeof(node_t)); // 새로운 노드에 메모리 할당

  // 새 노드 초기화
  newNode->key = key;
  newNode->color = RBTREE_RED;
  newNode->left = newNode->right = t->nil;
  newNode->parent = parent;

  // 빈 트리였다면 루트에 삽입
  if (parent == t->nil)
    t->root = newNode;

  // 노드가 있었다면 최종 이동한 지점에 이동
  else if (key < parent->key)
    parent->left = newNode;

  else
    parent->right = newNode;

  return newNode;
}

// 삽입 후 조정 //
////////////////////////////////////////////////////////////////////////////////////////////////
rbtree *modiTree(rbtree *t, node_t *z) {

  // 부모가 RED일 때만 수행
  while (z->parent->color == RBTREE_RED) 
  {
    node_t *gp = z->parent->parent; // 조부모 선언

    // 부모가 왼쪽 자식일 때,
    if (z->parent == gp->left) 
    {
      // 삼촌의 컬러가 RED이면 재채색
      node_t *uncle = gp->right;       
      if (uncle->color == RBTREE_RED) 
      {
        z->parent->color = RBTREE_BLACK;
        uncle->color = RBTREE_BLACK;
        gp->color = RBTREE_RED;
        z = gp; 
      } 

      // 삼촌의 컬러가 BLACK이면 회전
      else 
      {
        // 방향이 꺽여있다면 좌회전 먼저 수행
        if (z == z->parent->right) {
          z = z->parent;
          rotate_left(t, z);
        }

        // 부모와 자식의 연결선을 곧게 편 후 우회전
        z->parent->color = RBTREE_BLACK;
        gp->color = RBTREE_RED;
        rotate_right(t, gp);
      }
    }

    // 부모가 오른쪽 자식일 때,
    else {

      // 삼촌의 컬러가 RED이면 재채색
      node_t *uncle = gp->left;
      if (uncle->color == RBTREE_RED) {
        z->parent->color = RBTREE_BLACK;
        uncle->color = RBTREE_BLACK;
        gp->color = RBTREE_RED;
        z = gp;        
      }

      // 삼촌의 컬러가 BLACK이면 회전
      else 
      {
        // 방향이 꺽여있다면 우회전 먼저 수행
        if (z == z->parent->left) {
          z = z->parent;
          rotate_right(t, z);
        }

        // 부모와 자식의 연결선을 곧게 편 후 좌회전
        z->parent->color = RBTREE_BLACK;
        gp->color = RBTREE_RED;
        rotate_left(t, gp);
      }
    }
  }

  t->root->color = RBTREE_BLACK; // 종료 조건: 루트 컬러 재채색

  return t;
}


// 삽입 메인 //
////////////////////////////////////////////////////////////////////////////////////////////////
node_t *rbtree_insert(rbtree *t, const key_t key) 
{  
  node_t *inserted = node_insert(t, t->root, key); // 노드 삽입
  modiTree(t, inserted); // 노드 정렬
  return inserted; 
}


// 노드 출력 (중위 순회)//
////////////////////////////////////////////////////////////////////////////////////////////////
void printNode(const rbtree *t, const node_t *n, key_t *arr, int *idx)
{
  // 노드가 t.nil 이면 반환
  if(n == t->nil) 
    return;          

  // 재귀함수를 호출하여 왼쪽 최하단으로 이동 후 다시 올라오는 흐름으로 순회
  printNode(t, n->left, arr, idx);

  // arr에 오름차순대로 주목 노드의 키를 삽입하고 idx +1
  arr[*idx] = n->key;
  (*idx)++;

  // 왼쪽 자식 노드와 부모 노드를 순회한 다음에 오른쪽 자식 노드를 순회
  printNode(t, n->right, arr, idx);

  return;
}

// 노드 탐색 //
////////////////////////////////////////////////////////////////////////////////////////////////
node_t *rbtree_find(const rbtree *t, const key_t key) 
{
  // 이진 탐색 트리처럼 주목 노드의 키값과 비교하며 입력 받은 키값과 같은 노드를 발견할 때까지 반복
  node_t *cur = t->root;

  while(cur != t->nil)
  {    
    if(key == cur->key)
    {   
      return cur;    
    }

    if(key < cur->key)
      cur = cur->left;
    else
      cur = cur->right;    
  }

  return NULL;  
}


// 최소 노드 //
////////////////////////////////////////////////////////////////////////////////////////////////
node_t *rbtree_min(const rbtree *t) 
{
  // 왼쪽 최하단의 노드값을 반환
  node_t *cur = t->root;
  while(cur->left != t->nil)
    cur = cur->left;
  return cur;
}


// 최대 노드 //
////////////////////////////////////////////////////////////////////////////////////////////////
node_t *rbtree_max(const rbtree *t) 
{
  // 오른쪽 최하단의 노드값을 반환
  node_t *cur = t->root;
  while(cur->right != t->nil)
    cur = cur->right;
  return cur;
}


// 트리의 노드를 배열에 담기 //
////////////////////////////////////////////////////////////////////////////////////////////////
int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) 
{
  // printNode 함수에서 사용할 idx를 선언/초기화하여 전달
  int idx = 0;

  printNode(t, t->root, arr, &idx); 

  return 0;
}


// 노드 삭제 //
////////////////////////////////////////////////////////////////////////////////////////////////
int rbtree_erase(rbtree *t, node_t *p) {
  // TODO: implement erase
  return 0;
}