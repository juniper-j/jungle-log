"""
카드 2
https://www.acmicpc.net/problem/2164
[문제 및 해결 전략]
- N장의 카드가 번호 순서대로 놓임
- 홀수 회차 때는 첫 장을 popleft()
- 짝수 회차 때는 첫 장을 append(popleft())
- 마지막 1장이 남을 때까지 반복하고 해당 카드의 번호를 출력
"""

from collections import deque
import sys
input = sys.stdin.readline

n = int(input())
queue = deque(range(1, n+1))

while len(queue) > 1:
    queue.popleft()
    queue.append(queue.popleft())

print(queue[0])