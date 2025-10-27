#ifndef BLOKAS_H
#define BLOKAS_H

#include "lib.h"
#include "transakcija.h"

struct BlokoAntraste {
    std::string prev_block_hash;
    std::chrono::system_clock::time_point timestamp;
    uint32_t version;
    std::string merkle_root;
    uint64_t nonce;
    uint32_t difficulty_target;
    
    BlokoAntraste();
};

class Blokas {
private:
    BlokoAntraste antraste;
    std::vector<std::shared_ptr<Transakcija>> transakcijos;
    std::string bloko_hash;
    uint32_t bloko_numeris;
    
    std::string skaiciuotiMerkleRoot() const;
    std::string skaiciuotiBlokoHash() const;
    bool atliktiProofOfWork();
    bool arValidusHash(const std::string& hash) const;

public:
    Blokas(uint32_t numeris, 
           const std::string& prev_hash,
           const std::vector<std::shared_ptr<Transakcija>>& txs,
           const std::string& miner_address,
           uint32_t difficulty = 3);
    
    ~Blokas();
    Blokas(const Blokas& kitas);
    Blokas& operator=(const Blokas& kitas);
    Blokas(Blokas&& kitas) noexcept;
    Blokas& operator=(Blokas&& kitas) noexcept;
    
    std::string gautiBlokoHash() const { return bloko_hash; }
    std::string gautiPrevHash() const { return antraste.prev_block_hash; }
    std::string gautiMerkleRoot() const { return antraste.merkle_root; }
    uint32_t gautiBlokoNumeri() const { return bloko_numeris; }
    uint64_t gautiNonce() const { return antraste.nonce; }
    uint32_t gautiDifficulty() const { return antraste.difficulty_target; }
    size_t gautiTransakcijuKieki() const { return transakcijos.size(); }
    std::chrono::system_clock::time_point gautiTimestamp() const { return antraste.timestamp; }
    const std::vector<std::shared_ptr<Transakcija>>& gautiTransakcijas() const { 
        return transakcijos; 
    }
    
    bool arValid() const;
    void spausdintiInfo() const;
    void spausdintiVisasTransakcijas() const;
    double gautiBendraTransakcijuSuma() const;
};

class Blockchain {
private:
    std::vector<std::unique_ptr<Blokas>> grandine;
    uint32_t difficulty_target;
    
    void sukurtiGenesisBloka();

public:
    Blockchain(uint32_t difficulty = 3);
    ~Blockchain();
    
    bool pridetiBloka(std::unique_ptr<Blokas> blokas);
    bool arGrandineValidi() const;
    
    size_t gautiIlgi() const { return grandine.size(); }
    const Blokas* gautiPaskutiniBloka() const;
    const Blokas* gautiBlokaPagalIndeksa(size_t index) const {
        if (index < grandine.size()) return grandine[index].get();
        return nullptr;
    }
    uint32_t gautiDifficulty() const { return difficulty_target; }
    
    void spausdintiGrandine() const;
    void spausdintiStatistika() const;
};

#endif