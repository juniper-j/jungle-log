from collections import deque

# 입력
n, m, start = map(int, input().split())

# 인접 리스트 초기화
adj = [[] for _ in range(n + 1)]

# 간선 입력
# [adj[u].append(v) or adj[v].append(u) for u, v in (map(int, input().split()) for _ in range(m))]
for _ in range(m):
    u, v = map(int, input().split())
    adj[u].append(v)
    adj[v].append(u)

# 번호가 작은 것부터 방문하도록 정렬
# [adj[i].sort() for i in range(1, n + 1)]
for i in range(1, n + 1):
    adj[i].sort()

# ✅ DFS (재귀)
def dfs(cur, visited):
    visited[cur] = True
    print(cur, end=' ')
    for nxt in adj[cur]:
        if not visited[nxt]:
            dfs(nxt, visited)

# ✅ BFS
def bfs(start):
    visited = [False] * (n + 1)
    q = deque()
    q.append(start)
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
dfs(start, visited)
print()
bfs(start)