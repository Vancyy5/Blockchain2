#include "transakcija.h"
#include "vartotojas.h"
#include "hashas.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <random>
#include <algorithm>
#include <functional>

// Hash generavimas naudojant jūsų hashas funkcija
std::string Transakcija::generuotiHash(const std::string& sender, 
                                        const std::string& receiver, 
                                        double amount,
                                        long long timestamp_ms) const {
    // Sukuriame įvestį hash'ui
    std::stringstream ss;
    ss << sender << receiver << std::fixed << std::setprecision(8) << amount << timestamp_ms;
    std::string input = ss.str();
    
    // Naudojame jūsų hash funkciją
    std::string hash_result;
    hashas(input, hash_result);
    
    return hash_result;
}

// Timestamp generavimas
std::chrono::system_clock::time_point Transakcija::generuotiTimestamp() {
    return std::chrono::system_clock::now();
}

// Konstruktorius
Transakcija::Transakcija(const std::string& sender, 
                         const std::string& receiver, 
                         double amount)
    : sender_key(sender), receiver_key(receiver), amount(amount) {
    
    if (amount <= 0) {
        throw std::invalid_argument("Transakcijos suma turi būti teigiama");
    }
    
    timestamp = generuotiTimestamp();
    auto timestamp_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        timestamp.time_since_epoch()).count();
    
    transaction_id = generuotiHash(sender, receiver, amount, timestamp_ms);
}

// Konstruktorius su timestamp
Transakcija::Transakcija(const std::string& sender, 
                         const std::string& receiver, 
                         double amount,
                         std::chrono::system_clock::time_point ts)
    : sender_key(sender), receiver_key(receiver), amount(amount), timestamp(ts) {
    
    if (amount <= 0) {
        throw std::invalid_argument("Transakcijos suma turi būti teigiama");
    }
    
    auto timestamp_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        timestamp.time_since_epoch()).count();
    
    transaction_id = generuotiHash(sender, receiver, amount, timestamp_ms);
}

// Destruktorius
Transakcija::~Transakcija() {
    // Automatinis cleanup
}

// Kopijavimo konstruktorius
Transakcija::Transakcija(const Transakcija& kita)
    : transaction_id(kita.transaction_id),
      sender_key(kita.sender_key),
      receiver_key(kita.receiver_key),
      amount(kita.amount),
      timestamp(kita.timestamp) {
}

// Priskyrimo operatorius
Transakcija& Transakcija::operator=(const Transakcija& kita) {
    if (this != &kita) {
        transaction_id = kita.transaction_id;
        sender_key = kita.sender_key;
        receiver_key = kita.receiver_key;
        amount = kita.amount;
        timestamp = kita.timestamp;
    }
    return *this;
}

// Move konstruktorius
Transakcija::Transakcija(Transakcija&& kita) noexcept
    : transaction_id(std::move(kita.transaction_id)),
      sender_key(std::move(kita.sender_key)),
      receiver_key(std::move(kita.receiver_key)),
      amount(kita.amount),
      timestamp(kita.timestamp) {
}

// Move priskyrimo operatorius
Transakcija& Transakcija::operator=(Transakcija&& kita) noexcept {
    if (this != &kita) {
        transaction_id = std::move(kita.transaction_id);
        sender_key = std::move(kita.sender_key);
        receiver_key = std::move(kita.receiver_key);
        amount = kita.amount;
        timestamp = kita.timestamp;
    }
    return *this;
}

// Transakcijos validavimas
bool Transakcija::arValid() const {
    return !transaction_id.empty() && 
           !sender_key.empty() && 
           !receiver_key.empty() && 
           amount > 0 &&
           sender_key != receiver_key;
}

// Informacijos spausdinimas
void Transakcija::spausdintiInfo() const {
    std::cout << "TX ID: " << transaction_id.substr(0, 16) << "...\n";
    std::cout << "Siuntėjas: " << sender_key.substr(0, 20) << "...\n";
    std::cout << "Gavėjas: " << receiver_key.substr(0, 20) << "...\n";
    std::cout << "Suma: " << std::fixed << std::setprecision(2) << amount << " vnt.\n";
    
    auto time_t = std::chrono::system_clock::to_time_t(timestamp);
    std::cout << "Laikas: " << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S") << "\n";
}

