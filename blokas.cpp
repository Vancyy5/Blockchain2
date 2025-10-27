#include "blokas.h"
#include "hashas.h"
#include "laikas.h"

BlokoAntraste::BlokoAntraste()
    : prev_block_hash("0"), 
      timestamp(std::chrono::system_clock::now()),
      version(1), 
      merkle_root(""),
      nonce(0), 
      difficulty_target(3) {
}

std::string Blokas::skaiciuotiMerkleRoot() const {
    if (transakcijos.empty()) {
        return "0000000000000000000000000000000000000000000000000000000000000000";
    }
    
    std::vector<std::string> hashes;
    
    for (const auto& tx : transakcijos) {
        hashes.push_back(tx->gautiTransactionId());
    }
    
    while (hashes.size() > 1) {
        std::vector<std::string> naujasLygis;
        
        for (size_t i = 0; i < hashes.size(); i += 2) {
            std::string kombinuotas;
            
            if (i + 1 < hashes.size()) {
                kombinuotas = hashes[i] + hashes[i + 1];
            } else {
                kombinuotas = hashes[i] + hashes[i];
            }
            
            std::string naujas_hash;
            hashas(kombinuotas, naujas_hash);
            naujasLygis.push_back(naujas_hash);
        }
        
        hashes = std::move(naujasLygis);
    }
    
    return hashes[0];
}

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

bool Blokas::arValidusHash(const std::string& hash) const {
    if (hash.size() < antraste.difficulty_target) return false;
    
    for (size_t i = 0; i < antraste.difficulty_target; ++i) {
        if (hash[i] != '0') return false;
    }
    return true;
}

bool Blokas::atliktiProofOfWork() {
    const uint64_t MAX_NONCE = 10000000;
    
    std::cout << "Pradedamas kasimo procesas (Sunkumas: " 
              << antraste.difficulty_target << ")...\n";
    
    Laikas kasimo_laikas("Bloko kasimas");
    kasimo_laikas.pradeti();
    
    for (uint64_t n = 0; n < MAX_NONCE; ++n) {
        antraste.nonce = n;
        std::string hash = skaiciuotiBlokoHash();
        
        if (arValidusHash(hash)) {
            bloko_hash = hash;
            
            kasimo_laikas.baigti();
            
            std::cout << "Blokas iskastas! Nonce: " << n << "\n";
            std::cout << "Hash: " << hash << "\n\n";
            return true;
        }
        
        if (n % 50000 == 0 && n > 0) {
            std::cout << "Bandymu: " << n << "...\n";
        }
    }
    
    std::cout << "Nepavyko rasti tinkamo hash po " << MAX_NONCE << " bandymu\n";
    return false;
}

Blokas::Blokas(uint32_t numeris, 
               const std::string& prev_hash,
               const std::vector<std::shared_ptr<Transakcija>>& txs,
               const std::string& miner_address,
               uint32_t difficulty)
    : bloko_numeris(numeris) {
    
    antraste.prev_block_hash = prev_hash;
    antraste.timestamp = std::chrono::system_clock::now();
    antraste.version = 1;
    antraste.difficulty_target = difficulty;
    antraste.nonce = 0;
    
    // Add Coinbase transaction (block reward)
    double block_reward = 50.0;
    auto coinbase = std::make_shared<Transakcija>(
        "COINBASE",
        miner_address,
        block_reward,
        antraste.timestamp
    );
    
    transakcijos.push_back(coinbase);
    
    // Add regular transactions
    for (const auto& tx : txs) {
        transakcijos.push_back(tx);
    }
    
    antraste.merkle_root = skaiciuotiMerkleRoot();
    
    if (!atliktiProofOfWork()) {
        throw std::runtime_error("Nepavyko sukurti bloko - kasimas nepavyko");
    }
}

Blokas::~Blokas() {
}

Blokas::Blokas(const Blokas& kitas)
    : antraste(kitas.antraste),
      transakcijos(kitas.transakcijos),
      bloko_hash(kitas.bloko_hash),
      bloko_numeris(kitas.bloko_numeris) {
}

Blokas& Blokas::operator=(const Blokas& kitas) {
    if (this != &kitas) {
        antraste = kitas.antraste;
        transakcijos = kitas.transakcijos;
        bloko_hash = kitas.bloko_hash;
        bloko_numeris = kitas.bloko_numeris;
    }
    return *this;
}

Blokas::Blokas(Blokas&& kitas) noexcept
    : antraste(std::move(kitas.antraste)),
      transakcijos(std::move(kitas.transakcijos)),
      bloko_hash(std::move(kitas.bloko_hash)),
      bloko_numeris(kitas.bloko_numeris) {
}

Blokas& Blokas::operator=(Blokas&& kitas) noexcept {
    if (this != &kitas) {
        antraste = std::move(kitas.antraste);
        transakcijos = std::move(kitas.transakcijos);
        bloko_hash = std::move(kitas.bloko_hash);
        bloko_numeris = kitas.bloko_numeris;
    }
    return *this;
}

bool Blokas::arValid() const {
    if (!arValidusHash(bloko_hash)) return false;
    
    std::string perskaiciuotas = skaiciuotiBlokoHash();
    if (perskaiciuotas != bloko_hash) return false;
    
    std::string perskaiciuotas_merkle = skaiciuotiMerkleRoot();
    if (perskaiciuotas_merkle != antraste.merkle_root) return false;
    
    for (const auto& tx : transakcijos) {
        if (!tx->arValid()) return false;
    }
    
    return true;
}

