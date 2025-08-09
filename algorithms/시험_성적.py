S = int(input())

if 90 <= S <= 100:
    print("A")
elif 80 <= S <= 89:
    print("B")
elif 70 <= S <= 79:
    print("C")
elif 60 <= S <= 69:
    print("D")
else:
    print("F")


"""
#include <stdio.h>

int main() {
    int score;
    scanf("%d", &score);
    
    char grades[] = {'A', 'B', 'C', 'D', 'F'};
    int limits[] = {90, 80, 70, 60, 0};

    for (int i = 0; i < 5; i++) {
        if (score >= limits[i]) {
            printf("%c\n", grades[i]);
            break;
        }
    }
    
    return 0;
}
"""