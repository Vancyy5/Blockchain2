#include "blokas.h"
#include "hashas.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <cmath>
#include <algorithm>

// ============= BlokoAntraste implementacija =============

BlokoAntraste::BlokoAntraste()
    : prev_block_hash("0"), 
      timestamp(std::chrono::system_clock::now()),
      version(1), 
      merkle_root(""),
      nonce(0), 
      difficulty_target(2) {
}

// ============= Blokas implementacija =============

// Merkle Root skaičiavimas
std::string Blokas::skaiciuotiMerkleRoot() const {
    if (transakcijos.empty()) {
        return "0000000000000000000000000000000000000000000000000000000000000000";
    }
    
    std::vector<std::string> hashes;
    
    // Surenkame visų transakcijų hash'us
    for (const auto& tx : transakcijos) {
        hashes.push_back(tx->gautiTransactionId());
    }
    
    // Merkle medžio konstravimas
    while (hashes.size() > 1) {
        std::vector<std::string> naujasLygis;
        
        for (size_t i = 0; i < hashes.size(); i += 2) {
            std::string kombinuotas;
            
            if (i + 1 < hashes.size()) {
                kombinuotas = hashes[i] + hashes[i + 1];
            } else {
                kombinuotas = hashes[i] + hashes[i]; // Dubliuojame jei nelyginis
            }
            
            std::string naujas_hash;
            hashas(kombinuotas, naujas_hash);
            naujasLygis.push_back(naujas_hash);
        }
        
        hashes = std::move(naujasLygis);
    }
    
    return hashes[0];
}

// Bloko hash skaičiavimas
std::string Blokas::skaiciuotiBlokoHash() const {
    std::stringstream ss;
    
    ss << antraste.prev_block_hash;
    ss << std::chrono::duration_cast<std::chrono::milliseconds>(
        antraste.timestamp.time_since_epoch()).count();
    ss << antraste.version;
    ss << antraste.merkle_root;
    ss << antraste.nonce;
    ss << antraste.difficulty_target;
    
    std::string hash_input = ss.str();
    std::string hash_output;
    hashas(hash_input, hash_output);
    
    return hash_output;
}

// Hash validacija pagal difficulty
bool Blokas::arValidusHash(const std::string& hash) const {
    if (hash.size() < antraste.difficulty_target) return false;
    
    for (size_t i = 0; i < antraste.difficulty_target; ++i) {
        if (hash[i] != '0') return false;
    }
    return true;
}

// Proof-of-Work procesas
bool Blokas::atliktiProofOfWork() {
    const uint64_t MAX_NONCE = 1000000;
    
    std::cout << "Pradedamas mining procesas (Difficulty: " 
              << antraste.difficulty_target << ")...\n";
    
    auto start_time = std::chrono::high_resolution_clock::now();
    
    for (uint64_t n = 0; n < MAX_NONCE; ++n) {
        antraste.nonce = n;
        std::string hash = skaiciuotiBlokoHash();
        
        if (arValidusHash(hash)) {
            bloko_hash = hash;
            
            auto end_time = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
                end_time - start_time).count();
            
            std::cout << "✓ Blokas išmainintas! Nonce: " << n 
                     << ", Laikas: " << duration << " ms\n";
            std::cout << "Hash: " << hash << "\n\n";
            return true;
        }
        
        if (n % 10000 == 0 && n > 0) {
            std::cout << "Bandymai: " << n << "...\n";
        }
    }
    
    std::cout << "✗ Nepavyko rasti tinkamo hash po " << MAX_NONCE << " bandymų\n";
    return false;
}

// Konstruktorius
Blokas::Blokas(uint32_t numeris, 
               const std::string& prev_hash,
               const std::vector<std::shared_ptr<Transakcija>>& txs,
               uint32_t difficulty)
    : transakcijos(txs), bloko_numeris(numeris) {
    
    antraste.prev_block_hash = prev_hash;
    antraste.timestamp = std::chrono::system_clock::now();
    antraste.version = 1;
    antraste.difficulty_target = difficulty;
    antraste.nonce = 0;
    
    // Skaičiuojame Merkle Root
    antraste.merkle_root = skaiciuotiMerkleRoot();
    
    // Atliekame Proof-of-Work
    if (!atliktiProofOfWork()) {
        throw std::runtime_error("Nepavyko sukurti bloko - mining nepavyko");
    }
}

// Destruktorius
Blokas::~Blokas() {
    // Automatinis cleanup
}

