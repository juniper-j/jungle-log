#include "rbtree.h"
#include <stdlib.h>


//////////////////////////////////////////////////////////////////////////
//
// rbtree *new_rbtree(void)
//
// 레드-블랙 트리 구조체를 생성하고 초기화하는 함수
// - 트리의 root는 비어 있고 nil 노드만 존재함
// - sentinel 노드(nil)를 생성하여 모든 리프와 초기 포인터로 사용
// rbtree 구조체의 포인터를 반환하는 함수이므로 반환값은 '주소' 또는 'NULL'
//
// 구현 아이디어:
// 1. 트리 구조체를 calloc으로 할당하여 초기화
// 2. sentinel 역할을 하는 nil 노드를 별도로 동적 할당
// 3. nil 노드는 BLACK이며, 모든 포인터는 자기 자신을 가리킴
// 4. root는 아직 노드가 없으므로 nil을 가리키도록 설정
//
//////////////////////////////////////////////////////////////////////////
rbtree *new_rbtree(void) 
{ 
  rbtree *p = (rbtree *)calloc(1, sizeof(rbtree));  // 트리 구조체 메모리 할당
  
  node_t *nil = (node_t *)calloc(1, sizeof(node_t));  // sentinel nil node 메모리 할당
  nil->color = RBTREE_BLACK;
  nil->left = nil;
  nil->right = nil;
  nil->parent = nil;

  p->nil = nil;
  p->root = nil;

  return p;
}


//////////////////////////////////////////////////////////////////////////
//
// static void delete_node(node_t *node, node_t *nil)
//
// 레드-블랙 트리의 노드를 후위 순회 방식으로 재귀적으로 제거하는 내부 헬퍼 함수
// - 왼쪽 자식 → 오른쪽 자식 → 현재 노드 순으로 모든 노드를 해제
// - 종료 조건은 해당 노드가 sentinel(nil) 노드인지 여부로 판단
// - 단일 노드 또는 비어 있는 트리도 처리 가능
//
//////////////////////////////////////////////////////////////////////////
void delete_node(node_t *node, node_t *nil)
{
  if (node == nil) return;
  delete_node(node->left, nil);
  delete_node(node->right, nil);
  free(node);
  node = NULL;
  return;
}


//////////////////////////////////////////////////////////////////////////
//
// void delete_rbtree(rbtree *t)
//
// 레드-블랙 트리의 모든 노드, sentinel 노드(nil), 트리 구조체 메모리를 해제하는 함수
// - 내부적으로 delete_node() 함수를 호출하여 후위 순회 방식으로 노드를 제거
// - 트리 구조체를 해제하기 전, 연결된 모든 노드들을 순차적으로 free 처리
// - 트리 구조체(t)를 먼저 free할 경우, 내부 노드들에 접근할 수 없어 메모리 누수 발생
// - nil 노드는 NULL이 아니므로 반드시 명시적으로 free 해야 함
//
// 구현 순서:
// 1. 루트 노드부터 delete_node() 호출 → 모든 노드 후위 순회 해제
// 2. sentinel(nil) 노드 해제
// 3. rbtree 구조체 메모리 해제
//
//////////////////////////////////////////////////////////////////////////
void delete_rbtree(rbtree *t) 
{
  if (t == NULL || t->nil == NULL) return;
  node_t *node = t->root;
  delete_node(node, t->nil);
  free(t->nil);
  free(t);
  t = NULL;
}


//////////////////////////////////////////////////////////////////////////
//
// void left_rotate(rbtree *t, node_t *x)
//
// x를 기준으로 좌회전(left rotation)을 수행하는 함수
// 1. x의 오른쪽 자식 y의 왼쪽 자식을 x의 오른쪽으로 옮기고
// 2. y가 x의 자리를 대신하여 위로 올라가고 
//    - (1) x가 root (2) left child (3) right child
// 3. x는 y의 왼쪽 자식이 됨
// 4. 트리의 연결 관계(parent, left, right)를 모두 업데이트
//
// 전제조건:
//   - x->right != t->nil 이어야 함 (오른쪽 자식이 존재해야 좌회전 가능)
//
//////////////////////////////////////////////////////////////////////////
void left_rotate(rbtree *t, node_t *x) 
{
  node_t *y = x->right;         // y는 x의 오른쪽 자식

  /* 1. x의 오른쪽 자식 y의 왼쪽 자식을 x의 오른쪽으로 옮기고 */
  x->right = y->left;           // y의 왼쪽 서브트리를 x의 오른쪽으로 옮기기 (부모 → 자식)
  if (y->left != t->nil) {      // y의 왼쪽 자식이 있다면, 그 부모를 x로 갱신 (자식 → 부모)
    y->left->parent = x;
  } 
  y->parent = x->parent;        // y의 부모를 x의 부모로 설정

  /* 2. y가 x의 자리를 대신하여 위로 올라가고 */
  if (x->parent == t->nil) {    // (1) x가 루트였으면, y가 새로운 루트
    t->root = y;
  } else if (x == x->parent->left) {  // (2) x가 왼쪽 자식이었다면 y를 왼쪽에 연결
    x->parent->left = y;
  } else {                      // (3) x가 오른쪽 자식이었다면 y를 오른쪽에 연결
    x->parent->right = y;
  }

  /* 3. x는 y의 왼쪽 자식이 됨 */
  y->left = x;    // x를 y의 왼쪽 자식으로 설정
  x->parent = y;  // x의 부모를 y로 설정
}


