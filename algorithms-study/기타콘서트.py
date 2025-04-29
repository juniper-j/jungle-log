"""
기타콘서트
https://www.acmicpc.net/problem/1497

[Problem & Solvings]
- n(<=10)은 기타 개수, m(<=50)은 곡의 개수
- 연주 가능은 Y, 불가는 N으로 표시됨
- 최대한 많은 곡을 연주하기 위한 기타의 최소 개수 구하라.

[Pseudocode]
1. 입력: n, m
        n줄 동안 m개의 Y/N
2. 비트마스킹은 체크 용도로만 써야함 -> 1, 0 m개

[Time Complexity]
"""

import sys
input = sys.stdin.readline

n, m = map(int, input().split())

playlist = [list(map(int, input().split()[1])) for _ in range(n)]

min_guitar = float('INF')

for i in range(1, 1 << n):
    for j in range(n):