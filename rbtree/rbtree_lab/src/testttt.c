// 삭제 후 RB트리의 규칙을 복구하는 함수
// p는 삭제된 노드 자리를 대신하게 된 fix_target 노드
void erase_fixup(rbtree *t, node_t *p) {
    node_t *cur = p;
  
    // 현재 노드가 루트가 아니고, BLACK일 경우 재조정
    while (cur != t->root && cur->color == RBTREE_BLACK) {
      node_t *sibling;
  
      // 현재 노드가 왼쪽 자식인지 오른쪽 자식인지에 따라 형제 위치 결정
      if (cur == cur->parent->left) {
        sibling = cur->parent->right;
      } else {
        sibling = cur->parent->left;
      }
  
      // Case 1: 형제가 RED인 경우
      // → 부모-형제 색상 교환 후 회전하여 Case 2~4로 변환
      if (sibling->color == RBTREE_RED) {
        color_swap(t, sibling);  // 형제와 부모 색상 교환
        if (cur == cur->parent->left) {
          left_rotate(t, cur->parent);  // 왼쪽 자식이면 좌회전
        } else {
          right_rotate(t, cur->parent); // 오른쪽 자식이면 우회전
        }
        continue;  // 회전 후 형제가 BLACK이 되어 Case 2~4로 넘어감
      }
  
      // Case 2: 형제와 형제의 두 자식이 모두 BLACK인 경우
      // → 형제를 RED로 바꾸고, 부모로 올라가 다시 검사
      if ((sibling->left->color == RBTREE_BLACK) && (sibling->right->color == RBTREE_BLACK)) {
        sibling->color = RBTREE_RED;
        if (cur->parent->color == RBTREE_RED) {
          cur->parent->color = RBTREE_BLACK;  // 부모가 RED였다면 BLACK으로 바꾸고 종료
          break;
        } else {
          cur = cur->parent;  // 부모가 BLACK이면 다시 재조정
          continue;
        }
      }
  
      // Case 3: 형제가 BLACK이고, 형제의 자식 RED가 할아버지까지 꺾인 상태
      // → 형제와 자식 RED 색상 교환 후 회전 → Case 4로 변환
      if ((cur == cur->parent->left) && (sibling->left->color == RBTREE_RED)) {
        color_swap(t, sibling->left);
        right_rotate(t, sibling);
        sibling = cur->parent->right;  // 회전 후 형제가 바뀔 수 있으므로 갱신
      } else if ((cur == cur->parent->right) && (sibling->right->color == RBTREE_RED)) {
        color_swap(t, sibling->right);
        left_rotate(t, sibling);
        sibling = cur->parent->left;
      }
  
      // Case 4: 형제가 BLACK이고, 형제의 자식 RED가 할아버지까지 일자인 상태
      // → 형제의 색을 할아버지의 색으로 덮고, 할아버지와 형제 자식 Red는 BLACK으로
      // → 할아버지를 축으로 회전하면 fixup 종료
      sibling->color = cur->parent->color;
      if ((cur == cur->parent->left) && (sibling->right->color == RBTREE_RED)) {
        cur->parent->color = sibling->right->color = RBTREE_BLACK;
        left_rotate(t, cur->parent);
      } else if ((cur == cur->parent->right) && (sibling->left->color == RBTREE_RED)) {
        cur->parent->color = sibling->left->color = RBTREE_BLACK;
        right_rotate(t, cur->parent);
      }
      break;  // fixup 완료
    }
  
    // 루트 노드는 항상 BLACK으로 보장
    t->root->color = RBTREE_BLACK;
  }