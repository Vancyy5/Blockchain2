#ifndef VARTOTOJAS_H
#define VARTOTOJAS_H

#include "lib.h"

struct UTXO {
    std::string txid;
    unsigned int vout;
    double suma;
    
    UTXO(const std::string& id, unsigned int out, double s) 
        : txid(id), vout(out), suma(s) {}
};

class Vartotojas {
private:
    std::string vardas;
    std::string public_key;
    std::vector<UTXO> utxo_rinkinys;
    
    double skaiciuotiBalansa() const;
    static std::string generuotiPublicKey();
    void generuotiPradinisUTXO(double balansas);

public:
    Vartotojas(const std::string& vardas, double pradinis_balansas);
    ~Vartotojas();
    
    Vartotojas(const Vartotojas& kitas);
    Vartotojas& operator=(const Vartotojas& kitas);
    Vartotojas(Vartotojas&& kitas) noexcept;
    Vartotojas& operator=(Vartotojas&& kitas) noexcept;
    
    std::string gautiVarda() const { return vardas; }
    std::string gautiPublicKey() const { return public_key; }
    double gautiBalansa() const { return skaiciuotiBalansa(); }
    const std::vector<UTXO>& gautiUTXO() const { return utxo_rinkinys; }
    
    void pridetiUTXO(const UTXO& utxo);
    bool panaudotiUTXO(const std::string& txid, unsigned int vout);
    
    void spausdintiInfo() const;
    void spausdintiUTXO() const;
};

#endif