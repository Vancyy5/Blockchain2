#include "transakcija.h"
#include "vartotojas.h"
#include "hashas.h"

std::string Transakcija::generuotiHash(const std::string& sender, 
                                        const std::string& receiver, 
                                        double amount,
                                        long long timestamp_ms) const {
    std::stringstream ss;
    ss << sender << receiver << std::fixed << std::setprecision(8) << amount << timestamp_ms;
    std::string input = ss.str();
    
    std::string hash_result;
    hashas(input, hash_result);
    
    return hash_result;
}

std::chrono::system_clock::time_point Transakcija::generuotiTimestamp() {
    return std::chrono::system_clock::now();
}

Transakcija::Transakcija(const std::string& sender, 
                         const std::string& receiver, 
                         double amount)
    : sender_key(sender), receiver_key(receiver), amount(amount) {
    
    if (amount <= 0) {
        throw std::invalid_argument("Transaction amount must be positive");
    }
    
    timestamp = generuotiTimestamp();
    auto timestamp_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        timestamp.time_since_epoch()).count();
    
    transaction_id = generuotiHash(sender, receiver, amount, timestamp_ms);
}

Transakcija::Transakcija(const std::string& sender, 
                         const std::string& receiver, 
                         double amount,
                         std::chrono::system_clock::time_point ts)
    : sender_key(sender), receiver_key(receiver), amount(amount), timestamp(ts) {
    
    if (amount <= 0) {
        throw std::invalid_argument("Transaction amount must be positive");
    }
    
    auto timestamp_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        timestamp.time_since_epoch()).count();
    
    transaction_id = generuotiHash(sender, receiver, amount, timestamp_ms);
}

Transakcija::~Transakcija() {
}

Transakcija::Transakcija(const Transakcija& kita)
    : transaction_id(kita.transaction_id),
      sender_key(kita.sender_key),
      receiver_key(kita.receiver_key),
      amount(kita.amount),
      timestamp(kita.timestamp) {
}

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

Transakcija::Transakcija(Transakcija&& kita) noexcept
    : transaction_id(std::move(kita.transaction_id)),
      sender_key(std::move(kita.sender_key)),
      receiver_key(std::move(kita.receiver_key)),
      amount(kita.amount),
      timestamp(kita.timestamp) {
}

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

bool Transakcija::arValid() const {
    // Coinbase transactions can have COINBASE as sender
    if (sender_key == "COINBASE") {
        return !transaction_id.empty() && 
               !receiver_key.empty() && 
               amount > 0;
    }
    
    return !transaction_id.empty() && 
           !sender_key.empty() && 
           !receiver_key.empty() && 
           amount > 0 &&
           sender_key != receiver_key;
}

void Transakcija::spausdintiInfo() const {
    std::cout << "TX ID: " << transaction_id.substr(0, 16) << "...\n";
    std::cout << "Sender: " << sender_key.substr(0, 20) << "...\n";
    std::cout << "Receiver: " << receiver_key.substr(0, 20) << "...\n";
    std::cout << "Amount: " << std::fixed << std::setprecision(2) << amount << " units\n";
    
    auto time_t = std::chrono::system_clock::to_time_t(timestamp);
    std::cout << "Time: " << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S") << "\n";
}

TransakcijuBaseinas::TransakcijuBaseinas() : bendras_suma(0.0) {
}

TransakcijuBaseinas::~TransakcijuBaseinas() {
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
        std::cout << "Not enough users to generate transactions!\n";
        return;
    }
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> vartotojas_dis(0, vartotojai.size() - 1);
    
    std::cout << "\nGenerating transactions...\n";
    std::cout << "Please wait...\n\n";
    
    int sekmingos = 0;
    int bandymai = 0;
    const int max_bandymai = kiekis * 3;
    
    while (sekmingos < kiekis && bandymai < max_bandymai) {
        bandymai++;
        
        int sender_idx = vartotojas_dis(gen);
        int receiver_idx = vartotojas_dis(gen);
        
        if (sender_idx == receiver_idx) continue;
        
        auto& sender = vartotojai[sender_idx];
        auto& receiver = vartotojai[receiver_idx];
        
        double sender_balansas = sender->gautiBalansa();
        
        if (sender_balansas < 1.0) continue;
        
        std::uniform_real_distribution<> suma_dis(0.05, 0.5);
        double amount = sender_balansas * suma_dis(gen);
        amount = std::round(amount * 100.0) / 100.0;
        
        if (amount < 0.01) continue;
        
        try {
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
                std::cout << "Generated: " << sekmingos << " / " << kiekis << "\n";
            }
        } catch (const std::exception& e) {
            continue;
        }
    }
    
    std::cout << "\nSuccessfully generated " << sekmingos << " transactions!\n";
    std::cout << "Attempts: " << bandymai << "\n\n";
}

