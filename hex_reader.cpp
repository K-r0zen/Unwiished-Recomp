#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>

void printHexLine(unsigned long offset, std::vector<unsigned char>& buffer, int size) {
    // Affiche l'offset
    std::cout << std::hex << std::setw(8) << std::setfill('0') << offset << "  ";

    // Affiche les bytes en hex
    for (int i = 0; i < 16; i++) {
        if (i < size)
            std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)buffer[i] << " ";
        else
            std::cout << "   "; // padding si ligne incomplete
        if (i == 7) std::cout << " "; // espace au milieu
    }

    std::cout << " |";

    // Affiche les caractères ASCII
    for (int i = 0; i < size; i++) {
        if (buffer[i] >= 32 && buffer[i] <= 126)
            std::cout << buffer[i]; // caractère lisible
        else
            std::cout << "."; // caractère non lisible
    }

    std::cout << "|" << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Usage: hex_reader <fichier>" << std::endl;
        return 1;
    }

    std::ifstream file(argv[1], std::ios::binary);
    if (!file.is_open()) {
        std::cout << "Erreur: impossible d'ouvrir le fichier" << std::endl;
        return 1;
    }

    std::vector<unsigned char> buffer(16);
    unsigned long offset = 0;

    while (file.read(reinterpret_cast<char*>(buffer.data()), 16)) {
        int bytesRead = file.gcount();
        printHexLine(offset, buffer, bytesRead);
        offset += bytesRead;
    }

    // Derniere ligne si incomplete
    int remaining = file.gcount();
    if (remaining > 0) {
        printHexLine(offset, buffer, remaining);
    }

    return 0;
}