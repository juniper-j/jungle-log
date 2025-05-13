# 두 개의 숫자를 입력받기
A, B = input().split()

# 숫자를 거꾸로 변환
A_reversed = int(A[::-1])
B_reversed = int(B[::-1])

# 더 큰 수 출력
print(max(A_reversed, B_reversed))