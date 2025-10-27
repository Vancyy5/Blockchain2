#include "vartotojas.h"
#include "transakcija.h"
#include "blokas.h"
#include <iostream>
#include <vector>
#include <memory>
#include <random>

const std::vector<std::string> VARDAI = {
    "Vanesa", "Jokūbas", "Gabija", "Vakarė", "Patricija",
    "Margarita", "Tomas", "Mantas", "Lukas", "Matas",
    "Paulius", "Andrius", "Vytautas", "Rokas", "Laura",
    "Marija", "Ona", "Rasa", "Eglė", "Indrė"
};

int main() {
    std::vector<std::unique_ptr<Vartotojas>> vartotojai;
    TransakcijuBaseinas baseinas;
    std::unique_ptr<Blockchain> blockchain;
    
    int pasirinkimas;
    
    while (true) {
        std::cout << "\n=== BLOCKCHAIN SISTEMA ===\n";
        std::cout << "1. Generuoti 1000 vartotojų\n";
        std::cout << "2. Generuoti 10000 transakcijų\n";
        std::cout << "3. Formuoti naują bloką\n";
        std::cout << "0. Išeiti\n";
        std::cout << "Pasirinkimas: ";
        std::cin >> pasirinkimas;
        
        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            std::cout << "\nNeteisingas pasirinkimas!\n";
            continue;
        }
        
        switch (pasirinkimas) {
            case 1: {
                std::random_device rd;
                std::mt19937 gen(rd());
                std::uniform_int_distribution<> vardas_dis(0, VARDAI.size() - 1);
                std::uniform_real_distribution<> balansas_dis(100.0, 1000000.0);
                
                vartotojai.clear();
                std::cout << "\nGeneruojami 1000 vartotojai...\n";
                
                for (int i = 0; i < 1000; ++i) {
                    std::string vardas = VARDAI[vardas_dis(gen)] + std::to_string(i + 1);
                    double balansas = balansas_dis(gen);
                    vartotojai.push_back(std::make_unique<Vartotojas>(vardas, balansas));
                    
                    if ((i + 1) % 100 == 0) {
                        std::cout << "Sugeneruota: " << (i + 1) << "/1000\n";
                    }
                }
                std::cout << "✓ Sugeneruoti 1000 vartotojai!\n";
                break;
            }
            case 2: {
                if (vartotojai.size() < 2) {
                    std::cout << "\n✗ Pirmiau sugeneruokite vartotojus!\n";
                    break;
                }
                
                baseinas.isvalyti();
                baseinas.generuotiTransakcijas(vartotojai, 10000);
                baseinas.spausdintiStatistika();
                break;
            }
            case 3: {
                if (baseinas.gautiKieki() < 100) {
                    std::cout << "\n✗ Nepakanka transakcijų (reikia 100)!\n";
                    break;
                }
                
                if (!blockchain) {
                    blockchain = std::make_unique<Blockchain>(2);
                }
                
                const auto& visos_tx = baseinas.gautiTransakcijas();
                std::vector<std::shared_ptr<Transakcija>> pasirinktos;
                
                std::random_device rd;
                std::mt19937 gen(rd());
                std::vector<size_t> indeksai;
                for (size_t i = 0; i < visos_tx.size(); ++i) {
                    indeksai.push_back(i);
                }
                std::shuffle(indeksai.begin(), indeksai.end(), gen);
                
                for (int i = 0; i < 100; ++i) {
                    pasirinktos.push_back(
                        std::make_shared<Transakcija>(*visos_tx[indeksai[i]])
                    );
                }
                
                const Blokas* paskutinis = blockchain->gautiPaskutiniBloka();
                std::string prev_hash = paskutinis ? paskutinis->gautiBlokoHash() : 
                    "0000000000000000000000000000000000000000000000000000000000000000";
                
                auto naujas_blokas = std::make_unique<Blokas>(
                    blockchain->gautiIlgi(),
                    prev_hash,
                    pasirinktos,
                    2
                );
                
                naujas_blokas->spausdintiInfo();
                blockchain->pridetiBloka(std::move(naujas_blokas));
                blockchain->spausdintiStatistika();
                break;
            }
            case 0: {
                std::cout << "\nAčiū!\n";
                return 0;
            }
            default: {
                std::cout << "\nNeteisingas pasirinkimas!\n";
            }
        }
    }
    
    return 0;
}