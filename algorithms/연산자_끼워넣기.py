"""
연산자 끼워넣기
https://www.acmicpc.net/problem/14888

[Problem & Solvings]
- 숫자 리스트 A와 연산자 개수 리스트 op가 주어짐
- A 사이에 연산자를 순열처럼 끼워 넣어 만들 수 있는 결과값의 최댓값과 최솟값을 구하는 문제
- 연산자는 덧셈(+), 뺄셈(-), 곱셈(*), 나눗셈(//) 네 가지
- 가능한 모든 연산자 조합을 시도해야 하므로 완전 탐색 (Backtracking) 사용
- 연산자 개수는 최대 10개이므로, 경우의 수는 약 10! → 브루트포스 가능

[Pseudocode]
1. 입력 처리
    n ← 숫자 개수
    nums ← 숫자 리스트
    ops ← 연산자 개수 리스트 (덧셈, 뺄셈, 곱셈, 나눗셈 순)

2. 결과 초기화
    max_result ← -∞
    min_result ← ∞

3. DFS(idx, total) 정의
    if idx == n:
        max_result, min_result 갱신
        return

    for i in range(4):
        if ops[i] > 0:
            ops[i] -= 1
            다음 값을 계산해 DFS(idx + 1, 다음 값)
            ops[i] += 1

4. DFS(1, nums[0]) 호출

5. max_result, min_result 출력

[Time Complexity]
- 연산자 조합: 최대 10! = 3,628,800 (n = 11일 때)
- 각 재귀 호출은 O(1) 연산만 포함
- 총 시간 복잡도: O(n!) — n ≤ 11이므로 충분히 처리 가능
"""

n = int(input())
nums = list(map(int, input().split()))
ops = list(map(int, input().split()))  # +, -, *, // 개수 리스트

max_result = int(-1e9)  # 정수처리 하지 않으면 min, max 결과값이 float으로 반환됨. 최종 결과값에 .0이 붙어 나오면 백준에서 오답 처리됨
min_result = int(1e9)

def dfs(idx, total):    # parameter 정의: 함수가 호출될 때 외부로부터 전달받는 값
    global max_result, min_result

    # 모든 숫자를 다 사용한 경우
    if idx == n:
        max_result = max(max_result, total)
        min_result = min(min_result, total)
        return

    # 현재 위치에서 가능한 연산자 선택
    for i in range(4):
        if ops[i] > 0:
            ops[i] -= 1 # [백트래킹] 선택

            # 연산 수행 및 DFS 재귀 호출
            if i == 0:
                dfs(idx + 1, total + nums[idx]) # 탐색
            elif i == 1:
                dfs(idx + 1, total - nums[idx])
            elif i == 2:
                dfs(idx + 1, total * nums[idx])
            else:
                # 음수 나눗셈 처리: 파이썬의 // 연산은 음수 나눗셈에서 버림이 아닌 내림을 하기 때문에 따로 처리 필요
                if total < 0:
                    dfs(idx + 1, -(-total // nums[idx]))
                else:
                    dfs(idx + 1, total // nums[idx])

            ops[i] += 1  # [백트래킹] 연산자 복원 -> 다른 조합을 탐색하기 위한 백트래킹 핵심!

# DFS 시작: 첫 번째 숫자를 기준으로 시작
dfs(1, nums[0])

print(max_result)
print(min_result)



# 문어 아저씨 풀이
def dfs(n, sm, add, sub, mul, div):
    global mn, mx
    # [0] 결과값/중간 값의 범위: int(-1e8)~int(1e8)
    if sm < int(-1e9) or int(1e9)<sm:
        return

    # [1] 종료조건(정답처리)
    if n==N:
        mn = min(mn, sm)
        mx = max(mx, sm)
        return

    # [2] 하부호출: 연산자 개수 남았을 경우만 호출 가능
    if add>0:
        dfs(n+1, sm+lst[n], add-1, sub, mul, div)
    if sub>0:
        dfs(n+1, sm-lst[n], add, sub-1, mul, div)
    if mul>0:
        dfs(n+1, sm*lst[n], add, sub, mul-1, div)
    if div>0:
        dfs(n+1, int(sm/lst[n]), add, sub, mul, div-1)

N = int(input())
lst = list(map(int, input().split()))
add, sub, mul, div = map(int, input().split())

mn, mx = int(1e9), int(-1e9)
dfs(1, lst[0], add, sub, mul, div)
print(mx, mn, sep='\n')