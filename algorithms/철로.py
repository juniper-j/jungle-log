"""
철로
https://www.acmicpc.net/problem/13334
[문제]
- n명이 존재하고, (h(i), o(i))는 각각 i의 (집, 사무실)의 위치 (1 <= i <= n)
- 길이가 d인 철로 L에 대해, 집-사무실이 모두 L에 포함되는 사람의 최대수를 구하라

[해결 전략]
1. 입력값 n, 사람들의 집과 사무실 위치 (h, o) 저장
2. 각 사람마다 (start, end) = (min(h, o), max(h, o)) 형태로 변환
3. 사람 목록을 end 오름차순, end가 같으면 start 오름차순으로 정렬
4. max_cnt = 0
5. heap = 빈 최소 힙
6. for 사람 in 정렬된 목록:
    if end - start > d:
        continue    // 철로로 커버 불가능한 거리이므로 패스
    heap에 start 추가
    while heap[0] < end - d:
        heap에서 start 제거 (범위 밖인 사람 제거)
    max_cnt = max(max_cnt, heap 크기)
7. max_cnt 출력
"""

import heapq
import sys
input = sys.stdin.readline

# n개의 입력값 받기
n = int(input())
people = []
for _ in range(n):
    h, o = map(int, input().split())
    # 집과 사무실 중 좌표값이 작은 것을 시작점으로, 큰 것을 끝점으로
    start, end = min(h, o), max(h, o)
    people.append((start, end))

# 철로 길이 받기
d = int(input())

# 선분의 끝점(end)을 기준으로 오름차순 정렬한 다음, 앞점(start)을 기준으로 오름차순 정렬
people.sort(key=lambda x: (x[1], x[0]))

heap = []
max_cnt = 0

for start, end in people:   # 각 사람의 좌표를 기준으로 반복문 돌기
    if end - start > d:
        continue    # 철로에 포함하지 못하므로 제외
    
    heapq.heappush(heap, start) # 최소힙. pop 했을 때 시작점이 작은 것부터 나올 수 있도록 
    
    # 현재 철로 시작점보다 더 왼쪽에 있는 사람은 제외
    while heap and heap[0] < end - d:
        heapq.heappop(heap)

    # 현재 철로에 포함되는 사람 수가 최대값이 경우 갱신
    max_cnt = max(max_cnt, len(heap))

print(max_cnt)