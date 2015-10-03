CC=gcc
UFLAGS=
CFLAGS=${UFLAGS} -O3 -pedantic -Wall -Wextra -Wno-long-long 


all: xformattest xformattable


xformattest: ../xformatc.c ../xformattest.c ../xformatc.h Makefile
	$(CC) $(CFLAGS) ../xformattest.c ../xformatc.c -o xformattest 

xformattable: ../xformatc.c ../xformattable.c Makefile
	$(CC) $(CFLAGS) ../xformattable.c -o xformattable


clean:
	rm -fr *.o *.exe xformattest xformattable
