#include "stdio.h"

int main(){
    while(1){
        int choice;
        printf("Menu:\n1. Eksekusi\n2. Exit\n> ");
        scanf("%d", &choice);
        if(choice == 2){
            break;
        }
    }
    return 0;
}