all: build

build:
	gcc main.c tree.c -o sd_fs -g -std=c99

clean:
	rm sd_fs

run:
	./sd_fs
