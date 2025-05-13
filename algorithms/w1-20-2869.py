# 1st trial
# A, B, V = map(int, input().split())
# height = 0
# days = 0

# while True:
#     days += 1
#     height += A

#     if height >= V:
#         break

#     height -= B

# print(days)


# 2nd trial
# A, B, V = map(int, input().split())
# # A와 B가 같은 경우 → 단 하루에 끝남
# if A >= V:
#     print(1)
# else:
#     days = math.ceil((V - A) / (A - B)) + 1
#     print(days)


# 3rd trial
# 입력 받기
A, B, V = map(int, input().split())

# (V - A)를 (A - B)로 나눈 후, 올림하는 방법을 직접 구현
if (V - A) % (A - B) == 0:
    days = (V - A) // (A - B) + 1
else:
    days = (V - A) // (A - B) + 2

# 결과 출력
print(days)