# 표준 입력 및 출력을 빠르게 처리하기 위한 sys 모듈 불러오기
import sys

# 숫자의 개수 N 입력 받고, 숫자의 빈도를 저장할 딕셔너리 선언
N = int(sys.stdin.readline().rstrip())
cnt_dic = {}

# 숫자를 입력받아 등장 횟수를 딕셔너리에 저장
for _ in range(N):
    X = int(sys.stdin.readline().rstrip())   # 한 줄씩 입력받아 정수로 변환
    cnt_dic[X] = cnt_dic.get(X, 0) + 1  # `get()`을 사용하여 기본값을 0으로 설정 후 증가

# 정렬된 키를 순회하면서 출력 (불필요한 range 반복 제거)
for num in sorted(cnt_dic.keys()):  
    for _ in range(cnt_dic[num]):  
        print(num)



# # 표준 입력 및 출력을 빠르게 처리하기 위한 sys 모듈 불러오기
# import sys

# # 숫자의 개수 N 입력 받고, 숫자의 빈도를 저장할 딕셔너리 선언
# N = int(sys.stdin.readline().rstrip())
# cnt_dic = {}

# # 숫자를 입력받아 등장 횟수를 딕셔너리에 저장
# for _ in range(N):
#     X = int(sys.stdin.readline().rstrip())   # 한 줄씩 입력받아 정수로 변환
#     if X in cnt_dic:        # 이미 딕셔너리에 존재하는 숫자라면
#         cnt_dic[X] += 1     # 등장 횟수 +1
#     else:                   # 새로운 숫자라면
#         cnt_dic[X] = 1      # 1로 초기화 = 기본값 할당

# # 딕셔너리를 이용하여 정렬된 순서로 출력
# for i in range(max(cnt_dic.keys())+1):      # 0부터 가장 큰 숫자까지 반복
#     if i not in cnt_dic:    # 딕셔너리에 없는 숫자는 건너뜀
#         continue
#     else:                   # 딕셔너리에 존재하면 해당 숫자 만큼 반복하여 출력
#         for _ in range(cnt_dic[i]):
#             print(i)