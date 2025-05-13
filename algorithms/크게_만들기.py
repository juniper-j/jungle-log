"""
https://www.acmicpc.net/problem/2812
[문제]
- N자리 수에서 숫자(문자) K개를 지워서 만들 수 있는 가장 큰 수 출력 (1 ≤ K < N ≤ 500,000)
- 단, 출력할 숫자의 순서는 유지

[해결 전략 w/ GPT]
1. 입력받은 숫자 앞자리부터 stack에 넣고 비교
2. 큰 수를 만들려면 앞자리가 커야하므로, 앞자리부터 보면서 자신보다 작은 숫자를 스택에서 제거 & 카운트 다운
    더 큰 숫자가 들어오려고 하면 앞 숫자를 제거 -> 현재 숫자가 크면, 스택 top이 더 작을 때 pop()
3. 카운트 0 되면 종료하되, 아직 제거 못했다면 뒤에서 제거
** Stack & Greedy(탐욕법: 가장 좋아보이는 선택을 하는 방식) 활용 **
"""

import sys
input = sys.stdin.readline

N, K = map(int, input().split())
num = input()
stack = []
cnt = K

for digit in num:
    while stack and cnt > 0 and stack[-1] < digit:
        stack.pop()
        cnt -= 1
    stack.append(digit)

# 아직 제거 못한 숫자가 있다면 뒤에서 제거
result = stack[:N - K]
print(''.join(result))