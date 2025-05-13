"""
미로만들기
https://www.acmicpc.net/problem/2665

[Problem & Solvings]
- N*N 격자에서 0은 검은 방, 1은 흰 방을 의미
- 왼쪽 위(0,0)(흰 방)에서 오른쪽 아래(N-1,N-1)(흰 방)까지 이동할 때 최소한의 검은 방을 흰 방으로 바꿔 도착해야 함
- 방을 이동하면서 검은 방을 흰 방으로 바꾸는 최소 횟수를 구하는 문제
- 가중치가 다르기 때문에 단순한 BFS보다는 다익스트라로 해결하는 것이 효과적
- 우선순위 큐를 사용해, 벽을 부순 횟수가 적은 경로를 우선적으로 탐색

[Pseudocode]
1. 입력 처리
    N ← 미로 크기 입력
    maze ← 미로 정보를 2차원 배열로 입력

2. 우선순위 큐를 이용한 다익스트라 준비
    heap ← 최소 힙 [(벽을 부순 횟수, x, y)]
    visited ← 각 위치에서 최소로 벽을 부순 횟수 저장하는 배열 (모두 무한대 초기화)

3. 다익스트라 알고리즘 실행
    heap에 (0,0,0) 넣기 (시작 위치 벽을 0번 부숨)
    visited[0][0] ← 0

    while heap이 비지 않았다면:
        count, x, y ← heap에서 pop()
        if 현재 위치가 목적지라면 종료

        for (nx, ny)가 (x, y)의 인접 칸:
            범위 안에 있고, 방문 안 했거나 더 적은 벽을 부순다면:
                if maze[nx][ny] == 0:   # 검은 방이면 벽 부수고 가야함
                    if visited[nx][ny] > count + 1:
                        visited[nx][ny] ← count + 1
                        heap에 (count+1, nx, ny) 추가
                else:                  # 흰 방이면 그냥 이동
                    if visited[nx][ny] > count:
                        visited[nx][ny] ← count
                        heap에 (count, nx, ny) 추가

4. 결과 출력
    visited[N-1][N-1] 출력

[Time Complexity]
- 우선순위 큐(힙)을 이용한 다익스트라: O(N² log N²) = O(N² log N)
(모든 칸을 최대 1번씩 방문할 수 있고, 힙에서 push/pop 연산 O(log(N²))이 걸림)
"""

import heapq

n = int(input())
maze = [list(map(int, input().strip())) for _ in range(N)]
visited = [[float('inf')]*N for _ in range(N)]

dx = [-1, 1, 0, 0]
dy = [0, 0, -1, 1]

def dijkstra():
    heap = [(0, 0, 0)]
    visited[0][0] = 0

    while heap:
        count, x, y = heapq.heappop(heap)

        if x == N-1 and y == N-1:
            return count

        for i in range(4):
            nx, ny = x + dx[i], y + dy[i]
            
            if 0 <= nx < N and 0 <= ny < N:
                next_count = count + (1 if maze[nx][ny] == 0 else 0)
                if visited[nx][ny] > next_count:
                    visited[nx][ny] = next_count
                    heapq.heappush(heap, (next_count, nx, ny))

print(dijkstra())