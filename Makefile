CC=gcc
CCFLAGS=-c -Wall -Werror -fpic -Bsymbolic -ggdb
LIBS=-ldl
LDFLAGS=-shared
OBJS=glmd.o glxhooks.o glhooks.o glmdgl.o


build: glmd.so

%.o : %.c
	${CC} ${CCFLAGS} ${LIBS} $< -D_GNU_SOURCE 

glmd.so : ${OBJS}
	${CC} ${LDFLAGS} -o $@ $^ 

install: build
	cp ./glmd.so /usr/lib
	ldconfig

clean:
	export LD_PRELOAD=
	rm -f ${OBJS}
	rm -f glmd.so
	rm -f ./example


example: build example.c
	${CC} -fsanitize=address -lasan -lglut -lGL -o  $@ example.c -ggdb

test: build example
	LD_PRELOAD=libasan.so:${PWD}/glmd.so ./example

gears: build
	LD_PRELOAD=${PWD}/glmd.so glxgears


