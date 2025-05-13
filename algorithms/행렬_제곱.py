"""
행렬 제곱
https://www.acmicpc.net/problem/10830
[문제]
- 

[해결 전략]
1. 
"""

# 입력 받기: n은 행렬 크기, b는 거듭제곱할 횟수
n, b = map(int, input().split())

# n x n 크기의 행렬 입력 받기
arr = []
for _ in range(n):
    arr.append(list(map(int, input().split())))

# 단위 행렬 (Identity Matrix) 생성 함수
def make_unit_matrix(k):
    matrix = [[0] * k for _ in range(k)]
    for i in range(k):
        for j in range(k):
            if i == j:
                matrix[i][j] = 1
    return matrix

# 행렬 곱셈 함수: arr1과 arr2를 곱한 후 결과를 반환 (모듈로 1000)
def multiple_matrix(arr1, arr2):
    result = [[0] * n for _ in range(n)]
    for i in range(n):            # 결과 행렬의 행 인덱스
        for j in range(n):        # 결과 행렬의 열 인덱스
            for k in range(n):    # 중간 인덱스 (곱셈 기준)
                result[i][j] += (arr1[i][k] * arr2[k][j]) % 1000
            result[i][j] %= 1000  # 누적 결과에 대해서도 모듈로 연산
    return result

# 행렬의 b 제곱을 분할 정복 방식으로 계산하는 함수
def func(k):
    # 지수가 0이면 단위 행렬 반환
    if k == 0:
        return make_unit_matrix(n)
    
    # 재귀적으로 절반 지수 계산 (나누기 2)
    half = [[x % 1000 for x in row] for row in func(k // 2)]
    
    # k가 홀수인 경우: A^(2m+1) = A × (A^m)^2
    if k % 2 == 1:
        return [[x % 1000 for x in row] for row in multiple_matrix(multiple_matrix(arr, half), half)]
    # k가 짝수인 경우: A^(2m) = (A^m)^2
    else:
        return [[x % 1000 for x in row] for row in multiple_matrix(half, half)]

# 계산된 행렬 출력
for row in func(b):
    print(*row)