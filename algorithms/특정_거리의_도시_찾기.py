"""
특정 거리의 도시 찾기
https://www.acmicpc.net/problem/18352

[Problem & Solvings]
- 각 도시는 노드, 도로는 방향이 있는 간선
- 출발 도시 X에서 시작하여 최단 거리가 정확히 K인 도시들을 오름차순으로 출력
- 도시 수 N ≤ 300,000, 도로 수 M ≤ 1,000,000 → 시간 복잡도 주의!
- 간선 가중치는 모두 1이므로 BFS가 효율적 (O(N + M))
- 다익스트라도 가능하나, 우선순위 큐 사용으로 오버헤드 있음

[Pseudocode]
1. 입력 처리
    n, m, k, x ← 도시 수, 도로 수, 목표 거리, 시작 도시
    graph ← [[] for _ in range(n + 1)]
    도로 정보 m개 입력받아 graph[a].append(b)
2. 거리 배열 초기화
    distance ← [-1] * (n + 1)
    distance[x] ← 0
3. BFS 함수 정의
    함수 bfs(start):
        큐 ← deque([start])
        while 큐가 비어있지 않으면:
            cur ← 큐에서 pop
            for nxt in graph[cur]:
                if distance[nxt] == -1:
                    distance[nxt] ← distance[cur] + 1
                    큐에 nxt 추가
4. BFS 실행
    bfs(x)
5. 결과 출력
    distance[i] == k인 i를 리스트로 모아 정렬
    결과가 있으면 출력, 없으면 -1 출력

[Time Complexity]
- BFS: O(N + M)
- 다익스트라: O((N + M) log N) with heapq
"""

from collections import deque
import sys
input = sys.stdin.readline

# 입력
n, m, k, x = map(int, input().split())  # vertex, arc, 최단 거리 정보, 출발 도시 번호
graph = [[] for _ in range(n+1)]

# 도로 정보 입력
for _ in range(m):
    a, b = map(int, input().split())
    graph[a].append(b)  # 방향 그래프이므로 a → b

# 최단 거리 기록 배열
distance = [-1] * (n+1)
distance[x] = 0

# ✅ BFS
def bfs(start):
    q = deque([start])
    while q:
        cur = q.popleft()
        for nxt in graph[cur]:
            if distance[nxt] == -1: # 아직 방문하지 않은 도시라면
                distance[nxt] = distance[cur] + 1
                q.append(nxt)

bfs(x)

# 결과 출력
result = [i for i in range(1, n + 1) if distance[i] == k]
if result:
    for city in sorted(result):
        print(city)
else:
    print(-1)