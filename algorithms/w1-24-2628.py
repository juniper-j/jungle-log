# 직사각형의 가로, 세로를 입력 받음
w, h = map(int, input().split())
width = [0, w]
height = [ 0, h]

# 몇 회 자를지와 입력 받은 숫자를 a, b에 매핑
# 어느 방향으로 자르냐에 따라 너비, 높이에 값 저장
N = int(input())
for _ in range(N):
    a, b = map(int, input().split())
    if a == 0:
        height.append(b)
    elif a == 1:
        width.append(b)

# 너비, 높이 값 순서대로 정렬해 리스트 저장
width.sort()
height.sort()

# 너비, 높이에 따라 넓이 구하고, 최대 넓이 출력
result = 0
for i in range(len(width)-1):
    x = width[i+1] - width[i]
    for j in range(len(height)-1):
        y = height[j+1] - height[j]
        result = max(result, x*y)

print(result)