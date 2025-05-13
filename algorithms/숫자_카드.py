"""
숫자 카드
https://www.acmicpc.net/problem/10815
[문제]
- 상근이가 들고 있는 카드 n장
- 비교가 필요한 카드 m장
- 상근이가 들고 있다면 1, 아니라면 0 출력

[해결 전략 - 이진탐색]
1. 입력
    n, sg 
    m, targets
2. 이진탐색 함수 구현
3. targets 반복문으로 이진탐색 실행 및 출력
"""

import sys
input = sys.stdin.readline
n = int(input())
sg = list(map(int, input().split()))
m = int(input())
targets = list(map(int, input().split()))

sg.sort()

def binary_search(target, sg):
    start = 0
    end = len(sg) - 1

    while start <= end:
        mid = (start + end) // 2
        if sg[mid] == target:
            return mid
        elif sg[mid] > target:
            end = mid - 1
        else:
            start = mid + 1

print(' '.join(['1' if binary_search(target, sg) is not None else '0' for target in targets]))



"""
[해결 전략 - set]
1. 
"""



"""
[해결 전략 - hash]
1. 
"""
