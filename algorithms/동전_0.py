"""
동전 0
https://www.acmicpc.net/problem/11047

[Problem & Solvings]
- n, k <- 준규가 가진 동전 종류 수, 타겟 가치 합
- n개의 동전의 가치 a가 오름차순으로 주어짐 (1 <= a <= 1,000,000)
- k원을 만드는데 필요한 동전 개수 최소값 출력

[Pseudocode]
1. 입력 처리
    n, k ← 정수 입력
    coins ← n개의 정수 입력 (오름차순)
2. 변수 초기화
    count ← 0
3. 큰 동전부터 반복
    for coin in reversed(coins):
        if k == 0:
            break
        count += k // coin
        k %= coin
4. 출력
    print(count)

[Time Complexity]
시간 복잡도: O(N) → 동전의 수만큼 한 번씩 반복
공간 복잡도: O(N) → 동전 리스트 저장용
"""

n, k = map(int, input().split())
coins = [int(input()) for _ in range(n)]

count = 0
for coin in reversed(coins):    # 큰 동전부터 사용
    if k == 0:
        break
    count += k // coin          # 해당 동전으로 만들 수 있는 최대 개수
    k %= coin                   # 남은 금액 갱신

print(count)