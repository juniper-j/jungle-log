#ifndef _RBTREE_H_  // 헤더 파일 중복 포함 방지 (Include Guard)
#define _RBTREE_H_

#include <stddef.h> // size_t 등의 타입 정의 포함

/* 노드의 색깔을 나타내는 열거형 타입 정의 Red 또는 Black */
typedef enum { RBTREE_RED, RBTREE_BLACK } color_t;

/* 키의 타입 정의 (기본적으로 int 사용) */
typedef int key_t;

/* 노드 구조체 정의
노드의 색(R/B), 노드의 키값, 포인터 3종 - parent node, left child node, right child node */
typedef struct node_t {
  color_t color;
  key_t key;
  struct node_t *parent, *left, *right;
} node_t;

/* Red-Black Tree 구조체 정의
root node, sentinel node */
typedef struct {
  node_t *root;
  node_t *nil;  // 모든 리프를 나타내는 sentinel node (nil 포인터)
} rbtree;

rbtree *new_rbtree(void); // 새로운 RBTree 생성 (동적 할당 및 nil 초기화)
void delete_rbtree(rbtree *); // RBTree 메모리 해제 (모든 노드 및 트리 자체 제거)
node_t *rbtree_insert(rbtree *, const key_t); // 주어진 키를 트리에 삽입하고, 삽입된 노드 포인터 반환
node_t *rbtree_find(const rbtree *, const key_t); // 주어진 키를 트리에서 검색하여 노드 포인터 반환 (없으면 NULL 반환)
node_t *rbtree_min(const rbtree *); // 트리에서 가장 작은 키 값을 가지는 노드 반환
node_t *rbtree_max(const rbtree *); // 트리에서 가장 큰 키 값을 가지는 노드 반환
int rbtree_erase(rbtree *, node_t *); // 주어진 노드를 트리에서 삭제 (성공 시 0, 실패 시 -1 반환)

/* 중위 순회를 통해 트리 노드를 오름차순으로 배열에 저장
배열 크기 제한은 size_t 크기만큼 */
int rbtree_to_array(const rbtree *, key_t *, const size_t);

#endif  // _RBTREE_H_



/* 
Q1. 
A1. 

Q2. _t 는 왜 붙일까?
A2. "type"의 약자로 int, size_t, time_t, pid_t 같은 이름에 사용자 정의 타입임을 명시하기 위해 붙인다.abort

Q3. 
A3. 

*/