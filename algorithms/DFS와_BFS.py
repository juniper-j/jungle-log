"""
DFS와 BFS
https://www.acmicpc.net/problem/1260

[Problem & Solvings]
- 무방향 그래프에서 시작 정점 V로부터 DFS와 BFS를 수행한 순서를 출력하는 문제
- 정점 번호가 작은 순으로 방문해야 하므로, 인접 리스트를 오름차순 정렬해두고 탐색
- DFS는 재귀로 구현하고, BFS는 큐(deque)를 이용해 구현
- 각 탐색에서 방문 여부를 확인하기 위해 visited 배열 사용

[Pseudocode]
1. 정점 개수 n, 간선 개수 m, 시작 정점 v 입력
2. 인접 리스트 adj 생성 후 간선 정보 입력 (무방향이므로 양쪽 추가)
3. 각 정점의 인접 리스트를 정렬 (번호가 작은 정점부터 탐색하기 위해)
4. DFS: 현재 노드를 방문하고, 인접 노드를 재귀적으로 방문
5. BFS: 큐를 이용해 현재 노드의 인접 노드를 순차적으로 방문
6. DFS와 BFS 각각 방문 순서 출력

[Time Complexity]
- DFS: O(N + M) — 정점 N개, 간선 M개를 인접 리스트로 탐색
- BFS: O(N + M) — 큐를 사용한 너비 우선 탐색
"""

from collections import deque

n, m, v = map(int, input().split())
adj = [[] for _ in range(n+1)]

for _ in range(m):
    a, b = map(int, input().split())
    adj[a].append(b)
    adj[b].append(a)

for i in range(1, n + 1):
    adj[i].sort()

# ✅ DFS (재귀)
def dfs(cur, visited, adj):
    visited[cur] = True
    print(cur, end=' ')
    for nxt in adj[cur]:
        if not visited[nxt]:
            dfs(nxt, visited, adj)

# ✅ BFS
def bfs(start, visited, adj):
    q = deque([start])
    visited[start] = True

    while q:
        cur = q.popleft()
        print(cur, end=' ')
        for nxt in adj[cur]:
            if not visited[nxt]:
                visited[nxt] = True
                q.append(nxt)

# 실행
visited = [False] * (n + 1)
dfs(v, visited, adj)
print()
visited = [False] * (n + 1)
bfs(v, visited, adj)