//////////////////////////////////////////////////////////////////////////
//
// void right_rotate(rbtree *t, node_t *x)
//
// x를 기준으로 우회전(right rotation)을 수행하는 함수
// 1. x의 왼쪽 자식 y의 오른쪽 자식을 x의 왼쪽으로 옮기고
// 2. y가 x의 자리를 대신하여 위로 올라가고 
//    - (1) x가 root (2) left child (3) right child
// 3. x는 y의 오른쪽 자식이 됨
// 4. 트리의 연결 관계(parent, left, right)를 모두 업데이트
//
// 전제조건:
//   - x->left != t->nil 이어야 함 (왼쪽 자식이 존재해야 우회전 가능)
//
//////////////////////////////////////////////////////////////////////////
void right_rotate(rbtree *t, node_t *x) 
{
  node_t *y = x->left;          // y는 x의 왼쪽 자식

  /* 1. x의 왼쪽 자식 y의 오른쪽 자식을 x의 왼쪽으로 옮기고 */
  x->left = y->right;           // y의 오른쪽 서브트리를 x의 왼쪽으로 옮기기 (부모 → 자식)
  if (y->right != t->nil) {     // y의 오른쪽 자식이 있다면, 그 부모를 x로 갱신 (자식 → 부모)
    y->right->parent = x;
  }
  y->parent = x->parent;        // y의 부모를 x의 부모로 설정

  /* 2. y가 x의 자리를 대신하여 위로 올라가고 */
  if (x->parent == t->nil) {    // (1) x가 루트였으면, y가 새로운 루트
    t->root = y;
  } else if (x == x->parent->left) {   // (2) x가 왼쪽 자식이었다면 y를 왼쪽에 연결
    x->parent->left = y;
  } else {                      // (3) x가 오른쪽 자식이었다면 y를 오른쪽에 연결
    x->parent->right = y;
  }

  /* 3. x는 y의 오른쪽 자식이 됨 */
  y->right = x;   // x를 y의 오른쪽 자식으로 설정
  x->parent = y;  // x의 부모를 y로 설정
}


