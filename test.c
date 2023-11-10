#include "stdio.h"
#include "stdlib.h"

int main(){
    char* text = malloc(30);
    gets(text);
    printf("%s", text);
}