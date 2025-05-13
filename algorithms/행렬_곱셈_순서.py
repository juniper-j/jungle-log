"""
행렬 곱셈 순서 (행렬 체인 곱셈)
https://www.acmicpc.net/problem/11049

[Problem & Solvings]
- A = N*M, B = M*K, C = K*L
- (AB)C 연산 수 = N*M*K (AB 계산) + N*K*L (D(=A*B)C 계산)
- A(BC) 연산 수 = M*K*L (BC 계산) + N*M*L (AF(=B*C) 계산)
- n <- 행렬의 개수
- r, c <- 행렬의 크기 (1 <= r, c <= 500)
- 모든 행렬을 곱하는데 필요한 곱센 연산의 최솟값 출력

[Pseudocode]
1.	입력 데이터를 읽고 각 행렬의 크기를 저장
2.	DP 배열을 초기화하고, 작은 구간부터 큰 구간까지 순차적으로 처리
3.	각 구간에서 가능한 모든 분할 기준을 탐색하며 최소 연산 횟수를 계산
4.	최종적으로 모든 행렬을 곱하는 데 필요한 최소 연산 횟수를 출력

[Time Complexity]
- DP를 위한 3중 루프를 수행하기 때문에 O(N^3).
- N <= 1000 조건에서 파이썬으로 구현 시, 내부 연산을 최대한 최적화하여 간신히 시간 제한을 통과할 수 있음
"""

import sys
input = sys.stdin.readline

# 입력 처리
n = int(input().strip())
matrices = [tuple(map(int, input().split())) for _ in range(n)]

# dp 배열 초기화
dp = [[0] * n for _ in range(n)]

# 구간 길이별 계산
for length in range(2, n+1):        # length는 곱하려는 행렬의 수
    for i in range(n - length + 1): # i는 시작점
        j = i + length - 1          # j는 끝점 (i에서 length 만큼 떨어진 거리)
        dp[i][j] = float('inf')

        # 분할 기준 탐색
        for k in range(i, j):
            cost = (dp[i][k] 
                    + dp[k+1][j] 
                    + matrices[i][0] * matrices[k][1] * matrices[j][1])     # 연산 횟수 계산

            if cost < dp[i][j]:
                dp[i][j] = cost

print(dp[0][n-1])



## 시간 줄여보려고 만든 버전... 근데 안줄었음
# import sys
# input = sys.stdin.read

# def matrix_chain_order():
#     # 입력 처리
#     data = input().splitlines()
#     n = int(data[0])
#     matrices = [tuple(map(int, data[i+1].split())) for i in range(n)]

#     # dp 배열 초기화
#     dp = [[0] * n for _ in range(n)]

#     # 구간 길이별 계산
#     for length in range(2, n+1):            # length는 곱하려는 행렬의 수
#         for i in range(n - length + 1):     # i는 시작점
#             j = i + length - 1              # j는 끝점 (i에서 length 만큼 떨어진 거리)
#             dp[i][j] = float('inf')

#             # 분할 기준 탐색
#             for k in range(i, j):
#                 cost = (dp[i][k] 
#                         + dp[k+1][j] 
#                         + matrices[i][0] * matrices[k][1] * matrices[j][1])     # 연산 횟수 계산

#                 if cost < dp[i][j]:
#                     dp[i][j] = cost

#     print(dp[0][n-1])

# matrix_chain_order()