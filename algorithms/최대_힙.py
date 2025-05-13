"""
최대 힙
https://www.acmicpc.net/problem/11279
[문제]
- 입력받은 N 만큼 연산. N개 줄 만큼 x(<2^31)를 입력 받음
- x가 자연수라면 배열에 포함, 0이라면 배열 내 최대수 출력 및 배열에서 제거
    단, 배열이 비어있을 때 0이 입려되면 0 출력
- 입력된 0의 횟수만큼 최대수 출력되며, N만큼 연산이 끝나면 종료

[우선순위 큐 개념]
- 우선순위가 높은 요소가 먼저 나오는 큐로서, 선입선출 방식이 아닌 값의 크기나 조건에 따라 먼저 나가는 순서가 정해진다.
- 파이썬에는 기본적으로 heapq 모듈이 있고, 이 모듈은 min-heap 이다.
    e.g. heapq.heaqppush() / heappop() -> 시간복잡도 O(log n)
- 최대 힙처럼 쓰려면 값을 음수로 저장해서 큰 값이 우선 나오도록 해야한다.

[해결 전략]
1. 최대 힙을 구현하기 위해 Python의 heapq 모듈을 사용
2. heapq를 최대 힙처럼 (-x) 사용해, push/pop 연산으로 우선순위 큐 구현
"""

from collections import deque
import heapq
import sys

input = sys.stdin.readline
n = int(input())
heap = []

for _ in range(n):
    x = int(input())
    if x == 0:
        if heap:
            print(-heapq.heappop(heap))  # 가장 큰 값 출력 (음수로 저장했으니 - 붙임)
        else:
            print(0)
    else:
        heapq.heappush(heap, -x)  # 최대 힙처럼 사용하기 위해 -x 삽입