import sys

# 한 번에 입력을 받아 속도 최적화
input = sys.stdin.read
# 공백으로 구분된 입력을 한 번에 리스트 변환
data = list(map(int, input().split()))
# 첫 번째 원소(N) 제거
data.pop(0)
# 파이썬 내장 정렬 (Timsort, O(N \log N))
data.sort()

# 빠른 출력 사용 (print보다 훨씬 빠름)
sys.stdout.write('\n'.join(map(str, data)) + '\n')