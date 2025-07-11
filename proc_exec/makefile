.PHONY:all
all:procexec testexec

procexec:procexec.c
	gcc -o $@ $^
testexec:testexec.cc
	g++ -o $@ $^ -std=c++11
.PHONY:clean
clean:
	rm procexec testexec
