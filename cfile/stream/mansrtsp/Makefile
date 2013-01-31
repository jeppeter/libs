CFLAGS += -Wall  -g

all: mansrtsp

mansrtsp: mansrtsp.o test.o

mansrtsp.o test.o: mansrtsp.h

clean:
	rm -f *.o tags

distclean: clean
	rm -f mansrtsp