//////////////////////////////////////////////////////////////////////////
//
// void rbtree_insert_fixup(rbtree *t, node_t *node)
//
// 레드-블랙 트리에 노드를 삽입한 후, 트리의 균형과 색상 속성을 복구하는 함수
// - 삽입된 노드는 항상 RED이므로, 부모가 RED인 경우 RB 트리의 성질이 위반됨
// - 부모, 삼촌, 조부모의 색과 방향 관계에 따라 3가지 Case로 나누어 처리
//   (1) 부모와 삼촌이 모두 RED → 색상 반전 (Case 1)
//   (2) 삼촌은 BLACK, z가 부모의 반대쪽 자식 → 회전 준비 (Case 2)
//   (3) 삼촌은 BLACK, z가 부모와 일직선 → 회전 및 색상 교환 (Case 3)
// - 삽입 이후에도 트리 전체의 불변성을 유지하기 위한 핵심 함수
//
// 반환: 없음 (트리 내부 구조 직접 수정)
//
// 구현 아이디어:
// 1. z의 부모가 RED인 동안 반복하며 위로 올라감
// 2. 부모가 조부모의 왼쪽 자식인지, 오른쪽 자식인지에 따라 대칭 처리
// 3. 각 Case에 따라 색상 변경 및 left/right 회전 수행
// 4. 최종적으로 루트 노드는 항상 BLACK이어야 하므로 보정
//
//////////////////////////////////////////////////////////////////////////
void rbtree_insert_fixup(rbtree *t, node_t *node) 
{  
  while (node->parent->color == RBTREE_RED) // 자식-부모 Double Red로 속성 위반
  {
    node_t *gpar = node->parent->parent;
    node_t *par = node->parent;
    node_t *unc;

    if (par == gpar->left) {  // node의 부모가 조부모의 왼쪽 자식인지 검사
      unc = gpar->right;      // node의 삼촌은 오른쪽 자식
      if (unc->color == RBTREE_RED) 
      { // CASE 1: 부모와 삼촌이 모두 Red → 색상 반전
        par->color = RBTREE_BLACK;
        unc->color = RBTREE_BLACK;
        gpar->color = RBTREE_RED;
        node = gpar;
      }
      else { // 부모는 Red, 삼촌은 Black 
        if (node == par->right) 
        { // CASE 2: node가 "삼각형" 구조 → 왼쪽 회전 후 Case 3 구조로 전환
          node = par;                 // node를 부모로 이동 후 (이후 루프에서 par, gpar는 node 기준으로 다시 계산됨)
          left_rotate(t, node);       // 왼쪽 회전으로 CASE 3 구조로 변경
        }
        else 
        { // CASE 3: node가 "일자형" 구조 → 색상 교환 후 오른쪽 회전
          par->color = RBTREE_BLACK;  // 부모는 Red로
          gpar->color = RBTREE_RED;   // 조부모는 Black으로 변경 후
          right_rotate(t, gpar);      // 조부모 기준 오른쪽 회전
        }
      }
    }
    else {  // 대칭 처리: 부모가 조부모의 오른쪽 자식일 때
      unc = gpar->left;
      if (unc->color == RBTREE_RED) {
        par->color = RBTREE_BLACK;
        unc->color = RBTREE_BLACK;
        gpar->color = RBTREE_RED;
        node = gpar;
      }
      else {
        if (node == par->left) {
          node = par;
          right_rotate(t, node);
        }
        else {
          par->color = RBTREE_BLACK;
          gpar->color = RBTREE_RED;
          left_rotate(t, gpar);
        }
      }
    }
  }

  t->root->color = RBTREE_BLACK;  // 루트는 항상 Black 이어야 함
}


//////////////////////////////////////////////////////////////////////////
//
// node_t *rbtree_insert(rbtree *t, const key_t key)
//
// 주어진 키 값을 레드-블랙 트리에 삽입하는 함수
// - 트리는 이진 탐색 트리의 특성을 유지하며 노드를 삽입하고,
// - 삽입 후에는 색상과 회전을 통해 레드-블랙 트리 속성을 복구함
//
// 반환:
// - 삽입된 노드의 포인터 (성공 시)
// - NULL (할당 실패 또는 유효하지 않은 트리)
//
// 구현 아이디어:
// 1. 새 노드를 RED로 생성하고, nil 노드를 초기 자식으로 설정
// 2. 이진 탐색 트리 방식으로 삽입 위치 탐색
// 3. 부모 포인터와 부모의 자식 포인터를 통해 위치에 노드 연결
// 4. 삽입된 노드로 인해 위반된 레드-블랙 트리 속성은 fixup 함수로 복구
//
//////////////////////////////////////////////////////////////////////////
node_t *rbtree_insert(rbtree *t, const key_t key) 
{
  if (t == NULL || t->nil == NULL) return NULL; // 트리 포인터, nil 노드 유효성 검사

  /* Step 1. 새 노드 생성 및 기본 설정 */
  node_t *new_node = (node_t *)calloc(1, sizeof(node_t));
  if (new_node == NULL) return NULL;  // 할당 실패 시 NULL 반환
  new_node->color = RBTREE_RED;
  new_node->key = key;
  new_node->parent = t->nil;  
  new_node->left = t->nil;
  new_node->right = t->nil;

  /* Step 2. 새 노드 삽입 위치 찾기 */
  node_t *par = t->nil;   // 부모 후보
  node_t *cur = t->root;  // 탐색은 루트부터 시작

  while (cur != t->nil) // 현재 노드가 nil이 될 때까지 탐색
  {                         
    par = cur;          // 현재 위치를 부모 후보로 저장 (삽입 지점에 도달하기 직전의 노드를 저장)
    if (key < cur->key) { // (1) 삽입할 키값이 현재보다 작으면 왼쪽으로 이동
      cur = cur->left;
    }
    else {                // (2) 삽입할 키값이 현재와 같거나 크면 오른쪽으로 이동
      cur = cur->right;
    }
  }

  /* Step 3. 새 노드 부모 설정 (자식 → 부모) */
  new_node->parent = par;

  /* Step 3: 새 노드 부모 설정 (부모 → 자식) */
  if (par == t->nil) {        // (1) 부모가 NIL이면 트리가 비어있는 것 → 루트 설정
    t->root = new_node;
    new_node->color = RBTREE_BLACK;
  } 
  else if (key < par->key) {  // (2) 부모의 왼쪽 자식인지 확인
    par->left = new_node;
  } 
  else {                      // (3) 아니면 오른쪽 자식
    par->right = new_node;
  }

  /* Step 4: RB-트리 속성 복구 */
  rbtree_insert_fixup(t, new_node);
  return new_node;
}


