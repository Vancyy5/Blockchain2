#include "vartotojas.h"

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

void Vartotojas::generuotiPradinisUTXO(double balansas) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> utxo_kiekis(1, 5);
    static std::uniform_int_distribution<> txid_dis(0, 15);
    
    int kiek_utxo = utxo_kiekis(gen);
    double likutis = balansas;
    
    for (int i = 0; i < kiek_utxo - 1; ++i) {
        std::stringstream txid;
        for (int j = 0; j < 64; ++j) {
            txid << std::hex << txid_dis(gen);
        }
        
        double suma = likutis * (0.1 + (std::uniform_real_distribution<>(0, 0.4)(gen)));
        likutis -= suma;
        
        utxo_rinkinys.emplace_back(txid.str(), i, suma);
    }
    
    std::stringstream txid;
    for (int j = 0; j < 64; ++j) {
        txid << std::hex << txid_dis(gen);
    }
    utxo_rinkinys.emplace_back(txid.str(), kiek_utxo - 1, likutis);
}

double Vartotojas::skaiciuotiBalansa() const {
    double suma = 0.0;
    for (const auto& utxo : utxo_rinkinys) {
        suma += utxo.suma;
    }
    return suma;
}

Vartotojas::Vartotojas(const std::string& vardas, double pradinis_balansas)
    : vardas(vardas), public_key(generuotiPublicKey()) {
    if (pradinis_balansas < 0) {
        throw std::invalid_argument("Balansas negali buti neigiamas");
    }
    generuotiPradinisUTXO(pradinis_balansas);
}

Vartotojas::~Vartotojas() {
}

Vartotojas::Vartotojas(const Vartotojas& kitas)
    : vardas(kitas.vardas), 
      public_key(kitas.public_key),
      utxo_rinkinys(kitas.utxo_rinkinys) {
}

Vartotojas& Vartotojas::operator=(const Vartotojas& kitas) {
    if (this != &kitas) {
        vardas = kitas.vardas;
        public_key = kitas.public_key;
        utxo_rinkinys = kitas.utxo_rinkinys;
    }
    return *this;
}

Vartotojas::Vartotojas(Vartotojas&& kitas) noexcept
    : vardas(std::move(kitas.vardas)),
      public_key(std::move(kitas.public_key)),
      utxo_rinkinys(std::move(kitas.utxo_rinkinys)) {
}

Vartotojas& Vartotojas::operator=(Vartotojas&& kitas) noexcept {
    if (this != &kitas) {
        vardas = std::move(kitas.vardas);
        public_key = std::move(kitas.public_key);
        utxo_rinkinys = std::move(kitas.utxo_rinkinys);
    }
    return *this;
}

void Vartotojas::pridetiUTXO(const UTXO& utxo) {
    utxo_rinkinys.push_back(utxo);
}

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

void Vartotojas::spausdintiInfo() const {
    std::cout << "Vardas: " << vardas << "\n";
    std::cout << "Public Key: " << public_key << "\n";
    std::cout << "Balansas: " << std::fixed << std::setprecision(2) 
              << gautiBalansa() << " vnt.\n";
    std::cout << "UTXO kiekis: " << utxo_rinkinys.size() << "\n";
}

void Vartotojas::spausdintiUTXO() const {
    std::cout << "\n=== UTXO sarasas ===\n";
    for (size_t i = 0; i < utxo_rinkinys.size(); ++i) {
        const auto& utxo = utxo_rinkinys[i];
        std::cout << i + 1 << ". TXID: " << utxo.txid.substr(0, 16) << "...\n";
        std::cout << "   VOUT: " << utxo.vout << ", Suma: " 
                  << std::fixed << std::setprecision(2) << utxo.suma << " vnt.\n";
    }
}