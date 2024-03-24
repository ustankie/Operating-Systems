#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#ifdef DYNAMIC
    #include <dlfcn.h>
#else
extern int test_collatz_convergence(int, int);
#endif

void testCollatz(int, int);
#ifdef DYNAMIC
    int dynamicClient(int, int);
    void testCollatzDynamic(int, int (*test_collatz_convergence)(int, int), int);
#endif

int main()
{
    int n;
    int iter;
    printf("Wprowadź liczbę testów oraz maksymalną liczbę iteracji:\n");
    scanf("%d %d", &n, &iter);

    printf("Losowe %d wyników dla maksymalnej liczby iteracji %d:\n", n,iter);

    #ifdef DYNAMIC
        return dynamicClient(n, iter);
    #else
        testCollatz(n, iter);
    #endif
    return 0;
}

#ifndef DYNAMIC
void testCollatz(int n, int iter)
{
    srand(time(NULL));
    for (int i = 0; i < n; i++)
    {
        int r = rand() % 30;

        printf("%d: %d\n", r, test_collatz_convergence(r, iter));
    }
}
#endif

#ifdef DYNAMIC
int dynamicClient(int n, int iter)
{
    void *handle = dlopen("./build/libcollatz.so", RTLD_LAZY);
    if (!handle)
    {
        printf("Library open error\n");
        return 0;
    }
    int (*test_collatz_convergence)(int, int);
    test_collatz_convergence = dlsym(handle, "test_collatz_convergence");
    testCollatzDynamic(n, test_collatz_convergence, iter);

    dlclose(handle);
}

void testCollatzDynamic(int n, int (*test_collatz_convergence)(int, int), int iter)
{
    srand(time(NULL));
    for (int i = 0; i < n; i++)
    {
        int r = rand() % 30;
        printf("%d: %d\n", r, test_collatz_convergence(r, iter));
    }
}
#endif