// ============= TransakcijuBaseinas implementacija =============

TransakcijuBaseinas::TransakcijuBaseinas() : bendras_suma(0.0) {
}

TransakcijuBaseinas::~TransakcijuBaseinas() {
    // Automatinis cleanup su unique_ptr
}

void TransakcijuBaseinas::perskaiciuotiBendraSuma() {
    bendras_suma = 0.0;
    for (const auto& tx : transakcijos) {
        bendras_suma += tx->gautiAmount();
    }
}

void TransakcijuBaseinas::pridetiTransakcija(std::unique_ptr<Transakcija> transakcija) {
    if (transakcija && transakcija->arValid()) {
        bendras_suma += transakcija->gautiAmount();
        transakcijos.push_back(std::move(transakcija));
    }
}

void TransakcijuBaseinas::generuotiTransakcijas(
    std::vector<std::unique_ptr<Vartotojas>>& vartotojai, 
    int kiekis) {
    
    if (vartotojai.size() < 2) {
        std::cout << "Nepakanka vartotojų transakcijoms generuoti!\n";
        return;
    }
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> vartotojas_dis(0, vartotojai.size() - 1);
    
    std::cout << "\nGeneruojamos transakcijos...\n";
    std::cout << "Prašome palaukti...\n\n";
    
    int sekmingos = 0;
    int bandymai = 0;
    const int max_bandymai = kiekis * 3; // Apsauga nuo begalinės kilpos
    
    while (sekmingos < kiekis && bandymai < max_bandymai) {
        bandymai++;
        
        // Pasirenkame atsitiktinius siuntėją ir gavėją
        int sender_idx = vartotojas_dis(gen);
        int receiver_idx = vartotojas_dis(gen);
        
        // Užtikriname, kad siuntėjas ir gavėjas skirtųsi
        if (sender_idx == receiver_idx) continue;
        
        auto& sender = vartotojai[sender_idx];
        auto& receiver = vartotojai[receiver_idx];
        
        double sender_balansas = sender->gautiBalansa();
        
        // Siuntėjas turi turėti pakankamai lėšų
        if (sender_balansas < 1.0) continue;
        
        // Generuojame atsitiktinę sumą (5-50% nuo siuntėjo balanso)
        std::uniform_real_distribution<> suma_dis(0.05, 0.5);
        double amount = sender_balansas * suma_dis(gen);
        amount = std::round(amount * 100.0) / 100.0; // Suapvaliname iki 2 skaitmenų
        
        if (amount < 0.01) continue;
        
        try {
            // Sukuriame naują transakciją su timestamp offset
            auto base_time = std::chrono::system_clock::now();
            auto offset = std::chrono::milliseconds(sekmingos);
            auto tx_time = base_time + offset;
            
            auto tx = std::make_unique<Transakcija>(
                sender->gautiPublicKey(),
                receiver->gautiPublicKey(),
                amount,
                tx_time
            );
            
            pridetiTransakcija(std::move(tx));
            sekmingos++;
            
            if (sekmingos % 1000 == 0) {
                std::cout << "Sugeneruota: " << sekmingos << " / " << kiekis << "\n";
            }
        } catch (const std::exception& e) {
            // Praleidžiame klaidingą transakciją
            continue;
        }
    }
    
    std::cout << "\n✓ Sėkmingai sugeneruotos " << sekmingos << " transakcijos!\n";
    std::cout << "Bandymų kiekis: " << bandymai << "\n\n";
}

