#include <stdio.h>

void countdown(size_t n);

int main()
{
    countdown(10);
}

void countdown(size_t n)
{
    for (int i = n; i >= 0; i--)
    {
        printf("%d\n", i);
    }
}