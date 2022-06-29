#include <stdio.h>

extern int puts(const char *str);

int main () {
    char *string = "Hello World!";

    puts(string);

    return 0;
}
