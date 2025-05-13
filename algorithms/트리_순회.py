"""
트리 순회
https://www.acmicpc.net/problem/1991
[문제]
- 전위 순회: ABDCEFG // (루트) (왼쪽 자식) (오른쪽 자식)
- 중위 순회: DBAECFG // (왼쪽 자식) (루트) (오른쪽 자식)
- 후위 순회: DBEGFCA // (왼쪽 자식) (오른쪽 자식) (루트)

[해결 전략]
** 순회는 모두 재귀로 풀 수 있음 **
1. preorder: root -> left -> right
2. inorder: left -> root -> right
3. postorder: left -> right -> root
"""

"""
백준 1991 트리 순회
https://www.acmicpc.net/problem/1991

[문제]
	•	이진 트리의 노드 구조가 주어졌을 때,
	•	전위 순회, 중위 순회, 후위 순회의 결과를 출력하라.
	•	노드는 알파벳 대문자로 주어지며, 없는 자식 노드는 '.'으로 표시된다.

[해결 전략]
1.	트리 구조 저장
	•	딕셔너리 자료구조 tree에 각 노드의 왼쪽, 오른쪽 자식을 저장
	•	예: tree['A'] = ['B', 'C']는 A의 왼쪽 자식이 B, 오른쪽 자식이 C라는 뜻
2.	전위 순회 함수 정의 (Preorder)
	•	순서: 루트 → 왼쪽 자식 → 오른쪽 자식
	•	루트 노드 출력 후, 왼쪽 자식과 오른쪽 자식을 재귀적으로 탐색
3.	중위 순회 함수 정의 (Inorder)
	•	순서: 왼쪽 자식 → 루트 → 오른쪽 자식
	•	왼쪽 자식을 먼저 재귀적으로 순회, 루트 출력, 오른쪽 순회
4.	후위 순회 함수 정의 (Postorder)
	•	순서: 왼쪽 자식 → 오른쪽 자식 → 루트
	•	자식들을 모두 방문한 후 루트를 출력
5.	출력
	•	각 순회 결과를 한 줄에 출력하고, 순회 간에는 print()로 줄바꿈 처리
"""

n = int(input())    # 트리 노드 개수 입력
tree = {}   # 트리 구조를 저장할 딕셔너리

# 트리 정보 입력 받기
for _ in range(n):
	root, left, right = map(str, input().split())   # 루트, 왼쪽 자식, 오른쪽 자식 입력
	tree[root] = [left, right]  # 딕셔너리에 저장
	
    ### 트리 예시 구조
	# tree = {
    # 'A': ['B', 'C'],  # A의 왼쪽 자식은 B, 오른쪽 자식은 C
    # 'B': ['D', '.'],
    # 'C': ['E', 'F'],
    # 'E': ['.', '.'],
    # 'F': ['.', 'G'],
    # 'D': ['.', '.'],
    # 'G': ['.', '.']
    # }
	###

# 전위 트리 (루트 → 왼쪽 → 오른쪽)
def preorder(root):
	if root == '.':     # 자식이 없는 경우
		return
	print(root, end='')         # 줄바꿈 없이 현재 노드 출력
	preorder(tree[root][0])     # 왼쪽 자식 순회
	preorder(tree[root][1])     # 오른쪽 자식 순회

# 중위 순회 (왼쪽 → 루트 → 오른쪽)
def inorder(root):
	if root == '.':
		return
	inorder(tree[root][0])
	print(root, end='')
	inorder(tree[root][1])

# 후위 순회 (왼쪽 → 오른쪽 → 루트)
def postorder(root):
    if root == '.':
        return
    postorder(tree[root][0])
    postorder(tree[root][1])
    print(root, end='')

# 순회 결과 출력
preorder('A')
print()         # 줄바꿈
inorder('A')
print()         # 줄바꿈
postorder('A')