#include <iostream>
#include <windows.h>
#include <string>
#include <chrono>
#include <ctime>

#define SHM_SIZE 1024

char *conectar_memoria_compartida(const char *nombre_memoria) {
    HANDLE hMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, nombre_memoria);

    if (hMapFile == nullptr) {
        perror("OpenFileMapping");
        exit(EXIT_FAILURE);
    }

    char *memoria_compartida = static_cast<char *>(MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, SHM_SIZE));

    if (memoria_compartida == nullptr) {
        perror("MapViewOfFile");
        CloseHandle(hMapFile);
        exit(EXIT_FAILURE);
    }

    return memoria_compartida;
}

void desconectar_memoria_compartida(char *memoria_compartida) {
    if (!UnmapViewOfFile(memoria_compartida)) {
        perror("UnmapViewOfFile");
        exit(EXIT_FAILURE);
    }
}

void escritor(const char *nombre_memoria) {
    char *memoria_compartida = conectar_memoria_compartida(nombre_memoria);

    time_t rawtime;
    struct tm *infoFecha;
    char textoFecha[80];

    while (true) {
        std::cout << "Escribe un texto para transmitir a la memoria compartida (presiona 'x' para terminar): ";
        std::cout.flush();

        char msgUsuario[100];
        std::cin.getline(msgUsuario, sizeof(msgUsuario));

        if (msgUsuario[0] == 'x' || msgUsuario[0] == 'X') {
            break;
        }

        time(&rawtime);
        infoFecha = localtime(&rawtime);
        strftime(textoFecha, sizeof(textoFecha), "%Y-%m-%d %H:%M:%S", infoFecha);

        // Escribe el texto en la memoria compartida con formato
        strcpy(memoria_compartida, msgUsuario);

        std::cout << "Escrito en memoria: " << msgUsuario << " - (" << textoFecha << ")\n";
    }

    desconectar_memoria_compartida(memoria_compartida);
}

int main() {
    const char *nombre_memoria = "NombreMemoriaCompartida";  // Nombre único para la memoria compartida en Windows

    HANDLE hMapFile = CreateFileMapping(INVALID_HANDLE_VALUE, nullptr, PAGE_READWRITE, 0, SHM_SIZE, nombre_memoria);

    if (hMapFile == nullptr) {
        perror("CreateFileMapping");
        exit(EXIT_FAILURE);
    }

    escritor(nombre_memoria);

    CloseHandle(hMapFile);

    return 0;
}
