"""
단지번호붙이기
https://www.acmicpc.net/problem/2667

[문제 & 해결방법 - BFS]
- 지도는 N x N 크기의 2차원 배열로, 1은 집이 있는 곳, 0은 없는 곳
- 연결된 집들은 하나의 단지로 묶으며, 상하좌우로 연결된 집만 단지로 간주
- 총 단지 수와, 각 단지에 속한 집의 수를 오름차순으로 출력
- 인접한 노드 탐색 문제 → DFS 또는 BFS 사용 가능
- visited 배열을 활용해 이미 방문한 노드는 다시 방문하지 않도록 처리

[Time Complexity]
- 전체 탐색: O(N²)
- 각 노드는 한 번만 방문 → DFS/BFS 수행 총 시간도 O(N²)
- 입력 범위: N ≤ 25 → 완전탐색 충분히 가능
"""

from collections import deque

n = int(input())
group = [list(map(str, input().strip())) for _ in range(n)]

visited = [[False] * n for _ in range(n)]  # 방문 여부 기록

# 상, 하, 좌, 우 방향
dx = [1, 0, -1, 0]  # 행
dy = [0, 1, 0, -1]  # 열

# BFS 시작
def bfs(x, y):
    q = deque()
    q.append((x, y))      # 큐에 시작점 삽입
    visited[x][y] = True  # 시작점 (0, 0) 방문 처리
    count = 1

    while q:
        x, y = q.popleft()

        # 상하좌우 인접한 칸 확인
        for i in range(4):
            nx = x + dx[i]
            ny = y + dy[i]

            if 0 <= nx < n and 0 <= ny < n:
                if not visited[nx][ny] and group[nx][ny] == '1':
                    visited[nx][ny] = True
                    q.append((nx, ny))
                    count += 1
                    
    return count

result = []

for i in range(n):
    for j in range(n):
        if group[i][j] == '1' and not visited[i][j]:
            result.append(bfs(i, j))

result.sort()

print(len(result))
print(*result, sep='\n')



"""
[해결방법 - DFS]
1. 방향 벡터 설정
2. DFS 함수 정의
3. 전체 좌표에 대해 DFS 실행
4. 출력
"""

n = int(input())
group = [list(input().strip()) for _ in range(n)]
visited = [[False] * n for _ in range(n)]

dx = [1, 0, -1, 0]
dy = [0, 1, 0, -1]

def dfs(x, y):
    visited[x][y] = True
    count = 1

    for i in range(4):
        nx, ny = x + dx[i], y + dy[i]

        if 0 <= nx < n and 0 <= ny < n:
            if not visited[nx][ny] and group[nx][ny] == '1':
                count += dfs(nx, ny)
                
    return count

result = []

for i in range(n):
    for j in range(n):
        if group[i][j] == '1' and not visited[i][j]:
            result.append(dfs(i, j))

result.sort()
print(len(result))
print(*result, sep='\n')