# 4의 배수이면서 100의 배수가 아닐 때 or 400의 배수일 때 윤년

year = int(input())

print(1 if (year % 4 == 0 and year % 100 != 0) or (year % 400 == 0) else 0)


"""
#include <stdio.h>

int main() {
    int year;
    scanf("%d", &year);

    printf("%d\n", ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) ? 1 : 0);
    return 0;
}
"""