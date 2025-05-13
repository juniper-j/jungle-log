"""
연결 요소의 개수
https://www.acmicpc.net/problem/11724

[Problem & Solvings]
- 주어진 그래프에서 연결된 요소(컴포넌트)의 개수를 구하는 문제입니다.
- Union-Find(Disjoint Set Union, DSU) 알고리즘을 사용하여 연결 요소를 찾습니다.
- Union by Size 또는 Union by Rank를 사용하여 트리의 균형을 맞추고 효율적인 연산을 구현합니다.

[Pseudocode]
1.	입력
    노드 수 v와 간선 수 e 입력받기
    Union-Find 초기화 (각 노드는 자기 자신을 부모로 설정)
2.	Union-Find 구현
    find(x): x의 루트를 찾는 함수 (경로 압축)
    union(u, v): u와 v를 연결하는 함수 (Union by Size)
3.	간선 처리
    각 간선에 대해 union(a, b) 호출
4.	연결 요소의 개수 구하기
    모든 노드에 대해 find(i) 호출하여 루트를 구하고, 그 값을 집합에 추가
    집합의 크기를 출력

[Time Complexity]
- O(E α(V))
- find와 union은 경로 압축과 합병 시 크기/랭크 최적화 덕분에 거의 O(α(N)) 시간 복잡도를 가짐
- 여기서 α(N)은 아커만 함수의 역함수로 매우 느리게 증가하는 함수
"""

import sys
input = sys.stdin.readline

class UnionFind:
    def __init__(self, n):
        # p[i] < 0이면 루트 노드이자 집합 크기 (-크기)
        self.p = [-1] * (n + 1)  # 1-based index

    def find(self, x):
        if self.p[x] < 0:
            return x
        self.p[x] = self.find(self.p[x])  # 경로 압축
        return self.p[x]

    def union(self, u, v):
        u = self.find(u)
        v = self.find(v)
        if u == v:
            return False  # 이미 같은 집합

        # Union by Size: 작은 집합을 큰 집합에 붙임
        if self.p[v] < self.p[u]:
            u, v = v, u  # u가 더 큰 집합이 되도록 스왑

        # u가 v를 루트로 삼고, 집합 크기 갱신
        self.p[u] += self.p[v]  # u의 집합 크기를 합침 (음수 값을 더함)
        self.p[v] = u  # v의 부모를 u로 설정
        return True

# 입력 받기
v, e = map(int, input().split()) 
uf = UnionFind(v + 1)  # UnionFind 객체 생성, 1-based index 사용

# 간선 정보 입력받기
for _ in range(e):
    a, b = map(int, input().split())
    uf.union(a, b)  # a와 b가 연결되므로 합침

# 연결 요소의 개수 구하기 - 집합 컴프리헨션 (add, set 사용하지 않아도 됨)
components = {uf.find(i) for i in range(1, v+1)}

sys.stdout.write(f"{len(components)}\n")