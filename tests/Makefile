LIB_NAME=h264enc

all:
	${CC} -g -I../ -I../modules/ -L../ encode.c -o encode -l:lib$(LIB_NAME).a -lx264
