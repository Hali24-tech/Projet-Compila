#include <stdio.h>

int main(){
    int x, i;

    x = 1;
    if (x < 3) {
        printf("%d\n", x);
    } else {
        x = ((x + 2) * 3);
        printf("%d\n", x);
    }
    while (x != 20) {
        x = (x + 2);
    }
    for (i = 1; i <= 5; i++) {
        printf("%d\n", (i + x));
    }
    return 0;
}
