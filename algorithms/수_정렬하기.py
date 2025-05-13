N = int(input())
arr = [int(input()) for _ in range(N)]

for num in sorted(arr):
    print(num)

##############################################
##############################################


def heap_sort(arr):
    import heapq
    heapq.heapify(arr)  # 리스트를 최소 힙으로 변환
    return [heapq.heappop(arr) for _ in range(len(arr))]  # 힙에서 하나씩 추출

def merge_sort(arr):
    if len(arr) <= 1:
        return arr
    mid = len(arr) // 2
    left = merge_sort(arr[:mid])
    right = merge_sort(arr[mid:])
    
    sorted_arr = []
    i = j = 0
    while i < len(left) and j < len(right):  # 두 배열을 병합
        if left[i] < right[j]:
            sorted_arr.append(left[i])
            i += 1
        else:
            sorted_arr.append(right[j])
            j += 1
    sorted_arr.extend(left[i:])
    sorted_arr.extend(right[j:])
    return sorted_arr

def quick_sort(arr):
    if len(arr) <= 1:
        return arr
    pivot = arr[len(arr) // 2]  # 중앙값을 피벗으로 설정
    left = [x for x in arr if x < pivot]
    middle = [x for x in arr if x == pivot]
    right = [x for x in arr if x > pivot]
    return quick_sort(left) + middle + quick_sort(right)

def shell_sort(arr):
    n = len(arr)
    gap = n // 2  # 초기 간격 설정
    while gap > 0:
        for i in range(gap, n):
            temp = arr[i]
            j = i
            while j >= gap and arr[j - gap] > temp:
                arr[j] = arr[j - gap]
                j -= gap
            arr[j] = temp
        gap //= 2  # 간격 줄이기
    return arr

# 입력 받기
N = int(input())
arr = [int(input()) for _ in range(N)]

# 원하는 정렬 알고리즘 선택
sorted_arr = heap_sort(arr.copy())  # 힙 정렬 사용
# sorted_arr = merge_sort(arr.copy())  # 병합 정렬 사용
# sorted_arr = quick_sort(arr.copy())  # 퀵 정렬 사용
# sorted_arr = shell_sort(arr.copy())  # 셸 정렬 사용

# 결과 출력
for num in sorted_arr:
    print(num)