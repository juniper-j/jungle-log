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

    # connected 함수: 두 노드가 같은 집합에 속하는지 확인
    def connected(self, u, v):
        return self.find(u) == self.find(v)

    # size 함수: 특정 노드가 속한 집합의 크기 반환
    def size(self, x):
        root = self.find(x)
        return -self.p[root]  # p[root]는 음수로 크기를 나타냄, 절댓값이 집합 크기

# 입력 받기
v = int(input())    # v: 컴퓨터 개수
e = int(input())    # e: 간선 개수
uf = UnionFind(v + 1)  # UnionFind 객체 생성, 1-based index 사용

# 간선 정보 입력받기
for _ in range(e):
    a, b = map(int, input().split())
    uf.union(a, b)  # a와 b가 연결되므로 합침

# 예시로 connected()와 size()를 사용해보면:
print(uf.connected(4, 5))  # 1번과 2번이 같은 집합에 속하는지 여부
print(uf.size(1))  # 1번이 속한 집합의 크기