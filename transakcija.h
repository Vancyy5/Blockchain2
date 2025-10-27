#ifndef TRANSAKCIJA_H
#define TRANSAKCIJA_H

#include "lib.h"

class Vartotojas;

class Transakcija {
private:
    std::string transaction_id;
    std::string sender_key;
    std::string receiver_key;
    double amount;
    std::chrono::system_clock::time_point timestamp;
    
    std::string generuotiHash(const std::string& sender, 
                              const std::string& receiver, 
                              double amount,
                              long long timestamp_ms) const;
    
    static std::chrono::system_clock::time_point generuotiTimestamp();

public:
    Transakcija(const std::string& sender, 
                const std::string& receiver, 
                double amount);
    
    Transakcija(const std::string& sender, 
                const std::string& receiver, 
                double amount,
                std::chrono::system_clock::time_point ts);
    
    ~Transakcija();
    
    Transakcija(const Transakcija& kita);
    Transakcija& operator=(const Transakcija& kita);
    Transakcija(Transakcija&& kita) noexcept;
    Transakcija& operator=(Transakcija&& kita) noexcept;
    
    std::string gautiTransactionId() const { return transaction_id; }
    std::string gautiSender() const { return sender_key; }
    std::string gautiReceiver() const { return receiver_key; }
    double gautiAmount() const { return amount; }
    std::chrono::system_clock::time_point gautiTimestamp() const { return timestamp; }
    
    bool arValid() const;
    void spausdintiInfo() const;
    
    bool operator<(const Transakcija& kita) const {
        return timestamp < kita.timestamp;
    }
};

class TransakcijuBaseinas {
private:
    std::vector<std::unique_ptr<Transakcija>> transakcijos;
    double bendras_suma;
    
    void perskaiciuotiBendraSuma();

public:
    TransakcijuBaseinas();
    ~TransakcijuBaseinas();
    
    void pridetiTransakcija(std::unique_ptr<Transakcija> transakcija);
    void generuotiTransakcijas(std::vector<std::unique_ptr<Vartotojas>>& vartotojai, int kiekis);
    
    bool vykdytiTransakcija(Transakcija& transakcija,
                            std::vector<std::unique_ptr<Vartotojas>>& vartotojai);
    int vykdytiVisasTransakcijas(std::vector<std::unique_ptr<Vartotojas>>& vartotojai);
    
    size_t gautiKieki() const { return transakcijos.size(); }
    double gautiBendraSuma() const { return bendras_suma; }
    const std::vector<std::unique_ptr<Transakcija>>& gautiTransakcijas() const { 
        return transakcijos; 
    }
    
    void spausdintiStatistika() const;
    void spausdintiTransakcijas(int kiek = 10) const;
    void isvalyti();
};

#endif