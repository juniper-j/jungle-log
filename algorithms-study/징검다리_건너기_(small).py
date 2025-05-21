"""
징검다리 건너기 (small)
https://www.acmicpc.net/problem/22869

[Problem & Solvings]
- 징검다리 n개, 최대 힘 k
- 징검다리 마다 부여된 A(i)가 주어진다.
- 왼쪽에서 오른쪽으로 징검다리를 건너가야하며, 최대 힘 이내라면 여러 칸 씩 이동할 수 있다.
- i번째 돌에서 j(i < j)번째 돌로 이동할 때 (j - i) * (1 + |A_{i} - A_{j}|) 만큼 힘을 쓴다.
- 오른쪽 끝까지 도달할 수 있다면 YES, 아니라면 NO를 출력한다.
- 작은 문제부터 차근차근 위로 계산해야 하므로 Bottom-up DP, 반복문을 사용한다.

[Time Complexity]
- O(n²)
"""

import sys
input = sys.stdin.readline

n, k = map(int, input().split())
A = [0] + list(map(int, input().split()))   # 1-based indexing 맞추기

dp = [False] * (n + 1)
dp[1] = True

def solve(n):
    for i in range(1, n + 1):
        if dp[i]:
            for j in range(i + 1, n + 1):
                power = (j - i) * (1 + abs(A[i] - A[j]))
                if power <= k:
                    dp[j] = True
    return "YES" if dp[n] else "NO"

print(solve(n))


"""
징검다리 건너기 (small)
https://www.acmicpc.net/problem/22869

[Problem & Solvings]
- BFS를 활용해 각 징검다리에서 도달 가능한 다음 징검다리들을 탐색함
- 점프할 수 있는 범위(1~j)를 모두 시도하되, 힘이 k 이하일 경우에만 다음 징검다리로 이동
- 이미 방문한 징검다리는 재방문하지 않음으로써 불필요한 탐색을 줄임
- 마지막 징검다리에 도달할 수 있는지 여부를 visited[n-1]을 통해 판단

[Time Complexity]
- O(nj)
"""

from collections import deque
import sys
input = sys.stdin.readline

n, k = map(int, input().split())
A = list(map(int, input().split()))   # 0-based indexing 맞추기

visited = [False] * n

def bfs():
    queue = deque([0])
    visited[0] = True

    while queue:
        nq = queue.popleft()
        
        for i in range(1, j + 1):
            next_pos = nq + i
            if next_pos >= n:
                break
            power = i * (1 + abs(A[i] - A[j]))


    return "YES" if dp[n] else "NO"

print(solve(n))