LIB_NAME=h264enc

all:
	${CC} -O2 -Wall -Wextra -Werror -pedantic -c -fPIC lib$(LIB_NAME).c -o $(LIB_NAME).o
	#gcc -shared -Wl,-soname,lib$(LIB_NAME).o -lx264 -lm -o lib$(LIB_NAME).so \
	# $(LIB_NAME).o
	ar rcs lib${LIB_NAME}.a $(LIB_NAME).o
	make -C tests/

test:
	make -C tests/

clean:
	rm -f *.so
	rm -f *.o
	rm -f *.a
	rm -f tests/test*

