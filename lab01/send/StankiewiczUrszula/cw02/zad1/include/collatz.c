#include <stdio.h>


int collatz_conjecture(int);
int test_collatz_convergence(int, int);


int collatz_conjecture(int input)
{
    if (input % 2 == 0)
    {
        return input / 2;
    }
    return (3 * input) + 1;
}

int test_collatz_convergence(int input, int max_iter)
{
    int outcome = input;
    int counter = 0;

    while (outcome != 1)
    {
        outcome = collatz_conjecture(outcome);
        counter+=1;
        if (counter > max_iter)
        {
            return -1;
        }
        
    }

    return counter;
}