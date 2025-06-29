"""
예제: 보글 게임
문제 ID: BOGGLE
난이도: 하
"""

# 8방향 이동: 상, 상좌, 좌, 하좌, 하, 하우, 우, 상우
dx = [-1, -1, -1, 0, 1, 1, 1, 0]
dy = [-1, 0, 1, 1, 1, 0, -1, -1]

def has_word(board, y, x, word):
    # 1. 범위 체크
    if y < 0 or y >= len(board) or x < 0 or x >= len(board[0]):
        return False
    # 2. 현재 글자 불일치
    if board[y][x] != word[0]:
        return False
    # 3. 한 글자 맞으면 성공
    if len(word) == 1:
        return True
    # 4. 8방향 재귀 탐색
    for dir in range(8):
        next_y = y + dy[dir]
        next_x = x + dx[dir]
        if has_word(board, next_y, next_x, word[1:]):
            return True
    return False

board = [
    ['N', 'N', 'N', 'N', 'S'],
    ['N', 'E', 'E', 'N', 'N'],
    ['N', 'E', 'Y', 'E', 'N'],
    ['N', 'E', 'E', 'N', 'N'],
    ['N', 'N', 'N', 'N', 'N']
]

print(has_word(board, 2, 2, "YES"))  # True
print(has_word(board, 0, 0, "YES"))  # False