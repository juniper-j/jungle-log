"""
줄 세우기
https://www.acmicpc.net/problem/2252

[Problem & Solvings]
- 총 N명의 학생이 있고, M개의 키 비교 정보가 주어짐
- “A는 B보다 앞에 서야 한다”는 조건을 방향 그래프로 표현할 수 있음 (A → B)
- 위상 정렬을 통해, 모든 조건을 만족하는 한 가지 줄 세우기를 출력
    (조건을 만족하는 정렬이 여러 개일 수 있지만, 그 중 하나만 출력)
- 위상 정렬은 큐(BFS) 방식 또는 재귀(DFS) 방식이 가능하지만, 여기선 큐를 활용한 BFS 방식이 직관적

[Pseudocode]
1. 입력
    N, M ← 학생 수, 키 비교 횟수
    graph ← [ ]  // 인접 리스트 방식
    indegree ← [0] * (N+1)  // 진입 차수 배열 (1번부터 시작)

2. 그래프 구성
    for _ in range(M):
        A, B 입력
        graph[A].append(B)
        indegree[B] += 1

3. 큐 초기화 (진입 차수 0인 노드들)
    queue ← deque()
    for i in 1 ~ N:
        if indegree[i] == 0:
            queue.append(i)

4. 위상 정렬 수행
    result ← []
    while queue:
        node ← queue.popleft()
        result.append(node)

        for next_node in graph[node]:
            indegree[next_node] -= 1
            if indegree[next_node] == 0:
                queue.append(next_node)

5. 출력
    print(*result)

[Time Complexity]
- 입력 처리: O(M)
- 위상 정렬: O(N + M)
- 총 시간복잡도: O(N + M)
→ N(학생 수) ≤ 32,000, M(비교 수) ≤ 100,000 이므로 효율적
"""

from collections import deque
import sys
input = sys.stdin.readline

n, m = map(int, input().split())
graph = [[] for _ in range(n+1)]
indegree = [0] * (n + 1)

for _ in range(m):
    a, b = map(int, input().split())
    graph[a].append(b)
    indegree[b] += 1

def topological_sort(n, graph, indegree):
    indegree = indegree[:]  # 복사본 사용
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

result = topological_sort(n, graph, indegree)
if result is None:
    print("Cycle detected")
else:
    print(*result)