"""
잃어버린 괄호
https://www.acmicpc.net/problem/1541

[Problem & Solvings]
- 양수, +, - 로 이루어진 식에 ()를 적절히 쳐서 이 식의 값을 최소로 만들기
- 식의 길이 <= 50 -> 완전탐색 가능하지만, 수식 구조상 그리디가 더 적절
- 처음 나오는 - 부호 이후는 전부 괄호로 묶어서 빼면 최소값이 됨

[Pseudocode]
1.	수식 입력받기: expression ← input().split('-')
2.	각 그룹마다 +로 나눈 숫자들 합산 → sums 리스트 생성
4.	최종 결과는 sums[0] - sum(sums[1:])
5.	출력

[Time Complexity]
O(n)
"""

expression = input().split('-')
first_part = sum(map(int, expression[0].split('+')))
result = first_part

for part in expression[1:]:
    result -= sum(map(int, part.split('+')))

print(result)