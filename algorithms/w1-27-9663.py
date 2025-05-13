def solve_n_queens(row, N, columns, diag1, diag2, count):
    """row번째 행에 퀸을 배치하고 가능한 경우의 수를 찾음 (최적화된 백트래킹)"""
    if row == N:  # 모든 퀸을 배치 완료한 경우
        count[0] += 1
        return
    
    for col in range(N):
        if not columns[col] and not diag1[row - col + N - 1] and not diag2[row + col]:  
            # 현재 위치에 퀸을 놓을 수 있다면
            columns[col] = diag1[row - col + N - 1] = diag2[row + col] = True  # 퀸 배치
            solve_n_queens(row + 1, N, columns, diag1, diag2, count)  # 다음 행으로 이동
            columns[col] = diag1[row - col + N - 1] = diag2[row + col] = False  # 백트래킹 (되돌아가기)


# 입력 및 실행
N = int(input())  # 체스판 크기 입력
count = [0]  # 가능한 경우의 수 저장 (리스트 사용하여 참조 가능)

# 유효성 검사용 리스트 (O(1) 조회 가능)
columns = [False] * N  # 각 열(col)에 퀸이 있는지 체크
diag1 = [False] * (2 * N - 1)  # `/` 방향 대각선 체크
diag2 = [False] * (2 * N - 1)  # `\` 방향 대각선 체크

solve_n_queens(0, N, columns, diag1, diag2, count)  # row=0부터 시작
print(count[0])  # 가능한 N-Queen 배치 경우의 수 출력