#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

char *str;

void fun(char *inp, char ch)
{
    int i = 0;
    int n = strlen(inp);
    for (i = 0; i < strlen(inp); i++)
    {
        if (inp[i] == ch)
        {
            inp[i] = toupper(ch);
        }
    }
    // printf("%s \n", str);
}

void *charA(void *args)
{
    char *inp;
    inp = (char *)args;
    printf("Input string: %s \n", inp);
    printf("Capitalizing a \n");
    fun(inp, 'a');
    printf("Output string: %s \n", inp);
    pthread_exit(NULL);
}

void *charE(void *args)
{
    char *inp;
    inp = (char *)args;
    printf("Input string: %s \n", inp);
    printf("Capitalizing e \n");
    fun(inp, 'e');
    printf("Output string: %s \n", inp);
    pthread_exit(NULL);
}

void *charI(void *args)
{
    char *inp;
    inp = (char *)args;
    printf("Input string: %s \n", inp);
    printf("Capitalizing i \n");
    fun(inp, 'i');
    printf("Output string: %s \n", inp);
    pthread_exit(NULL);
}

void *charO(void *args)
{
    char *inp;
    inp = (char *)args;
    printf("Input string: %s \n", inp);
    printf("Capitalizing o \n");
    fun(inp, 'o');
    printf("Output string: %s \n", inp);
    pthread_exit(NULL);
}

void *charU(void *args)
{
    char *inp;
    inp = (char *)args;
    printf("Input string: %s \n", inp);
    printf("Capitalizing u \n");
    fun(inp, 'u');
    printf("Output string: %s \n", inp);
    pthread_exit(NULL);
}

int main()
{
    pthread_t thread_id;
    str = malloc(1000);
    strcpy(str, "aeiouHELLAEIOU");

    char *input = malloc(1000);
    strcpy(input, str);
    // printf("Before Thread\n");
    pthread_create(&thread_id, NULL, charA, (void *)input);
    pthread_join(thread_id, NULL);
    // printf("After Thread\n");

    // printf("\nFinal string is:%s\n", input);

    exit(0);

    return 0;
}
