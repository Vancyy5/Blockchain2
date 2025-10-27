#ifndef LAIKAS_H
#define LAIKAS_H

#include <chrono>
#include <iostream>

class Laikas 
{
private:
    std::chrono::high_resolution_clock::time_point start; 
    std::chrono::high_resolution_clock::time_point end;  
    std::string veiksmoPavadinimas;                       

public:
    Laikas(const std::string& pavadinimas);

    void pradeti();

    void baigti();

    double gautiLaikoSkirtuma();
};

#endif