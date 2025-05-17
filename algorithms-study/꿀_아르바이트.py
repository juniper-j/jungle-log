"""
꿀 아르바이트
https://www.acmicpc.net/problem/12847

[Problem & Solvings]
- 편의점 일급은 매일 다르며, 일을 시작하면 매일 끊이지 않고 해야 함
- 연속된 n일의 일급을 알고 있고, 이 중 최대 m일 연속 근무할 수 있기 때문에
- 최대 효율을 내도록 근무 시작일을 정해야 함
- 누적합, 슬라이딩 윈도우 방식으로 풀어야 함

[Time Complexity]
- 처음 m개 항 더하기 O(m) + 슬라이딩 윈도우 O(n - m) = O(n)
"""

import sys
input = sys.stdin.readline

n, m = map(int, input().split())
salary = list(map(int, input().split()))
# salary.sort(reverse=True) -> 연속된 m일 동안 일해야 하기에 큰 순으로 정렬하면 안됨

# 초기 구간 합
window_sum = sum(salary[:m])
max_sum = window_sum

# 슬라이딩 윈도우
for i in range(m, n):
    window_sum = window_sum + salary[i] - salary[i - m]
    max_sum = max(max_sum, window_sum)

print(max_sum)