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
        throw std::invalid_argument("Transakcijos suma turi buti teigiama");
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
        throw std::invalid_argument("Transakcijos suma turi buti teigiama");
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

bool Transakcija::arValidusHash() const {
    auto timestamp_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        timestamp.time_since_epoch()).count();
    
    std::string perskaiciuotas = generuotiHash(sender_key, receiver_key, amount, timestamp_ms);
    
    return perskaiciuotas == transaction_id;
}

bool Transakcija::arValid() const {
    // 1. Patikrinti pagrindinius laukus
    if (transaction_id.empty()) {
        std::cout << "  Klaida: Tuscias transaction ID\n";
        return false;
    }
    
    if (receiver_key.empty()) {
        std::cout << "  Klaida: Tuscias receiver key\n";
        return false;
    }
    
    if (amount <= 0) {
        std::cout << "  Klaida: Neteigiama arba nuline suma\n";
        return false;
    }
    
    // 2. Coinbase transakcijos specialus atvejis
    if (sender_key == "COINBASE") {
        return true;
    }
    
    // 3. Patikrinti, kad siuntejas ir gavejas nera tas pats
    if (sender_key.empty()) {
        std::cout << "  Klaida: Tuscias sender key\n";
        return false;
    }
    
    if (sender_key == receiver_key) {
        std::cout << "  Klaida: Siuntejas ir gavejas yra tas pats\n";
        return false;
    }
    
    // 4. Patikrinti hash teisingumo
    if (!arValidusHash()) {
        std::cout << "  Klaida: Neteisingas transaction hash\n";
        return false;
    }
    
    return true;
}

void Transakcija::spausdintiInfo() const {
    std::cout << "TX ID: " << transaction_id.substr(0, 16) << "...\n";
    std::cout << "Siuntejas: " << (sender_key == "COINBASE" ? "COINBASE" : sender_key.substr(0, 20) + "...") << "\n";
    std::cout << "Gavejas: " << receiver_key.substr(0, 20) << "...\n";
    std::cout << "Suma: " << std::fixed << std::setprecision(2) << amount << " vnt.\n";
    
    auto time_t = std::chrono::system_clock::to_time_t(timestamp);
    std::cout << "Laikas: " << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S") << "\n";
}

