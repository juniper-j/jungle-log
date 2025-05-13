"""
요세푸스 문제 0
https://www.acmicpc.net/problem/11866
[문제]
- 1번부터 N번까지의 사람이 동그랗게 모여있음
- 양의 정수 K가 주어지면 K번째 사람을 제거하고, N을 < > 안에 출력
- 모두 제거될 때까지 계속

[해결 전략]
1. 큐에 1부터 N까지의 숫자를 넣는다 (deque 사용)
2. 큐에서 k-1명은 pop해서 다시 뒤로 append (앞으로 돌리기)
3. k번째 사람은 pop해서 result에 추가 (제거 대상)
4. 큐가 빌 때까지 반복
5. 최종 result를 요세푸스 순열 형태로 출력
"""

from collections import deque
import sys
input = sys.stdin.readline

n, k = map(int, input().split())
queue = deque(range(1, n+1))
result = []

while queue:
    for _ in range(k-1):
        queue.append(queue.popleft())
    result.append(queue.popleft())

print("<" + ", ".join(map(str, result)) + ">")