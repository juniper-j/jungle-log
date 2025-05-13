import sys
import math

def is_prime(x):
    if x < 2:
        return False
    for i in range(2, int(math.sqrt(x)+1)):
        if x % i == 0:
            return False
    return True

def goldbach_partition(x):
    for j in range(x//2, 1, -1):
        y = x - j
        if is_prime(j) and is_prime(y):
            print (j, y)
            return

T = int(sys.stdin.readline())
for _ in range(T):
    n = int(sys.stdin.readline())
    goldbach_partition(n) 