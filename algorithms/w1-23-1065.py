# 99 이하는 모두 한수
# 100 이상이면 숫자를 하나씩 끊어서 저장
# 한수 판단하는 함수
# 정수 입력 받기
# 한수 판단하면 count 하는 for문

def is_hansu(n):
    if n < 100:
        return True
    
    digits = list(map(int, str(n)))  # 숫자를 자릿수별로 나누어 리스트로 변환
    gap = digits[1] - digits[0]

    for i in range(1, len(digits)-1):
        if digits[i+1] - digits[i] != gap:
            return False

    return True


N = int(input())
count = 0


for i in range(1, N+1):
    if is_hansu(i):
        count += 1

print(count)