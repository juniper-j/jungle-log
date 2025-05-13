"""
01타일
https://www.acmicpc.net/problem/1904

[Problem & Solvings]
- 이진수열을 만들 수 있는 수 1, 00
- N자리 이진수열의 최대수 (N은 최대 1,000,000이므로 완전 탐색 불가)
- 최대수 % 15746 출력
- 경우의 수를 빠르게 구해야 하므로, DP 점화식 세워야 함
    마지막에 '1' 붙이기 -> N-1에서 만들 수 있는 경우
    마지막에 '00' 붙이기 -> N-2에서 만들 수 있는 경우

[Pseudocode]
입력: n ← 정수 (1 ≤ n ≤ 1,000,000)
모듈러 ← 15746  # 문제 조건상 큰 수 방지

1. dp 배열 생성 (크기 n+1)
   dp[1] ← 1
   dp[2] ← 2
2. for i from 3 to n:
       dp[i] ← (dp[i-1] + dp[i-2]) % 모듈러
3. 결과 출력: dp[n]

[Time Complexity]
- 시간복잡도: O(N) - 점화식 한 번씩 계산
- 공간복잡도:
- 일반 DP 방식: O(N)
- 최적화 방식(변수 2개만 사용): O(1)
"""

# 바텀업 > 반복문
import sys
input = sys.stdin.readline

n = int(input())
MOD = 15746

if n == 1:
    print(1)
elif n == 2:
    print(2)
else:
    a, b = 1, 2
    for _ in range(3, n+1):
        a, b = b, (a+b) % MOD
    print(b)

# 탑다운 > 재귀 + 수동 메모이제이션
# 점화식: tile(n) = tile(n-1) + tile(n-2)
# sys.setrecursionlimit(10**6)으로 재귀 깊이 제한을 늘려줘도 시간 초과 남
# n이 너무 커서 재귀 호출도 너무 깊어지고 많아져서 파이썬의 재귀 처리 속도 한계에 걸림
# 메모이제이션을 해도 함수 호출 자체에 시간이 많이 듦
# 특히 파있너은 C/C++에 비해 함수 호출 오버헤드가 큼
# 실전에서는 반복문 방식(바텀업)이 압도적으로 안정적