double Blokas::gautiBendraTransakcijuSuma() const {
    double suma = 0.0;
    for (const auto& tx : transakcijos) {
        suma += tx->gautiAmount();
    }
    return suma;
}

void Blokas::spausdintiInfo() const {
    std::cout << "\n======================================================\n";
    std::cout << "                    BLOKAS #" << std::setw(4) << bloko_numeris 
              << "                      \n";
    std::cout << "======================================================\n";
    
    std::cout << "Hash:         " << bloko_hash.substr(0, 40) << "...\n";
    std::cout << "Prev Hash:    " << antraste.prev_block_hash.substr(0, 40) << "...\n";
    std::cout << "Merkle Root:  " << antraste.merkle_root.substr(0, 40) << "...\n";
    
    auto time_t = std::chrono::system_clock::to_time_t(antraste.timestamp);
    std::cout << "Laikas:       " << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S") 
              << "\n";
    
    std::cout << "Versija:      " << antraste.version << "\n";
    std::cout << "Nonce:        " << std::setw(10) << antraste.nonce << "\n";
    std::cout << "Sunkumas:     " << antraste.difficulty_target << "\n";
    std::cout << "Transakciju:  " << std::setw(4) << transakcijos.size() << "\n";
    std::cout << "Bendra suma:  " << std::fixed << std::setprecision(2) 
              << std::setw(12) << gautiBendraTransakcijuSuma() << " vnt.\n";
    
    std::cout << "======================================================\n";
}

void Blokas::spausdintiVisasTransakcijas() const {
    std::cout << "\n=== BLOKO #" << bloko_numeris << " TRANSAKCIJOS ===\n\n";
    
    for (size_t i = 0; i < transakcijos.size(); ++i) {
        std::cout << "--- Transakcija #" << (i + 1) << " ---\n";
        transakcijos[i]->spausdintiInfo();
        std::cout << "\n";
    }
}

void Blockchain::sukurtiGenesisBloka() {
    std::vector<std::shared_ptr<Transakcija>> tuscios_tx;
    
    auto genesis = std::make_unique<Blokas>(
        0, 
        "0000000000000000000000000000000000000000000000000000000000000000",
        tuscios_tx,
        "GENESIS",
        difficulty_target
    );
    
    grandine.push_back(std::move(genesis));
    std::cout << "Genesis blokas sukurtas!\n";
}

Blockchain::Blockchain(uint32_t difficulty)
    : difficulty_target(difficulty) {
    sukurtiGenesisBloka();
}

Blockchain::~Blockchain() {
}

bool Blockchain::pridetiBloka(std::unique_ptr<Blokas> blokas) {
    if (!blokas) return false;
    
    if (!blokas->arValid()) {
        std::cout << "Netinkamas blokas!\n";
        return false;
    }
    
    if (!grandine.empty()) {
        const auto& paskutinis = grandine.back();
        if (blokas->gautiPrevHash() != paskutinis->gautiBlokoHash()) {
            std::cout << "Bloko prev_hash neatitinka paskutinio bloko!\n";
            return false;
        }
    }
    
    grandine.push_back(std::move(blokas));
    std::cout << "Blokas sekmingai pridetas i grandine!\n";
    return true;
}

bool Blockchain::arGrandineValidi() const {
    if (grandine.empty()) return false;
    
    if (!grandine[0]->arValid()) return false;
    
    for (size_t i = 1; i < grandine.size(); ++i) {
        const auto& dabartinis = grandine[i];
        const auto& ankstesnis = grandine[i - 1];
        
        if (!dabartinis->arValid()) return false;
        
        if (dabartinis->gautiPrevHash() != ankstesnis->gautiBlokoHash()) {
            return false;
        }
    }
    
    return true;
}

const Blokas* Blockchain::gautiPaskutiniBloka() const {
    if (grandine.empty()) return nullptr;
    return grandine.back().get();
}

void Blockchain::spausdintiGrandine() const {
    std::cout << "\n";
    std::cout << "==========================================================\n";
    std::cout << "                   BLOCKCHAIN GRANDINE                    \n";
    std::cout << "==========================================================\n";
    
    for (const auto& blokas : grandine) {
        blokas->spausdintiInfo();
    }
    
    std::cout << "\n==========================================================\n";
}

void Blockchain::spausdintiStatistika() const {
    std::cout << "\n======================================================\n";
    std::cout << "               BLOCKCHAIN STATISTIKA                  \n";
    std::cout << "======================================================\n";
    
    std::cout << "Bloku skaicius:      " << std::setw(6) << grandine.size() << "\n";
    
    size_t bendras_tx = 0;
    double bendra_suma = 0.0;
    
    for (const auto& blokas : grandine) {
        bendras_tx += blokas->gautiTransakcijuKieki();
        bendra_suma += blokas->gautiBendraTransakcijuSuma();
    }
    
    std::cout << "Transakciju skaicius:" << std::setw(6) << bendras_tx << "\n";
    std::cout << "Bendra suma:         " << std::fixed << std::setprecision(2) 
              << std::setw(14) << bendra_suma << " vnt.\n";
    std::cout << "Sunkumas:            " << std::setw(6) << difficulty_target << "\n";
    std::cout << "Grandine validi:     " << (arGrandineValidi() ? "TAIP" : "NE  ") << "\n";
    
    std::cout << "======================================================\n";
}