"""
https://www.acmicpc.net/problem/9012
[문제 해결 전략]
1. 입력받은 괄호 문자열에서 '('는 스택에 포함
2. ')'는 스택이 비어있다면 NO, 아니라면 pop()
3. 문자열이 끝났을 때 스택이 비어있으면 YES, 아니라면 NO
"""

import sys
input = sys.stdin.readline

T = int(input().strip())  # 테스트 케이스 개수 입력

for _ in range(T):
    stack = []
    is_vps = True
    line = input().strip()  # 문자열 입력 (개행문자 제거)

    for char in line:
        if char == '(':
            stack.append(char)
        else char == ')':
            if stack:
                stack.pop()  # '('가 있다면 제거
            else:
                is_vps = False  # 짝이 안 맞음
                break  # 불필요한 연산 방지

    if stack:
        is_vps = False  # 모든 과정이 끝난 후, 스택에 '('가 남아있으면 NO

    print("YES" if is_vps else "NO")



#########################################
#########################################

# import sys
# input = sys.stdin.readline

# T = int(input().strip())  # 테스트 케이스 개수 입력

### 효율화될 것 같아서 추가했으나 차이 없음
# for _ in range(T):
#     line = input().strip()  # 괄호 문자열 입력
#     if line[0] == ')':  # 첫 번째 문자가 닫는 괄호라면 즉시 "NO"
#         print("NO")
#         continue

#     stack = []
#     is_vps = True

#     for char in line:
#         if char == '(':
#             stack.append(char)
#         elif char == ')':
#             if stack:
#                 stack.pop()  # '('가 있다면 제거
#             else:
#                 is_vps = False  # ')'가 먼저 나와서 짝이 안 맞음
#                 break  # 불필요한 연산 방지

#     if stack:
#         is_vps = False  # '('가 남아 있으면 NO

#     print("YES" if is_vps else "NO")

#########################################
#########################################