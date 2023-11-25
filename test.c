#include "stdio.h"
#include "lib/sha256.h"

int main(){
    printf("%s", SHA256("12345"));
    return 0;
}