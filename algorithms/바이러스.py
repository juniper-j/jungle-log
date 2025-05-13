"""
바이러스
https://www.acmicpc.net/problem/2606

[Problem & Solvings]
- 네트워크 안에 있는 컴퓨터들이 어떻게 연결되어 있는지 나타내는 그래프가 주어지고, 
  특정 컴퓨터가 바이러스에 감염되었을 때, 그 컴퓨터를 통해 바이러스가 전염되는 다른 컴퓨터들을 찾는 문제
- 감염된 컴퓨터에서 시작해서, 연결된 컴퓨터들을 탐색하며 바이러스가 퍼지는 방식을 시뮬레이션
- 그래프 탐색을 통해 감염된 컴퓨터 수를 구하는 문제로, 깊이 우선 탐색(DFS)이나 너비 우선 탐색(BFS) 알고리즘을 사용하여 해결

[Pseudocode - dfs/bfs]
1.	입력
    v, e ← 컴퓨터 개수 v, 연결된 간선(컴퓨터 쌍) e
    간선 정보를 받아와 연결된 컴퓨터들을 리스트로 저장
2.	바이러스 감염 컴퓨터 탐색 함수
    DFS(혹은 BFS)를 이용해, 컴퓨터가 감염되었는지 확인하고 연결된 컴퓨터를 재귀적으로 탐색
    바이러스 감염된 컴퓨터 수를 세기
3.	출력
    바이러스가 감염된 컴퓨터 수 출력

[Time Complexity]
- 그래프 탐색을 수행하므로 시간 복잡도는 O(V + E)
"""

from collections import deque

v = int(input())
e = int(input())
graph = [[] for _ in range(v+1)]
visited = [False] * (v+1)

for _ in range(e):
    a, b = map(int, input().split())
    graph[a].append(b)
    graph[b].append(a)

def dfs(cur, visited, graph):
    visited[cur] = True
    for nxt in graph[cur]:
        if not visited[nxt]:
            dfs(nxt, visited, graph)

# dfs(1, visited, graph)
# print(visited.count(True) - 1) # 1번 컴퓨터는 제외하고 계산

def bfs(start, visited, graph):
    q = deque([start])
    visited[start] = True

    while q:
        cur = q.popleft()
        for nxt in graph[cur]:
            if not visited[nxt]:
                visited[nxt] = True
                q.append(nxt)

bfs(1, visited, graph)
print(visited.count(True) - 1)  # 1번 컴퓨터는 제외하고 계산



"""
[Pseudocode - UnionFind - Union by Rank]
1. Union-Find 자료구조 초기화
    - 부모 배열 `parent`: 각 노드의 부모를 자기 자신으로 초기화 (parent[i] = i)
    - 랭크 배열 `rank`: 각 노드의 트리 높이를 0으로 초기화 (Union by Rank) (rank[i] = 0)
2. 입력 받기
    - 정점의 수 v(컴퓨터 개수)와 간선의 수 e(연결된 컴퓨터 수)를 입력 받음
    - UnionFind 객체 생성 (1-based index 사용)
3. 간선 정보 입력 받기
    - 각 간선에 대해 (a, b) 입력 받음 (a와 b는 연결된 두 컴퓨터)
    - `union(a, b)`를 통해 두 컴퓨터를 하나의 집합으로 합침
4. 감염된 컴퓨터 수 계산
    - 1번 컴퓨터가 속한 집합의 대표를 찾음 (find(1))
    - 그 집합에 속한 모든 컴퓨터를 찾아 감염된 컴퓨터 수를 셈
5. 출력
    - 감염된 컴퓨터 수 출력 (1번 컴퓨터를 제외한 컴퓨터 수)

### 주요 함수
- **find**: 특정 컴퓨터가 속한 집합의 대표 노드를 찾음 (경로 압축 포함)
- **union**: 두 컴퓨터를 하나의 집합으로 합침 (Union by Rank)

[Time Complexity]
- 그래프 탐색을 수행하므로 시간 복잡도는 O(V + E)
- Find: 경로 압축을 적용하면 O(1)에 가까움
- Union: 두 find 연산 후 합치기만 하면 되므로 O(α(n))
- 따라서 전체 시간 복잡도는 O((V + E) * α(V))
"""

# Union-Find 자료구조
class UnionFind:
    def __init__(self, n):
        # 각 노드의 부모를 자기 자신으로 초기화
        self.parent = list(range(n))  # 1-based index
        self.rank = [0] * n  # 트리의 깊이를 관리하는 배열 (Union by Rank)

    # find 연산: 부모 노드를 찾는 함수
    def find(self, node):
        # 자기 자신이 부모가 아니면, 부모를 따라 계속 올라감
        if self.parent[node] != node:
            self.parent[node] = self.find(self.parent[node])  # 경로 압축
        return self.parent[node]

    # union 연산: 두 집합을 합침 (Union by Rank)
    def union(self, node1, node2):
        root1 = self.find(node1)
        root2 = self.find(node2)
        
        if root1 != root2:
            # rank가 작은 트리를 큰 트리 아래에 붙이기
            if self.rank[root1] > self.rank[root2]:
                self.parent[root2] = root1
            elif self.rank[root1] < self.rank[root2]:
                self.parent[root1] = root2
            else:
                self.parent[root2] = root1
                self.rank[root1] += 1

# 입력 받기
v = int(input())    # v: 컴퓨터 개수
e = int(input())    # e: 간선 개수
uf = UnionFind(v + 1)  # UnionFind 객체 생성, 1-based index 사용

# 간선 정보 입력받기
for _ in range(e):
    a, b = map(int, input().split())
    uf.union(a, b)  # a와 b가 연결되므로 합침

# 1번 컴퓨터가 속한 집합을 찾고, 그 집합에 속한 컴퓨터들을 셈
infected_count = sum(1 for i in range(1, v + 1) if uf.find(i) == uf.find(1))

# 출력 (1번 컴퓨터를 제외한 감염된 컴퓨터 수)
print(infected_count - 1)