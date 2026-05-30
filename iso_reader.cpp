#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstring>
#include <vector>
#include <string>
#include <cstdint>

// Structure de l'en-tête d'un ISO Wii
struct WiiHeader {
    char gameID[6];
    char disc_number;
    char disc_version;
    char audio_streaming;
    char streaming_buf_size;
    char padding[2];
    char gameName[64];
};

// Lit un entier 32 bits en big-endian (format Wii)
uint32_t readU32BE(std::ifstream& file) {
    unsigned char bytes[4];
    file.read(reinterpret_cast<char*>(bytes), 4);
    return (bytes[0] << 24) | (bytes[1] << 16) | (bytes[2] << 8) | bytes[3];
}

void readPartitionTable(std::ifstream& file) {
    // La table des partitions est à l'offset 0x40000
    file.seekg(0x40000);
    
    uint32_t partCount = readU32BE(file);
    uint32_t partTableOffset = readU32BE(file) << 2;
    
    std::cout << "\n=== Table des partitions ===" << std::endl;
    std::cout << "Nombre de partitions : " << std::dec << partCount << std::endl;
    
    // Lire chaque partition
    file.seekg(partTableOffset);
    for (uint32_t i = 0; i < partCount; i++) {
        uint32_t partOffset = readU32BE(file) << 2;
        uint32_t partType = readU32BE(file);
        
        std::cout << "Partition " << i << " : offset=0x" 
                  << std::hex << partOffset 
                  << " type=" << (partType == 0 ? "DATA" : "UPDATE")
                  << std::endl;
    }
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

    // Game ID est toujours aux premiers 6 bytes
    char gameID[7] = {0};
    file.read(gameID, 6);

    // Game Name commence exactement à l'offset 0x20
    char gameName[65] = {0};
    file.seekg(0x20);
    file.read(gameName, 64);

    std::cout << "=== Wii ISO Reader ===" << std::endl;
    std::cout << "Game ID   : " << gameID << std::endl;
    std::cout << "Game Name : " << gameName << std::endl;

    // Vérifier le magic number Wii à l'offset 0x18
    file.seekg(0x18);
    uint32_t magic = readU32BE(file);
    if (magic == 0x5D1C9EA3) {
        std::cout << "Format    : Wii ISO valide ✓" << std::endl;
    } else {
        std::cout << "Format    : Non reconnu (magic: " << std::hex << magic << ")" << std::endl;
    }
    
    readPartitionTable(file);
    file.close();
    return 0;
}