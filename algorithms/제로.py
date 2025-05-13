"""
https://www.acmicpc.net/problem/10773
[문제 해결 전략]
1. K는 입력 받은 정수 개수
2. 정수가 0이면 가장 최근에 쓴 수를 pop(), 아닐 경우 append()
3. 최종적으로 적은 수의 합을 출력
"""

import sys

K = int(sys.stdin.readline())
stack = []
sum = 0

for _ in range(K):
    n = int(sys.stdin.readline())
    if n == 0:
        stack.pop()    
    else:
        stack.append(n)

for num in stack:   # 스택에 남은 모든 숫자에 대해 더함
    sum += num

print(sum)


#########################################
#########################################

# import sys
# input = sys.stdin.readline

# K = int(input())
# wallet = []
# for i in range(K):
#   num = int(input())
#   if num == 0:
#     wallet.pop()
#   else:
#     wallet.append(num)

# print(sum(wallet))


#########################################
#########################################