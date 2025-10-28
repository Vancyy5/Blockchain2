#ifndef LAIKAS_H
#define LAIKAS_H

#include <chrono>
#include <iostream>
#include <string>

class Laikas 
{
private:
    std::chrono::high_resolution_clock::time_point start; 
    std::string veiksmoPavadinimas;                       

public:
    // RAII: automatiskai pradeda matuoti konstruktoriuje
    // PATAISYTA: taisyklinga inicializavimo tvarka
    explicit Laikas(const std::string& pavadinimas) 
        : start(std::chrono::high_resolution_clock::now()),
          veiksmoPavadinimas(pavadinimas) {
    }

    // RAII: automatiskai baigia matuoti destruktoriuje
    ~Laikas() {
        auto end = std::chrono::high_resolution_clock::now();
        double trukme = std::chrono::duration<double>(end - start).count();
        std::cout << "---> " << veiksmoPavadinimas 
                  << " uztruko: " << trukme << " s\n";
    }
    
    // Uzdrausta kopijuoti (RAII objektas)
    Laikas(const Laikas&) = delete;
    Laikas& operator=(const Laikas&) = delete;
};

#endif