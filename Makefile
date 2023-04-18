CFLAGS = `pkg-config --cflags --libs wpe-webkit-1.0`

all: webkit-content-filter-validator compile_flags.txt

webkit-content-filter-validator: src/main.c
	gcc src/main.c -o webkit-content-filter-validator $(CFLAGS)

# for clangd IDE integration
compile_flags.txt:
	printf '%s\n' $(CFLAGS) > compile_flags.txt

clean:
	rm -f compile_flags.txt
	rm -f webkit-content-filter-validator
