#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <thread>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sstream>

#define MAX_CLIENTES 10
#define MAX_FILAS 6
#define MAX_COLUMNAS 7

using namespace std;

// tablero
class Tablero {
public:
    Tablero() {
        inicializar();
    }

    void inicializar() {
        for (int i = 0; i < MAX_FILAS; i++) {
            for (int j = 0; j < MAX_COLUMNAS; j++) {
                tablero[i][j] = ' ';
            }
        }
    }

    string mostrar() {
        stringstream ss;
        ss << "TABLERO\n";
        for (int i = 0; i < MAX_FILAS; i++) {
            ss << i + 1;
            for (int j = 0; j < MAX_COLUMNAS; j++) {
                ss << " " << tablero[i][j];
            }
            ss << "\n";
        }
        ss << "-------------\n";
        ss << " 1 2 3 4 5 6 7\n";
        return ss.str();
    }

    bool realizarJugada(char jugador, int columna) {
        columna--;
        if (columna < 0 || columna >= MAX_COLUMNAS || tablero[0][columna] != ' ') {
            return false;
        }

        for (int i = MAX_FILAS - 1; i >= 0; i--) {
            if (tablero[i][columna] == ' ') {
                tablero[i][columna] = jugador;
                return true;
            }
        }

        return false;
    }

    bool hayGanador(char jugador) {
        // filas
        for (int i = 0; i < MAX_FILAS; i++) {
            for (int j = 0; j <= MAX_COLUMNAS - 4; j++) {
                if (tablero[i][j] == jugador && tablero[i][j + 1] == jugador && tablero[i][j + 2] == jugador && tablero[i][j + 3] == jugador) {
                    return true;
                }
            }
        }

        // Verificar columnas
        for (int i = 0; i <= MAX_FILAS - 4; i++) {
            for (int j = 0; j < MAX_COLUMNAS; j++) {
                if (tablero[i][j] == jugador && tablero[i + 1][j] == jugador && tablero[i + 2][j] == jugador && tablero[i + 3][j] == jugador) {
                    return true;
                }
            }
        }

        // diagonales hacia arriba
        for (int i = 0; i <= MAX_FILAS - 4; i++) {
            for (int j = 0; j <= MAX_COLUMNAS - 4; j++) {
                if (tablero[i][j] == jugador && tablero[i + 1][j + 1] == jugador && tablero[i + 2][j + 2] == jugador && tablero[i + 3][j + 3] == jugador) {
                    return true;
                }
            }
        }

        // diagonales hacia abajo
        for (int i = 0; i <= MAX_FILAS - 4; i++) {
            for (int j = 3; j < MAX_COLUMNAS; j++) {
                if (tablero[i][j] == jugador && tablero[i + 1][j - 1] == jugador && tablero[i + 2][j - 2] == jugador && tablero[i + 3][j - 3] == jugador) {
                    return true;
                }
            }
        }

        return false;
    }

    bool hayEmpate() {
        for (int i = 0; i < MAX_COLUMNAS; i++) {
            if (tablero[0][i] == ' ') {
                return false;
            }
        }
        return true;
    }

private:
    char tablero[MAX_FILAS][MAX_COLUMNAS];
};

// jugador 
class Jugador {
public:
    Jugador(char id) : id(id), puntaje(0) {}

    char getId() {
        return id;
    }

    int getPuntaje() {
        return puntaje;
    }

    void incrementarPuntaje() {
        puntaje++;
    }

private:
    char id;
    int puntaje;
};

