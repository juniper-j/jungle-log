# 표준 입력 및 출력을 빠르게 처리하기 위한 sys 모듈 불러오기
import sys

# 입력받을 단어의 개수
N = int(sys.stdin.readline())
# set을 사용하여 중복 제거 후 리스트로 변환
arr = list(set(sys.stdin.readline().strip() for _ in range(N)))

# 사전 순 정렬 (python 함수의 sort() 함수가 안정 정렬(stable sort)이기 때문에 우선적으로 정렬)
arr.sort()
# 길이 기준 정렬 (길이가 같으면 사전 순 유지됨)
arr.sort(key = len)

# 정렬된 단어 리스트 출력
for num in arr:
    print(num)