bool TransakcijuBaseinas::vykdytiTransakcija(
    Transakcija& transakcija,
    std::vector<std::unique_ptr<Vartotojas>>& vartotojai) {
    
    // Randame siuntėją ir gavėją
    Vartotojas* sender = nullptr;
    Vartotojas* receiver = nullptr;
    
    for (auto& v : vartotojai) {
        if (v->gautiPublicKey() == transakcija.gautiSender()) {
            sender = v.get();
        }
        if (v->gautiPublicKey() == transakcija.gautiReceiver()) {
            receiver = v.get();
        }
        if (sender && receiver) break;
    }
    
    if (!sender || !receiver) return false;
    
    // Tikriname ar siuntėjas turi pakankamai UTXO
    double amount = transakcija.gautiAmount();
    double sender_balansas = sender->gautiBalansa();
    
    if (sender_balansas < amount) return false;
    
    // UTXO modelis: surandame reikiamus UTXO
    const auto& utxo_list = sender->gautiUTXO();
    std::vector<size_t> naudojami_utxo;
    double surinkta_suma = 0.0;
    
    for (size_t i = 0; i < utxo_list.size() && surinkta_suma < amount; ++i) {
        surinkta_suma += utxo_list[i].suma;
        naudojami_utxo.push_back(i);
    }
    
    if (surinkta_suma < amount) return false;
    
    // Pašaliname panaudotus UTXO (iš galo, kad išlaikytume indeksus)
    for (auto it = naudojami_utxo.rbegin(); it != naudojami_utxo.rend(); ++it) {
        const auto& utxo = utxo_list[*it];
        sender->panaudotiUTXO(utxo.txid, utxo.vout);
    }
    
    // Sukuriame naujus UTXO
    double graza = surinkta_suma - amount;
    
    // Gavėjui - nauja UTXO su gauta suma
    receiver->pridetiUTXO(UTXO(transakcija.gautiTransactionId(), 0, amount));
    
    // Siuntėjui - grąža (jei yra)
    if (graza > 0.01) {
        sender->pridetiUTXO(UTXO(transakcija.gautiTransactionId(), 1, graza));
    }
    
    return true;
}

int TransakcijuBaseinas::vykdytiVisasTransakcijas(
    std::vector<std::unique_ptr<Vartotojas>>& vartotojai) {
    
    std::cout << "\nVykdomos transakcijos...\n";
    std::cout << "Prašome palaukti...\n\n";
    
    int sekmingos = 0;
    int nesekmingos = 0;
    
    for (auto& tx : transakcijos) {
        if (vykdytiTransakcija(*tx, vartotojai)) {
            sekmingos++;
        } else {
            nesekmingos++;
        }
        
        if ((sekmingos + nesekmingos) % 1000 == 0) {
            std::cout << "Apdorota: " << (sekmingos + nesekmingos) 
                     << " / " << transakcijos.size() << "\n";
        }
    }
    
    std::cout << "\n✓ Sėkmingai įvykdytos: " << sekmingos << "\n";
    std::cout << "✗ Nepavykusios: " << nesekmingos << "\n\n";
    
    return sekmingos;
}

void TransakcijuBaseinas::spausdintiStatistika() const {
    std::cout << "\n=== TRANSAKCIJŲ STATISTIKA ===\n";
    std::cout << "Transakcijų kiekis: " << transakcijos.size() << "\n";
    std::cout << "Bendra suma: " << std::fixed << std::setprecision(2) 
              << bendras_suma << " vnt.\n";
    
    if (!transakcijos.empty()) {
        double vidutine = bendras_suma / transakcijos.size();
        std::cout << "Vidutinė suma: " << std::fixed << std::setprecision(2) 
                  << vidutine << " vnt.\n";
    }
    std::cout << "\n";
}

void TransakcijuBaseinas::spausdintiTransakcijas(int kiek) const {
    std::cout << "\n=== PIRMOSIOS " << kiek << " TRANSAKCIJOS ===\n\n";
    
    int count = 0;
    for (const auto& tx : transakcijos) {
        if (count >= kiek) break;
        
        std::cout << "--- Transakcija #" << (count + 1) << " ---\n";
        tx->spausdintiInfo();
        std::cout << "\n";
        
        count++;
    }
}

void TransakcijuBaseinas::isvalyti() {
    transakcijos.clear();
    bendras_suma = 0.0;
}