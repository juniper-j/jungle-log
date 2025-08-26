import math

def is_prime(n):
    if n < 2:
        return False
    for i in range(2, int(math.sqrt(n)) + 1):  # √n까지만 확인
        if n % i == 0:
            return False
    return True

# 입력 처리
N = int(input())  # 숫자의 개수
nums = list(map(int, input().split()))  # 공백으로 구분된 N개의 숫자 입력

# 소수 개수 세기
prime_count = sum(1 for num in nums if is_prime(num))
print(prime_count)

print(int(math.sqrt(38)))