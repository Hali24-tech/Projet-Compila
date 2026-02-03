#include <stdio.h>

int main(){
    int x;

    x = 1;
    if (x == 0) {
        printf("%d\n", x);
    } else {
        x = (x + 10);
        printf("%d\n", x);
    }
    return 0;
}