// Kopijavimo konstruktorius
Blokas::Blokas(const Blokas& kitas)
    : antraste(kitas.antraste),
      transakcijos(kitas.transakcijos),
      bloko_hash(kitas.bloko_hash),
      bloko_numeris(kitas.bloko_numeris) {
}

// Priskyrimo operatorius
Blokas& Blokas::operator=(const Blokas& kitas) {
    if (this != &kitas) {
        antraste = kitas.antraste;
        transakcijos = kitas.transakcijos;
        bloko_hash = kitas.bloko_hash;
        bloko_numeris = kitas.bloko_numeris;
    }
    return *this;
}

// Move konstruktorius
Blokas::Blokas(Blokas&& kitas) noexcept
    : antraste(std::move(kitas.antraste)),
      transakcijos(std::move(kitas.transakcijos)),
      bloko_hash(std::move(kitas.bloko_hash)),
      bloko_numeris(kitas.bloko_numeris) {
}

// Move priskyrimo operatorius
Blokas& Blokas::operator=(Blokas&& kitas) noexcept {
    if (this != &kitas) {
        antraste = std::move(kitas.antraste);
        transakcijos = std::move(kitas.transakcijos);
        bloko_hash = std::move(kitas.bloko_hash);
        bloko_numeris = kitas.bloko_numeris;
    }
    return *this;
}

// Bloko validavimas
bool Blokas::arValid() const {
    // Tikriname ar hash atitinka difficulty
    if (!arValidusHash(bloko_hash)) return false;
    
    // Tikriname ar hash sutampa su perskaičiuotu
    std::string perskaiciuotas = skaiciuotiBlokoHash();
    if (perskaiciuotas != bloko_hash) return false;
    
    // Tikriname Merkle Root
    std::string perskaiciuotas_merkle = skaiciuotiMerkleRoot();
    if (perskaiciuotas_merkle != antraste.merkle_root) return false;
    
    // Tikriname visas transakcijas
    for (const auto& tx : transakcijos) {
        if (!tx->arValid()) return false;
    }
    
    return true;
}

// Bendros transakcijų sumos skaičiavimas
double Blokas::gautiBendraTransakcijuSuma() const {
    double suma = 0.0;
    for (const auto& tx : transakcijos) {
        suma += tx->gautiAmount();
    }
    return suma;
}

// Informacijos spausdinimas
void Blokas::spausdintiInfo() const {
    std::cout << "\n╔══════════════════════════════════════════════════════════════╗\n";
    std::cout << "║                        BLOKAS #" << std::setw(4) << bloko_numeris 
              << "                          ║\n";
    std::cout << "╠══════════════════════════════════════════════════════════════╣\n";
    
    std::cout << "║ Hash:         " << bloko_hash.substr(0, 40) << "... ║\n";
    std::cout << "║ Prev Hash:    " << antraste.prev_block_hash.substr(0, 40) << "... ║\n";
    std::cout << "║ Merkle Root:  " << antraste.merkle_root.substr(0, 40) << "... ║\n";
    
    auto time_t = std::chrono::system_clock::to_time_t(antraste.timestamp);
    std::cout << "║ Timestamp:    " << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S") 
              << "                         ║\n";
    
    std::cout << "║ Version:      " << antraste.version << "                                              ║\n";
    std::cout << "║ Nonce:        " << std::setw(10) << antraste.nonce << "                                     ║\n";
    std::cout << "║ Difficulty:   " << antraste.difficulty_target << "                                              ║\n";
    std::cout << "║ Transakcijos: " << std::setw(4) << transakcijos.size() 
              << "                                         ║\n";
    std::cout << "║ Bendra suma:  " << std::fixed << std::setprecision(2) 
              << std::setw(12) << gautiBendraTransakcijuSuma() << " vnt.                        ║\n";
    
    std::cout << "╚══════════════════════════════════════════════════════════════╝\n";
}

// Visų transakcijų spausdinimas
void Blokas::spausdintiVisasTransakcijas() const {
    std::cout << "\n=== BLOKO #" << bloko_numeris << " TRANSAKCIJOS ===\n\n";
    
    for (size_t i = 0; i < transakcijos.size(); ++i) {
        std::cout << "--- Transakcija #" << (i + 1) << " ---\n";
        transakcijos[i]->spausdintiInfo();
        std::cout << "\n";
    }
}

// ============= Blockchain implementacija =============

