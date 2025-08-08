"""
귀여운 라이언
https://www.acmicpc.net/problem/15565

[Problems]
- 주어진 조건
    - N(1 ≤ N ≤ 1,000,000): 인형의 개수
    - K(1 ≤ K ≤ N): 포함되어야 하는 라이언 인형의 최소 개수
    - N개의 숫자 배열 (1 = 라이언, 2 = 어피치)
- 목표
    - 라이언 인형이 K개 이상 포함된 연속된 구간 중 가장 짧은 구간의 길이를 구하라
    - 존재하지 않으면 -1 출력

[Solvings]
- 

[Pseudocode]
READ N, K
READ dolls[0..N-1]

lions = []
FOR i FROM 0 TO N-1:
    IF dolls[i] == 1:
        lions.APPEND(i)

IF len(lions) < K:
    PRINT -1
    EXIT

[Time Complexity]
- 인덱스 수집: O(N)
- 구간 길이 계산: O(L) (L = 라이언 개수 ≤ N)
- 총합: O(N)
- 공간 복잡도: O(L) (라이언 인덱스 저장)

"""

import sys
input = sys.stdin.readline

N, K = map(int, input().split())
kakao_friends = list(map(int, input().split()))

#1. 라이언 위치 저장
lions = [i for i, friend in enumerate(kakao_friends) if friend == 1]

#2. 라이언이 K개 미만이면 불가능
if len(lions) < K :
    print (-1)
    exit()

min_len = sys.maxsize 

#3. K개 씩 묶여서 최소 길이 구하기
for i in range(len(lions) - K + 1):
    start = lions[i]
    end = lions[i + K - 1]
    min_len = min(min_len, end - start + 1)

print(min_len)