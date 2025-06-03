"""
주간 미팅
https://www.acmicpc.net/problem/12834

[Problem]
- 팀원 N명의 집과 KIST, 씨알푸드가 주어짐
- 각 팀원마다 (집 → KIST 거리) + (집 → 씨알푸드 거리) = d_i
- 단, 도달 불가능한 경우는 -1로 처리
    - 둘 다 불가능: -2
    - 한 곳만 가능: 도달 거리 - 1
- 모든 팀원의 d_i를 더한 값을 출력

[Solving]
- 입력
    - n, v, e: 팀원 수, 장소 수(노드 수), 도로 수(간선 수)
    - kisa, cal: KIST 위치, 씨알푸드 위치
    - 팀원들의 집 위치 n개
    - 도로 정보 e개 (양방향, 거리 있음)
- KIST 위치에서 다익스트라 실행 → 각 집까지 거리 저장
- 씨알푸드 위치에서 다익스트라 실행 → 각 집까지 거리 저장
- 각 집에 대해 문제 조건에 따라 거리 계산 후 합산

[Time Complexity]
- 다익스트라 2회: O(E log V)
- 팀원별 거리 계산: O(N)
- 전체: O(E log V + N)
"""

from collections import defaultdict
import heapq
import sys
input = sys.stdin.readline
INF = float('inf')

# 다익스트라 함수
def dijkstra(n, start, graph):              # 정점, 출발지 노드, 그래프 정보
    dist = [INF] * (n + 1)
    dist[start] = 0
    heap = [(0, start)]                     # (거리, 노드) 형태로 최소 힙 초기화. 시작 노드부터 시작

    while heap:
        cur_dist, now = heapq.heappop(heap)
        
        if cur_dist > dist[now]:            # 이미 처리된 노드라면 무시
            continue

        for next_node, cost in graph[now]:  # 현재 노드에서 인접한 노드들을 확인
            cost += cur_dist                # 현재 노드를 거쳐서 가는 비용 계산
            if cost < dist[next_node]:      # 더 짧은 경로가 발견되면 갱신
                dist[next_node] = cost
                heapq.heappush(heap, (cost, next_node))

    return dist       # 도달 못했으면 -1 반환


n, v, e = map(int, input().split())
kisa, cal = map(int, input().split())
homes = list(map(int, input().split()))
graph = defaultdict(list)

# 시작, 끝, 가중치 입력받기
for _ in range(e):
    a, b, w = map(int, input().split())
    graph[a].append((b, w))
    graph[b].append((a, w))

# 다익스트라 2번 실행
dist_from_kisa = dijkstra(v, kisa, graph)
dist_from_cal = dijkstra(v, cal, graph)

# 각 집마다 거리 계산
total = 0
for home in homes:
    d1 = dist_from_kisa[home]
    d2 = dist_from_cal[home]

    if d1 == INF:
        d1 = -1
    if d2 == INF:
        d2 = -1

    total += d1 + d2

print(total)