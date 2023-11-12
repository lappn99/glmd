CC=gcc
CCFLAGS=-c -Wall -Werror -fpic -Bsymbolic -ggdb
LIBS=-ldl -lGLEW
build: glmd.so

%.o : %.c
	${CC} ${CCFLAGS} ${LIBS} $< -D_GNU_SOURCE 

glmd.so : glmd.o glxhooks.o glhooks.o glmdgl.o
	${CC} -shared -o $@ $^ 

install: build
	cp ./glmd.so /usr/lib
	ldconfig

clean:
	export LD_PRELOAD=
	rm -f glmd.o
	rm -f glmd.so
	rm -f glxhooks.o
	rm -f glhooks.o
	rm -f glmdgl.o
	rm -f ./example


example: build example.c
	${CC} -lglfw -o $@ example.c -ggdb

test: build
	LD_PRELOAD=${PWD}/glmd.so glxgears

