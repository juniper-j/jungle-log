N = int(input())
for i in range(1, 10):
    print(N, '*', i, '=', N * i)
    # print(f"{N} * {i} = {N*i}") # f-string 방식, 성능 더 좋음, {} 안에서 연산/함수 호출 가능