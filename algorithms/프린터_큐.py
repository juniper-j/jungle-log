"""
프린터 큐
https://www.acmicpc.net/problem/1966
[문제]
- 중요도(priority 1~9)가 높은 문서를 가장 먼저 인쇄
- 이때, 요세푸스 순열처럼 기존 순서가 유지되며, 인쇄 순서만 변경됨
- 타겟 문서가 몇번째로 인쇄되는지 출력

[해결 전략 - deque + heapq 병행 사용]
[Refactoring ver 3.0]
** queue로 문서 순서대로 처리, heap으로 중요도만 따로 저장해서 가장 높은 중요도 값만 빠르게 확인 **
1. 입력 처리
    T ← 테스트 케이스 수
    반복문으로 각 테스트 케이스 수행
        n, target ← 문서 수, 타겟 문서의 인덱스
        priorities ← 문서들의 중요도 리스트
        
2. 자료 구조 초기화
    queue ← deque((idx, priority)) 형태로 저장
        (문서의 원래 인덱스와 중요도 저장)
    heap ← 최대 힙(max heap)을 위해 priorities에 -1을 곱해서 저장
        heapify로 힙 구조 구성 (O(n))

3. 시뮬레이션 시작 (while queue)
    - queue의 맨 앞 문서를 확인
    - 만약 해당 문서의 중요도 < heap의 최댓값:
        → queue.append(queue.popleft())  # 뒤로 보냄
    - 그렇지 않으면:
        → 인쇄 진행 (count += 1)
        → heap에서도 heappop으로 제거
        → 만약 인쇄한 문서가 타겟이면 count 출력하고 break

4. 출력
    각 테스트 케이스마다 타겟 문서가 몇 번째로 인쇄되는지 출력
"""

from collections import deque
import heapq
import sys
input = sys.stdin.readline

T = int(input())

for _ in range(T):
    n, target = map(int, input().split())
    priorities = list(map(int, input().split()))
    queue = deque((i, p) for i, p in enumerate(priorities))

    # 중요도만 최대 힙으로 구성
    heap = [-p for p in priorities]
    heapq.heapify(heap)

    count = 0

    while queue:
        idx, pri = queue[0]

        if pri < -heap[0]:  # 현재 문서보다 더 중요한 문서가 있음
            queue.append(queue.popleft())  # 뒤로 보냄
        else:
            count += 1
            queue.popleft()
            heapq.heappop(heap)  # 중요도 힙에서도 제거

            if idx == target:
                print(count)
                break



"""
[해결 전략 - 큐] 
[Refactoring ver 2.0]
** any() 대신 max()로 중요도 비교 -> 로직은 동일하지만, 조금 더 직관적이고 빠름 **
1. 입력 처리
    iteration ← 테스트 케이스 개수
    for i in range(iteration):
        n, target ← 문서 수, 타겟 문서 인덱스
        queue ← deque((idx, priority)) 형태로 저장

2. 큐 시뮬레이션 (while queue)
    - 현재 큐 맨 앞 문서를 확인
    - 현재 문서의 중요도가 큐 전체에서 가장 큰 값이 아니라면:
        → 다시 뒤로 보냄 (queue.append(queue.popleft()))
    - 아니라면 인쇄:
        → count += 1
        → 만약 해당 문서가 타겟이면 count 출력하고 종료

3. 출력
    - 각 테스트케이스마다 타겟 문서가 몇 번째로 인쇄되었는지 출력
"""

from collections import deque
import sys
input = sys.stdin.readline

iteration = int(input())

for _ in range(iteration):
    count = 0
    n, target = map(int, input().split())
    priorities = list(map(int, input().split()))
    queue = deque((i, p) for i, p in enumerate(priorities))

    while queue:
        idx, pri = queue[0]
        if pri < max(q[1] for q in queue):  # 현재 문서보다 더 중요한 문서가 있음
            queue.append(queue.popleft())
        else:
            count += 1
            popped = queue.popleft()
            if popped[0] == target:
                print(count)
                break



"""
[해결 전략 - 큐] 
[Refactoring ver 1.0]
1. 입력 처리
    iteration ← 테스트케이스 개수
    for i in range(iteration):
        n, target ← 문서 개수, 타겟 문서 인덱스
        queue ← deque에 (문서 초기 인덱스, 중요도) 형태로 저장

2. 큐 시뮬레이션
    while queue:
        if queue의 첫 번째 문서보다 중요도가 높은 문서가 뒤에 존재:
            queue.append(queue.popleft())  # 문서를 맨 뒤로 보냄
        else:
            count += 1  # 인쇄 카운트 증가
            문서 꺼냄
            만약 꺼낸 문서가 target이라면:
                break → 해당 순서를 출력

3. 출력
    각 테스트케이스마다 타겟 문서의 인쇄 순서를 출력
"""

from collections import deque
import sys
input = sys.stdin.readline

iteration = int(input())

for _ in range(iteration):
    count = 0
    n, target = map(int, input().split())
    queue = deque((idx, priority) for idx, priority in enumerate(map(int, input().split())))

    while queue:
        if any(queue[0][1] < item[1] for item in queue):
            queue.append(queue.popleft())
        else:
            count += 1
            idx, _ = queue.popleft()
            if idx == target:
                break

    print(count)



"""
[해결 전략 - 큐] 
1. 입력
    iteration ← 테스트 케이스 반복수
    tc ← 문서의 개수, 타겟 문서의 순서(0부터 시작)
    queue ← (idx, priprity)     # (문서 초기 순서, 중요도)
    result ← 인쇄 순서 저장할 리스트
2. 큐 함수 구현
    for j in range(tc[0]):  # 문서의 중요도를 탐색
        if queue[0][1] < queue[j][1]:   # First-In 중요도가 다른 것보다 낮으면 출&입력
            queue.append(queue.popleft())
        else:   # FI 중요도가 같거나 높으면 
            queue.popleft()             # 인쇄
            count += 1                  # 인쇄 횟수 증가
            result = queue.popleft()    
            if result[0] == tc[1]        # idx = target? 인쇄한 문서 초기 순서가 타겟 순서와 같다면 인쇄 멈춤 
                break
3. 출력
    count
** 방향은 잘 잡았으나, count 위치, for j 잘못 사용해서 결과값 다르게 나옴 **
"""

from collections import deque
import sys
input = sys.stdin.readline

iteration = int(input())
count = 0

for i in range(iteration):
    tc = list(map(int, input().split()))
    queue = deque((idx, priority) for idx, priority in enumerate(map(int, input().split())))
    
    for j in range(tc[0]):
        if any(queue[0][1] < item[1] for item in queue):
            queue.append(queue.popleft())
        else:        
            count += 1      
            result = queue.popleft()         
            if result[0] == tc[1]:
                break
            
    print(count)