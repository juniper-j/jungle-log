"""
동전
https://www.acmicpc.net/problem/9084

[Problem & Solvings]
- t, n <- 테스트 케이스, 동전의 개수
- 각 테스트 케이스마다 해당 금액을 만들 수 있는 경우의 수 출력
- 중복을 허용하며 동전을 조합하는 방식 -> DP의 1차원 배열 방식으로 해결 가능
- 경우의 수를 세는 문제이므로 DFS나 완전탐색은 비효율적 (시간 초과)
- 금액 0부터 m까지 dp[i] = 금액 i를 만드는 경우의 수로 정의
- bottom-up 방식의 1차원 DP 배열 사용
- 중복 조합 조건이므로 동전을 하나씩 고정한 상태에서 반복

[Pseudocode]
T ← 테스트 케이스 수
for _ in range(T):
    N ← 동전의 가지 수
    coins ← 동전 금액 리스트
    M ← 만들어야 할 금액

    dp ← [0] * (M + 1)
    dp[0] ← 1  # 금액 0을 만드는 방법은 아무 동전도 쓰지 않는 1가지

    for coin in coins:
        for i in range(coin, M + 1):
            dp[i] += dp[i - coin]

    print(dp[M])

[Time Complexity]
- 각 테스트 케이스마다 coin 개수 N에 대해 M까지 탐색 → O(N * M)
- 전체 테스트 케이스 수 T가 최대 100일 경우, 시간 복잡도는 O(T * N * M) 수준 → M이 1만 이하일 경우 충분히 통과 가능
"""

import sys
input = sys.stdin.readline

t = int(input())

for _ in range(t):
    n = int(input())    # 동전의 종류 수
    coins = list(map(int, input().split()))     # 각 동전의 금액
    m = int(input())    # 목표 금액

    dp = [0] * (m + 1)  # dp[i] = i원을 만드는 경우의 수
    dp[0] = 1  # 0원을 만드는 방법은 아무 동전도 쓰지 않는 1가지

    # 각 동전마다 경우의 수 누적
    for coin in coins:
        # 현재 coin을 사용해서 만들 수 있는 금액 i에 대해
        for i in range(coin, m + 1):
            # i - coin원을 만드는 방법에서 coin을 추가한 경우를 더해줌
            dp[i] += dp[i - coin]

    print(dp[m])