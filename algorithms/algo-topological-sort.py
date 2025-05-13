from collections import deque

def topological_sort(n, edges):
    graph = [[] for _ in range(n + 1)]
    indegree = [0] * (n + 1)

    # 간선 정보 반영
    for a, b in edges:
        graph[a].append(b)
        indegree[b] += 1

    # 진입 차수 0인 노드부터 큐에 넣음
    q = deque([i for i in range(1, n + 1) if indegree[i] == 0])
    result = []

    while q:
        current = q.popleft()
        result.append(current)

        for neighbor in graph[current]:
            indegree[neighbor] -= 1
            if indegree[neighbor] == 0:
                q.append(neighbor)

    # 결과 확인
    if len(result) != n:
        return None  # 사이클 존재
    return result