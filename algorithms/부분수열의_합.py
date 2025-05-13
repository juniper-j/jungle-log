"""
문제 해결 전략:
1. 주어진 수열에서 원소를 포함하거나 포함하지 않는 방식으로 모든 부분수열을 탐색한다.
2. 백트래킹(재귀)를 활용하여 부분수열의 합을 구하고, 목표 값 S와 일치하는 경우 카운트를 증가시킨다.
3. 단, 크기가 양수인 부분수열만 고려해야 한다.

입력: N (1 <= N <= 20), S ( |S| <= 1,000,000)
      N개의 정수 (-100,000 <= 각 원소 <= 100,000)
출력: 합이 S가 되는 부분수열의 개수
"""

import sys

# 입력 받기
N, S = map(int, sys.stdin.readline().split())
arr = list(map(int, sys.stdin.readline().split()))

# 부분수열의 개수 카운트 변수
global count
count = 0

def backtrack(index, total, selected):
    """
    백트래킹을 이용하여 모든 부분수열 탐색
    :param index: 현재 탐색 중인 원소의 인덱스
    :param total: 현재까지 선택한 원소들의 합
    :param selected: 선택된 원소 개수 (공집합 제외 목적)
    """
    global count
    
    # 배열 끝까지 탐색한 경우
    if index == N:
        # 크기가 양수인 부분수열 중에서 합이 S인 경우 카운트 증가
        if total == S and selected > 0:
            count += 1
        return
    
    # 현재 원소를 포함하는 경우
    backtrack(index + 1, total + arr[index], selected + 1)
    
    # 현재 원소를 포함하지 않는 경우
    backtrack(index + 1, total, selected)

# 백트래킹 시작
backtrack(0, 0, 0)

# 결과 출력
print(count)