// Genesis bloko kūrimas
void Blockchain::sukurtiGenesisBloka() {
    std::vector<std::shared_ptr<Transakcija>> tuscios_tx;
    
    auto genesis = std::make_unique<Blokas>(
        0, 
        "0000000000000000000000000000000000000000000000000000000000000000",
        tuscios_tx,
        difficulty_target
    );
    
    grandinė.push_back(std::move(genesis));
    std::cout << "✓ Genesis blokas sukurtas!\n";
}

// Konstruktorius
Blockchain::Blockchain(uint32_t difficulty)
    : difficulty_target(difficulty) {
    sukurtiGenesisBloka();
}

// Destruktorius
Blockchain::~Blockchain() {
    // Automatinis cleanup
}

// Naujo bloko pridėjimas
bool Blockchain::pridetiBloka(std::unique_ptr<Blokas> blokas) {
    if (!blokas) return false;
    
    // Tikriname ar blokas validus
    if (!blokas->arValid()) {
        std::cout << "✗ Blokas nevalidus!\n";
        return false;
    }
    
    // Tikriname ar prev_hash sutampa su paskutinio bloko hash
    if (!grandinė.empty()) {
        const auto& paskutinis = grandinė.back();
        if (blokas->gautiPrevHash() != paskutinis->gautiBlokoHash()) {
            std::cout << "✗ Bloko prev_hash nesutampa su paskutiniu bloku!\n";
            return false;
        }
    }
    
    grandinė.push_back(std::move(blokas));
    std::cout << "✓ Blokas sėkmingai pridėtas į grandinę!\n";
    return true;
}

// Grandinės validavimas
bool Blockchain::arGrandineValidi() const {
    if (grandinė.empty()) return false;
    
    // Tikriname genesis bloką
    if (!grandinė[0]->arValid()) return false;
    
    // Tikriname visus blokus ir jų ryšius
    for (size_t i = 1; i < grandinė.size(); ++i) {
        const auto& dabartinis = grandinė[i];
        const auto& ankstesnis = grandinė[i - 1];
        
        // Tikriname bloko validumą
        if (!dabartinis->arValid()) return false;
        
        // Tikriname ar prev_hash sutampa
        if (dabartinis->gautiPrevHash() != ankstesnis->gautiBlokoHash()) {
            return false;
        }
    }
    
    return true;
}

// Paskutinio bloko gavimas
const Blokas* Blockchain::gautiPaskutiniBloka() const {
    if (grandinė.empty()) return nullptr;
    return grandinė.back().get();
}

// Grandinės spausdinimas
void Blockchain::spausdintiGrandine() const {
    std::cout << "\n";
    std::cout << "════════════════════════════════════════════════════════════════\n";
    std::cout << "                      BLOCKCHAIN GRANDINĖ                       \n";
    std::cout << "════════════════════════════════════════════════════════════════\n";
    
    for (const auto& blokas : grandinė) {
        blokas->spausdintiInfo();
    }
    
    std::cout << "\n════════════════════════════════════════════════════════════════\n";
}

// Statistikos spausdinimas
void Blockchain::spausdintiStatistika() const {
    std::cout << "\n╔══════════════════════════════════════════════════════════════╗\n";
    std::cout << "║                   BLOCKCHAIN STATISTIKA                      ║\n";
    std::cout << "╠══════════════════════════════════════════════════════════════╣\n";
    
    std::cout << "║ Blokų kiekis:        " << std::setw(6) << grandinė.size() 
              << "                                 ║\n";
    
    size_t bendras_tx = 0;
    double bendra_suma = 0.0;
    
    for (const auto& blokas : grandinė) {
        bendras_tx += blokas->gautiTransakcijuKieki();
        bendra_suma += blokas->gautiBendraTransakcijuSuma();
    }
    
    std::cout << "║ Transakcijų kiekis:  " << std::setw(6) << bendras_tx 
              << "                                 ║\n";
    std::cout << "║ Bendra suma:         " << std::fixed << std::setprecision(2) 
              << std::setw(14) << bendra_suma << " vnt.                  ║\n";
    std::cout << "║ Difficulty:          " << std::setw(6) << difficulty_target 
              << "                                 ║\n";
    std::cout << "║ Grandinė validi:     " << (arGrandineValidi() ? "TAIP" : "NE  ") 
              << "                                    ║\n";
    
    std::cout << "╚══════════════════════════════════════════════════════════════╝\n";
}