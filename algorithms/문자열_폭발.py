"""
문자열 폭발
https://www.acmicpc.net/problem/9935
[문제]
- 검토할 문자열, 폭발 문자열 입력 받음
- 폭발할 문자열을 제거하면 나머지 문자열이 연결됨
- 폭발 후 남은 문자열 출력

[해결 전략 - 스택]
1. 스택을 사용해 문자를 한 글자씩 처리하면서, bomb 문자열이 끝부분에 오면 제거
2. 문자열을 처음부터 끝까지 한 번만 순회하여 **O(N)**에 가깝게 처리
3. 스택에 쌓인 최종 결과를 출력
"""

import sys

# .rstrip()으로 줄 끝 개행 문자 \n 제거
sentence = sys.stdin.readline().rstrip()
bomb = sys.stdin.readline().rstrip()
stack = []  # 문자들을 순서대로 쌓아 둘 스택

# 입력 문자열을 한 글자씩 확인
for char in sentence:
    stack.append(char)

    # 스택의 끝이 폭발 문자열과 같으면 제거
    if len(stack) >= len(bomb):
        # 맨 끝 글자가 폭발 문자열의 끝과 같고, 리스트로 비교
        if char == bomb[-1] and stack[-len(bomb):] == list(bomb):
            del stack[-len(bomb):]

# 최종 결과 생성 및 결과 출력 (빈 문자열이면 "FRULA" 출력)
result = ''.join(stack)
print(result if result else "FRULA")