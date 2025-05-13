# 함수 정의
# 단, 입력 받을 두 자연수는 1보다 크고 10,000보다 작음 # if 조건 만족하지 않으면 fail
# 두 자연수 비교해서 같거나 큰 것을 A로 두기
# 두 자연수 입력 받기

A, B = map(int, input().split())

print(A+B) # 덧셈
print(A-B) # 뺄셈
print(A*B) # 곱셈
print(A//B) # 몫
print(A%B) # 나머지