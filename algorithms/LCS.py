"""
LCS
https://www.acmicpc.net/problem/9251

[Time Complexity]
dp[i][j] = A의 i번째 문자까지, B의 j번째 문자까지 고려했을 때의 LCS 길이
-> 전체 테이블 크기: (len(A) + 1) * (len(B) + 1)
-> 각 칸을 채우는 데 걸리는 시간은 O(1)
-> 시간 복잡도는 O(n * m)
-> 최대 1,000 * 1,000 = 1,000,000 연산이므로 충분히 통과 가능
"""

import sys

def lcs(s1, s2):
    n, m = len(s1), len(s2)
    # (n+1)*(m+1) 크기의 DP 테이블 생성 (0으로 초기화)
    dp = [[0] * (m + 1) for _ in range(n + 1)]
	
    # DP 테이블 채우기
    for i in range(1, n + 1):			# s1의 문자 순회
        for j in range(1, m + 1):		# s2의 문자 순회
            if s1[i - 1] == s2[j - 1]:
            	# 문자가 같으면 대각선에서 +1
                dp[i][j] = dp[i - 1][j - 1] + 1
            else:
            	# 문자가 다르면 왼쪽 또는 위쪽 중 큰 값 선택
                dp[i][j] = max(dp[i - 1][j], dp[i][j - 1])

    return dp[n][m]		# LCS 길이 반환

s1 = sys.stdin.readline().strip()
s2 = sys.stdin.readline().strip()

print(lcs(s1, s2))