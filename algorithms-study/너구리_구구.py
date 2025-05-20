"""
너구리 구구
https://www.acmicpc.net/problem/18126

[Problem & Solvings - DFS]
- 첫 줄에 정점의 개수 n (1 <= n <= 100,000) 주어짐
- 이후 n-1개의 간선의 정보와 가중치가 주어짐
- 루트 노드(1번 방)에서 가장 멀리 떨어진 노드까지의 거리(가중치 합) 출력
- 트리 구조이므로 사이클이 없음 
    → 부모 노드를 인자로 넘겨 재귀 dfs, visited 배열 없이 par 인자만으로 역방향 재방문 방지 가능

[Time Complexity]
- 시간: O(N) (트리 구조에서 모든 노드를 정확히 한 번씩 탐색)
- 공간: O(N) (인접 리스트 + 재귀 콜스택)
"""

import sys
sys.setrecursionlimit(10**6)
input = sys.stdin.readline

# 입력: 노드 개수
n = int(input())

# 인접 리스트 그래프 초기화 (# 1-indexed 노드)
graph = [[] for _ in range(n + 1)]

# 간선 입력 받아 양방향 그래프 구성
for _ in range(n - 1):
    u, v, dist = map(int, input().split())
    graph[u].append((v, dist))
    graph[v].append((u, dist))

# ✅ DFS 함수 정의
def dfs(cur, par, total):
    """
    cur: 현재 노드
    par: 직전 노드(부모 노드), 역방향 재방문 방지
    total: 루트부터 현재 노드까지 누적 거리
    """
    max_dist = total

    # 연결된 모든 노드 탐색, 부모 노드는 다시 방문하지 않음
    # 누적 거리 갱신하며 DFS 진행
    for neighbor, weight in graph[cur]:     
        if neighbor != par:
            max_child = dfs(neighbor, cur, total + weight)
            max_dist = max(max_dist, max_child)
    return max_dist

# 결과 출력: 루트 노드(1번 방)에서 DFS 시작해 가장 먼 거리
print(dfs(1, 0, 0))



"""
[Problem & Solvings - BFS]
- 트리 구조 (노드 N개, 간선 N-1개)에서 루트(1번) 노드로부터 가장 멀리 떨어진 노드까지의 거리 출력
- 사이클이 없는 트리이므로 visited 없이도 BFS에서 '부모 노드만 제외'하면 무한 순환 방지 가능
- BFS는 큐를 사용하므로 재귀 깊이 제한 걱정이 없고, DFS 대비 안정적인 구현 방식
- 다익스트라로도 풀 수 있으나, 이 문제에서는 단순한 BFS가 더 효율적

[Time Complexity]
- 시간: O(N) (모든 노드를 정확히 한 번씩 방문)
- 공간: O(N) (인접 리스트 + 큐 공간 사용)

!!! 다익스트라로도 풀 수 있으니 추후 풀어볼 것 !!!
"""

from collections import deque
import sys
input = sys.stdin.readline

# 입력: 노드 개수
n = int(input())

# 인접 리스트 그래프 초기화 (# 1-indexed 노드)
graph = [[] for _ in range(n + 1)]

# 간선 입력 받아 양방향 그래프 구성
for _ in range(n - 1):
    u, v, dist = map(int, input().split())
    graph[u].append((v, dist))
    graph[v].append((u, dist))

# ✅ BFS 함수 정의
def bfs(start):
    max_dist = 0
    q = deque()
    q.append((start, 0, 0))     # (현재 노드, 부모 노드, 누적 거리)

    while q:
        cur, par, total = q.popleft()
        max_dist = max(max_dist, total)

        for neighbor, weight in graph[cur]:
            if neighbor != par:
                q.append((neighbor, cur, total + weight))

    return max_dist

# 결과 출력: 루트 노드(1번 방)에서 BFS 시작해 가장 먼 거리
print(bfs(1))