
help:
	@echo "make test  - will test the library vs your system library"
	@echo "make table - will make the decoding table"
	@echo "make clean - will remove  all object files"

	
list:
	gcc -O3 -Wa,-a,-ad -pedantic -Wall -Wextra -c xformatc.c >xformatc.lst


test: Makefile xformatc.c xformatc.h
	gcc -pedantic -Wall -Wextra -DXCFG_FORMAT_TEST xformatc.c -o test

table: Makefile xformatc.c xformatc.h
	gcc -DXCFG_FORMAT_MAKE xformatc.c -o table

clean:
	rm -f *.o test* table*
