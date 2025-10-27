#include "vartotojas.h"
#include <iostream>
#include <random>
#include <sstream>
#include <iomanip>
#include <algorithm>

// Privatus metodas viešojo rakto generavimui
std::string Vartotojas::generuotiPublicKey() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(0, 15);
    
    std::stringstream ss;
    ss << "0x";
    for (int i = 0; i < 40; ++i) {
        ss << std::hex << dis(gen);
    }
    return ss.str();
}

// Privatus metodas pradinių UTXO generavimui
void Vartotojas::generuotiPradinisUTXO(double balansas) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> utxo_kiekis(1, 5);
    static std::uniform_int_distribution<> txid_dis(0, 15);
    
    int kiek_utxo = utxo_kiekis(gen);
    double likutis = balansas;
    
    for (int i = 0; i < kiek_utxo - 1; ++i) {
        // Generuojame atsitiktinį txid
        std::stringstream txid;
        for (int j = 0; j < 64; ++j) {
            txid << std::hex << txid_dis(gen);
        }
        
        // Paskirstome balansą į kelis UTXO
        double suma = likutis * (0.1 + (std::uniform_real_distribution<>(0, 0.4)(gen)));
        likutis -= suma;
        
        utxo_rinkinys.emplace_back(txid.str(), i, suma);
    }
    
    // Paskutinis UTXO gauna likusį balansą
    std::stringstream txid;
    for (int j = 0; j < 64; ++j) {
        txid << std::hex << txid_dis(gen);
    }
    utxo_rinkinys.emplace_back(txid.str(), kiek_utxo - 1, likutis);
}

// Privatus metodas balanso skaičiavimui iš UTXO
double Vartotojas::skaiciuotiBalansa() const {
    double suma = 0.0;
    for (const auto& utxo : utxo_rinkinys) {
        suma += utxo.suma;
    }
    return suma;
}

// Konstruktorius
Vartotojas::Vartotojas(const std::string& vardas, double pradinis_balansas)
    : vardas(vardas), public_key(generuotiPublicKey()) {
    if (pradinis_balansas < 0) {
        throw std::invalid_argument("Balansas negali būti neigiamas");
    }
    generuotiPradinisUTXO(pradinis_balansas);
}

// Destruktorius (RAII idioma - automatinis resource cleanup)
Vartotojas::~Vartotojas() {
    // UTXO vector automatiškai išvalomas
    // Jei turėtume dinaminių resursų, juos išvalytume čia
}

// Kopijavimo konstruktorius
Vartotojas::Vartotojas(const Vartotojas& kitas)
    : vardas(kitas.vardas), 
      public_key(kitas.public_key),
      utxo_rinkinys(kitas.utxo_rinkinys) {
}

// Priskyrimo operatorius
Vartotojas& Vartotojas::operator=(const Vartotojas& kitas) {
    if (this != &kitas) {
        vardas = kitas.vardas;
        public_key = kitas.public_key;
        utxo_rinkinys = kitas.utxo_rinkinys;
    }
    return *this;
}

// Move konstruktorius
Vartotojas::Vartotojas(Vartotojas&& kitas) noexcept
    : vardas(std::move(kitas.vardas)),
      public_key(std::move(kitas.public_key)),
      utxo_rinkinys(std::move(kitas.utxo_rinkinys)) {
}

// Move priskyrimo operatorius
Vartotojas& Vartotojas::operator=(Vartotojas&& kitas) noexcept {
    if (this != &kitas) {
        vardas = std::move(kitas.vardas);
        public_key = std::move(kitas.public_key);
        utxo_rinkinys = std::move(kitas.utxo_rinkinys);
    }
    return *this;
}

// UTXO pridėjimas
void Vartotojas::pridetiUTXO(const UTXO& utxo) {
    utxo_rinkinys.push_back(utxo);
}

// UTXO panaudojimas (pašalinimas iš rinkinio)
bool Vartotojas::panaudotiUTXO(const std::string& txid, unsigned int vout) {
    auto it = std::find_if(utxo_rinkinys.begin(), utxo_rinkinys.end(),
        [&txid, vout](const UTXO& utxo) {
            return utxo.txid == txid && utxo.vout == vout;
        });
    
    if (it != utxo_rinkinys.end()) {
        utxo_rinkinys.erase(it);
        return true;
    }
    return false;
}

// Informacijos spausdinimas
void Vartotojas::spausdintiInfo() const {
    std::cout << "Vardas: " << vardas << "\n";
    std::cout << "Public Key: " << public_key << "\n";
    std::cout << "Balansas: " << std::fixed << std::setprecision(2) 
              << gautiBalansa() << " vnt.\n";
    std::cout << "UTXO kiekis: " << utxo_rinkinys.size() << "\n";
}

// UTXO spausdinimas
void Vartotojas::spausdintiUTXO() const {
    std::cout << "\n=== UTXO sąrašas ===\n";
    for (size_t i = 0; i < utxo_rinkinys.size(); ++i) {
        const auto& utxo = utxo_rinkinys[i];
        std::cout << i + 1 << ". TXID: " << utxo.txid.substr(0, 16) << "...\n";
        std::cout << "   VOUT: " << utxo.vout << ", Suma: " 
                  << std::fixed << std::setprecision(2) << utxo.suma << " vnt.\n";
    }
}