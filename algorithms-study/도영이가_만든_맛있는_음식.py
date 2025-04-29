"""
도영이가 만든 맛있는 음식
https://www.acmicpc.net/problem/2961

[Problem & Solvings]
- N개의 재료마다 신맛 S, 쓴맛 B를 가짐
- 신맛을 곱하고, 쓴맛은 더해 |신맛 - 쓴맛|이 가장 적은 요리를 만들기
- 모든 조합을 탐색하며(브루트포스) 

[Pseudocode]
1. 입력: n, (s, b) * line
2. 
4. 
5. 

[Time Complexity]
O(n)
"""


import sys
input = sys.stdin.readline

n = int(input())

item_list = [list(map(int, input().split())) for _ in range(n)]

min_result = float('INF')

for i in range(1, 1 << n):
    sour = 1
    acerbity = 0

    for j in range(n):
        if i & (1 << j):
            sour *= item_list[j][0]
            acerbity += item_list[j][1]
    min_result = min(min_result, abs(sour - acerbity))

print(min_result)







# 배고파 문제
def is_power_of_two(n):
    return n > 0 and (n & (n - 1)) == 0

def find_xy(m):
    for x in range(61):
        pow_x = 1 << x
        if pow_x > m:
            break
        remaining = m - pow_x
        if is_power_of_two(remaining):
            y = (remaining).bit_length() - 1
            if x <= y:
                return x, y
    return None

n = int(input())
for _ in range(n):
    m = int(input())
    result = find_xy(m)
    if result:
        print(f"{result[0]} {result[1]}")
    else:
        print("No valid pair found")