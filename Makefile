.Phony : all clean

all : test

test : test.c buddy.c
	gcc -g -Wall -std=c99 -o $@ $^

clean : 
	rm -f test test.exe
	
