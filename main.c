#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "lib/sha256.h"

#define BUF_SIZE 65536

// buat tipe data string untuk mempermudah development
typedef char* string;

// tipe data user, menyimpan data tiap user sebagai object
typedef struct Users{
    int id;
    char username[100];
    char password[100];
} User;

// hitung jumlah baris dalam file, mengembalikan n - 1 baris
int countLines(string name){
    FILE* file = fopen(name, "r");
    char buf[BUF_SIZE];
    int counter = 0;
    for(;;)
    {
        size_t res = fread(buf, 1, BUF_SIZE, file);
        if (ferror(file))
            return -1;

        int i;
        for(i = 0; i < res; i++)
            if (buf[i] == '\n')
                counter++;

        if (feof(file))
            break;
    }
    fclose(file);
    return counter;
}

int login(string username, string password, User* users, int len){
    int isLoggedIn = 0;
    for(int i = 0; i < len; i++){
        if(strcmp(users[i].username, username) == 0){
            if(strcmp(SHA256(password), users[i].password) == 0){
                printf("Login berhasil\n");
                return 1;
            }
        }
    }
    printf("Login gagal, silahkan coba lagi\n");
    return 0;
}

// membuka file database/users.csv dan memproses file tersebut
// file akan dipisah per baris, dan baris pertama akan dihiraukan (header csv)
// tiap baris akan dilakukan string splitting pada tiap karakter koma, menghasilkan array string
// elemen dari array tersebut yaitu:
// 0: id user
// 1: username user
// 2: password user dalam bentuk hash sha256
void loadUsers(FILE *file, User* users, int len){
    fseek(file, 0L, SEEK_END);
    int size = ftell(file);
    rewind(file);
    char ch;
    string db = (string) malloc(size * sizeof(char));
    for(int i = 0; i < size; i++){
        ch = getc(file);
        *(&db[0] + i) = ch;   
    }
    *(&db[0] + size) = '\0';
    char* perline;
    char* percomma;
    char* dbrest = db;
    char* linerest;
    int i = 0;
    while((perline = strtok_r(dbrest, "\n", &dbrest))){
        if(i == 0){
            i++;
            continue;
        }
        linerest = perline;
        users[i - 1].id = atoi(strtok_r(linerest, ",", &linerest));
        strcpy(users[i - 1].username, strtok_r(linerest, ",", &linerest));
        strcpy(users[i - 1].password, strtok_r(linerest, ",", &linerest));
        i++;
    }

}

int main(){

    printf("  _____                              _ _    _____               \n");
    printf(" |  __ \\                            | (_)  / ____|              \n");
    printf(" | |__) |__ _ __ _ __ ___   __ _  __| |_  | |     __ _ _ __ ___ \n");
    printf(" |  ___/ _ \\ '__| '_ ` _ \\ / _` |/ _` | | | |    / _` | '__/ _ \\\n");
    printf(" | |  |  __/ |  | | | | | | (_| | (_| | | | |___| (_| | | |  __/\n");
    printf(" |_|   \\___|_|  |_| |_| |_|\\__,_|\\__,_|_|  \\_____\\__,_|_|  \\___|\n");

    int usersAmount = countLines("database/users.csv");
    FILE *fdu = fopen("database/users.csv", "r");
    printf("loaded %d users\n", usersAmount);
    User users[usersAmount];
    loadUsers(fdu, users, usersAmount);
    fclose(fdu);

    char username[100], password[100];
    printf("\nv. 0.1 Alpha\n");
    do{
        printf("Login -\nMasukan Username: ");
        gets(username);
        printf("Masukan Password: ");
        gets(password);
    } while (login(username, password, users, usersAmount) != 1);
    return 0;
}