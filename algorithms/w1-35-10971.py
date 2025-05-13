# Im Joon Hyuk

import sys

# 입력을 빠르게 받기 위한 sys.stdin.readline() 사용
def input(): 
    return sys.stdin.readline().rstrip()

# 도시 개수 입력
N = int(input())

# 여행 비용 행렬 입력 (N x N 행렬)
travel_cost = [list(map(int, input().split())) for _ in range(N)]

# 방문 여부를 체크하는 리스트
visit = [False] * N

# 최소 비용을 저장할 변수 (초기값을 큰 값으로 설정)
result = int(1e9)  # 무한대에 가까운 큰 값

def visiting(count, node, depth, start):
    """
    백트래킹을 이용하여 최소 비용을 찾는 함수

    :param count: 현재까지 이동한 비용
    :param node: 현재 방문한 도시
    :param depth: 방문한 도시 개수
    :param start: 시작 도시 (마지막에 되돌아오기 위해 필요)
    """
    global result  # 전역 변수 result 사용

    # 모든 도시를 방문한 경우 (depth == N)
    if depth == N:
        # 출발 도시로 돌아올 수 있는 경우만 고려
        if travel_cost[node][start] != 0:
            # 최소 비용 갱신
            result = min(result, count + travel_cost[node][start])
        return  # 탐색 종료

    # 모든 도시를 확인하며 방문하지 않은 도시를 탐색
    for i in range(N):
        if not visit[i] and travel_cost[node][i] != 0:  # 방문하지 않았으며, 길이 존재할 경우
            visit[i] = True  # 방문 표시
            visiting(count + travel_cost[node][i], i, depth + 1, start)  # 재귀 호출
            visit[i] = False  # 백트래킹 (원래 상태로 복구)

# 각 도시를 시작점으로 설정하여 탐색 (모든 경우 고려)
for i in range(N):
    visit[i] = True  # 시작 도시 방문 표시
    visiting(0, i, 1, i)  # (현재 비용, 현재 도시, 방문한 도시 수, 시작 도시)
    visit[i] = False  # 탐색 종료 후 초기화

# 최소 비용 출력
print(result)