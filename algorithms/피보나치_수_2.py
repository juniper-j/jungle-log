"""
피보나치 수 2
https://www.acmicpc.net/problem/2748

[Problem & Solvings]
1. 탑다운 > 수동 메모이제이션
    메모리 32412KB, 시간 36ms
2. 탑다운 > 자동 메모이제이션
    메모리 35948KB, 시간 68ms -> 비효율?
3. 바텀업 > 반복문
    메모리 32412KB, 시간 36ms
"""

# 1. 탑다운 > 수동 메모이제이션
def fib(n, memo={}):
    if n in memo:			# 이미 계산한 값이 있다면 그대로 반환
        return memo[n]
    if n <= 1:				# 기본값: fib(0) = 0, fib(1) = 1
        return n
    # 아직 계산되지 않은 경우 -> 재귀 호출 후 저장
    memo[n] = fib(n - 1, memo) + fib(n - 2, memo)	# 계산 후 저장
    return memo[n]

n = int(input())
print(fib(n, memo={}))



# 2. 탑다운 > 자동 메모이제이션
# @lru_cache를 활용한 자동 메모이제이션
from functools import lru_cache

@lru_cache(maxsize=None)  # 모든 결과를 제한 없이 저장
def fib(n):
    if n <= 1:
        return n
    return fib(n - 1) + fib(n - 2)

n = int(input())
print(fib(n))



# 3. 바텀업 > 반복문
def fib(n):
    dp = [0] * (n + 1)
    dp[1] = 1
    for i in range(2, n + 1):
        dp[i] = dp[i - 1] + dp[i - 2]
    return dp[n]

n = int(input())
print(fib(n))