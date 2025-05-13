"""
공유기 설치
https://www.acmicpc.net/problem/2110
[문제 & 해결 전략]
- 공유기는 중복 설치가 아니면 어디든 설치해도 괜찮다.
- 하지만 도현이는 공유기 C개를 설치하되,가장 가까운 두 공유기 사이의 거리가 가장 크도록 설치하고 싶다.
- 이를 해결하기 위해, 공유기 사이의 최소 거리를 D라고 정의하고, 그 D로 공유기 C개를 설치할 수 있는지를 판단하는 결정 문제로 바꾼다.
- 이때 가능한 최소 거리 start = 1 (같은 위치에 공유기를 둘 수 없으므로 0은 불가능)
    가능한 최대 거리 end = 집들 중 가장 오른쪽 위치 - 가장 왼쪽 위치 (두 공유기를 가장 멀리 떨어뜨릴 수 있는 경우)
- 이분 탐색으로 D의 최댓값을 찾아가는 방식이며, 공유기 설치는 왼쪽 집부터 시작해서, 설치 가능한 가장 가까운 곳에 설치하면서, D 이상 간격을 유지하며 몇 개를 설치할 수 있는지를 계산한다.

[Pseudocode]
1. 입력 
    n, c ← 집 개수 n, 설치할 공유기 수 c
    home ← n개의 집 좌표 입력받아 리스트로 저장
    home.sort()  // 집 위치 정렬
2. 설치 가능 여부 판단 함수 (Greedy)
    함수 공유기_설치_가능(distance, home, c):
        count ← 1  // 첫 번째 집에는 무조건 설치
        last_location ← home[0] // 첫 설치 위치

        for i from 1 to n-1:
            if home[i] - last_location ≥ distance:
                count ← count + 1   // 공유기 설치
                last_location ← home[i]
                if count ≥ c:
                    return True     // 공유기 C개 이상 설치 성공
        return False    // 끝까지 C개 못채웠으면 False
3. 이분 탐색 초기화
    start ← 1   // 공유기 사이 최소 거리 후보의 하한
    end ← home[-1] - home[0]  // 가장 먼 두 집 사이 거리 = 최대 거리 후보
    result ← 0  // 가능한 거리 중 가장 큰 값 저장용

    while start ≤ end:
        mid ← (start + end) // 2  // 현재 판단 중인 최소 거리 후보
        if 공유기_설치_가능(mid, home, c):  // mid 거리로 c개 공유기 설치 가능?
            result ← mid  // 가능한 거리면 일단 저장하고 거리 더 큰 거리 시도
            start ← mid + 1
        else:
            end ← mid - 1  // 불가능하면 거리 줄이기
    return result   // 최종적으로 가능한 최대 거리 반환
4. 출력 
    result <- search_router_distance(home, c)
"""

n, c = map(int, input().split())
home = []
for _ in range(n):
    home.append(int(input()))
home.sort()

def count_router(distance, home, c):
    count = 1
    last_location = home[0]
    for i in range(1, len(home)):
        if home[i] - last_location >= distance:
            count += 1
            last_location = home[i]
            if count >= c:
                return True
    return False

def search_router_distance(home, c):
    start = 1                      
    end = home[-1] - home[0]     
    result = 0                    

    while start <= end:
        mid = (start + end) // 2   
        if count_router(mid, home, c):
            result = mid           
            start = mid + 1
        else:
            end = mid - 1          
    return result

print(search_router_distance(home, c))