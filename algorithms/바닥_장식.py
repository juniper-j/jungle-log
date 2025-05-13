"""
바닥 장식
https://www.acmicpc.net/problem/1388

[문제]
- 방의 세로 크기 n, 방의 가로 크기 m -> n줄 입력 받아야 함
- '-', '|'의 개수를 카운트하는 문제. 단, 연속된 '-', '|'는 하나로 카운트 함

[해결 방법 - 반복문]
- room 리스트에 n개의 요소를 입력 받고, 요소 안은 m개의 요소로 이루어짐
- 탐색
    -는 가로방향으로만 이어지므로 각 행마다 판자의 시작점을 찾아 세고,
	|는 세로방향으로만 이어지므로 각 열마다 판자의 시작점을 찾아 셈
- 집계한 count 총합을 출력
"""

n, m = map(int, input().split())
room = [list(input().strip()) for _ in range(n)]

count = 0

# 가로 방향 탐색: '-'
for i in range(n):
    for j in range(m):
        if room[i][j] == '-':
            if j == 0 or room[i][j-1] != '-':
                count += 1

# 세로 방향 탐색: '|'
for j in range(m):
    for i in range(n):
        if room[i][j] == '|':
            if i == 0 or room[i-1][j] != '|':
                count += 1

print(count)



"""
[해결 방법 - 반복문 + 방문 체크]
- 탐색
    각 셀을 방문하며 해당 문양(- 또는 |)이 시작되는 시점에서만 카운트
    이후 동일 문양이 연속되는 동안은 방문 표시만 하고 넘어감
	이미 방문한 셀은 무시
- 집계한 count 총합을 출력

[Time Complexity]
- 전체 방의 모든 칸을 최대 한 번씩만 방문 → O(N * M)
- 제한 조건 N, M ≤ 50 이므로 충분히 빠름
"""

n, m = map(int, input().split())
room = [list(map(str, input().strip())) for _ in range(n)]
visited = [[False] * m for _ in range(n)]
count = 0

# 가로 탐색
for i in range(n):
    for j in range(m):
        if not visited[i][j] and room[i][j] == '-':
            count += 1
            k = j
            while k < m and room[i][k] == '-':
                visited[i][k] = True
                k += 1
# 세로 탐색
for j in range(m):
    for i in range(n):
        if not visited[i][j] and room[i][j] == '|':
            count += 1
            k = i
            while k < n and room[k][j] == '|':
                visited[k][j] = True
                k += 1

print(count)



"""
[해결 방법 - DFS 재귀]
1. 입력 처리 및 visited 배열 생성
2. DFS 함수 정의:
    - 방향은 문양에 따라 다르게 설정:
        - '-'이면 오른쪽만 탐색
        - '|'이면 아래쪽만 탐색
3. 전체 반복문으로 각 칸을 확인하면서:
    - 아직 방문하지 않았다면 DFS 시작하고 판자 수 +1
4. 시간 복잡도 O(N * M)
"""

import sys
sys.setrecursionlimit(10**6)

n, m = map(int, input().split())
room = [list(input().strip()) for _ in range(n)]
visited = [[False] * m for _ in range(n)]

def dfs(x, y, shape):
    visited[x][y] = True
    if shape == '-':
        if y + 1 < m and not visited[x][y+1] and room[x][y+1] == '-':
            dfs(x, y+1, shape)
    elif shape == '|':
        if x + 1 < n and not visited[x+1][y] and room[x+1][y] == '|':
            dfs(x+1, y, shape)

count = 0
for i in range(n):
    for j in range(m):
        if not visited[i][j]:
            dfs(i, j, room[i][j])
            count += 1

print(count)