bool TransakcijuBaseinas::vykdytiTransakcija(
    Transakcija& transakcija,
    std::vector<std::unique_ptr<Vartotojas>>& vartotojai) {
    
    // Skip COINBASE transactions
    if (transakcija.gautiSender() == "COINBASE") {
        Vartotojas* receiver = nullptr;
        for (auto& v : vartotojai) {
            if (v->gautiPublicKey() == transakcija.gautiReceiver()) {
                receiver = v.get();
                break;
            }
        }
        
        if (receiver) {
            receiver->pridetiUTXO(UTXO(transakcija.gautiTransactionId(), 0, transakcija.gautiAmount()));
            return true;
        }
        return false;
    }
    
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
    
    double amount = transakcija.gautiAmount();
    double sender_balansas = sender->gautiBalansa();
    
    if (sender_balansas < amount) return false;
    
    const auto& utxo_list = sender->gautiUTXO();
    std::vector<size_t> naudojami_utxo;
    double surinkta_suma = 0.0;
    
    for (size_t i = 0; i < utxo_list.size() && surinkta_suma < amount; ++i) {
        surinkta_suma += utxo_list[i].suma;
        naudojami_utxo.push_back(i);
    }
    
    if (surinkta_suma < amount) return false;
    
    for (auto it = naudojami_utxo.rbegin(); it != naudojami_utxo.rend(); ++it) {
        const auto& utxo = utxo_list[*it];
        sender->panaudotiUTXO(utxo.txid, utxo.vout);
    }
    
    double graza = surinkta_suma - amount;
    
    receiver->pridetiUTXO(UTXO(transakcija.gautiTransactionId(), 0, amount));
    
    if (graza > 0.01) {
        sender->pridetiUTXO(UTXO(transakcija.gautiTransactionId(), 1, graza));
    }
    
    return true;
}

int TransakcijuBaseinas::vykdytiVisasTransakcijas(
    std::vector<std::unique_ptr<Vartotojas>>& vartotojai) {
    
    std::cout << "\nExecuting transactions...\n";
    std::cout << "Please wait...\n\n";
    
    int sekmingos = 0;
    int nesekmingos = 0;
    
    for (auto& tx : transakcijos) {
        if (vykdytiTransakcija(*tx, vartotojai)) {
            sekmingos++;
        } else {
            nesekmingos++;
        }
        
        if ((sekmingos + nesekmingos) % 1000 == 0) {
            std::cout << "Processed: " << (sekmingos + nesekmingos) 
                     << " / " << transakcijos.size() << "\n";
        }
    }
    
    std::cout << "\nSuccessfully executed: " << sekmingos << "\n";
    std::cout << "Failed: " << nesekmingos << "\n\n";
    
    return sekmingos;
}

// NAUJA FUNKCIJA: Pašalina transakcijas iš baseino pagal ID
void TransakcijuBaseinas::pasalintiTransakcijas(const std::vector<std::shared_ptr<Transakcija>>& pasalinti) {
    std::set<std::string> pasalinti_ids;
    for (const auto& tx : pasalinti) {
        pasalinti_ids.insert(tx->gautiTransactionId());
    }
    
    auto nauja_pabaiga = std::remove_if(transakcijos.begin(), transakcijos.end(),
        [&pasalinti_ids](const std::unique_ptr<Transakcija>& tx) {
            return pasalinti_ids.count(tx->gautiTransactionId()) > 0;
        });
    
    transakcijos.erase(nauja_pabaiga, transakcijos.end());
    perskaiciuotiBendraSuma();
}

void TransakcijuBaseinas::spausdintiStatistika() const {
    std::cout << "\n=== TRANSACTION STATISTICS ===\n";
    std::cout << "Transaction count: " << transakcijos.size() << "\n";
    std::cout << "Total amount: " << std::fixed << std::setprecision(2) 
              << bendras_suma << " units\n";
    
    if (!transakcijos.empty()) {
        double vidutine = bendras_suma / transakcijos.size();
        std::cout << "Average amount: " << std::fixed << std::setprecision(2) 
                  << vidutine << " units\n";
    }
    std::cout << "\n";
}

void TransakcijuBaseinas::spausdintiTransakcijas(int kiek) const {
    std::cout << "\n=== FIRST " << kiek << " TRANSACTIONS ===\n\n";
    
    int count = 0;
    for (const auto& tx : transakcijos) {
        if (count >= kiek) break;
        
        std::cout << "--- Transaction #" << (count + 1) << " ---\n";
        tx->spausdintiInfo();
        std::cout << "\n";
        
        count++;
    }
}

void TransakcijuBaseinas::isvalyti() {
    transakcijos.clear();
    bendras_suma = 0.0;
}