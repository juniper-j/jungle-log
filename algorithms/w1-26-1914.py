import sys

# n : 이동할 원판 개수
# start : 현재 원판이 있는 기둥
# end : 원판을 옮길 목표 기둥
# aux : 보조 기둥
# moves : 이동 과정을 저장할 리스트

def hanoi(n, start, end, aux, moves):
    """하노이의 탑 이동 과정 저장"""
    if n == 1:
        moves.append((start, end))
        return
    hanoi(n - 1, start, aux, end, moves)  # 1단계: N-1개를 보조 기둥으로 이동
    moves.append((start, end))            # 2단계: 가장 큰 원판을 목적지로 이동
    hanoi(n - 1, aux, end, start, moves)  # 3단계: 보조 기둥에 있던 N-1개를 목적지로 이동

# 입력 처리
N = int(sys.stdin.readline().strip())

# 이동 횟수 계산
K = 2**N - 1
print(K)

# N이 20 이하일 때만 이동 과정 출력
if N <= 20:
    moves = []
    hanoi(N, 1, 3, 2, moves)  # (N개 원판, 시작 기둥, 목표 기둥, 보조 기둥)
    for move in moves:
        print(*move)  # 이동 과정 출력