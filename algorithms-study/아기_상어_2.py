"""
아기상어
https://www.acmicpc.net/problem/17086

[Problems]
- N*M 크기의 2차원 공간에 아기 상어(1)와 빈 칸(0)이 있음
- 안전 거리란, 어떤 빈 칸에서 가장 가까운 아기 상어까지의 최단 거리 (8방향 이동)
- 모든 빈 칸 중에서 안전 거리가 가장 큰 칸의 값을 출력하라

[Solvings]
- 각 빈 칸에서 아기상어까지 최단 거리를 매번 BFS로 구하면 시간 초과 위험
- 대신 모든 상어를 동시에 시작점으로 BFS를 실행하여, 각 칸까지의 최단 거리를 한 번에 계산 (다중 시작점 BFS)
- BFS에서 이동은 8방향 (대각선 포함)으로 진행
- [다른 전략] ❌ DFS: 최단 거리를 보장하지 않음
            ❌ Dijkstra: 만약 칸마다 가중치가 달랐다면 다익스트라로 푸는게 더 빠르고 간단함
            ❌ Floyd-Warshall: 모든 정점 간 최단 거리 알고리즘으로, 시간복잡도 O(v^3)이라 불가능

[Pseudocode]
1. 입력: n, m, 2차원 배열 graph[n][m]
2. 거리 배열 dist[n][m]을 -1로 초기화
3. 이중 for문으로 모든 상어(1)의 위치를 찾아 큐에 넣고, dist[x][y] = 0으로 설정
4. BFS 시작
   while 큐가 비어있지 않다면:
       (x, y) = 큐에서 꺼내기
       for (nx, ny) in 8방향 이웃:
           if 범위 안이고 dist[nx][ny] == -1:
               dist[nx][ny] = dist[x][y] + 1
               큐에 (nx, ny) 추가
5. dist 배열에서 최댓값 반환

[Time Complexity]
- 입력 크기: N, M ≤ 50 → 총 칸 수 = N*M ≤ 2,500
- 각 칸은 최대 한 번 방문 → 전체 BFS 시간: O(N*M)
- 거리 최댓값 찾기: O(N*M)
"""

from collections import deque

# [1] 입력 받기
n, m = map(int, input().split())
graph = [list(map(int, input().split())) for _ in range(n)]

# [2] BFS를 위한 준비
q = deque()
dist = [[-1] * m for _ in range(n)] # 거리 기록용

# 8방향 (위쪽부터 시계방향으로)
dx = [-1, -1, 0, 1, 1, 1, 0, -1]
dy = [0, 1, 1, 1, 0, -1, -1, -1]
# 4방향 (위쪽부터 시계방향으로)
# dx = [-1, 0, 1, 0]
# dy = [0, 1, 0, -1]

# [3] 모든 상어를 큐에 넣고 거리 0으로 설정
for i in range(n):
    for j in range(m):
        if graph[i][j] == 1:
            q.append((i, j))
            dist[i][j] = 0

# [4] BFS 실행 (다중 시작점)
while q:
    x, y = q.popleft()
    for dir in range(8):
        nx = x + dx[dir]
        ny = y + dy[dir]
        if 0 <= nx < n and 0 <= ny < m and dist[nx][ny] == -1:
            dist[nx][ny] = dist[x][y] + 1
            q.append((nx, ny))

# [5] 최대 거리 찾기
safe_space = 0
for i in range(n):
    for j in range(m):
        safe_space = max(safe_space, dist[i][j])

print(safe_space)