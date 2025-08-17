C = int(input())

for _ in range(C):
    data = list(map(int, input().split()))
    N = data[0]
    scores = data[1:]
    avg = sum(scores)/N
    above_avg_count = sum(1 for score in scores if score > avg)

    percentage = (above_avg_count/N)*100
    print(f"{percentage:.3f}%")