//////////////////////////////////////////////////////////////////////////
//
// node_t *rbtree_find(const rbtree *t, const key_t key)
//
// 레드-블랙 트리에서 key를 갖는 노드를 탐색하는 함수
// - key와 일치하는 노드가 있으면 해당 노드 포인터 반환
// - 없으면 NULL 반환함
//
// 일반적 호출 예시:
//   node_t *p = rbtree_find(t, 12);
//   if (p != t->nil) { printf("%d found!\n", p->key); }
//
//////////////////////////////////////////////////////////////////////////
node_t *rbtree_find(const rbtree *t, const key_t key) 
{
  if (t == NULL || t->root == NULL || t->nil == NULL) return NULL;
  node_t *cur = t->root;  // 탐색은 루트부터 시작

  while (cur != t->nil) { // 현재 노드가 nil이 될 때까지 반복
    if (key == cur->key) return cur;  // 현재 노드의 키와 찾는 키가 같으면 해당 노드를 반환  
    else if (key < cur->key) cur = cur->left;  // 찾는 키가 현재 노드보다 크면 오른쪽 서브트리로 이동
    else cur = cur->right;  // 찾는 키가 더 작으면 왼쪽 서브트리로 이동
  }
  return NULL;
}


//////////////////////////////////////////////////////////////////////////
//
// node_t *rbtree_min(const rbtree *t)
//
// 레드-블랙 트리에서 가장 작은 값을 가진 노드를 반환하는 함수
// - 트리의 왼쪽 자식을 따라 내려가면 항상 값이 작아지므로, 가장 왼쪽 리프 노드가 최소값을 갖는다.
// - sentinel(nil) 노드가 리프 역할을 하므로, left 포인터가 nil일 때까지 탐색을 반복한다.
// - 트리가 비어 있는 경우(t->root == t->nil)는 그대로 nil을 반환한다.
//
// 일반적 사용 예시:
//   node_t *min = rbtree_min(t);
//   if (min != t->nil) printf("최소값은 %d입니다\n", min->key);
//
//////////////////////////////////////////////////////////////////////////
node_t *rbtree_min(const rbtree *t) 
{
  node_t *cur = t->root;  // 루트부터 탐색 시작
  if (cur == t->nil) return t->nil; // 트리가 비어 있으면 nil 반환

  while (cur->left != t->nil) { // 가장 왼쪽 자식이 nil일 때까지 왼쪽으로 이동
    cur = cur->left;
  }

  return cur; // 최소값 노드 반환
}


//////////////////////////////////////////////////////////////////////////
//
// node_t *rbtree_max(const rbtree *t)
//
// 레드-블랙 트리에서 가장 큰 값을 가진 노드를 반환하는 함수
// - 트리의 오른쪽 자식을 따라 내려가면 항상 값이 커지므로, 가장 왼쪽 리프 노드가 최댓값을 갖는다.
// - sentinel(nil) 노드가 리프 역할을 하므로, right 포인터가 nil일 때까지 탐색을 반복한다.
// - 트리가 비어 있는 경우(t->root == t->nil)는 그대로 nil을 반환한다.
//
// 일반적 사용 예시:
//   node_t *max = rbtree_max(t);
//   if (max != t->nil) printf("최댓값은 %d입니다\n", max->key);
//
//////////////////////////////////////////////////////////////////////////
node_t *rbtree_max(const rbtree *t) 
{
  node_t *cur = t->root;  // 루트부터 탐색 시작
  if (cur == t->nil) return t->nil; // 트리가 비어 있으면 nil 반환

  while (cur->right != t->nil) {  // 가장 오른쪽 자식이 nil일 때까지 오른쪽으로 이동
    cur = cur->right;
  }

  return cur; // 최댓값 노드 반환
}


