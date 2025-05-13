"""
[문제]
- 무방향 그래프에서 모든 정점을 연결하는 최소 비용의 신장 트리를 구하는 문제
- 최소 신장 트리(MST)는 V개의 정점을 V-1개의 간선으로 연결하며, 사이클이 없어야 함

[해결 전략 - Kruskal]
1. 모든 간선을 (정점1, 정점2, 비용)의 튜플로 edge_list에 저장
    - 무방향 그래프이므로 입력 그대로 한 번씩만 저장

2. edge_list를 비용(cost) 기준으로 **오름차순 정렬**

3. Union-Find를 위한 parent 배열 초기화
    - parent[i] = i  (처음엔 자기 자신이 대표)

4. 다음을 V-1개의 간선이 선택될 때까지 반복:
    - edge_list에서 비용이 가장 적은 간선 (u, v, cost)를 하나씩 꺼냄
    - find(parent, u), find(parent, v)를 통해 u와 v의 루트 노드를 찾음
    - 두 노드가 서로 **다른 집합(root가 다름)**에 속해 있다면:
        1. 사이클이 생기지 않으므로 해당 간선을 선택
        2. union(parent, u, v)를 통해 두 집합을 합침
        3. 비용을 누적(total_cost += cost)
        4. 선택된 간선 수(edge_count)를 +1

5. edge_count가 V - 1이 되면 MST 완성 → 종료

6. 누적된 total_cost를 반환

💡 주요 포인트
- 간선 중심 접근 방식: 비용이 적은 간선부터 선택
- 사이클 방지를 위해 Union-Find 자료구조 사용
    - find(): 루트 노드 찾기 (경로 압축 포함)
    - union(): 서로 다른 집합을 하나로 합침
- 시간 복잡도: O(E log E) = 간선 정렬 시간
- 정점 수 V는 최대 10,000, 간선 수 E는 최대 100,000 → 효율성 OK
"""

# 부모 노드를 찾는 함수 (Find)
def find(parent, node):
    # 자기 자신이 부모가 아니면, 부모를 따라 계속 올라감
    if parent[node] != node:
        parent[node] = find(parent, parent[node])  # 경로 압축
    return parent[node]

# 두 집합을 하나로 합치는 함수 (Union)
def union(parent, node1, node2):
    root1 = find(parent, node1)
    root2 = find(parent, node2)
    if root1 != root2:
        parent[root2] = root1  # 하나의 집합으로 합치기

# Kruskal 알고리즘 구현
def kruskal(num_nodes, edge_list):
    # 1. 각 노드는 처음에 자기 자신이 부모
    parent = [i for i in range(num_nodes + 1)]

    # 2. 간선을 가중치 기준으로 정렬 (오름차순)
    edge_list.sort(key=lambda edge: edge[2])  # edge = (노드1, 노드2, 비용)

    total_cost = 0  # MST의 총 비용
    edge_count = 0

    # 3. 정렬된 간선을 하나씩 확인
    for node1, node2, cost in edge_list:
        # 사이클이 생기지 않는다면 (서로 다른 집합이면)
        if find(parent, node1) != find(parent, node2):
            union(parent, node1, node2)  # 두 노드를 연결
            total_cost += cost  # 비용 추가
            edge_count += 1
            if edge_count == num_nodes - 1:
                break

    return total_cost

import sys
sys.setrecursionlimit(10**6)
input = sys.stdin.readline

# 입력 처리
v, e = map(int, input().split())
edge_list = []

for _ in range(e):
    a, b, c = map(int, input().split())
    edge_list.append((a, b, c))
    
print(kruskal(v, edge_list))