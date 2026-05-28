#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstring>
#include <vector>
#include <string>
#include <cstdint>

// Structure de l'en-tête d'un ISO Wii
struct WiiHeader {
    char gameID[6];        // Ex: "RSOP8P" pour Sonic Unleashed Wii
    char padding[2];
    char gameName[64];     // Nom complet du jeu
};

// Lit un entier 32 bits en big-endian (format Wii)
uint32_t readU32BE(std::ifstream& file) {
    unsigned char bytes[4];
    file.read(reinterpret_cast<char*>(bytes), 4);
    return (bytes[0] << 24) | (bytes[1] << 16) | (bytes[2] << 8) | bytes[3];
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Usage: iso_reader <fichier.iso>" << std::endl;
        return 1;
    }

    std::ifstream file(argv[1], std::ios::binary);
    if (!file.is_open()) {
        std::cout << "Erreur: impossible d'ouvrir le fichier" << std::endl;
        return 1;
    }

    // Lire l'en-tête
    WiiHeader header;
    file.read(reinterpret_cast<char*>(&header), sizeof(WiiHeader));

    std::cout << "=== Wii ISO Reader ===" << std::endl;
    std::cout << "Game ID   : " << std::string(header.gameID, 6) << std::endl;
    std::cout << "Game Name : " << std::string(header.gameName, 64) << std::endl;

    // Vérifier le magic number Wii à l'offset 0x18
    file.seekg(0x18);
    uint32_t magic = readU32BE(file);
    if (magic == 0x5D1C9EA3) {
        std::cout << "Format    : Wii ISO valide ✓" << std::endl;
    } else {
        std::cout << "Format    : Non reconnu (magic: " << std::hex << magic << ")" << std::endl;
    }

    file.close();
    return 0;
}