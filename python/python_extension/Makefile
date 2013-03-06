# this to test for the make


all:libalgo.so mandelbrot.so

libalgo.so:algo.o
	gcc -pthread -shared -Wl,-O1 -Wl,-Bsymbolic-functions -Wl,-Bsymbolic-functions algo.o -o libalgo.so
algo.o:algo.c
	gcc -pthread -fno-strict-aliasing -DNDEBUG -g -fwrapv -O2 -Wall -Wstrict-prototypes -fPIC -c algo.c -o algo.o

mandelbrot.so:build/lib.linux-i686-2.7/mandelbrot.so
	cp $< $@

build/lib.linux-i686-2.7/mandelbrot.so:
	python setup.py build



clean:
	rm -rf build/
	rm -f *.o *.so

install:all
	python setup.py install --verbose
	install libalgo.so /usr/lib/
