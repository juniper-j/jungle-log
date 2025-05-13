"""
https://www.acmicpc.net/problem/10828
[문제 해결 전략]
1. 각 명령어를 ' ' 단위로 쪼개 cmd 리스트에 저장
2. cmd[0]에는 명령어, cmd[1]에는 값이 저장됨
3. cmd[0]에 따라 명령어 수행
"""

import sys
n = int(sys.stdin.readline())
stack = []

for _ in range(n):
    cmd = sys.stdin.readline().split()

    # 정수 X를 스택에 넣는 연산
    if cmd[0] == 'push':
        stack.append(cmd[1])
    
    # 스택에서 가장 위에 있는 정수를 빼고, 그 수를 출력
    elif cmd[0] == 'pop':
        if len(stack) == 0:
            print(-1)
        else:
            print(stack.pop()) # 만약 없으면 -1 출력

    # 스택에 들어있는 정수의 개수를 출력
    elif cmd[0] == 'size':
        print(len(stack))
    
    # 스택이 비어있으면 1, 아니면 0 출력
    elif cmd[0] == 'empty':
        if len(stack) == 0:
            print(1)
        else:
            print(0)
    
    # 스택의 가장 위에 있는 정수를 출력
    elif cmd[0] == 'top':
        if len(stack) == 0:
            print(-1)
        else:
            print(stack[-1]) # 만약 없으면 -1 출력