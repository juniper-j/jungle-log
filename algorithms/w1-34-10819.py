# 주어진 배열의 모든 순열을 생성
# 각 순열에 대해 |A[0] - A[1]| + |A[1] - A[2]| + … + |A[N-2] - A[N-1]| 값을 계산
# 계산한 값 중 최댓값 출력

from itertools import permutations  # 순열을 생성하기 위해 itertools의 permutations 사용

def calculate_max_difference(N, A):
    max_value = 0  # 최댓값을 저장할 변수 초기화

    # 모든 가능한 순열을 생성하여 탐색 (완전 탐색 방식)
    for perm in permutations(A):  
        # |A[0] - A[1]| + |A[1] - A[2]| + ... + |A[N-2] - A[N-1]| 계산
        total = sum(abs(perm[i] - perm[i+1]) for i in range(N-1))
        
        # 현재까지의 최댓값과 비교하여 갱신
        max_value = max(max_value, total)

    return max_value  # 최댓값 반환

# 입력 처리
N = int(input())  # 첫 번째 줄에서 정수 N 입력받기 (3 ≤ N ≤ 8)
A = list(map(int, input().split()))  # 두 번째 줄에서 배열 A 입력받기

# 결과 출력
print(calculate_max_difference(N, A))  # 최댓값을 계산하여 출력