#include <stdio.h>
struct Person
{
    char name[20];
    int age;
};
int add(int a, int b)
{
    return a + b;
}
typedef struct Person Person;
Person p = {.name = "John", .age = 20};

int main()
{
    printf("Hello, World!\n");
    return 0;
}
