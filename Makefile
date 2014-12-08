
help:
	@echo "make test  - will test the library vs your system library"
	@echo "make table - will make the decoding table"
	@echo "make clen  - will remove  all object files"

	
test: Makefile xformatc.c xformatc.h
	gcc -DXCFG_FORMAT_TEST xformatc.c -o test

table: Makefile xformatc.c xformatc.h
	gcc -DXCFG_FORMAT_MAKE xformatc.c -o table
clean:
	rm -f *.o test table
