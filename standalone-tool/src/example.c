#include <stdio.h>
#include <stdlib.h>

void swap(unsigned char **p, unsigned char **q)
{
    unsigned char *t = *p;
    *p               = *q;
    *q               = t;
}

int main(int argc, char **argv, char **envp)
{
    printf("argc: %d; argv: %p; envp: %p\n", argc, (void *)argv, (void *)envp);

    unsigned char a1, b1;
    a1 = rand() % 256;
    b1 = rand() % 256;
    printf("a1: %d; b1: %d\n", a1, b1);

    unsigned char *a = &a1;
    unsigned char *b = &b1;
    printf("a: %p; b: %p\n", (void *)a, (void *)b);

    swap(&a, &b);
    printf("a1: %d; b1: %d\n", a1, b1);
    printf("a: %p; b: %p\n", (void *)a, (void *)b);
}
