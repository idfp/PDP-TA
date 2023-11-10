#include "stdio.h"

int main(){
    FILE* file = fopen("database/transactions.csv", "r");
    fseek(file, 0L, SEEK_END);
    int size = ftell(file);
    rewind(file);
    printf("transactions is %d bytes\n", size);
}