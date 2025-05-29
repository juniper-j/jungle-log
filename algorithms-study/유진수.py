"""
유진수
https://www.acmicpc.net/problem/1356

[Problem]
- 정수 n이 주어졌을 때, 자릿수를 한 지점에서 나누어 왼쪽과 오른쪽의 곱이 같으면 유진수라고 함
- 단, 왼쪽과 오른쪽은 각각 한 자릿수 이상이어야 함
- 조건을 만족하는 분할이 하나라도 있으면 "YES", 없으면 "NO" 출력

[Solving]
- 입력값을 문자열로 받음 → 인덱스로 쉽게 분할 가능
- 모든 가능한 분할 위치를 반복 → i from 1 to len-1 (이때, n은 숫자가 아니므로 꼭 len을 써줘야 함)
- 각 분할에 대해 각 자릿수 곱 계산
- 두 곱이 같으면 True, 다르면 False
- True면 "YES" False면 "NO" 출력

[Time Complexity]
- 문자열 길이: 최대 50 (문제 제한)
- 분할 횟수: O(n)
- 각 분할마다 곱 계산: O(n)
- 총 시간 복잡도: O(n²) (n ≤ 50이므로 충분히 빠름)
"""


def anewjeansu(s):
    result = 1
    for char in s:
        result *= int(char)
    return result

n = input().strip()
anewjean_hbd = False

for i in range(1, len(n)):
    left = n[:i]
    right = n[i:]
    
    if anewjeansu(left) == anewjeansu(right):
        anewjean_hbd = True
        break
    
print("YES" if anewjean_hbd else "NO")