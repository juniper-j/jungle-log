"""
# 간단한 흐름
1. 임의의 정점에서 시작
2. 현재 연결된 정점들과 인접한 간선 중 가장 작은 가중치를 가진 간선을 선택
3. 연결되지 않은 정점을 계속 추가"
"""

import heapq

def prim(graph, start):
    visited = [False] * len(graph)
    heap = [(0, start)]
    total = 0

    while heap:
        cost, u = heapq.heappop(heap)
        if visited[u]:
            continue
        visited[u] = True
        total += cost
        for v, w in graph[u]:
            if not visited[v]:
                heapq.heappush(heap, (w, v))
    return total