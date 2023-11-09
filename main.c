#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "lib/sha256.h"

#define BUF_SIZE 65536
#define DEBUG 1

// buat tipe data string untuk mempermudah development
typedef char* string;

// tipe data user, menyimpan data tiap user sebagai object
typedef struct Users{
    int id;
    char username[100];
    char password[100];
} User;

//tipe data obat, menyimpan properti dari tiap obat di farmasi
typedef struct Drugs{
    int id;
    char name[100];
    float priceUSD;
    int price;
    int stock;
} Drug;

// log untuk debugging program
void LOG(char* log){
    if(DEBUG == 1){
        printf("[DEBUG] %s \n", log);
    }
}

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

// login ke sistem dengan username dan password
// users adalah array dari object user yang didapat dari file database/users.csv
// apabila login berhasil akan menampilkan "Login berhasil" dan fungsi ini akan mengembalikan nilai 1
// apabila login gagal akan menampilkan "Login gagal, silahkan coba lagi" dan fungsi ini akan mengembalikan nilai 0
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
void loadUsers(FILE *file, User* users){
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

// membuka file database/drugs.csv dan menyimpannya pada memori
// sama seperti fungsi sebelumnya, akan dipisah per baris dan per comma
void loadDrugs(FILE* file, Drug* drugs){
    fseek(file, 0L, SEEK_END);
    int size = ftell(file);
    rewind(file);
    char ch;
    string db = (string) malloc(size * sizeof(char) + 4);
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
        drugs[i - 1].id = atoi(strtok_r(linerest, ",", &linerest));
        strcpy(drugs[i - 1].name, strtok_r(linerest, ",", &linerest));
        drugs[i - 1].priceUSD = atof(strtok_r(linerest, ",", &linerest));
        drugs[i - 1].price = atoi(strtok_r(linerest, ",", &linerest));
        drugs[i - 1].stock = atoi(strtok_r(linerest, ",", &linerest));
        i++;
    }
}

void showMenu(){
    printf("Welcome! Pilih aksi yang akan dilakukan:\n");
    printf("1. List stok obat\n");
    printf("2. Cari obat\n");
    printf("3. Ubah data obat\n");
    printf("4. Hapus obat\n");
    printf("5. Tambah obat baru\n");
    printf("6. List transaksi\n");
    printf("7. Cari transaksi\n");
    printf("8. Ubah transaksi\n");
    printf("9. Hapus transaksi\n");
    printf("10. Tambah transaksi baru\n");
    printf("11. Exit\n> ");
}

int searchDrug(Drug* drugs, int len, char* column, char* value, Drug* result){
    if(strcmp(column, "id") == 0){
        for(int i = 0; i < len; i++){
            if(drugs[i].id == atoi(value)){
                *result = drugs[i];
                return 1;
            }
        }
    }
    else if(strcmp(column, "name") == 0){
        for(int i = 0; i < len; i++){
            if(strcmp(drugs[i].name, value) == 0){
                *result = drugs[i];
                return 1;
            }
        }
    }
    else if(strcmp(column, "price") == 0){
        for(int i = 0; i < len; i++){
            if(drugs[i].price == atoi(value)){
                *result = drugs[i];
                return 1;
            }
        }
    }
    else if(strcmp(column, "stock") == 0){
        for(int i = 0; i < len; i++){
            if(drugs[i].stock == atoi(value)){
                *result = drugs[i];
                return 1;
            }
        }
    }
    else{
        return 0;
    }
}

// mengubah obat dengan id yang diberikan dalam argument
// property dan value juga harus diberikan sebagai argumen
// mengembalikan nilai integer yang berupa error code, yaitu:
// 0 : obat tidak ditemukan
// 1 : perubahan berhasil
// 2 : properti yang diberikan tidak sesuai
int updateDrug(Drug* drugs, char* property, char* value, int id, int len){
    int index = -1;
    for(int i = 0; i < len; i++){
        if(drugs[i].id == id){
            index = i;
        }
    }
    int code = 0;
    if(index == -1){
        return code;
    }
    if(strcmp(property, "id") == 0){
        drugs[index].id = atoi(value);
        code = 1;
    }
    else if(strcmp(property, "name") == 0){
        strcpy(drugs[index].name, value);
        code = 1;
    }
    else if(strcmp(property, "price") == 0){
        drugs[index].price = atoi(value);
        code = 1;
    }
    else if(strcmp(property, "stock") == 0){
        drugs[index].stock = atoi(value);
        code = 1;
    }
    else{
        code = 2;
        return code;
    }
    LOG("done updating in memory data, otw save to disk");
    FILE* db = fopen("database/drugs.csv", "w");
    LOG("file opened\n");
    char* text = (char*)malloc(sizeof(Drug) * (len + 1));
    LOG("done memory allocating");
    strcpy(text, "id,name,price_usd,price,stock");
    LOG("first string copy");
    for(int i = 0; i < len; i++){
        char buf[116];
        sprintf(buf, "\n%d,%s,%.2f,%d,%d", drugs[i].id, drugs[i].name, drugs[i].priceUSD, drugs[i].price, drugs[i].stock);
        strcat(text, buf);
    }
    LOG("done doing string concat, about to print");
    fprintf(db, text);
    fclose(db);
    LOG("data written to disk");
    return code;
}


