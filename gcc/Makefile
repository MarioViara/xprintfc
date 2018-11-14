CC=gcc
UFLAGS=
CFLAGS=${UFLAGS} -O3 -pedantic -Wall -Wextra -Wno-long-long 


all: xformattest xformattable xformatspeed


xformattest: ../src/xformatc.c ../src/xformattest.c ../src/xformatc.h Makefile
	$(CC) $(CFLAGS) ../src/xformattest.c ../src/xformatc.c -o xformattest 

xformattable: ../src/xformatc.c ../src/xformattable.c Makefile
	$(CC) $(CFLAGS) ../src/xformattable.c -o xformattable

xformatspeed: ../src/xformatc.c ../src/xformatspeed.c Makefile
	$(CC) $(CFLAGS) ../src/xformatspeed.c ../src/xformatc.c  -o xformatspeed



clean:
	rm -fr *.o *.exe xformattest xformattable xformatspeed
