#include "stdio.h"

typedef struct Drugs{
    int id;
    char name[100];
    float priceUSD;
    int price;
    int stock;
} Drug;

int main(){
    printf("%d\n", sizeof(Drug));
    return 0;
}