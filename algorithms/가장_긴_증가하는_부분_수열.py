"""
가장 긴 증가하는 부분 수열
https://www.acmicpc.net/problem/11053

[Time Complexity]
DP: 이중 for문이므로 O(N^2) -> N <= 1000 이므로 통과 가능
Binary Search: O(n log n) -> 더 빠름
"""

# DP
# 나보다 앞에 있는 숫자 중에서, 나보다 작은 값을 찾아서 그 수열을 이어받는 방식
n = int(input())
nums = list(map(int, input().split()))

dp = [1] * n    # dp[i]: i번째 숫자를 마지막 원소로 하는 LIS의 최대 길이

for i in range(1, n):
    for j in range(i):
        if nums[j] < nums[i]:
            dp[i] = max(dp[i], dp[j] + 1)

print(max(dp))


# # 1주차 시험 때 쓴 코드
# # LIS의 길이만 구하는 알고리즘
# def lower_bound(arr, target):
#     # arr 안에서 target보다 크거나 같은 값이 처음 나오는 위치 찾기
#     # (즉, target이 들어갈 위치 = 정렬을 유지하면서 삽입할 수 있는 위치)
#     start, end = 0, len(arr)
#     while start < end:
#         mid = (start + end) // 2
#         if arr[mid] < target:
#             start = mid + 1
#         else:
#             end = mid
#     return start    # target이 들어갈 수 있는 위치 반환

# n = int(input()) 
# nums = list(map(int, input().split()))

# dp = []  # 오름차순 수열 후보 리스트

# for num in nums:
#     if not dp or num > dp[-1]:
#         # ① dp가 비어있거나, num이 가장 마지막 값보다 크면
#         #    그대로 뒤에 붙일 수 있으므로 append
#         dp.append(num)
#     else:
#         # ② num이 dp의 중간 어딘가에 들어가야 하는 경우
#         #    lower_bound를 통해 들어갈 위치(idx)를 찾고
#         #    그 위치의 값을 num으로 교체 (길이는 그대로지만 수열을 더 최적화)        
#         idx = lower_bound(dp, num)
#         dp[idx] = num

# print(len(dp))  # 최종적으로 만든 수열의 길이가 정답