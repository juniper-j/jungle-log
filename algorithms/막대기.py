"""
https://www.acmicpc.net/problem/17608
[문제 해결 전략]
1. 입력 받은 값을 정수로 stack에 쌓는다.
2. reversed() 함수를 이용해 오른쪽 끝부터 높이를 비교하며 카운트를 한다.
"""

import sys

N = int(sys.stdin.readline())
stack = []

for _ in range(N):
    stack.append(int(sys.stdin.readline()))

cnt = 0
max_height = 0

for h in reversed(stack):   # 값 자체를 사용해야 함
    if h > max_height:
        max_height = h
        cnt += 1

print(cnt)