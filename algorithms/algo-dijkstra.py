import heapq

INF = int(1e9)

def dijkstra_with_path(v, edges, start, end):
    """
    다익스트라 알고리즘으로 최단 거리 계산 + 최단 경로 복원
    :param v: 정점(노드) 개수
    :param edges: 간선 정보 리스트 (u, to, cost)
    :param start: 시작 노드 번호
    :param end: 도착 노드 번호
    :return: (최단 거리, 경로 리스트)
    """
    # 그래프 초기화 (인접 리스트)
    graph = [[] for _ in range(v + 1)]
    for u, to, cost in edges:
        graph[u].append((cost, to))  # u → to 비용이 cost

    # 최단 거리 및 이전 노드 테이블 초기화
    dist = [INF] * (v + 1)       # dist[i]: start → i까지의 최단 거리
    prev = [-1] * (v + 1)        # prev[i]: i에 오기 전 노드 (경로 복원용)
    dist[start] = 0              # 시작점의 거리는 0

    # 최소 힙 우선순위 큐: (누적 거리, 현재 노드)
    pq = [(0, start)]

    while pq:
        cur_dist, cur = heapq.heappop(pq)

        # 이미 더 짧은 거리로 방문된 경우 스킵
        if dist[cur] < cur_dist:
            continue

        # 인접 노드들 확인
        for cost, neighbor in graph[cur]:
            new_dist = cur_dist + cost
            if dist[neighbor] > new_dist:
                dist[neighbor] = new_dist
                prev[neighbor] = cur  # neighbor 이전에 거친 노드를 기록
                heapq.heappush(pq, (new_dist, neighbor))

    # 최단 거리 결과
    distance = dist[end]

    # ✅ 경로 복원: prev 배열을 따라 도착 노드부터 시작점까지 역추적
    path = []
    node = end
    while node != -1:
        path.append(node)
        node = prev[node]
    path.reverse()  # 역순으로 쌓았으므로 반전

    return distance, path