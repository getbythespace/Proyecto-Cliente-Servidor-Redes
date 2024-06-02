all: servidor_bin cliente_bin

servidor_bin: servidor/servidor.cpp
	g++ -o servidor_bin servidor/servidor.cpp -pthread

cliente_bin: cliente/cliente.cpp
	g++ -o cliente_bin cliente/cliente.cpp

clean:
	rm -f servidor_bin cliente_bin
