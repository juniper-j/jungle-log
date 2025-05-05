"""
ABCED
https://www.acmicpc.net/problem/13023

[Problem & Solvings]
- n명의 관계 m이 주어짐
- 이 중 A -> B -> C -> D -> E 방향으로 친구관계인 경우가 존재하면 1, 없으면 0을 출력
- 관계는 (a, b)로 주어지는데 a -> b가 친구라는 뜻

[Psuedocode]
1. 입력 n, m
2. 단방향 그래프를 인접 리스트로 구성
graph[i][j] 순회하면서 graph[j][k]가 있으면 반복문 종료 및 print(1), 끝까지 갔는데 없으면 print(0)
"""

# n, m = map(int, input().split())

# graph = [[] for _ in range(n)]
# for _ in range(m):
#     a, b = map(int, input().split())
#     graph[a].append(b)  # 단방향 그래프

# def dfs(now, depth, path):
#     if depth >= 3:
#         print(1)
#         exit()
#     for next in graph[now]:
#         if next not in graph:
#             dfs(next, depth + 1, path + [next])

# for i in range(n):
#     dfs(i, 0, [i])
# print(0)



n, m = map(int, input().split())

graph = [[] for _ in range(n)]
for _ in range(m):
    a, b = map(int, input().split())
    graph[a].append(b)
    graph[b].append(a)  # 친구 관계는 양방향

visited = [False] * n
found = False  # 조건 만족 여부 플래그

def dfs(now, depth):
    global found
    if depth == 5:
        found = True
        return
    visited[now] = True
    for nxt in graph[now]:
        if not visited[nxt]:
            dfs(nxt, depth + 1)
            if found:
                return
    visited[now] = False  # 백트래킹

for i in range(n):
    dfs(i, 1)
    if found:
        break

print(1 if found else 0)