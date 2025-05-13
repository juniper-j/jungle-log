"""
두 용액
https://www.acmicpc.net/problem/2470
[문제]
!! 이진탐색처럼 중간값을 기준으로 줄여나가는 방식보다는, 
투 포인터를 사용해 양쪽 끝에서 가운데로 좁혀가는 방식이 더 적합 !!

[해결 전략 - 투 포인터]
1. 입력
    n, ph ← 용액 수 n, 특성값 리스트 ph
    ph.sort()   // 특성값 정렬
2. 초기화
    left ← 0
    right ← n-1
    closest ← float('inf')  // 절댓값 비교에서 최솟값을 찾기 위해 초기값을 매우 크게 설정
    ans ← (ph[left], ph[right])
3. 두 포인터를 이동시키는 함수
    while left < right:
        total ← ph[left] + ph[right]    // 총합 정의
        if abs(total) < closest:
            closest = abs(total)
            ans = (ph[left], ph[right])
        
        if total < 0:
            left ← left + 1
        else:
            right ← right - 1
4. 출력
    print(ans)
"""

n = int(input())
ph = list(map(int, input().split()))
ph.sort()

left = 0
right = n - 1
closest = float('inf')
ans = (ph[left], ph[right])

while left < right:
    total = ph[left] + ph[right]
    
    if abs(total) < closest:
        closest = abs(total)
        ans = (ph[left], ph[right])
    
    if total < 0:
        left += 1
    else:
        right -= 1

print(*ans)



"""
[해결 전략 - 두 용액]
1. 하나를 고정하고 나머지에서 탐색
2. 이분 탐색의 두 후보 비교
3. 가장 0에 가까운 조합을 기억
"""

import bisect

n = int(input())  # 용액의 수 입력
arr = list(map(int, input().split()))  # 용액 특성값 리스트 입력
arr.sort()  # 이분 탐색을 위해 오름차순 정렬

min_diff = float('inf')  # 0에 가장 가까운 합의 절댓값을 저장할 변수 (초기값: 무한대)
ans = (0, 0)  # 정답으로 출력할 두 용액의 조합

# 0번째부터 n-2번째까지 순회하면서 각 원소를 고정
for i in range(n - 1):
    fixed = arr[i]  # 현재 고정된 값
    target = -fixed  # 이 값과 합쳐서 0에 가까운 수를 찾기 위한 목표값

    # target에 가장 가까운 값을 찾기 위해 이분 탐색 수행
    # arr[i+1]부터 arr[n-1] 사이에서 target의 삽입 위치를 찾음
    idx = bisect.bisect_left(arr, target, i + 1, n)

    # [후보 1] 이분 탐색이 가리킨 위치의 값과 조합
    if idx < n:  # 인덱스가 범위를 벗어나지 않으면
        total = fixed + arr[idx]  # 두 용액의 합
        if abs(total) < min_diff:  # 절댓값이 더 작으면 정답 갱신
            min_diff = abs(total)
            ans = (fixed, arr[idx])

    # [후보 2] 이분 탐색 바로 이전 위치의 값과 조합 (더 가까울 수도 있음!)
    if idx - 1 > i:  # 인덱스가 유효하고 고정값보다 오른쪽일 경우만
        total = fixed + arr[idx - 1]
        if abs(total) < min_diff:
            min_diff = abs(total)
            ans = (fixed, arr[idx - 1])

# 정답 출력
print(ans[0], ans[1])