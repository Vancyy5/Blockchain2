#ifndef BLOKAS_H
#define BLOKAS_H

#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include "transakcija.h"

// Bloko antraštės struktūra
struct BlokoAntraste {
    std::string prev_block_hash;
    std::chrono::system_clock::time_point timestamp;
    uint32_t version;
    std::string merkle_root;
    uint64_t nonce;
    uint32_t difficulty_target;
    
    BlokoAntraste();
};

// Bloko klasė
class Blokas {
private:
    BlokoAntraste antraste;
    std::vector<std::shared_ptr<Transakcija>> transakcijos;
    std::string bloko_hash;
    uint32_t bloko_numeris;
    
    // Privatus metodas Merkle Root skaičiavimui
    std::string skaiciuotiMerkleRoot() const;
    
    // Privatus metodas bloko hash skaičiavimui
    std::string skaiciuotiBlokoHash() const;
    
    // Privatus metodas Proof-of-Work
    bool atliktiProofOfWork();
    
    // Privatus metodas hash validacijai pagal difficulty
    bool arValidusHash(const std::string& hash) const;

public:
    // Konstruktorius
    Blokas(uint32_t numeris, 
           const std::string& prev_hash,
           const std::vector<std::shared_ptr<Transakcija>>& txs,
           uint32_t difficulty = 2);
    
    // Destruktorius
    ~Blokas();
    
    // Kopijavimo konstruktorius
    Blokas(const Blokas& kitas);
    
    // Priskyrimo operatorius
    Blokas& operator=(const Blokas& kitas);
    
    // Move konstruktorius
    Blokas(Blokas&& kitas) noexcept;
    
    // Move priskyrimo operatorius
    Blokas& operator=(Blokas&& kitas) noexcept;
    
    // Getteriai
    std::string gautiBlokoHash() const { return bloko_hash; }
    std::string gautiPrevHash() const { return antraste.prev_block_hash; }
    std::string gautiMerkleRoot() const { return antraste.merkle_root; }
    uint32_t gautiBlokoNumeri() const { return bloko_numeris; }
    uint64_t gautiNonce() const { return antraste.nonce; }
    uint32_t gautiDifficulty() const { return antraste.difficulty_target; }
    size_t gautiTransakcijuKieki() const { return transakcijos.size(); }
    const std::vector<std::shared_ptr<Transakcija>>& gautiTransakcijas() const { 
        return transakcijos; 
    }
    
    // Bloko validavimas
    bool arValid() const;
    
    // Informacijos spausdinimas
    void spausdintiInfo() const;
    void spausdintiVisasTransakcijas() const;
    
    // Statistika
    double gautiBendraTransakcijuSuma() const;
};

// Blockchain klasė - blokų grandinė
class Blockchain {
private:
    std::vector<std::unique_ptr<Blokas>> grandinė;
    uint32_t difficulty_target;
    
    // Privatus metodas genesis blokui
    void sukurtiGenesisBloka();

public:
    // Konstruktorius
    Blockchain(uint32_t difficulty = 2);
    
    // Destruktorius
    ~Blockchain();
    
    // Naujo bloko pridėjimas
    bool pridetiBloka(std::unique_ptr<Blokas> blokas);
    
    // Grandinės validavimas
    bool arGrandineValidi() const;
    
    // Getteriai
    size_t gautiIlgi() const { return grandinė.size(); }
    const Blokas* gautiPaskutiniBloka() const;
    uint32_t gautiDifficulty() const { return difficulty_target; }
    
    // Informacijos spausdinimas
    void spausdintiGrandine() const;
    void spausdintiStatistika() const;
};

#endif // BLOKAS_H