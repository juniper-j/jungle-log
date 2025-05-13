"""
# 간단한 흐름
1. 간선을 가중치 기준으로 오름차순 정렬
2. 사이클을 만들지 않도록 하나씩 선택
3. V-1개의 간선이 선택되면 종료"
"""

# Union-Find + 간선 정렬
def find(parent, x):
    if parent[x] != x:
        parent[x] = find(parent, parent[x])
    return parent[x]

def union(parent, a, b):
    a, b = find(parent, a), find(parent, b)
    if a != b:
        parent[b] = a

def kruskal(V, edges):
    parent = [i for i in range(V)]
    edges.sort(key=lambda x: x[2])  # 가중치 기준 정렬
    total = 0

    for a, b, cost in edges:
        if find(parent, a) != find(parent, b):
            union(parent, a, b)
            total += cost
    return total