#include<stdio.h>
unsigned long long int fibo(int n){
    unsigned long long int res[1000] = {1, 1, 0};
    for(int i = 2; i <= n; i++){
        if( res[i] == 0){
            res[i] = res[i-1] + res[i-2];
        }
    }
    return res[n];
}
int main(int argc, char const *argv[])
{
    int n;
    unsigned long long int res;
    scanf("%d", &n);
    res = fibo(n);
    printf("%llu\n", res);
    return 0;
}
// unsigned long long int fibo(int n){
//     if(n == 0 || n == 1){
//         return 1;
//     }
//     else{
//         return fibo(n-1) + fibo(n-2);
//     }
// }
