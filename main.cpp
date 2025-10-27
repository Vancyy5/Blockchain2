#include "vartotojas.h"
#include <iostream>
#include <vector>
#include <memory>
#include <random>
#include <iomanip>
#include <algorithm>

// Vardų sąrašas generavimui
const std::vector<std::string> VARDAI = {
    "Vanesa", "Jokūbas", "Gabija", "Vakarė", "Patrcija",
    "Margarita", "Tomas", "Mantas", "Lukas", "Matas",
    "Paulius", "Andrius", "Vytautas", "Rokas", "Laura",
    "Marija", "Ona", "Rasa", "Eglė", "Indrė",
    "Kristina", "Laura", "Gabrielė", "Ugnė", "Justina",
    "Ieva", "Greta", "Emilija", "Austėja", "Viktorija"
};

// Funkcija vartotojų generavimui
std::vector<std::unique_ptr<Vartotojas>> generuotiVartotojus(int kiekis) {
    std::vector<std::unique_ptr<Vartotojas>> vartotojai;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> vardas_dis(0, VARDAI.size() - 1);
    std::uniform_real_distribution<> balansas_dis(100.0, 1000000.0);
    
    std::cout << "\nGeneruojami " << kiekis << " vartotojai...\n";
    std::cout << "Prašome palaukti...\n\n";
    
    for (int i = 0; i < kiekis; ++i) {
        std::string vardas = VARDAI[vardas_dis(gen)] + std::to_string(i + 1);
        double balansas = balansas_dis(gen);
        
        vartotojai.push_back(std::make_unique<Vartotojas>(vardas, balansas));
        
        // Progreso indikatorius
        if ((i + 1) % 100 == 0) {
            std::cout << "Sugeneruota: " << (i + 1) << " / " << kiekis << "\n";
        }
    }
    
    std::cout << "\n✓ Sėkmingai sugeneruoti " << kiekis << " vartotojai!\n\n";
    return vartotojai;
}

// Pagrindinis meniu
void rodytiMeniu() {
    std::cout << "1. Generuoti 1000 vartotojų\n";
    std::cout << "0. Išeiti\n";
    std::cout << "\nPasirinkimas: ";
}

int main() {
    std::vector<std::unique_ptr<Vartotojas>> vartotojai;
    int pasirinkimas;
    
    while (true) {
        rodytiMeniu();
        std::cin >> pasirinkimas;
        
        // Įvesties validacija
        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            std::cout << "\nNeteisingas pasirinkimas! Bandykite dar kartą.\n";
            continue;
        }
        
        switch (pasirinkimas) {
            case 1: {
                vartotojai = generuotiVartotojus(1000);
                break;
            }
            case 0: {
                std::cout << "\nAčiū, kad naudojotės programa!\n";
                return 0;
            }
            default: {
                std::cout << "\nNeteisingas pasirinkimas! Bandykite dar kartą.\n";
            }
        }
    }
    
    return 0;
}