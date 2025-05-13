"""
이분 그래프
https://www.acmicpc.net/problem/1707

[Problem & Solvings]
- 그래프가 이분 그래프인지 판별하는 문제
- 이분 그래프란? 인접한 정점끼리는 다른 집합에 속하도록 두 개의 집합으로 나눌 수 있는 그래프
- 즉, 인접한 노드끼리 같은 색으로 색칠할 수 없으면 이분 그래프가 아님
- 입력: 테스트 케이스 수 T, 각 테스트 케이스마다 정점 수 V(≤ 20,000), 간선 수 E(≤ 200,000)
- 연결 그래프가 아닐 수 있음 → 모든 정점에서 탐색 필요
- DFS 또는 BFS로 두 가지 그룹(예: 1, -1)을 번갈아 마크하면서 인접한 노드에 같은 그룹이 나오면 이분 그래프 아님

[Pseudocode]
1. 입력 처리
    k ← 테스트 케이스 수
    for _ in range(k):
        v, e ← 정점 수, 간선 수
        graph ← [[] for _ in range(v + 1)]
        for _ in range(e):
            a, b ← 입력받아 graph[a].append(b), graph[b].append(a)

2. 방문/그룹 배열 초기화
    visited ← [0] * (v + 1)  # 0: 방문 안 함, 1: 그룹A, -1: 그룹B

3. DFS 함수 정의
    def dfs(current, group):
        visited[current] ← group
        for nxt in graph[current]:
            if visited[nxt] == 0:
                if not dfs(nxt, -group):
                    return False
            elif visited[nxt] == group:
                return False
        return True

4. 모든 노드에 대해 이분성 검사
    is_bipartite ← True
    for i in range(1, v + 1):
        if visited[i] == 0:
            if not dfs(i, 1):
                is_bipartite ← False
                break

5. 결과 출력
    is_bipartite이면 'YES', 아니면 'NO'

[Time Complexity]
- 각 테스트 케이스마다 O(V + E)
- 전체 입력 기준 최대 20,000 노드, 200,000 간선이므로 효율적
- DFS, BFS 모두 사용 가능 (스택 or 큐만 다름)
"""

import sys
sys.setrecursionlimit(10**6)
input = sys.stdin.readline

def dfs(current, group):
    visited[current] = group    # 현재 노드의 그룹 저장
    # 인접 노드 탐색
    for neighbor in graph[current]:
        if visited[neighbor] == 0:  # 아직 방문하지 않은 노드
            if not dfs(neighbor, -group):   # 다른 그룹으로 dfs 재귀
                return False
        elif visited[neighbor] == group:    # 이미 같은 그룹이면 이분 그래프 아님
                return False
    return True
    
k = int(input())    # 테스트 케이스 개수
for _ in range(k):
    v, e = map(int, input().split())
    graph = [[] for _ in range(v+1)]
    visited = [0] * (v+1)

    for _ in range(e):
        a, b = map(int, input().split())
        graph[a].append(b)
        graph[b].append(a)

    is_bipartite = True
    for i in range(1, v+1):
        if visited[i] == 0:     # 아직 방문하지 않은 노드면
            if not dfs(i, 1):   # 그룹A로 시작해서 dfs 수행해서 이분 그래프 조건을 위반하면
                is_bipartite = False    
                break   # 하나라도 이분 그래프가 아니면 중단

    print("YES" if is_bipartite else "NO")