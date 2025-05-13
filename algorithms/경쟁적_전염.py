"""
경쟁적 전염
https://www.acmicpc.net/problem/18405

[문제 요약]
- NxN 크기의 시험관에서 1~K번까지의 바이러스가 존재
- 매 초마다 번호가 낮은 바이러스부터 인접한 네 칸에 전염
- S초가 지난 후 (X,Y)에 존재하는 바이러스 종류를 출력
- 바이러스는 동시에 전염되며, 한 칸에는 하나의 바이러스만 존재 가능

[해결 전략]
- 우선순위가 있는 BFS로 해결
- 처음부터 바이러스 위치를 번호순으로 정렬한 뒤 큐에 삽입
- BFS를 수행하면서, 시간이 S초가 되면 탐색 중지
- 바이러스가 전파되는 순서는 번호가 작은 순 → 먼저 큐에 들어간 순
- 이걸 구현하려면 큐에 (시간, 바이러스 번호, x, y) 형태로 넣고 시간 기준으로 종료 조건 체크

[Pseudocode]
1. 입력 받기
   - n, k ← 시험관 크기, 바이러스 종류 수
   - graph ← n x n 시험관 정보
   - s, target_x, target_y ← S초, 출력할 위치

2. 초기 바이러스 정보 저장
   - data ← []
   - for i in range(n):
       for j in range(n):
           if graph[i][j] != 0:
               data.append((graph[i][j], i, j))  # (virus, x, y)
   - data를 virus 번호 순으로 정렬

3. BFS 준비
   - 큐 ← deque([(virus, x, y, 0) for virus, x, y in data])

4. BFS 시작
   - while 큐:
       virus, x, y, time ← 큐.popleft()
       if time == s:
           break
       for dx, dy in [(-1,0),(1,0),(0,-1),(0,1)]:
           nx, ny ← x + dx, y + dy
           if 범위 내이고 graph[nx][ny] == 0:
               graph[nx][ny] = virus
               큐.append((virus, nx, ny, time + 1))

5. 결과 출력
   - print(graph[target_x - 1][target_y - 1])

[Time Complexity]
- 초기 정렬: O(K log K)
- BFS 전체: O(N²) (최대 모든 칸을 한 번씩 탐색)
- 최종 복잡도: O(N² + K log K) → N 최대 200, 충분히 통과 가능
"""

from collections import deque

# 입력 처리
n, k = map(int, input().split())    # n*n 크기의 시험관, 1~k번까지의 바이러스
graph = []  # 시험관
data = []   # 바이러스 정보

# 초기 바이러스 정보 저장
for i in range(n):
    row = list(map(int, input().split()))
    graph.append(row)
    for j in range(n):
        if row[j] != 0:
            data.append((row[j], i, j))    # 바이러스가 있는 값, 좌표만 저장

s, target_x, target_y = map(int, input().split())   # s초 뒤에 (x, y)에 존재하는 바이러스의 종류 출력할 것
data.sort()

# BFS
q = deque()
for virus, x, y in data:
    q.append((virus, 0, x, y))  # (바이러스 번호, 시간, x, y)

dx = [1, 0, -1, 0]
dy = [0, 1, 0, -1]

while q:
    virus, time, x, y = q.popleft()
    if time == s:
        break
    for i in range(4):
        nx = x + dx[i]
        ny = y + dy[i]
        if 0 <= nx < n and 0 <= ny < n:
            if graph[nx][ny] == 0:
                graph[nx][ny] = virus
                q.append((virus, time + 1, nx, ny))

# 결과 출력
print(graph[target_x - 1][target_y - 1])