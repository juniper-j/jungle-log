"""
최소 스패닝 트리 (Prim)
https://www.acmicpc.net/problem/1197

[문제]
- 무방향 그래프에서 모든 정점을 연결하는 최소 비용의 신장 트리를 구하는 문제
- 최소 신장 트리(MST)는 V개의 정점을 V-1개의 간선으로 연결하며, 사이클이 없어야 함

[해결 전략 - Prim]
1. 그래프를 인접 리스트 형태로 저장
    - graph[u] = [(v, cost), ...] 형태로 저장
    - 간선은 양방향이므로 a→b, b→a 모두 추가

2. 시작 정점을 선택하고 방문 표시
    - 일반적으로 1번 정점에서 시작
    - visited[시작 정점] = True

3. 시작 정점에서 뻗는 모든 간선을 우선순위 큐에 삽입
    - 각 간선은 (비용, 도착 정점) 형태로 삽입

4. 다음을 모든 정점이 연결될 때까지 반복:
    - 우선순위 큐에서 가장 가중치가 낮은 간선을 꺼냄
    - 도착 정점이 이미 방문한 정점이면 무시하고 건너뜀
    - 방문하지 않았다면:
        1. 해당 정점을 방문 표시
        2. 해당 간선의 비용을 total_cost에 더함
        3. 그 정점에서 뻗는 간선들을 다시 우선순위 큐에 삽입

5. 모든 정점이 MST에 포함되면 반복 종료

6. 누적된 total_cost를 반환

💡 주요 포인트
- 최소 비용 간선을 고르기 위해 우선순위 큐(heapq) 사용
- 시간 복잡도는 O(E log V)
- visited 배열로 사이클 방지
- 정점 수 V는 최대 10,000, 간선 수 E는 최대 100,000
"""

import heapq
import sys
input = sys.stdin.readline

def prim(start, graph, v):
    visited = [False] * (v + 1)  # 각 정점 방문 여부
    min_heap = []
    total_cost = 0

    visited[start] = True
    for to, cost in graph[start]:
        heapq.heappush(min_heap, (cost, to))  # (비용, 도착 정점)

    while min_heap:
        cost, node = heapq.heappop(min_heap)  # 가장 비용이 적은 간선 선택
        if visited[node]:
            continue

        visited[node] = True  # 새로운 정점을 트리에 포함
        total_cost += cost  # MST 비용 누적

        for to, next_cost in graph[node]:  # 현재 정점에서 뻗는 간선들 추가
            if not visited[to]:
                heapq.heappush(min_heap, (next_cost, to))

    return total_cost

# 입력 처리
v, e = map(int, input().split())
graph = [[] for _ in range(v + 1)]

for _ in range(e):
    a, b, c = map(int, input().split())
    graph[a].append((b, c))
    graph[b].append((a, c))  # 무방향 그래프이므로 양방향 간선 추가

print(prim(1, graph, v))