void Transakcija::spausdintiDetaliInfo() const {
    std::cout << "\n========== TRANSAKCIJOS DETALES ==========\n";
    std::cout << "Transaction ID: " << transaction_id << "\n";
    std::cout << "Siuntejo adresas: " << sender_key << "\n";
    std::cout << "Gavejo adresas: " << receiver_key << "\n";
    std::cout << "Suma: " << std::fixed << std::setprecision(8) << amount << " vnt.\n";
    
    auto time_t = std::chrono::system_clock::to_time_t(timestamp);
    std::cout << "Timestamp: " << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S") << "\n";
    
    std::cout << "Hash validus: " << (arValidusHash() ? "TAIP" : "NE") << "\n";
    std::cout << "Transakcija validi: " << (arValid() ? "TAIP" : "NE") << "\n";
    std::cout << "==========================================\n";
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
        std::cout << "Per mazai vartotoju transakcijoms generuoti!\n";
        return;
    }
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> vartotojas_dis(0, vartotojai.size() - 1);
    
    std::cout << "\n";
    std::cout << "========================================================\n";
    std::cout << "         TRANSAKCIJU GENERAVIMO PROCESAS                \n";
    std::cout << "========================================================\n";
    std::cout << "Tikslas: Sugeneruoti " << kiekis << " transakciju\n";
    std::cout << "Vartotoju: " << vartotojai.size() << "\n";
    std::cout << "Su balanso tikrinimu: TAIP\n";
    std::cout << "========================================================\n\n";
    
    int sekmingos = 0;
    int bandymai = 0;
    const int max_bandymai = kiekis * 3;
    
    bool rodyti_detales = true;
    int detales_parodytos = 0;
    const int max_detales = 5;
    
    while (sekmingos < kiekis && bandymai < max_bandymai) {
        bandymai++;
        
        int sender_idx = vartotojas_dis(gen);
        int receiver_idx = vartotojas_dis(gen);
        
        if (sender_idx == receiver_idx) continue;
        
        auto& sender = vartotojai[sender_idx];
        auto& receiver = vartotojai[receiver_idx];
        
        double sender_balansas = sender->gautiBalansa();
        
        // BALANSO TIKRINIMAS
        if (sender_balansas < 1.0) {
            if (rodyti_detales && detales_parodytos < 2) {
                std::cout << "  [Praleista: " << sender->gautiVarda() 
                          << " neturi pakankamai lesu]\n";
            }
            continue;
        }
        
        std::uniform_real_distribution<> suma_dis(0.05, 0.5);
        double amount = sender_balansas * suma_dis(gen);
        amount = std::round(amount * 100.0) / 100.0;
        
        // Papildomas balanso tikrinimas
        if (amount < 0.01 || amount > sender_balansas) {
            continue;
        }
        
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
            
            // Patikrinti transakcijos validumą
            if (!tx->arValid()) {
                std::cout << "  [Klaida: Netinkama transakcija]\n";
                continue;
            }
            
            if (rodyti_detales && detales_parodytos < max_detales) {
                std::cout << "\n--- TRANSAKCIJA #" << (sekmingos + 1) << " ---\n";
                std::cout << "Siuntejas: " << sender->gautiVarda() 
                          << " (Balansas: " << std::fixed << std::setprecision(2) 
                          << sender_balansas << " vnt.)\n";
                std::cout << "Gavejas: " << receiver->gautiVarda() << "\n";
                std::cout << "Suma: " << amount << " vnt. (" 
                          << std::fixed << std::setprecision(1) 
                          << (amount * 100.0 / sender_balansas) << "% balanso)\n";
                std::cout << "TX ID: " << tx->gautiTransactionId().substr(0, 32) << "...\n";
                std::cout << "Hash validus: " << (tx->arValidusHash() ? "TAIP" : "NE") << "\n";
                detales_parodytos++;
                
                if (detales_parodytos == max_detales) {
                    std::cout << "\n[Tolimesnes transakcijos generuojamos be detalu...]\n";
                }
            }
            
            pridetiTransakcija(std::move(tx));
            sekmingos++;
            
            if (sekmingos % 1000 == 0) {
                std::cout << "\nPazanga: " << sekmingos << " / " << kiekis 
                          << " (" << (sekmingos * 100 / kiekis) << "%)\n";
            }
        } catch (const std::exception& e) {
            std::cout << "  [Klaida: " << e.what() << "]\n";
            continue;
        }
    }
    
    std::cout << "\n========================================================\n";
    std::cout << "           GENERAVIMO REZULTATAI                        \n";
    std::cout << "========================================================\n";
    std::cout << "Sekmingai sugeneruota: " << sekmingos << " transakciju\n";
    std::cout << "Is viso bandymu: " << bandymai << "\n";
    std::cout << "Sekmes procentas: " << std::fixed << std::setprecision(1) 
              << (sekmingos * 100.0 / bandymai) << "%\n";
    std::cout << "Bendra suma: " << std::fixed << std::setprecision(2) 
              << bendras_suma << " vnt.\n";
    std::cout << "========================================================\n\n";
}

bool TransakcijuBaseinas::vykdytiTransakcija(
    Transakcija& transakcija,
    std::vector<std::unique_ptr<Vartotojas>>& vartotojai,
    bool verbose) {
    
    // Coinbase transakcijos
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
            if (verbose) {
                std::cout << "  Coinbase transakcija: " << transakcija.gautiAmount() << " vnt.\n";
            }
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
    
    if (!sender || !receiver) {
        if (verbose) std::cout << "   Siuntejas arba gavejas nerastas\n";
        return false;
    }
    
    double amount = transakcija.gautiAmount();
    double sender_balansas = sender->gautiBalansa();
    
    // KRITINIS BALANSO TIKRINIMAS
    if (sender_balansas < amount) {
        if (verbose) {
            std::cout << "   Nepakankamas balansas: " << sender_balansas 
                      << " < " << amount << "\n";
        }
        return false;
    }
    
    const auto& utxo_list = sender->gautiUTXO();
    std::vector<size_t> naudojami_utxo;
    double surinkta_suma = 0.0;
    
    for (size_t i = 0; i < utxo_list.size() && surinkta_suma < amount; ++i) {
        surinkta_suma += utxo_list[i].suma;
        naudojami_utxo.push_back(i);
    }
    
    if (surinkta_suma < amount) {
        if (verbose) std::cout << "   Nepakanka UTXO\n";
        return false;
    }
    
    for (auto it = naudojami_utxo.rbegin(); it != naudojami_utxo.rend(); ++it) {
        const auto& utxo = utxo_list[*it];
        sender->panaudotiUTXO(utxo.txid, utxo.vout);
    }
    
    double graza = surinkta_suma - amount;
    
    receiver->pridetiUTXO(UTXO(transakcija.gautiTransactionId(), 0, amount));
    
    if (graza > 0.01) {
        sender->pridetiUTXO(UTXO(transakcija.gautiTransactionId(), 1, graza));
    }
    
    if (verbose) {
        std::cout << "   Transakcija: " << amount << " vnt. (Graza: " 
                  << graza << ")\n";
    }
    
    return true;
}

