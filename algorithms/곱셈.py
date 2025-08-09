A = int(input())
B = input()

print(A * int(B[2]))
print(A * int(B[1]))
print(A * int(B[0]))
print(A * int(B))


"""
#include <stdio.h>

int main() {
    int A, B;
    scanf("%d %d", &A, &B);

    int b1 = B % 10;
    int b10 = (B / 10) % 10;
    int b100 = B / 100;

    printf("%d\n%d\n%d\n%d\n", A * b1, A * b10, A * b100, A * B);
    return 0;
}
"""