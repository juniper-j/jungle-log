A = int(input())
B = int(input())
C = int(input())
multiple = list(str(A*B*C))

for i in range(10):
    print(multiple.count(str(i)))