"""
색종이 만들기
https://www.acmicpc.net/problem/2630
[문제]
- N×N 크기의 색종이가 있고, 각 칸은 하얀색(0) 또는 파란색(1)이다.
- 색종이를 적절히 잘라서 각 조각이 모두 같은 색이 되도록 만들고,
  하얀색 색종이 조각의 수와 파란색 색종이 조각의 수를 출력하라.
- 자를 때는 항상 정사각형을 4등분(같은 크기)으로 나눈다.
- 입력: N (1 ≤ N ≤ 128, N은 2의 거듭제곱), 이후 N줄에 걸쳐 0 또는 1로 된 배열
- 출력: 하얀색 조각 개수, 파란색 조각 개수

[해결 방법]
1. 분할 정복(Divide and Conquer)을 이용해 전체 종이를 재귀적으로 4등분한다.
2. 현재 영역이 모두 같은 색인지 확인한다.
   - 같다면, 해당 색에 따라 카운트를 증가시킨다.
   - 다르다면, 4개의 정사각형으로 나눠 각각 재귀적으로 검사한다.
3. 영역은 (x, y) 좌표와 정사각형의 한 변 길이 n을 기준으로 처리한다.
4. 전역 변수 또는 외부 리스트를 활용해 하얀색과 파란색 개수를 누적한다.
"""

import sys
input = sys.stdin.readline
n = int(input())
paper = [list(map(int, input().split())) for _ in range(n)] 
white, blue = 0, 0

def detector(n, x, y):
  global white, blue
  color = paper[y][x]
  for i in range(y, y+n):
    for j in range(x, x+n):
      if color != paper[i][j]:
        detector(n//2, x, y)
        detector(n//2, x+n//2, y)
        detector(n//2, x, y+n//2)
        detector(n//2, x+n//2, y+n//2)
        return
  if color == 0:
    white += 1
  else:
    blue += 1

detector(n, 0, 0)
print(white)
print(blue)