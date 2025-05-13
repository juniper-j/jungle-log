"""
회의실 배정
https://www.acmicpc.net/problem/1931

[Problem & Solvings]
- n <- 회의의 수 (1 <= n <= 100,000)
- 배열에 집어넣고, 끝나는 시간 오름차순으로 정렬해, 지금 선택 가능한 것 중 가장 일찍 끝나는 회의를 고름
- 끝나는 시간이 같다면, 시작 시간을 기준으로 오름차순 정렬
- 현재 고려중인 회의가 이전에 선택된 회의의 끝나는 시간 이후에 시작하면 해당 회의를 선택
- 선택된 회의의 끝나는 시간을 갱신하고, 카운트를 증가

[Time Complexity]
시간 복잡도 O(n log n) + 공간 복잡도 O(n)
"""

import sys
input = sys.stdin.read
data = input().splitlines()

n = int(data[0])
meetings = [tuple(map(int, line.split())) for line in data[1:]]
meetings.sort(key=lambda x: (x[1], x[0]))

count = 0
end_time = 0

for start, end in meetings:
    if start >= end_time:
        count += 1
        end_time = end

print(count)