//////////////////////////////////////////////////////////////////////////
//
// void rbtree_transplant(rbtree *t, node_t *u, node_t *v)
//
// 트리에서 노드 u를 노드 v로 "이식"하는 함수 (서브트리 교체)
// - u가 있는 위치에 v를 삽입함으로써, u를 제거하거나 대체하는 데 사용됨
// - 이 과정은 u의 부모 노드가 v를 대신 가리키도록 하며,
// - v의 부모 포인터도 u의 부모로 설정하여 연결을 유지함
//
//////////////////////////////////////////////////////////////////////////
void rbtree_transplant(rbtree *t, node_t *u, node_t *v)
{ 
  if (u->parent == t->nil) t->root = v; // 루트 노트가 바뀌는 케이스
  else if (u == u->parent->left) u->parent->left = v;
  else u->parent->right = v;
  v->parent = u->parent;  // v가 nil이어도 nil의 parent가 적절히 설정되도록 반영
}



void rbtree_erase_fixup(rbtree *t, node_t *p)
{
  while (p != t->root && p->color == RBTREE_BLACK)
  {
    node_t *par = p->parent;
    node_t *sib;
    if (p == par->left)
    {
      sib = par->right;
      if (sib->color == RBTREE_RED)
      {
        sib->color = RBTREE_BLACK;
        par->color = RBTREE_RED;
        left_rotate(t, par);
        sib = par->right;
      }

      else {
        if (sib->left->color == RBTREE_BLACK && sib->right->color == RBTREE_BLACK)
        {
          sib->color = RBTREE_RED;
          p = par;
        }
        else
        {
          if (sib->right->color == RBTREE_BLACK)
          {
            sib->left->color = RBTREE_BLACK;
            sib->color = RBTREE_RED;
            right_rotate(t, sib);
            sib = par->right;
          }
          sib->color = par->color;
          par->color = RBTREE_BLACK;
          sib->right->color = RBTREE_BLACK;
          left_rotate(t, par);
          p = t->root;
        }
      }
    }

    else
    {
      sib = par->left;

      if (sib->color == RBTREE_RED) {
        sib->color = RBTREE_BLACK;
        par->color = RBTREE_RED;
        right_rotate(t, par);
        sib = par->left;
      }

      if (sib->left->color == RBTREE_BLACK && sib->right->color == RBTREE_BLACK) {
        sib->color = RBTREE_RED;
        p = par;
      } 
      else 
      {
        if (sib->left->color == RBTREE_BLACK) 
        {
          sib->right->color = RBTREE_BLACK;
          sib->color = RBTREE_RED;
          left_rotate(t, sib);
          sib = par->left;
        }
        else
        {
        sib->color = par->color;
        par->color = RBTREE_BLACK;
        sib->left->color = RBTREE_BLACK;
        right_rotate(t, par);
        p = t->root;
        }
      }
    }
  }
  // 이중 블랙 상태 해제
  p->color = RBTREE_BLACK;
} 


