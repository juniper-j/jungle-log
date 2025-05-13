"""
가장 긴 증가하는 부분 수열 4
https://www.acmicpc.net/problem/14002

[Problem & Solvings - DP]
- 주어진 수열에서 가장 긴 증가하는 부분 수열(LIS)의 길이와 그 수열 자체를 구하는 문제
- dp[i]: i번째 숫자를 마지막 원소로 하는 LIS의 최대 길이
- prev[i]: dp[i]가 어디에서 왔는지를 저장해, 수열을 나중에 역추적 가능하게 함
- 이중 for문을 통해 앞의 원소들 중 자신보다 작은 원소를 찾아 최적 길이를 갱신
- 복원은 dp에서 최대값을 가진 인덱스부터 prev를 따라가며 리스트를 구성

[Time Complexity]
- DP 계산: O(N²)
- 수열 복원: O(N)
- 총 시간 복잡도: O(N²)
"""

# DP
n = int(input())
nums = list(map(int, input().split()))

dp = [1] * n     # dp[i]: i번째 숫자를 마지막 원소로 하는 LIS의 최대 길이
prev = [-1] * n  # 수열 복원을 위한 경로 저장

for i in range(1, n):
    for j in range(i):
        if nums[j] < nums[i] and dp[i] < dp[j] + 1:
            dp[i] = dp[j] + 1
            prev[i] = j
            
# LIS의 최대 길이와 그 인덱스 구하기
max_len = max(dp)            # 가장 긴 증가 부분 수열의 길이
idx = dp.index(max_len)      # 그 최대 길이를 갖는 마지막 숫자의 인덱스

# 증가 수열을 역추적해서 복원
lis = []
while idx != -1:
    lis.append(nums[idx])    # 현재 인덱스의 숫자 추가
    idx = prev[idx]          # 이전 인덱스로 이동

lis.reverse()  # 뒤에서부터 추가했으므로 순서를 뒤집기

# 출력
print(max_len)
print(*lis)



# # Binary Search
# def lower_bound(arr, target):
#     start, end = 0, len(arr)
#     while start < end:
#         mid = (start + end) // 2
#         if arr[mid] < target:
#             start = mid + 1
#         else:
#             end = mid
#     return start

# n = int(input())
# nums = list(map(int, input().split()))

# dp = []  # LIS 후보들
# dp_idx = []  # 각 num이 dp 어디에 들어갔는지 기록
# trace = [-1] * n  # 연결 정보

# # 실제 LIS 구성 요소의 위치를 추적하기 위한 리스트
# pos_in_dp = []

# for i, num in enumerate(nums):
#     idx = lower_bound(dp, num)

#     if idx == len(dp):
#         dp.append(num)
#         pos_in_dp.append(i)
#     else:
#         dp[idx] = num
#         pos_in_dp[idx] = i

#     dp_idx.append(idx)

#     if idx != 0:
#         trace[i] = pos_in_dp[idx - 1]  # 이전 요소 인덱스 저장

# # LIS 수열 복원
# lis_length = len(dp)
# lis = []
# cur_idx = pos_in_dp[-1]

# # pos_in_dp[-1]이 실제 LIS의 마지막 인덱스임
# for i in range(n - 1, -1, -1):
#     if dp_idx[i] == lis_length - 1:
#         cur_idx = i
#         break

# while cur_idx != -1:
#     lis.append(nums[cur_idx])
#     cur_idx = trace[cur_idx]

# lis.reverse()

# print(len(lis))
# print(*lis)