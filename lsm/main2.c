#include "db.h"

#include <stdio.h>

int main(){
    struct db_type *db = db_open("./wal.log");

    uint64_t value;

    if(db_get(db, 1, &value) == 0){
        printf("1 -> %ld\n", value);
    }else{
        printf("1 not found\n");
    }

    if(db_get(db, 2, &value) == 0){
        printf("2 -> %ld\n", value);
    }else{
        printf("2 not found\n");
    }
    return 0;
}