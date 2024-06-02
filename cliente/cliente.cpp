#include <iostream>
#include <string>
#include <unistd.h>
#include <arpa/inet.h>
#include <cstring>  

#define MAX_BUFFER 1024

using namespace std;

void mostrarTablero(const string &tablero) {
    cout << tablero << endl;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        cout << "Uso: " << argv[0] << " <IP> <puerto>" << endl;
        return 1;
    }

    const char* ip = argv[1];
    int puerto = atoi(argv[2]);

    // Crear el socket del cliente
    int socketCliente = socket(AF_INET, SOCK_STREAM, 0);
    if (socketCliente == -1) {
        cerr << "Error al crear el socket del cliente" << endl;
        return 1;
    }

    // Configurar la dirección del servidor
    sockaddr_in direccionServidor;
    direccionServidor.sin_family = AF_INET;
    direccionServidor.sin_port = htons(puerto);
    inet_pton(AF_INET, ip, &direccionServidor.sin_addr);

    // Conectar al servidor
    if (connect(socketCliente, (sockaddr*)&direccionServidor, sizeof(direccionServidor)) == -1) {
        cerr << "Error al conectar al servidor" << endl;
        return 1;
    }

    char buffer[MAX_BUFFER];
    while (true) {
        // Recibir mensaje del servidor
        int bytesRecibidos = recv(socketCliente, buffer, sizeof(buffer) - 1, 0);
        if (bytesRecibidos <= 0) {
            break;
        }
        buffer[bytesRecibidos] = '\0';

        // Mostrar mensaje recibido
        cout << buffer << endl;

        // Si el mensaje pide una jugada, solicitar al usuario
        if (strstr(buffer, "Ingrese la columna")) {
            int columna;
            cout << "Columna: ";
            cin >> columna;
            string jugada = to_string(columna) + "\n";
            send(socketCliente, jugada.c_str(), jugada.length(), 0);
        }
    }

    // Cerrar el socket del cliente
    close(socketCliente);

    return 0;
}
