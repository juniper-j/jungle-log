"""
곱셈
https://www.acmicpc.net/problem/1629
[문제]
a ** b % c

[필요 개념]
1. 지수 법칙
    A**(m+n) = A**m * A**n 
2. 나머지 분배 법칙 (Modulo)
    (A + B) % p = ((A % p) + (B % p)) % p
    (A * B) % p = ((A % p) * (B % p)) % p
    (A - B) % p = ((A % p) - (B % p) + p) % p
"""

# 정경호님 풀이
def func(a, b, c):
    if b == 0:
        return 1

    half = func(a, b//2, c)

    if b % 2 == 1:
        return a * half * half % c
    else:
        return half * half % c
    
a, b, c = map(int, input().split())

print(func(a, b, c))