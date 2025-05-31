"""
불켜기
https://www.acmicpc.net/problem/11967

[Problem & Solvings]
1. 입력 받기
    - n, m -> n: n*n 행렬, m : m개 줄 정보 제공
    - x, y, a, b -> (x, y) 좌표에서 (a, b) 좌표로 이동 가능
2. BFS 준비

- 이동 가능한 좌표를 모두 구하라


"""

import sys
from collections import defaultdict, deque

# [1] 입력 받기
input = sys.stdin.readline
n, m = map(int, input().split())    # 정점 수, 간선 수

# 인접 리스트: (x, y) → [(a1, b1), (a2, b2), ...]
adj = defaultdict(list)

for _ in range(m):
    x, y, a, b = map(int, input().split())
    adj[(x, y)].append((a, b))


# [2] BFS를 위한 준비
q = deque()
visited = set()
start = (1, 1)
count = 0

# 4방향 (위쪽부터 시계방향으로)
dxy = [(-1, 0), (0, 1), (1, 0), (0, -1)]


# [3] 
q.append(start)
visited.add(start)

while q:
    cur = q.popleft()
    for nxt in adj[cur]:   # 지정된 이동만 고려
        if nxt not in visited:
            visited.add(nxt)
            q.append(nxt)