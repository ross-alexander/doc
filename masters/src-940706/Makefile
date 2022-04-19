C++ = c++
CC = gcc
CFLAGS = -DDEBUG
BINDIR = /dept/ccc/rossa/bin

all: demux hexdump dump arrival correct stat varplot

varplot: varplot.o
	$(CC) varplot.o -lm -o varplot

stat: stat.o
	$(C++) stat.o -o stat

stat.o: stat.cc
	$(C++) $(CFLAGS) -O2 -c stat.cc -o stat.o

correct: correct.o

dump: dump.o generic.o

arrival: arrival.o generic.o

demux: demux.o cond.o lex.o

hexdump: hexdump.o

lex.c: lex.l
	$(LEX) lex.l
	mv lex.yy.c lex.c

install: all
	cp demux hexdump dump arrival correct $(BINDIR)

clean:
	rm stat varplot demux hexdump dump arrival correct *.o *~
