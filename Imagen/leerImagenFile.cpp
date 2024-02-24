#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

// Definición de la estructura Pixel
struct Pixel {
    unsigned char blue;
    unsigned char green;
    unsigned char red;
};

// Definición de la estructura BMPHeader
#pragma pack(push, 1)
struct BMPHeader {
    char signature[2];
    int fileSize;
    int reserved;
    int dataOffset;
    int headerSize;
    int width;
    int height;
    short planes;
    short bitsPerPixel;
    int compression;
    int dataSize;
    int horizontalResolution;
    int verticalResolution;
    int colors;
    int importantColors;
};

#pragma pack(pop)
vector<vector<Pixel>> leerArchivoBMP(const char* nombreArchivo) {
    ifstream archivo(nombreArchivo, ios::binary);

    if (!archivo) {
        cerr << "No se pudo abrir el archivo BMP" << endl;
        exit(1);
    }

    BMPHeader header;
    
    archivo.read(reinterpret_cast<char*>(&header), sizeof(BMPHeader));

    std::cout << "Ancho: " << header.width << "  Alto: " << header.height << std::endl;
    
    if (header.bitsPerPixel != 24) {
        cerr << "El archivo BMP debe tener 24 bits por píxel" << endl;
        exit(1);
    }

    // Mover el puntero al inicio de los datos de píxeles
    archivo.seekg(header.dataOffset, ios::beg);
    vector<vector<Pixel>> matriz(header.height, vector<Pixel>(header.width));
    for (int i = 0; i < header.height; ++i) {
        for (int j = 0; j < header.width; ++j) {
            archivo.read(reinterpret_cast<char*>(&matriz[i][j]), sizeof(Pixel));
        }
        archivo.seekg(header.width % 4, ios::cur);
    }
    archivo.close();

    return matriz;
}

void verMatriz(int x1, int x2, int y1, int y2, vector<vector<Pixel>> matrizPixeles){
        std::cout << "Algunos valores de píxeles:" << std::endl;
        for (int i = x1; i < x2; ++i) {
            for (int j = y1; j < y2; ++j) {
                std::cout << "(" 
                    << static_cast<int>(matrizPixeles[i][j].red) << ", "
                    << static_cast<int>(matrizPixeles[i][j].green) << ", "
                    << static_cast<int>(matrizPixeles[i][j].blue) << 
                ") ";
            }
            std::cout << std::endl;
        }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cerr << "Uso: " << argv[0] << " <nombre_del_archivo.bmp>" << endl;
        return 1;
    }

    vector<vector<Pixel>> matriz = leerArchivoBMP(argv[1]);

    verMatriz(100, 105, 200, 205, matriz);

    return 0;
}
