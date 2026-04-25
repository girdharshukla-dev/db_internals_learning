#include "db.h"

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int main(){
  struct db_type *db = db_open("./wal.log");
	db_put(db, 1, 100);
	db_put(db, 1, 300);
	db_put(db, 2, 200);
	uint64_t value;
	int check = db_get(db, 1, &value);
	if(check == -1) fprintf(stderr, "Value of check not found\n");
	fprintf(stdout, "Value of check is: %ld\n", value);
	return 0;
}