#ifndef TRANSAKCIJA_H
#define TRANSAKCIJA_H

#include <string>
#include <vector>
#include <memory>
#include <chrono>

// Forward deklaracija
class Vartotojas;

class Transakcija {
private:
    std::string transaction_id;
    std::string sender_key;
    std::string receiver_key;
    double amount;
    std::chrono::system_clock::time_point timestamp;
    
    // Privatus metodas hash generavimui
    std::string generuotiHash(const std::string& sender, 
                              const std::string& receiver, 
                              double amount,
                              long long timestamp_ms) const;
    
    // Privatus metodas timestamp generavimui
    static std::chrono::system_clock::time_point generuotiTimestamp();

public:
    // Konstruktorius
    Transakcija(const std::string& sender, 
                const std::string& receiver, 
                double amount);
    
    // Konstruktorius su timestamp
    Transakcija(const std::string& sender, 
                const std::string& receiver, 
                double amount,
                std::chrono::system_clock::time_point ts);
    
    // Destruktorius (RAII idioma)
    ~Transakcija();
    
    // Kopijavimo konstruktorius
    Transakcija(const Transakcija& kita);
    
    // Priskyrimo operatorius
    Transakcija& operator=(const Transakcija& kita);
    
    // Move konstruktorius
    Transakcija(Transakcija&& kita) noexcept;
    
    // Move priskyrimo operatorius
    Transakcija& operator=(Transakcija&& kita) noexcept;
    
    // Getteriai (enkapsuliacija)
    std::string gautiTransactionId() const { return transaction_id; }
    std::string gautiSender() const { return sender_key; }
    std::string gautiReceiver() const { return receiver_key; }
    double gautiAmount() const { return amount; }
    std::chrono::system_clock::time_point gautiTimestamp() const { return timestamp; }
    
    // Transakcijos validavimas
    bool arValid() const;
    
    // Informacijos spausdinimas
    void spausdintiInfo() const;
    
    // Operatorius lyginimui (pagal timestamp)
    bool operator<(const Transakcija& kita) const {
        return timestamp < kita.timestamp;
    }
};

// Transakcijų baseino klasė (Transaction Pool)
class TransakcijuBaseinas {
private:
    std::vector<std::unique_ptr<Transakcija>> transakcijos;
    double bendras_suma;
    
    // Privatus metodas bendros sumos skaičiavimui
    void perskaiciuotiBendraSuma();

public:
    // Konstruktorius
    TransakcijuBaseinas();
    
    // Destruktorius
    ~TransakcijuBaseinas();
    
    // Transakcijos pridėjimas
    void pridetiTransakcija(std::unique_ptr<Transakcija> transakcija);
    
    // Transakcijų generavimas
    void generuotiTransakcijas(std::vector<std::unique_ptr<Vartotojas>>& vartotojai, 
                                int kiekis);
    
    // Transakcijų vykdymas (UTXO atnaujinimas)
    bool vykdytiTransakcija(Transakcija& transakcija,
                            std::vector<std::unique_ptr<Vartotojas>>& vartotojai);
    
    // Visų transakcijų vykdymas
    int vykdytiVisasTransakcijas(std::vector<std::unique_ptr<Vartotojas>>& vartotojai);
    
    // Getteriai
    size_t gautiKieki() const { return transakcijos.size(); }
    double gautiBendraSuma() const { return bendras_suma; }
    const std::vector<std::unique_ptr<Transakcija>>& gautiTransakcijas() const { 
        return transakcijos; 
    }
    
    // Statistika
    void spausdintiStatistika() const;
    void spausdintiTransakcijas(int kiek = 10) const;
    
    // Išvalymas
    void isvalyti();
};

#endif // TRANSAKCIJA_H