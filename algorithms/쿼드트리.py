"""
[문제 해결 전략]
1. 현재 영역에 대해 모든 값이 동일한지 확인한다.
2. 모든 값이 동일하다면 해당 값을 반환한다. 그렇지 않다면 4개 영역으로 분할하여 각각 압축을 수행한다.
3. 네 개의 하위 영역을 압축한 결과를 합쳐서 "(A B C D)" 형태로 출력한다.
"""

def compress_quad_tree(x, y, size):
    """
    쿼드트리 방식으로 주어진 영상 압축
    x, y: 현재 탐색하는 영역의 왼쪽 위 좌표
    size: 현재 탐색하는 영역의 크기
    """
    first_value = video[x][y]  # 첫 번째 값을 기준으로 영역 검사
    all_same = True  # 모든 값이 동일한지 여부

    for i in range(x, x + size):
        for j in range(y, y + size):
            if video[i][j] != first_value:
                all_same = False
                break
        if not all_same:
            break

    if all_same:
        return first_value  # 영역이 모두 같다면 해당 값 반환

    # 4개 영역으로 분할하여 압축 수행
    half = size // 2
    top_left = compress_quad_tree(x, y, half)  # 좌상단
    top_right = compress_quad_tree(x, y + half, half)  # 우상단
    bottom_left = compress_quad_tree(x + half, y, half)  # 좌하단
    bottom_right = compress_quad_tree(x + half, y + half, half)  # 우하단

    return f"({top_left}{top_right}{bottom_left}{bottom_right})"  # 결과 조합

# 입력 처리
N = int(input())  # 영상 크기 (N x N)
video = [input().strip() for _ in range(N)]  # 2D 리스트 형태의 영상 데이터

# 결과 출력
print(compress_quad_tree(0, 0, N))



####################################################################################################



import sys

# 입력 속도 최적화: sys.stdin.read()를 사용하여 한 번에 입력을 처리
input = sys.stdin.read
data = input().split()  # 공백과 개행 문자를 기준으로 전체 입력을 리스트로 변환

# 첫 번째 값은 N (영상 크기)
N = int(data[0])

# 이후 값들은 영상의 각 행을 나타냄 (2차원 리스트로 저장)
image = data[1:]

def quad_tree(n, x, y):
    """
    쿼드 트리 압축을 수행하는 재귀 함수
    
    n: 현재 탐색 중인 영역의 크기
    y, x: 현재 탐색 중인 영역의 왼쪽 위 좌표
    """
    first_value = image[x][y]  # 기준이 되는 첫 번째 값을 저장

    # 현재 영역(n x n)이 같은 값(0 또는 1)으로만 이루어져 있는지 확인
    for i in range(x, x + n):   # 행 반복
        for j in range(y, y + n):   # 열 반복
            if image[i][j] != first_value:  # 다른 값이 존재하면 분할 필요
                half = n // 2  # 현재 영역을 4등분

                # 4개의 부분을 재귀적으로 압축하고, 결과를 괄호로 감싸서 반환
                return (
                    "("  # 압축된 영역의 시작
                    + quad_tree(half, x, y)  # 좌상단 영역
                    + quad_tree(half, x, y + half)  # 우상단 영역
                    + quad_tree(half, x + half, y)  # 좌하단 영역
                    + quad_tree(half, x + half, y + half)  # 우하단 영역
                    + ")"  # 압축된 영역의 끝
                )

    # 모든 값이 동일한 경우, 해당 숫자 그대로 반환
    return first_value

# 결과 출력 (쿼드 트리 압축 결과)
print(quad_tree(N, 0, 0))