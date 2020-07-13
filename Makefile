
build:
	mkdir build
	gcc servidor.c -o build/servidor.out
	gcc cliente.c -o build/cliente.out

clean:
	rm -rf build
	