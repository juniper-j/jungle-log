"""
수 찾기
https://www.acmicpc.net/problem/1920
[문제]
- 정수 리스트(n개)에 특정 정수(m개)가 존재한다면 1, 존재하지 않으면 0 출력

[해결 전략 - 반복]
1. 이진탐색 함수 구현
    타겟 데이터가 mid 값과 같다면 mid 리턴
    타겟 데이터가 작으면 왼쪽을 더 탐색
    타겟 데이터가 크면 오른쪽을 더 탐색
2. 입력값 n, data 리스트에 정수 n개 저장 후 sort() 
3. 입력값 m, targets 리스트에 정수 m개 저장
4. 이진탐색 반환 값이 없다면 0, 있다면 1 출력
"""

def binary_search(target, data):
    start = 0
    end = len(data) - 1

    while start <= end:
        mid = (start + end) // 2    # 중간값
        if data[mid] == target:
            return mid              # target 위치 반환
        elif data[mid] > target:    # target이 작으면 왼쪽을 더 탐색
            end = mid - 1
        else:                       # target이 크면 오른쪽을 더 탐색
            start = mid + 1

import sys
input = sys.stdin.readline

n = int(input())
data = list(map(int, input().split()))
data.sort()
m = int(input())
targets = list(map(int, input().split()))

for target in targets:
    if binary_search(target, data) is None:
        print(0)
    else:
        print(1)

"""
[해결 전략 - 재귀]
1. 이진탐색 함수 구현
    start > end인 경우 종료 조건으로 -1 리턴
    타겟 데이터가 mid 값과 같다면 mid 리턴
    타겟 데이터가 작으면 start ~ mid-1 범위로 재귀 호출
    타겟 데이터가 크면 mid+1 ~ end 범위로 재귀 호출
2. 이진탐색 함수를 간단하게 호출하기 위한 wrapping 함수 구현
    start = 0, end = len(data) - 1로 설정 후 binary_search() 호출
3. 입력값 n, data 리스트에 정수 n개 저장 후 sort() 
4. 입력값 m, targets 리스트에 정수 m개 저장
5. 이진탐색 결과가 -1이라면 0, 아니라면 1 출력
"""

def binary_search(target, start, end):
    if start > end:
        return -1
    mid = (start + end) // 2
    if data[mid] == target:
        return mid
    elif data[mid] > target:
        return binary_search(target, start, mid - 1)
    else:
        return binary_search(target, mid + 1, end)

def solution(target, data):
    return binary_search(target, 0, len(data) - 1)

import sys
input = sys.stdin.readline

n = int(input())
data = list(map(int, input().split()))
data.sort()
m = int(input())
targets = list(map(int, input().split()))

for target in targets:
    if solution(target, data) == -1:
        print(0)
    else:
        print(1)