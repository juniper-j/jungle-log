"""
가운데를 말해요
https://www.acmicpc.net/problem/1655
[문제]
- 시간제한 0.1초(python 0.6초), 메모리제한 128mb
- 숫자 입력하면 중앙값 출력

[해결 전략]
1. 만약 leftHeap과 rightHeap의 길이가 같다면 중간값의 기준이 되는 leftHeap에 수를 넣는다.
    길이가 같지 않다면 rightHeap에 수를 넣어 길이를 맞춰준다.
2. 만약 leftHeap의 루트가 rightHeap의 루트보다 크면, leftHeap의 루트와 rightHeap의 루트를 바꾼다.

[Pseudocode]
input: integer n
        Then n lines, each with an integer
initialize: leftHeap as an empty max-heap (store negative values)
            rightHeap as an empty min-heap
For i from 1 to n:
    // Decide where to insert based on heap sizes
    If sizes of leftHeap and rightHeap are equal:
        Push -num into leftHeap  // add to max-heap
    Else:
        Push num into rightHeap  // add to min-heap
    // Ensure ordering: leftHeap (max) ≤ rightHeap (min)
    If rightHeap is not empty AND rightHeap[0] < -leftHeap[0]:
        Pop top elements from both heaps
        Swap them by pushing -rightValue into leftHeap,
        and -leftValue into rightHeap
    // Print median
    Output: -leftHeap[0]  // current median
"""  

import heapq
import sys
input = sys.stdin.readline

n = int(input())
leftHeap = []
rightHeap = []

for _ in range(n):
    num = int(input())

    # Decide where to insert based on heap sizes
    if len(leftHeap) == len(rightHeap):
        heapq.heappush(leftHeap, -num)
    else:
        heapq.heappush(rightHeap, num)
    
    # Ensure ordering: leftHeap (max) ≤ rightHeap (min)
    if rightHeap and rightHeap[0] < -leftHeap[0]:
        leftValue = heapq.heappop(leftHeap)
        rightValue = heapq.heappop(rightHeap)
        heapq.heappush(leftHeap, -rightValue)
        heapq.heappush(rightHeap, -leftValue)
    
    print(-leftHeap[0])