import sys
input = sys.stdin.readline

INF = int(1e9)

# 입력 처리
n = int(input())
m = int(input())

# 거리 초기화
dist = [[INF] * (n + 1) for _ in range(n + 1)]

# 자기 자신으로의 거리 0
for i in range(1, n + 1):
    dist[i][i] = 0

# 간선 입력 (u→v 비용 w)
for _ in range(m):
    u, v, w = map(int, input().split())
    dist[u][v] = min(dist[u][v], w)

# 플로이드-워셜 알고리즘 수행
for k in range(1, n + 1):
    for i in range(1, n + 1):
        for j in range(1, n + 1):
            dist[i][j] = min(dist[i][j], dist[i][k] + dist[k][j])

# 결과 출력
for i in range(1, n + 1):
    for j in range(1, n + 1):
        print(dist[i][j] if dist[i][j] != INF else 0, end=' ')
    print()