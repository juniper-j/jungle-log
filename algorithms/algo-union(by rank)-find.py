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

            # # union 후 상태 출력
            # print("After union({},{})".format(node1, node2))
            # print("parent:", self.parent)
            # print("rank:", self.rank)

    # connected 함수: 두 노드가 같은 집합에 속하는지 확인
    def connected(self, node1, node2):
        return self.find(node1) == self.find(node2)
    
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

# 예시로 connected()와 size()를 사용해보면:
print(uf.connected(1, 2))  # 1번과 2번이 같은 집합에 속하는지 여부