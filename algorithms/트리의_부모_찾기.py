"""
트리의 부모 찾기
https://www.acmicpc.net/problem/11725

[Problem & Solvings]
- 루트가 1번(부모 없음)인 트리가 주어질 때, 각 노드의 부모 노드를 출력하는 문제
- 트리는 사이클이 없는 연결 그래프이다, 간선 수는 항상 N-1개
- 루트 노드 1번에서부 터 DFS or BFS로 탐색하면서 방문한 노드의 부모를 저장

[Pseudocode - DFS 재귀]
1. 입력 처리
    n ← int(input())
    graph ← [[] for _ in range(n + 1)]
    for _ in range(n - 1):
        a, b ← map(int, input().split())
        graph[a].append(b)
        graph[b].append(a)

2. 부모 정보 저장 배열
    parent ← [0] * (n + 1)

3. DFS 함수 정의
    def dfs(current, par):
        parent[current] ← par
        for neighbor in graph[current]:
            if neighbor != par:
                dfs(neighbor, current)

4. DFS 실행 (루트 노드는 1, 부모는 0)
    dfs(1, 0)

5. 2번 노드부터 부모 출력
    for i in range(2, n + 1):
        print(parent[i])

[Time Complexity]
- 입력 처리: O(N)
- DFS 탐색: O(N)
- 전체 시간 복잡도: O(N)
"""

# import sys
# sys.setrecursionlimit(10**6)
# input = sys.stdin.readline

# # 입력 처리
# n = int(input())
# graph = [[] for _ in range(n+1)]

# for _ in range(n-1):
#     a, b = map(int, input().split())
#     graph[a].append(b)
#     graph[b].append(a)

# # 부모 정보 저장 배열
# parent = [0] * (n+1)

# # [Pseudocode]
# # 1. DFS(current, par): 현재 노드와 부모 노드를 인자로 받아
# # 2. 현재 노드의 부모를 기록
# # 3. 인접 노드 중, 부모 노드가 아닌 경우에만 재귀적으로 탐색
# def dfs(current, par):
#     parent[current] = par   # 현재 노드의 부모 저장
#     for neighbor in graph[current]:     # 인접 노드 순회
#         if neighbor != par:             # 부모 방향은 제외
#             dfs(neighbor, current)      # 자식 노드로 내려감

# dfs(1, 0)

# for i in range(2, n + 1):
#     print(parent[i])



import sys
sys.setrecursionlimit(10**6)
input = sys.stdin.readline

n = int(input())
graph = [[] for _ in range(n+1)]

for _ in range(n-1):
    a, b = map(int, input().split())
    graph[a].append(b)
    graph[b].append(a)

parent = [0] * (n+1)
visited = [False]* (n + 1)

"""
[Pseudocode - DFS 반복문, 스택]
stack ← [1]
visited[1] ← True
while stack is not empty:
    cur ← stack.pop()
    for nxt in graph[cur]:
        if not visited[nxt]:
            parent[nxt] ← cur
            visited[nxt] ← True
            stack.append(nxt)

[Time Complexity]
- 전체 시간 복잡도: O(N)
"""
# stack = [1]
# visited[1] = True

# while stack:
#     current = stack.pop()
#     for neighbor in graph[current]:
#         if not visited[neighbor]:
#             parent[neighbor] = current
#             visited[neighbor] = True
#             stack.append(neighbor)

"""
[Pseudocode - BFS]
queue ← deque([1])
visited[1] ← True
while queue is not empty:
    cur ← queue.popleft()
    for nxt in graph[cur]:
        if not visited[nxt]:
            parent[nxt] ← cur
            visited[nxt] ← True
            queue.append(nxt)

[Time Complexity]
- 전체 시간 복잡도: O(N)
"""

from collections import deque
q = deque([1])
visited[1] = True

while q:
    current = q.popleft()
    for neighbor in graph[current]:
        if not visited[neighbor]:
            parent[neighbor] = current
            visited[neighbor] = True
            q.append(neighbor)

for i in range(2, n+1):
    print(parent[i])