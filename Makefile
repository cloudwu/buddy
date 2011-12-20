.Phony : all clean

all : test

test : test.c buddy.c
	gcc -g -Wall -o $@ $^

clean : 
	rm -f test test.exe
	
