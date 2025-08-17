N = int(input())

for i in range(1, N+1):
    score = 0
    streak = 0
    marking = input()
    for ch in marking:
        if ch == 'O':
            streak += 1
            score += streak
        else:
            streak = 0
    print(score)