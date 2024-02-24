#include <iostream>
#include <windows.h>
#include <signal.h>
#include <cstring>
#include <ctime>
#include <thread>
#include <atomic> // Agregar esta línea

#define SHM_SIZE 1024

volatile std::atomic<bool> stop(false);

void sigint_handler(int signum) {
    stop = true;
}

void *conectar_memoria_compartida(const char *nombre_memoria) {
    HANDLE hMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, nombre_memoria);

    if (hMapFile == nullptr) {
        perror("OpenFileMapping");
        exit(EXIT_FAILURE);
    }

    void *memoria_compartida = MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, SHM_SIZE);

    if (memoria_compartida == nullptr) {
        perror("MapViewOfFile");
        CloseHandle(hMapFile);
        exit(EXIT_FAILURE);
    }

    return memoria_compartida;
}

void desconectar_memoria_compartida(void *memoria_compartida) {
    if (!UnmapViewOfFile(memoria_compartida)) {
        perror("UnmapViewOfFile");
        exit(EXIT_FAILURE);
    }
}

void lectorTeclado() {
    char ch;
    while (true) {
        std::cout << "Presiona 'x' para terminar! \n";
        std::cout.flush();

        ch = getchar();
        if (ch == 'x' || ch == 'X') {
            stop = true;
            break;
        }
    }
}

void lectorMemoria(const char *nombre_memoria) {
    char *memoria_compartida = static_cast<char *>(conectar_memoria_compartida(nombre_memoria));

    signal(SIGINT, sigint_handler);

    // Iniciar hilo para atender el teclado
    std::thread tecladoThread(lectorTeclado);

    char valor_leido_anterior[SHM_SIZE] = "";

    while (!stop) {
        time_t rawtime;
        struct tm *infoFechaHora;
        char textoFechaHora[80];

        if (strcmp(valor_leido_anterior, memoria_compartida) != 0) {
            time(&rawtime);
            infoFechaHora = localtime(&rawtime);
            strftime(textoFechaHora, sizeof(textoFechaHora), "%Y-%m-%d %H:%M:%S", infoFechaHora);
            std::cout << textoFechaHora << " = " << memoria_compartida << "\n";
            strcpy(valor_leido_anterior, memoria_compartida);
        }

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    tecladoThread.join();

    desconectar_memoria_compartida(memoria_compartida);
}

int main() {
    const char *nombre_memoria = "NombreMemoriaCompartida";

    HANDLE hMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, nombre_memoria);

    if (hMapFile == nullptr) {
        perror("OpenFileMapping");
        exit(EXIT_FAILURE);
    }

    lectorMemoria(nombre_memoria);

    CloseHandle(hMapFile);

    return 0;
}
