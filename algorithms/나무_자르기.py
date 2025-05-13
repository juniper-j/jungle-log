"""
나무 자르기
https://www.acmicpc.net/problem/2805
[문제]
- 길이 m 만큼의 나무가 필요하다. 
- n개의 나무를 h 높이의 절단기로 잘라, m을 만든다.
    단, h보다 낮은 나무는 잘리지 않는다.
- h의 최댓값을 구하라.

[해결 전략]
1. 입력값 나무의 수 n, 필요한 나무의 길이 m
2. 입력값 나무의 높이 trees 리스트에 저장
3. start, end=max, result 초기화
4. start <= end 동안 반복:
    mid = (start + end) // 2  ← 현재 실험해볼 절단 높이
    sum_cut 초기화
    for문을 돌며 잘린 나무 길이 더하기
    잘린 나무 합이 목표보다 크다면, result에 값 저장하고 mid 높이 올림
    잘린 나무 합이 목표보다 작다면, result에 값 저장하고 mid 높이 내림
5. 반복이 끝나면 result 출력 (조건을 만족한 가장 높은 절단 높이)
"""

import sys
input = sys.stdin.readline

n, m = map(int, input().split())
trees = list(map(int, input().split()))
start = 0
end = max(trees)
result = 0

while start <= end:
    mid = (start + end) // 2
    sum_cut = 0
    
    for tree in trees:
        if tree > mid:
            sum_cut += tree - mid
    
    if sum_cut >= m:
        result = mid
        start = mid + 1
    else:
        end = mid - 1

print(result)