#include "laikas.h"

using namespace std::chrono;

Laikas::Laikas(const std::string& pavadinimas) 
{
    veiksmoPavadinimas = pavadinimas;
}

void Laikas::pradeti() {
    start = high_resolution_clock::now();
}

void Laikas::baigti() {
    end = high_resolution_clock::now();
    std::cout << "---> " << veiksmoPavadinimas << " took: " << gautiLaikoSkirtuma() << " s" << std::endl;
}

double Laikas::gautiLaikoSkirtuma() {
    return duration<double>(end - start).count();
}