//////////////////////////////////////////////////////////////////////////
//
// int rbtree_erase(rbtree *t, node_t *p)
//
// 레드-블랙 트리에서 주어진 노드 p를 삭제하는 함수
// - 레드-블랙 트리의 성질을 유지하며 노드를 제거함
// - 삭제되는 노드가 검정색일 경우, 불균형 복구를 위해 fixup 수행 필요
// - 성공적으로 삭제가 완료되면 0, 입력이 NULL인 경우(에러) -1 반환
//
// 구현 전략:
// 1. 삭제 대상 노드를 `cur`에 저장하고, 그 색을 따로 보관
// 2. 자식 수에 따라 다음 세 가지 경우로 분기 처리
//    (1) 왼쪽 자식 없음: 오른쪽 자식으로 대체
//    (2) 오른쪽 자식 없음: 왼쪽 자식으로 대체
//    (3) 양쪽 자식 존재: successor로 대체 (rbtree_min 사용)
//       → successor를 transplant한 후, 부모-자식 포인터 및 색상 조정
// 3. 삭제된 노드가 검정색일 경우, fixup을 호출해 트리 속성 복원
// 4. 삭제된 노드 메모리 해제
//
//////////////////////////////////////////////////////////////////////////
int rbtree_erase(rbtree *t, node_t *p) 
{ 
  if (t == NULL || p == NULL || p == t->nil) return -1;

  // printf("[erase] key: %d, color: %d\n", p->key, p->color);

  /* Step 1. 초기 설정 */
  node_t *cur = p;            // 실제 삭제 대상: 수도코드 y
  color_t delc;               // 실제 삭제 대상의 원래 색 → black이면 fixup 필요
  node_t *fix;                // 삭제 대상의 원래 위치를 대체하는 노드. fixup의 대상: 수도코드 x

  /* Step 2. 삭제할 노드의 자식 수에 따라 분기 */
  if (p->left == t->nil) 
  { // Case 1. 왼쪽 자식이 없음 → 오른쪽 자식으로 대체
    fix = p->right;
    delc = p->color;
    rbtree_transplant(t, p, p->right);
  } 
  else if (p->right == t->nil)
  { // Case 2: 오른쪽 자식 없음 → 왼쪽 자식으로 대체
    fix = p->left;
    delc = p->color;
    rbtree_transplant(t, p, p->left);
  }
  else
  { // Case 3: 양쪽 자식 존재 → successor(오른쪽 최소값)로 대체 후 부모-자식 포인터 및 색상 조정
    cur = p->right;
    while (cur->left != t->nil) cur = cur->left;  // 오른쪽 서브트리의 최소값 찾기 (successor)
    fix = cur->right;           // successor의 유일 자식이자 대체 노드
    delc = cur->color;          // successor의 색

    if (cur->parent == p)
    { // successor가 바로 p의 자식일 경우: transplant 없이 포인터만 조정
      fix->parent = cur;
    }
    else 
    { // 아닌 경우: successor를 먼저 자식으로서 제거 (cur 제거)
      rbtree_transplant(t, cur, fix);
      fix = p->right;
      fix->parent = cur;
    }
    // cur이 p의 자리에 올라감 (p를 cur로 교체)
    rbtree_transplant(t, p, cur);
    cur->left = p->left;
    cur->left->parent = cur;
    cur->color = p->color;

    cur->right = p->right;  // 추가
    cur->right->parent = cur;   // 추가
  }

  /* Step 3. 삭제된 노드가 검정색일 경우, fixup을 호출해 트리 속성 복원 */
  if (delc == RBTREE_BLACK) {
    rbtree_erase_fixup(t, fix);
  } 

  /* Step 4. 트리에 남은 노드가 없으면 root 초기화 */
  if (t->root != t->nil && t->root->left == t->nil && t->root->right == t->nil && t->root == p) {
    t->root = t->nil;
  }

  /* Step 5. 삭제된 노드 메모리 해제*/
  free(p);
  return 0; 
}


//////////////////////////////////////////////////////////////////////////
//
// static int inorder_fill_array(...)
//
// 레드-블랙 트리를 중위 순회하면서 key 값을 배열에 저장하는 헬퍼 함수
// - 순회 순서: 왼쪽 → 현재 → 오른쪽
// - 배열 크기(n)를 초과하지 않도록 idx 포인터를 통해 삽입 위치를 추적
// - 항상 0 반환
//
//////////////////////////////////////////////////////////////////////////
static int inorder_fill_array(const rbtree *t, node_t *node, key_t *arr, size_t n, size_t *idx)
{
  if (node == t->nil || *idx >=n) return 0;
  inorder_fill_array(t, node->left, arr, n, idx);
  if (*idx < n) { arr[(*idx)++] = node->key; }
  inorder_fill_array(t, node->right, arr, n, idx);
  return 0;
}


//////////////////////////////////////////////////////////////////////////
//
// int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n)
//
// 레드-블랙 트리를 중위 순회(inorder traversal)하여 key 값을 오름차순으로 배열에 저장하는 함수
// - 중위 순회의 특성상 배열에는 항상 정렬된 값이 저장됨
// - 배열 크기 n보다 노드 수가 많을 경우, 앞에서부터 최대 n개까지 저장됨
// - 성공 시 실제 저장한 key의 개수, 예외(: 트리 또는 배열이 NULL인 경우) 시 -1 반환
//
// 구현 전략:
// 1. 내부 헬퍼 함수 inorder_fill_array()를 재귀적으로 호출
// 2. 중위 순회 방식으로 탐색하면서 배열에 key 삽입
// 3. 삽입 위치는 size_t 타입 인덱스(idx)를 포인터로 전달하여 누적 관리
//
//////////////////////////////////////////////////////////////////////////
int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) 
{
  if (t == NULL || arr == NULL || t->nil == NULL || t->root == NULL) return -1;

  size_t idx = 0;
  inorder_fill_array(t, t->root, arr, n, &idx); // &idx로 넘겨야 재귀 누적됨
  return (int)idx;
}