#include <iostream>
#include <fstream>
#include <iomanip>

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

    unsigned char byte;
    int count = 0;

    while (file.read(reinterpret_cast<char*>(&byte), 1)) {
        std::cout << std::hex << std::setw(2) << std::setfill('0') 
                  << (int)byte << " ";
        count++;
        if (count % 16 == 0) std::cout << std::endl;
    }

    std::cout << std::endl;
    return 0;
}