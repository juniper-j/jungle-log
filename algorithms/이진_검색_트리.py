"""
이진 검색 트리
https://www.acmicpc.net/problem/5639
[문제]
- 전위 순회 결과를 입력으로 받아, 이진 검색 트리의 후위 순회 결과를 출력한다.
- 이진 검색 트리의 성질:
    - 왼쪽 서브트리의 모든 노드는 루트보다 작음
    - 오른쪽 서브트리의 모든 노드는 루트보다 큼

[해결 전략]
1. 입력 받기
    - 입력은 전위 순회 결과이며, 개수 제한 없이 EOF까지 주어짐
    - sys.stdin을 사용하여 모든 줄을 정수로 파싱

2. 재귀 호출 한도를 늘리기
    - 트리 깊이가 최대 10,000이므로, sys.setrecursionlimit을 충분히 크게 설정

3. 후위 순회를 위한 재귀 함수 정의
    - 함수 정의: post_order(start_idx, end_idx)
        - 종료 조건: start_idx > end_idx이면 종료
        - 루트 노드는 nodes[start_idx]이다
        - 오른쪽 서브트리 시작 지점 찾기:
            - start_idx+1부터 루트보다 큰 값이 나오는 첫 인덱스를 찾는다
        - 왼쪽 서브트리 범위: (start_idx + 1) ~ (오른쪽 시작 인덱스 - 1)
        - 오른쪽 서브트리 범위: (오른쪽 시작 인덱스) ~ end_idx
        - 왼쪽 → 오른쪽 → 루트 순서로 재귀 호출 및 출력

4. 재귀 호출 시작
    - 전체 입력에 대해 post_order(0, len(nodes) - 1) 호출
"""

import sys
sys.setrecursionlimit(10 ** 6)

# 입력 받기 (EOF까지)
nodes = [int(line.strip()) for line in sys.stdin]

def post_order(root_idx, last_idx):
    if root_idx > last_idx: # 재귀 중단 조건
        return

    root_value = nodes[root_idx]    # 루트 노드
    right_subtree_idx = root_idx + 1  # 오른쪽 서브트리의 시작 인덱스를 담을 변수 = 오른쪽 서브트리를 시작할 가능성이 있는 첫번째 위치

    # 실제 오른쪽 서브트리 시작 인덱스 찾기 (루트보다 큰 값부터 시작)
    while right_subtree_idx <= last_idx and nodes[right_subtree_idx] < root_value:
        right_subtree_idx += 1

    # 왼쪽 서브 트리 : 루트의 다음부터 ~ 루트보다 크지 않은 노드까지만
    post_order(root_idx + 1, right_subtree_idx - 1)

    # 오른쪽 서브 트리 : 루트보다 큰 노드부터 ~ 끝까지
    post_order(right_subtree_idx, last_idx)

    # 루트 출력
    print(root_value)

# 후위 순회 실행
post_order(0, len(nodes) - 1)