int deleteDrug(Drug* drugs, char* identifier, char* value, int len){
    int code = 0;
    int index;
    if(strcmp(identifier, "id") == 0){
        for(int i = 0; i < len; i++){
            if(drugs[i].id == atoi(value)){
                index = i;
                code = 1;
            }
        }
    }
    else if(strcmp(identifier, "name") == 0){
        for(int i = 0; i < len; i++){
            if(strcmp(drugs[i].name, value) == 0){
                index = i;
                code = 1;
            }
        }
    }
    else if(strcmp(identifier, "price") == 0){
        for(int i = 0; i < len; i++){
            if(drugs[i].price == atoi(value)){
                index = i;
                code = 1;
            }
        }
    }
    else if(strcmp(identifier, "stock") == 0){
        for(int i = 0; i < len; i++){
            if(drugs[i].stock == atoi(value)){
                index = i;
                code = 1;
            }
        }
    }
    else{
        code = 2;
        return code;
    }
    for(int i = index; i < len - 1; i++){
        drugs[i] = drugs[i + 1];
    }
    FILE* db = fopen("database/drugs.csv", "w");
    LOG("file opened\n");
    char* text = (char*)malloc(sizeof(Drug) * (len + 1));
    LOG("done memory allocating");
    strcpy(text, "id,name,price_usd,price,stock");
    LOG("first string copy");
    for(int i = 0; i < len; i++){
        char buf[116];
        sprintf(buf, "\n%d,%s,%.2f,%d,%d", drugs[i].id, drugs[i].name, drugs[i].priceUSD, drugs[i].price, drugs[i].stock);
        strcat(text, buf);
    }
    LOG("done doing string concat, about to print");
    fprintf(db, text);
    fclose(db);
    LOG("data written to disk");
    return code;
}

Drug* addDrug(Drug* drugs, char* name, int price, int stock, int len){
    Drug* drugsx = realloc(drugs, sizeof(Drug) * (len + 1));
    drugsx[len].id = drugs[len - 1].id + 1;
    strcpy(drugsx[len].name, name);
    drugsx[len].price = price;
    drugsx[len].priceUSD = (float) price / 15500;
    drugsx[len].stock = stock;
    FILE* db = fopen("database/drugs.csv", "w");
    LOG("file opened\n");
    char* text = (char*)malloc(sizeof(Drug) * (len + 1));
    LOG("done memory allocating");
    strcpy(text, "id,name,price_usd,price,stock");
    LOG("first string copy");
    for(int i = 0; i <= len; i++){
        char buf[116];
        sprintf(buf, "\n%d,%s,%.2f,%d,%d", drugsx[i].id, drugsx[i].name, drugsx[i].priceUSD, drugsx[i].price, drugsx[i].stock);
        strcat(text, buf);
    }
    LOG("done doing string concat, about to print");
    fprintf(db, text);
    fclose(db);
    LOG("data written to disk");

    return drugsx;
}

