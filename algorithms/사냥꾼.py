"""
사냥꾼
https://www.acmicpc.net/problem/8983
[문제]
- 사격대에서만 사격할 수 있고, 사격대는 x축 위에 위치 ← (x, 0)
- 사격대상인 동물의 위치는 (a, b)
- 사격 거리는 대각이 아닌 (|x - a| + b)이며, 사정거리 l은 주어짐

[해결 전략 - 이분탐색]
1. 입력
    m, n, l ← 사격대 수 m, 동물 수 n, 사정거리 l
    sadae ← 리스트로 저장
    animals ← 튜플 형태로 리스트에 저장
    sadae.sort()
2. 동물과 가장 가까운 사격대 위치를 찾기 위한 이분탐색 함수 구현
    def near_sadae_search(sadae, x):
        left ← 0
        right ← m
        while left < right:
            mid ← (left + right) // 2
            if sadae[mid] < x:
                left ← mid + 1
            else:
                right ← mid
        return left
3. 해당 사대가 사정거리 이내인지 확인하는 조건 함수
    def hunt_available(sadae, x, y, l):
        if y > l:
            return False
        idx ← near_sadae_search(sadae, x)
        for i in [idx - 1, idx]
            if 0 <= i <= len(sadae) and abs(sadae[i] - x) + y <= 1:
                return True
            return False
4. 출력
    result ← 0
    for animal in animals:
        x, y ← 동물
        if hunt_available(sadae, x, y, l):
            result ← result + 1
    print(result)
"""




"""
[해결 전략 - bisect_left] -> 이진탐색을 하는 모듈
1. 입력
    m, n, l ← 사격대 수 m, 동물 수 n, 사정거리 l
    sadae ← 리스트로 저장
    animals ← 튜플 형태로 리스트에 저장
    sadae.sort()
2. 동물별로 사정거리 안에 사격대가 있는지 확인
    result ← 0  // 사정거리 안에 있는 동물 수 저장용
    for each (x, y) in animals:
        if y > l:
            continue    // y 만으로도 사정거리 초과 -> 건너뜀
        idx ← bisect_left(sadae, x)     // 동물의 x보다 크거나 같은 사대 인덱스
        for i in [idx-1, idx]:  // 인접한 두 사대만 확인
            if 0 <= i < m and abs(sadae[i] - x) + y <= l:
                result ← result + 1
                break   // 이 동물은 하나라도 맞으면 넘어감
3. 출력
    print(result)
"""

# 정확히 그 값과 일치하면 그 인덱스를 가리키기 때문에 그 인덱스를 반환
# 중간 값에 대해서 오른쪽 인덱스를 가리키게됨.
# 그렇다면 중간 값일 때 i와 i-1의 값을 각각 비교해 더 가까운 쪽(인덱스)을 반환

from bisect import bisect_left

# m 사대 n 동물 l 사거리
m, n, l = map(int, input().split())
fires = list(map(int, input().split()))
fires.sort()

def find_near_hunter(arr, x):
    idx = bisect_left(arr, x)
    if idx == 0:
        return 0
    elif idx == m:
        return m-1    
    elif arr[idx] == x:
        return idx
    else:  
        if abs(arr[idx] - x) < abs(arr[idx-1] - x):
            return idx
        else:
            return idx-1

def is_dead(x, y):
    if y > l:
        return False
    
    hunter_x = fires[find_near_hunter(fires, x)]
    if abs(hunter_x - x) + y <= l:
        return True
    return False

count = 0
for _ in range(n):
    a, b = map(int, input().split())
    if is_dead(a, b):
        count += 1
print(count)