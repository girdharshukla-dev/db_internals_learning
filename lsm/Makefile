put:
	gcc db.c memtable.c wal.c main.c -o lsm

replay:
	gcc db.c memtable.c wal.c main2.c -o lsm

clean:
	rm -rf lsm wal.log