"""
평범한 배낭
https://www.acmicpc.net/problem/12865

[Problem & Solvings]
- n, k <- 여행에 필요한 물건, 배낭이 수용할 수 있는 최대 무게
- w, v <- 물건 무게, 가치
- 전형적인 0-1 Knapsack 문제 -> 각 물건은 1번만 넣을 수 있고, 최대 가치를 구해야 함
- 무게 합이 k를 넘지 않으면서, 가치의 합이 최대가 되도록 물건을 선택
- dp[i][w]: i번째 물건까지 고려했을 대, 배낭의 무게가 w일 때의 최대 가치
- 시간 제한 2초, N <= 100, K <= 100,000 -> O(NK) 풀이 필요

[Pseudocode]
1. 입력 처리
    n, k = 물건 수, 배낭 최대 무게
    items = [(W1, V1), (W2, V2), ..., (Wn, Vn)]
2. DP 테이블 초기화
    dp = [0] * (k + 1)
3. 각 물건에 대해 DP 갱신
    for w, v in items:
        for weight in range(k, w - 1, -1):  # 역순으로 해야 중복 사용 방지
            dp[weight] = max(dp[weight], dp[weight - w] + v)
4. 결과 출력
    print(dp[k])

[Time Complexity]
- DP 테이블 크기: O(K)
- 외부 반복(N회): 각 물건마다
- 내부 반복(K회): 최대 무게 K부터 역순 반복
- 총 O(N * K) -> 1,000만 회 이하의 연산으로 제한 시간 내 충분히 통과 가능
"""

# 1. 입력 처리
import sys
input = sys.stdin.readline
n, k = map(int, input().split())
items = [tuple(map(int, input().split())) for _ in range(n)]

# 2. DP 테이블 초기화
dp = [0] * (k + 1)

# 3. DP 점화식 적용
for weight, value in items:     # 현재 넣으는 물건의 무게, 가치
    for w in range(k, weight - 1, -1):  # 뒤에서부터 줄여가며 순회 (0-1 Knapsack)
        dp[w] = max(dp[w], dp[w - weight] + value)

# 4. 결과 출력
print(dp[k])