"""
신입 사원
https://www.acmicpc.net/problem/1946

[Problem & Solvings]
- t <- 테스트 케이스 개수
- n <- 지원자 수
- score1, scroe2 <- 서류 심사 성적, 면접 성적 순위(동점 없음)
- 서류 순위 기준 오름차순 정렬해 면접 순위를 비교하며 최소값을 갱신해 선발 여부 결정
- 각 케이스의 선발인원을 줄바꿈해 출력

[Time Complexity]
정렬 O(n log n) + 탐색/비교 O(n)
전체 시간복잡도는 O(T * n log n)
"""

def recruit():
    import sys
    input = sys.stdin.readline
    t = int(input().strip())
    
    for _ in range(t):
        n = int(input())
        applications = []

        for _ in range(n):
            doc, inter = map(int, input().split())
            applications.append((doc, inter))

        applications.sort(key=lambda x: x[0])   # 1. 서류 등수 오른차순 정렬

        count = 1                               # 2. 첫 번째 지원자는 무조건 선발
        min_interview = applications[0][1]

        for i in range(1, n):                   # 3. 두 번째 지원자부터 체크
            if applications[i][1] < min_interview:
                count += 1
                min_interview = applications[i][1]

        print(count)

recruit()


## 좀 더 느림
# import sys
# input = sys.stdin.readline
# t = int(input().strip())
# results = []

# for _ in range(t):
#     n = int(input().strip())
#     applications = []

#     for _ in range(n):
#         doc, inter = map(int, input().split())
#         applications.append((doc, inter))

#     applications.sort(key=lambda x: x[0])   # 서류 등수 기준 오름차순 정렬
#     min_inter_rank = float('inf')           # 면접 등수 최솟값을 '무한대'로 초기화
#     count = 0

#     for _, inter_rank in applications:      # 면접 등수를 순회하며 최솟값보다 작은 경우 선발
#         if inter_rank < min_inter_rank:
#             count += 1
#             min_inter_rank = inter_rank
    
#     results.append(count)

# print('\n'.join(map(str, results)))