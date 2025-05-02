"""
결혼식
https://www.acmicpc.net/problem/5567

[Problem]
- 상근이(1번)의 친구와 친구의 친구까지 초대할 수 있음
- 친구 관계는 방향이 없는 그래프 형태로 표현됨
- 총 동기 수 n, 친구 관계 수 m이 주어짐
- m개의 줄에 걸쳐 친구 관계 a b가 주어짐 (a < b는 의미 없음)

[Solvings - 인접 리스트 & BFS]
- 인접 리스트 또는 인접 행렬로 무방향 그래프 구성
- BFS 또는 DFS를 통해 상근이(1번)부터 2단계 깊이(상근이 친구의 친구)까지 탐색
- 방문한 친구 수를 세되, 상근이 자신은 제외

[Pseudocode - 인접 리스트 & BFS]
1. 입력: n, m
2. graph 초기화: [[] for _ in range(n+1)]  
3. 친구 관계 입력: graph[a].append(b), graph[b].append(a)
4. visited 배열 생성 및 BFS 시작:
    - queue에 (1, 0) 삽입 (사람 번호, 깊이)
    - 깊이 0에서 2까지의 노드를 탐색하며 count
5. 출력: count (단, 본인 제외)
"""

from collections import deque

n = int(input()) 
m = int(input())

graph = [[] for _ in range(n+1)]

for _ in range(m):
    a, b = map(int, input().split())
    graph[a].append(b)
    graph[b].append(a)

visited = [False] * (n+1)
visited[1] = True

queue = deque()
queue.append((1, 0))    # (사람 번호, 깊이)

count = 0

while queue:
    person, depth = queue.popleft()
    
    if depth == 2:
        continue  # 친구의 친구까지만 탐색

    for friend in graph[person]:
        if not visited[friend]:
            visited[friend] = True
            count += 1
            queue.append((friend, depth + 1))

print(count)


"""
[Solvings - 인접 리스트 & DFS]


"""


"""
[Solvings - 인접 행렬]


"""