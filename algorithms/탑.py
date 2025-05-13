"""
https://www.acmicpc.net/problem/2493
[문제 해결 전략]
1. tower 리스트로 높이 입력 받기 -> list(map(int, input().split()))
2. 역방향으로 [0]까지 for문 돌면서 더 큰 값의 index+1 찾기 -> j in i-1 ~ 0 이동하면서, tower[j] > tower[i]인 가장 가까운 j를 찾음
    단, tower[0]은 무조건 0 리턴한다. 더 큰 value가 없다면 0 리턴
3. stack.pop()을 result 리스트에 넣고 출력
"""
"""
[timeout fail]
탑마다 왼쪽 모든 탑을 하나씩 확인해서 최악의 시간 복잡도 O(n^2)를 가짐

[안되는 아이디어]
1. 완전 탐색 -> 내 첫 전략
2. 역방향 스택
3. Segment Tree / Binary Indexed Tree
    성능은 좋지만 이 문제에선 스택이 더 직관적이고 빠름
"""

# import sys
# input = sys.stdin.readline

# N = int(input())  # 탑의 개수 입력
# towers = list(map(int, input().split()))    # 탑들의 높이 입력
# stack = []  # 결과 저장용

# for i in range(N):
#     found = False
#     # 왼쪽으로 이동하면서 더 큰 탑을 찾기
#     for j in range(i-1, -1, -1):    # 현재 인덱스의 왼쪽부터 -1 전까지(0까지) -1 만큼 씩 이동
#         if towers[j] > towers[i]:
#             stack.append(j+1)
#             found = True
#             break
#     if not found:
#         stack.append(0) # 더 큰 탑이 없다면 0 저장

# # 결과 출력 -> 스택의 내용을 차례대로 공란과 함께 출력
# for val in stack:
#     print(val, end=' ')



"""
[문제]
- 탑 N개(1<=N<=500,000)가 있다. 탑의 높이 H는 1<=H<=100,000,000 이다.
- '<-' 방향으로 신호를 보내며, 보내는 탑보다 높은 탑 1개에 신호가 도달한다.
- 각 탑이 보낸 신호가 도달한 탑의 번호를 출력하라.

[아이디어]
✔️ 스택을 활용해 작은 탑들은 버리며 시간복잡도를 줄이는 게 포인트
기존 아이디어 유지 및 스택 활용한 버전으로 GPT랑 코딩 함
 
[해결 전략]
1. 입력된 타워의 높이는 towers에 저장
2. stack에 타워의 index, height을 저장
    스택에 값이 있다면 현재 타워의 왼쪽 방향으로 높이가 더 큰게 있는지 탐색. 찾으면 종료
3. 앞서 탐색한 탑의 index(stack[-1][0])를 result에 저장 후 출력
"""

import sys
input = sys.stdin.readline

N = int(input())
towers = list(map(int, input().split()))
stack = []  # 수신 가능성 있는 탑들만 저장 (index, height) -> stack = [(0, 6), (1, 9), ...]
result = []

for i in range(N):
    current_height = towers[i]

    # 왼쪽으로 보되, 전부 보는게 아니라 낮은 탑은 버림
    while stack and stack[-1][1] < current_height:
        stack.pop()

    if stack:   # 스택에 값이 있으면, 더 높으면서 왼쪽으로 가장 가까운 탑의 순서를 저장
        result.append(stack[-1][0] + 1)
    else:
        result.append(0)

    # 현재 탑은 다음 탑의 왼쪽이 되므로 스택에 저장
    stack.append((i, current_height))
    
print(*result)