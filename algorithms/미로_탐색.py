"""
미로 탐색
https://www.acmicpc.net/problem/2178

[Problem & Solvings]
- (1, 1)에서 시작해서 (N, M)까지 이동할 때 지나야 하는 최소 칸 수를 구하라
- 상하좌우로 인접한 칸 중 값이 1인 곳만 이동 가능
- 모든 간선의 가중치가 동일(=1)이므로 BFS를 사용하면 최단 거리를 구할 수 있음
- 방문한 좌표를 다시 방문하지 않도록 처리하며, BFS로 탐색하면서 이동 거리를 함께 기록

[Pseudocode]
1. 입력 처리
    n, m ← 지도 크기
    maze ← n줄의 문자열을 정수 리스트로 변환하여 저장
2. 방문 여부 저장 배열 visited 생성
    visited ← n x m 크기의 False 배열
3. 방향 배열 정의
    dx, dy ← [-1, 1, 0, 0], [0, 0, -1, 1]
4. BFS 정의
    함수 bfs(start, visited, maze):
        큐 ← deque([start])
        visited[start] ← True
        while 큐가 비지 않으면:
            x, y ← 큐에서 pop
            for i in 0~3:
                nx ← x + dx[i], ny ← y + dy[i]
                if 범위 안이고, maze[nx][ny] == 1이고, 방문하지 않았으면:
                    maze[nx][ny] ← maze[x][y] + 1
                    visited[nx][ny] ← True
                    큐에 (nx, ny) 추가
5. BFS 실행
    bfs((0, 0), visited, maze)
6. 결과 출력
    maze[n-1][m-1] 출력

[Time Complexity]
- 시간 복잡도: O(N * M)
- 각 좌표를 한 번씩만 방문하고, 큐에서 한 번씩만 꺼내기 때문
"""

from collections import deque
import sys
input = sys.stdin.readline

# 입력
n, m = map(int, input().split())
maze = [list(map(int, input().strip())) for _ in range(n)]

# 방문 처리 배열
visited = [[False] * m for _ in range(n)] 
# visited = [[False] * 4 for _ in range(3)]
# [
#  [False, False, False, False]
#  [False, False, False, False],
#  [False, False, False, False]
# ]


# 이동 방향
#      ↑  ↓  ←  →
dx = [-1, 1, 0, 0]  # 행 변화량
dy = [0, 0, -1, 1]  # 열 변화량

# ✅ BFS
def bfs(start, visited, maze):
    q = deque([start])
    visited[start[0]][start[1]] = True

    while q:
        x, y = q.popleft()

        for i in range(4):
            nx = x + dx[i]
            ny = y + dy[i]
            
            # 범위 밖, 벽(0), 방문한 곳은 패스
            if not (0 <= nx < n and 0 <= ny < m):
                continue
            if maze[nx][ny] == 0 or visited[nx][ny]:
                continue

            # 거리 갱신 + 방문 처리 + 큐 추가
            maze[nx][ny] = maze[x][y] + 1
            visited[nx][ny] = True
            q.append((nx, ny))

bfs((0, 0), visited, maze)
print(maze[n-1][m-1])