// juego entre clientes
void manejarJuego(int socketCliente1, int socketCliente2) {
    Tablero tablero;
    Jugador jugador1('S'), jugador2('C');
    char turnoJugador = 'S';
    bool juegoTerminado = false;

    // info inicial
    string mensajeInicio = "Bienvenido al juego Cuatro en Línea!\n";
    send(socketCliente1, mensajeInicio.c_str(), mensajeInicio.length(), 0);
    send(socketCliente2, mensajeInicio.c_str(), mensajeInicio.length(), 0);

    // quien parte el juego
    srand(time(NULL));
    if (rand() % 2 == 0) {
        turnoJugador = 'S';
    } else {
        turnoJugador = 'C';
    }

    string mensajeTurno = "Comienza el jugador " + string(1, turnoJugador) + ".\n";
    send(socketCliente1, mensajeTurno.c_str(), mensajeTurno.length(), 0);
    send(socketCliente2, mensajeTurno.c_str(), mensajeTurno.length(), 0);

    while (!juegoTerminado) {
        int columnaJugada;
        char jugadorActual = (turnoJugador == 'S') ? jugador1.getId() : jugador2.getId();

        // solicita jugada
        string mensajeSolicitud = "Ingrese la columna (1-7): ";
        send((turnoJugador == 'S') ? socketCliente1 : socketCliente2, mensajeSolicitud.c_str(), mensajeSolicitud.length(), 0);

        // recibe jugada
        char bufferRecibido[2];
        int bytesRecibidos = recv((turnoJugador == 'S') ? socketCliente1 : socketCliente2, bufferRecibido, sizeof(bufferRecibido), 0);
        bufferRecibido[bytesRecibidos] = '\0';
        columnaJugada = atoi(bufferRecibido);

        // jugada en el tablero
        if (tablero.realizarJugada(jugadorActual, columnaJugada)) {
            // actualiza tablero
            string tableroCadena = tablero.mostrar();
            send(socketCliente1, tableroCadena.c_str(), tableroCadena.length(), 0);
            send(socketCliente2, tableroCadena.c_str(), tableroCadena.length(), 0);

            // verificador de triunfo o empate
            if (tablero.hayGanador(jugadorActual)) {
                string mensajeGanador = "El jugador " + string(1, jugadorActual) + " ha ganado el juego!\n";
                send(socketCliente1, mensajeGanador.c_str(), mensajeGanador.length(), 0);
                send(socketCliente2, mensajeGanador.c_str(), mensajeGanador.length(), 0);

                if (jugadorActual == jugador1.getId()) {
                    jugador1.incrementarPuntaje();
                } else {
                    jugador2.incrementarPuntaje();
                }

                juegoTerminado = true;
            } else if (tablero.hayEmpate()) {
                string mensajeEmpate = "El juego ha terminado en empate.\n";
                send(socketCliente1, mensajeEmpate.c_str(), mensajeEmpate.length(), 0);
                send(socketCliente2, mensajeEmpate.c_str(), mensajeEmpate.length(), 0);
                juegoTerminado = true;
            }

            // cambio de turno
            turnoJugador = (turnoJugador == 'S') ? 'C' : 'S';
        } else {
            string mensajeError = "Jugada inválida. Intente nuevamente.\n";
            send((turnoJugador == 'S') ? socketCliente1 : socketCliente2, mensajeError.c_str(), mensajeError.length(), 0);
        }
    }

    // cierre de sockets 
    close(socketCliente1);
    close(socketCliente2);

    cout << "Juego terminado entre [" << socketCliente1 << "] y [" << socketCliente2 << "]" << endl;
    cout << "Puntaje final: " << "Jugador 1 (S): " << jugador1.getPuntaje() << ", Jugador 2 (C): " << jugador2.getPuntaje() << endl;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cout << "Uso: " << argv[0] << " <puerto>" << endl;
        return 1;
    }

    int puerto = atoi(argv[1]);

    // socket server
    int socketServidor = socket(AF_INET, SOCK_STREAM, 0);
    if (socketServidor == -1) {
        cerr << "Error al crear el socket del servidor" << endl;
        return 1;
    }

    // configuracion socket serber
    sockaddr_in direccionServidor;
    direccionServidor.sin_family = AF_INET;
    direccionServidor.sin_addr.s_addr = INADDR_ANY;
    direccionServidor.sin_port = htons(puerto);

    // vinculo socket con direccion server
    if (bind(socketServidor, (sockaddr*)&direccionServidor, sizeof(direccionServidor)) == -1) {
        cerr << "Error al vincular el socket con la dirección del servidor" << endl;
        return 1;
    }

    // conexiones
    if (listen(socketServidor, MAX_CLIENTES) == -1) {
        cerr << "Error al escuchar conexiones entrantes" << endl;
        return 1;
    }

    cout << "Servidor iniciado en el puerto " << puerto << ". Esperando conexiones..." << endl;

    vector<int> socketClientes;

    while (true) {
        // aceptar conexion
        sockaddr_in direccionCliente;
        socklen_t longitudDireccion = sizeof(direccionCliente);
        int socketCliente = accept(socketServidor, (sockaddr*)&direccionCliente, &longitudDireccion);
        if (socketCliente == -1) {
            cerr << "Error al aceptar una nueva conexión" << endl;
            continue;
        }

        // agregar socket de cliente a la lista
        socketClientes.push_back(socketCliente);
        cout << "Nueva conexión desde: " << inet_ntoa(direccionCliente.sin_addr) << ":" << ntohs(direccionCliente.sin_port) << endl;

        // verificador de clientes para comenzar
        if (socketClientes.size() >= 2) {
            
            int socketCliente1 = socketClientes[0];
            int socketCliente2 = socketClientes[1];
            socketClientes.erase(socketClientes.begin(), socketClientes.begin() + 2);
            thread hiloJuego(manejarJuego, socketCliente1, socketCliente2);
            hiloJuego.detach();
        }
    }

    // cerrar sesion socket servidor 
    close(socketServidor);

    return 0;
}
