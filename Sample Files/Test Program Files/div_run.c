#include <stdio.h>

void main(int argc, char const *argv[])
{   
    int x[10];
    int num1, num2;
    scanf("%d %d", &num1, &num2);
    float result = num1 / (num2 * 1.0);
    if (num2 == 0)
        printf("%d",x[10000]);
    else
        printf("%.2f\n", result);
}
