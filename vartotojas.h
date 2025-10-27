#ifndef VARTOTOJAS_H
#define VARTOTOJAS_H

#include <string>
#include <vector>
#include <memory>

// UTXO (Unspent Transaction Output) struktūra
struct UTXO {
    std::string txid;           // Transakcijos ID
    unsigned int vout;          // Output indeksas
    double suma;                // UTXO suma
    
    UTXO(const std::string& id, unsigned int out, double s) 
        : txid(id), vout(out), suma(s) {}
};

class Vartotojas {
private:
    std::string vardas;
    std::string public_key;
    std::vector<UTXO> utxo_rinkinys;  // UTXO rinkinys
    
    // Privatus metodas balanso skaičiavimui
    double skaiciuotiBalansa() const;
    
    // Privatus metodas viešojo rakto generavimui
    static std::string generuotiPublicKey();
    
    // Privatus metodas UTXO generavimui
    void generuotiPradinisUTXO(double balansas);

public:
    // Konstruktorius su vardu ir balansu
    Vartotojas(const std::string& vardas, double pradinis_balansas);
    
    // Destruktorius (RAII idioma)
    ~Vartotojas();
    
    // Kopijavimo konstruktorius
    Vartotojas(const Vartotojas& kitas);
    
    // Priskyrimo operatorius
    Vartotojas& operator=(const Vartotojas& kitas);
    
    // Move konstruktorius (C++11)
    Vartotojas(Vartotojas&& kitas) noexcept;
    
    // Move priskyrimo operatorius
    Vartotojas& operator=(Vartotojas&& kitas) noexcept;
    
    // Getteriai (enkapsuliacija)
    std::string gautiVarda() const { return vardas; }
    std::string gautiPublicKey() const { return public_key; }
    double gautiBalansa() const { return skaiciuotiBalansa(); }
    const std::vector<UTXO>& gautiUTXO() const { return utxo_rinkinys; }
    
    // UTXO operacijos
    void pridetiUTXO(const UTXO& utxo);
    bool panaudotiUTXO(const std::string& txid, unsigned int vout);
    
    // Informacijos spausdinimas
    void spausdintiInfo() const;
    void spausdintiUTXO() const;
};

#endif // VARTOTOJAS_H