all: servidor cliente

servidor: servidor/servidor.cpp
    g++ -o servidor servidor/servidor.cpp -pthread

cliente: cliente/cliente.cpp
    g++ -o cliente cliente/cliente.cpp

clean:
    rm -f servidor cliente
