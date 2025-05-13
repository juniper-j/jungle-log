"""
큐 2
https://www.acmicpc.net/problem/18258
[문제]
- push X, pop, size, empty는 스택과 동일
- front, back 큐의 가장 앞, 뒤에 있는 정수를 출력. 없으면 -1 출력

[해결 전략]
1. 각 명령어를 ' ' 단위로 쪼개 cmd 리스트에 저장
2. cmd[0]에는 명령어, cmd[1]에는 값이 저장됨
3. cmd[0]에 따라 명령어 수행
4. 단, 큐에서 pop은 queue.popleft()
"""

from collections import deque
import sys
input = sys.stdin.readline

n = int(input())
queue = deque()

for _ in range(n):
    cmd = input().split()

    # 정수 X를 스택에 넣는 연산
    if cmd[0] == 'push':
        queue.append(cmd[1])
    
    # 스택에서 가장 위에 있는 정수를 빼고, 그 수를 출력
    elif cmd[0] == 'pop':
        if len(queue) == 0: # 만약 없으면 -1 출력
            print(-1)
        else:
            print(queue.popleft()) 

    # 스택에 들어있는 정수의 개수를 출력
    elif cmd[0] == 'size':
        print(len(queue))
    
    # 스택이 비어있으면 1, 아니면 0 출력
    elif cmd[0] == 'empty':
        if len(queue) == 0:
            print(1)
        else:
            print(0)
    
    # 스택의 가장 앞에 있는 정수를 출력
    elif cmd[0] == 'front': 
        if len(queue) == 0: # 만약 없으면 -1 출력
            print(-1)
        else:
            print(queue[0]) 

    # 스택의 가장 뒤에 있는 정수를 출력
    elif cmd[0] == 'back':
        if len(queue) == 0: # 만약 없으면 -1 출력
            print(-1)
        else:
            print(queue[-1])