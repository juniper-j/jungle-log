"""
카드 정렬하기
https://www.acmicpc.net/problem/1715
[문제]
- 두 묶음의 카드를 합치려면 각 묶음의 개수의 합 만큼 비교
- n개의 카드 묶음이 하나로 합쳐질 때까지 반복
- 최소로 비교해 합칠 수 있는 횟수 출력

[해결 전략]
1. a, b, c, d -> (a+b) + (a+b) + c + (a+b+c) + d = 3a+3b+2c+d
    즉, 가장 작은 수를 우선으로 합산해야 함
2. 가장 작은 수 2개를 뽑아 합산하고, 다시 스택에 쌓음
3. 스택에 하나만 남을 때까지 반복

[Pseudocode]
입력: 카드 묶음의 개수 N
Input: Integer N (number of card bundles)
입력: 각 카드 묶음의 크기들
Input: List of N integers representing the size of each card bundle

초기화: 최소 힙 heap ← 모든 카드 묶음을 삽입
Initialize a min-heap 'heap' with all card bundles
초기화: total_cost ← 0
Initialize total_cost ← 0

while heap에 카드 묶음이 2개 이상 남아있을 때:
    가장 작은 카드 묶음 A 꺼냄
    두 번째로 작은 카드 묶음 B 꺼냄
    cost ← A + B
    total_cost ← total_cost + cost
    합친 카드 묶음 cost를 다시 heap에 넣음
while size of heap > 1:
    A ← extract smallest element from heap
    B ← extract next smallest element from heap
    cost ← A + B
    total_cost ← total_cost + cost
    insert cost back into heap

출력: total_cost
Output total_cost
"""

import heapq
import sys

input = sys.stdin.readline
n = int(input())
heap = []
total = 0

for _ in range(n):
    heapq.heappush(heap, int(input()))

while len(heap) > 1:
    a = heapq.heappop(heap)
    b = heapq.heappop(heap)
    total += a+b
    heapq.heappush(heap, a+b)

print(total)