int main(){

    printf("[32m  _____                              _ _    _____               \n");
    printf(" |  __ \\                            | (_)  / ____|              \n");
    printf(" | |__) |__ _ __ _ __ ___   __ _  __| |_  | |     __ _ _ __ ___ \n");
    printf(" |  ___/ _ \\ '__| '_ ` _ \\ / _` |/ _` | | | |    / _` | '__/ _ \\\n");
    printf(" | |  |  __/ |  | | | | | | (_| | (_| | | | |___| (_| | | |  __/\n");
    printf(" |_|   \\___|_|  |_| |_| |_|\\__,_|\\__,_|_|  \\_____\\__,_|_|  \\___|[0m\n");

    int usersAmount = countLines("database/users.csv");
    FILE *fdu = fopen("database/users.csv", "r");
    User users[usersAmount];
    loadUsers(fdu, users);
    fclose(fdu);

    char username[100], password[100];
    printf("\nv. 0.1 Alpha\n");
    do{
        printf("Login -\nMasukan Username: ");
        gets(username);
        printf("Masukan Password: ");
        gets(password);
    } while (login(username, password, users, usersAmount) != 1);

    int drugsAmount = countLines("database/drugs.csv");
    printf("Loaded %d drugs\n", drugsAmount);
    FILE *fdd = fopen("database/drugs.csv", "r");
    Drug* drugs = (Drug*) malloc(sizeof(Drug) * drugsAmount);
    loadDrugs(fdd, drugs);
    fclose(fdd);
    while(1){
        showMenu();
        int input;
        char holder[5];
        gets(holder);
        input = atoi(holder);

        char property[10];
        char value[10];
        int res;
        Drug drug;
        char price[10];
        switch(input){
            char holder[10];
            case 1:
                printf("[31mId\tNama\t\t\tHarga\t\tStok[0m\n");
                for(int i = 0; i < drugsAmount; i++){
                    char tabs[10] = "\t\t";
                    if(strlen(drugs[i].name) < 8){
                        strcpy(tabs, "\t\t\t");
                    }
                    if(strlen(drugs[i].name) > 16){
                        strcpy(tabs, "\t");
                    }
                    char price[10];
                    int x = drugs[i].price / 1000;
                    int y = drugs[i].price - (x * 1000);
                    sprintf(price, "%d.%d", x, y);
                    printf("%d\t%s%sRp.%s,00\t%d\n", drugs[i].id, drugs[i].name, tabs, price, drugs[i].stock);
                }
                printf("\n");
                break;

            case 2:
                printf("Masukan properti obat yang diketahui (lowercase): ");
  
                gets(property);
                printf("Masukan nilai dari properti yang diketahui: ");
                gets(value);
                res = searchDrug(drugs, drugsAmount, property, value, &drug);
                if(res == 0){
                    printf("Tidak ada obat yang ditemukan\n");
                    break;
                }
                int x = drug.price / 1000;
                int y = drug.price - (x * 1000);
                sprintf(price, "%d.%d", x, y);
                printf("[34m%s[0m\nID: %d\nHarga: Rp.%s,00\nHarga (USD): $%.2f\nStock: %d\n\n", drug.name, drug.id, price, drug.priceUSD, drug.stock);
                break;

            case 3:
                int id;
                printf("Masukkan ID obat yang akan diubah > ");
                gets(holder);
                printf("Masukkan properti yang akan diubah > ");
                gets(property);
                printf("Masukkan nilai baru properti tersebut > ");
                gets(value);
                id = atoi(holder);
                res = updateDrug(drugs, property, value, id, drugsAmount);
                if(res == 0){
                    printf("Obat tidak ditemukan... \n");
                    break;
                }else if(res == 2){
                    printf("Properti yang diberikan tidak sesuai...\n");
                    break;
                }
                printf("Data berhasil diubah.\n");
                int index = 0;
                for(int i = 0; i < drugsAmount; i++){
                    if(drugs[i].id == id){
                        index = i;
                    }
                }
                drug = drugs[index];
                int a = drug.price / 1000;
                int b = drug.price - (a * 1000);
                sprintf(price, "%d.%d", a, b);
                printf("[34m%s[0m\nID: %d\nHarga: Rp.%s,00\nHarga (USD): $%.2f\nStock: %d\n\n", drug.name, drug.id, price, drug.priceUSD, drug.stock);

                break;
            case 4:
                printf("Masukkan properti obat yang diketahui > ");
                gets(property);
                printf("Masukkan nilai properti dari obat yang akan dihapus > ");
                gets(value);
                res = deleteDrug(drugs, property, value, drugsAmount);
                if(res == 0){
                    printf("Obat tidak ditemukan... \n");
                    break;
                }else if(res == 2){
                    printf("Properti yang diberikan tidak sesuai...\n");
                    break;
                }
                printf("Obat berhasil dihapus\n");
                break;
            case 5:
                char name[100];
                int pprice;
                int stock;

                printf("Masukan nama obat: ");
                gets(name);
                printf("Masukan harga obat: ");
                gets(price);
                pprice = atoi(price);
                printf("Masukan stock obat: ");
                gets(holder);
                stock = atoi(holder);
                drugs = addDrug(drugs, name, pprice, stock, drugsAmount);
                drugsAmount++;
                break;
            case 11:
                printf("Bye-bye!\n");
                return 0;

            default:
                printf("Masukan pilihan yang sesuai, yaitu berupa angka\n");
        }
    }
    return 0;
}