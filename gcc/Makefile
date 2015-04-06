CC=gcc
CFLAGS=-pedantic -Wall -Wextra -Wno-long-long -DXCFG_FORMAT_LONGLONG=1


all: xformattest xformattable


xformattest: ../xformatc.c ../xformattest.c Makefile
	$(CC) $(CFLAGS) ../xformattest.c ../xformatc.c -o xformattest 

xformattable: ../xformatc.c ../xformattable.c Makefile
	$(CC) $(CFLAGS) ../xformattable.c -o xformattable


clean:
	rm -fr build/*