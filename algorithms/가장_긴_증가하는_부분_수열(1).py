"""
가장 긴 증가하는 부분 수열
https://www.acmicpc.net/problem/11053
[문제]
- n개의 정수로 이루어진 수열 A 존재
- 수가 증가하는 양상을 가진 부분 수열의 최대 길이 출력

[해결 전략]
** 탑다운, 바텀업, 이분 탐색으로 모두 풀 수 있으나 시간 복잡도 측면에서 이분 탐색이 효율적!
    재귀/반복문 = O(n^2), 이분 탐색 = O(n logn)
1. 입력값 n, n개의 정수를 저장
2. dp 리스트가 비어있거나 현재
3. 만약 더 큰 숫자가 나오면 리스트에 추가
4. 작거나 같은 숫자가 나오면 바꿔치기
"""

n = int(input())  # 숫자의 개수
nums = list(map(int, input().split()))  # 숫자들 입력받기
dp = []  # 오름차순 수열 후보 리스트

def lower_bound(arr, target):
    # arr 안에서 target보다 크거나 같은 값이 처음 나오는 위치 찾기
    start, end = 0, len(arr)
    while start < end:
        mid = (start + end) // 2
        if arr[mid] < target:
            start = mid + 1
        else:
            end = mid
    return start

for num in nums:
    if not dp or num > dp[-1]:
        # 리스트가 비어있거나, 가장 끝보다 크면 그냥 추가
        dp.append(num)
    else:
        # num이 중간 어딘가에 들어가야 해 → 위치 찾아서 바꿔
        idx = lower_bound(dp, num)
        dp[idx] = num

print(len(dp))  # 최종적으로 만든 수열의 길이가 정답


"""
bisect_left 활용
"""

import bisect

def lis_length(arr):
    temp = []

    for num in arr:
        idx = bisect.bisect_left(temp, num)
        if idx == len(temp):
            temp.append(num)
        else:
            temp[idx] = num

    return len(temp)

# 입력
n = int(input())
a = list(map(int, input().split()))

# 출력
print(lis_length(a))