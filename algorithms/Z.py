def z_order(n, r, c):
    if n == 0:
        return 0  # 1x1 크기일 때 첫 번째 방문
    
    size = 2 ** (n - 1)  # 현재 배열의 한 변 크기의 절반
    if r < size and c < size:  # 1사분면
        return z_order(n - 1, r, c)
    elif r < size and c >= size:  # 2사분면
        return size * size + z_order(n - 1, r, c - size)
    elif r >= size and c < size:  # 3사분면
        return 2 * size * size + z_order(n - 1, r - size, c)
    else:  # 4사분면
        return 3 * size * size + z_order(n - 1, r - size, c - size)

# 입력 받기
N, r, c = map(int, input().split())

# 결과 출력
print(z_order(N, r, c))