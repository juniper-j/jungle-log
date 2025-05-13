"""
최소비용 구하기
https://www.acmicpc.net/problem/1916

[Problem & Solvings]
- n개의 도시, m개의 버스(간선), Directed Graph
- 출발 도착 start에서 도착 도시 end까지의 최소 비용 출력
- 간선에 가중치가 있기 때문에 BFS, MST는 부적절
- 단일 출발점에서 다른 노드로의 최단 거리를 구해야 하기 때문에 Dijkstra 적합
- N ≤ 1,000 / M ≤ 100,000 → O((N + M) log N) 이하 필요

[Pseudocode]
1. 입력 처리
    n ← 도시 수
    m ← 버스 수
    graph ← [[] for _ in range(n + 1)]
    for m번:
        a, b, cost ← 입력
        graph[a].append((b, cost))
    start, end ← 출발지, 도착지

2. 거리 배열 초기화
    dist ← [inf] * (n + 1)
    dist[start] ← 0

3. 다익스트라 함수 정의
    함수 dijkstra(start, end):
        pq ← [(start, 0)]  # (현재 노드, 누적 비용)
        while heap이 비어있지 않으면:
            current, cur_dist ← heappop(heap)
            if dist[current] < cur_dist:
                continue
            for nxt, cost in graph[current]:
                if dist[nxt] > cur_dist + cost:
                    dist[nxt] ← cur_dist + cost
                    heappush(heap, (nxt, distance[nxt]))
        return dist[end]

4. 다익스트라 실행 및 결과 출력
    print(dijkstra(start, end))

[Time Complexity]
- 다익스트라 (heapq): O((N + M) log N) → N ≤ 1,000, M ≤ 100,000일 때 무리 없음
- 플로이드 워셜: O(N^3) → N = 1000일 경우 10^9 연산 → ❌
"""

# 입력
import sys, heapq
input = sys.stdin.readline

n = int(input())
m = int(input())
graph = [[] for _ in range(n+1)]

for _ in range(m):
    a, b, cost = map(int, input().split())
    graph[a].append((b, cost))

start, end = map(int, input().split())

# 거리 배열 초기화
INF = int(1e9)
dist = [INF] * (n+1)
dist[start] = 0

# 다익스트라 함수 구현
def dijkstra(start, end):
    pq = [(start, 0)]

    while pq:
        current, cur_dist = heapq.heappop(pq)
        if dist[current] < cur_dist:
            continue
        for nxt, cost in graph[current]:
            if dist[nxt] > cur_dist + cost:
                dist[nxt] = cur_dist + cost
                heapq.heappush(pq, (nxt, dist[nxt]))

    return dist[end]  # 못 도달한 경우 대비

# 다익스트라 실행 및 결과 출력
print(dijkstra(start, end))