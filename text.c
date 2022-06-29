#include <stdio.h>

extern int puts(const char *str);
extern int printf(const char *str, ...);

int do_math(int a)
{
    int b = a + 1;

    printf("b = %d", b);
    puts(" ");

    return b;
}

int main () {
    char *string = "Hello World!";

    do_math(1);
    puts(string);

    return 0;
}