int TransakcijuBaseinas::vykdytiVisasTransakcijas(
    std::vector<std::unique_ptr<Vartotojas>>& vartotojai) {
    
    std::cout << "\n========================================================\n";
    std::cout << "         TRANSAKCIJU VYKDYMO PROCESAS                   \n";
    std::cout << "========================================================\n";
    std::cout << "Transakciju kiekis: " << transakcijos.size() << "\n";
    std::cout << "========================================================\n\n";
    
    int sekmingos = 0;
    int nesekmingos = 0;
    
    for (auto& tx : transakcijos) {
        if (vykdytiTransakcija(*tx, vartotojai, false)) {
            sekmingos++;
        } else {
            nesekmingos++;
        }
        
        if ((sekmingos + nesekmingos) % 1000 == 0) {
            std::cout << "Apdorota: " << (sekmingos + nesekmingos) 
                     << " / " << transakcijos.size() 
                     << " (" << ((sekmingos + nesekmingos) * 100 / transakcijos.size()) << "%)\n";
        }
    }
    
    std::cout << "\n========================================================\n";
    std::cout << "              VYKDYMO REZULTATAI                        \n";
    std::cout << "========================================================\n";
    std::cout << "Sekmingai ivykdyta: " << sekmingos << "\n";
    std::cout << "Nepavyko: " << nesekmingos << "\n";
    std::cout << "Sekmes procentas: " << std::fixed << std::setprecision(1)
              << (sekmingos * 100.0 / transakcijos.size()) << "%\n";
    std::cout << "========================================================\n\n";
    
    return sekmingos;
}

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

const Transakcija* TransakcijuBaseinas::rastiTransakcija(const std::string& tx_id) const {
    for (const auto& tx : transakcijos) {
        if (tx->gautiTransactionId() == tx_id) {
            return tx.get();
        }
    }
    return nullptr;
}

void TransakcijuBaseinas::spausdintiStatistika() const {
    std::cout << "\n========================================================\n";
    std::cout << "           TRANSAKCIJU BASEINO STATISTIKA               \n";
    std::cout << "========================================================\n";
    std::cout << "Transakciju kiekis: " << std::setw(8) << transakcijos.size() << "\n";
    std::cout << "Bendra suma:        " << std::setw(14) << std::fixed << std::setprecision(2) 
              << bendras_suma << " vnt.\n";
    
    if (!transakcijos.empty()) {
        double vidutine = bendras_suma / transakcijos.size();
        std::cout << "Vidutine suma:      " << std::setw(14) << std::fixed << std::setprecision(2) 
                  << vidutine << " vnt.\n";
        
        double min_suma = transakcijos[0]->gautiAmount();
        double max_suma = transakcijos[0]->gautiAmount();
        
        for (const auto& tx : transakcijos) {
            double suma = tx->gautiAmount();
            if (suma < min_suma) min_suma = suma;
            if (suma > max_suma) max_suma = suma;
        }
        
        std::cout << "Minimali suma:      " << std::setw(14) << std::fixed << std::setprecision(2)
                  << min_suma << " vnt.\n";
        std::cout << "Maksimali suma:     " << std::setw(14) << std::fixed << std::setprecision(2)
                  << max_suma << " vnt.\n";
    }
    std::cout << "========================================================\n\n";
}

void TransakcijuBaseinas::spausdintiTransakcijas(int kiek) const {
    std::cout << "\n========================================================\n";
    std::cout << "             PIRMOS " << kiek << " TRANSAKCIJOS                \n";
    std::cout << "========================================================\n\n";
    
    int count = 0;
    for (const auto& tx : transakcijos) {
        if (count >= kiek) break;
        
        std::cout << "--- Transakcija #" << (count + 1) << " ---\n";
        tx->spausdintiInfo();
        std::cout << "\n";
        
        count++;
    }
    
    std::cout << "========================================================\n";
}

void TransakcijuBaseinas::isvalyti() {
    transakcijos.clear();
    bendras_suma = 0.0;
    std::cout << "Transakciju baseinas isvalytas.\n";
}