#include "vartotojas.h"
#include "transakcija.h"
#include "blokas.h"
#include "lib.h"
#include "laikas.h"
#include <direct.h>

const std::vector<std::string> VARDAI = {
    "Vanesa", "Jokūbas", "Gabija", "Vakarė", "Patricija",
    "Margarita", "Tomas", "Mantas", "Lukas", "Matas",
    "Paulius", "Andrius", "Vytautas", "Rokas", "Laura",
    "Marija", "Ona", "Rasa", "Eglė", "Indrė"
};

void issaugotiVartotojus(const std::vector<std::unique_ptr<Vartotojas>>& vartotojai, const std::string& failas) {
    std::ofstream out(failas);
    if (!out.is_open()) {
        std::cout << "Klaida: Nepavyko atidaryti failo " << failas << "\n";
        return;
    }
    
    out << "Name,PublicKey,Balance,UTXO_Count\n";
    for (const auto& v : vartotojai) {
        out << v->gautiVarda() << ","
            << v->gautiPublicKey() << ","
            << std::fixed << std::setprecision(2) << v->gautiBalansa() << ","
            << v->gautiUTXO().size() << "\n";
    }
    out.close();
    std::cout << "Vartotojai issaugoti i " << failas << "\n";
}

void issaugotiTransakcijas(const TransakcijuBaseinas& baseinas, const std::string& failas) {
    std::ofstream out(failas);
    if (!out.is_open()) {
        std::cout << "Klaida: Nepavyko atidaryti failo " << failas << "\n";
        return;
    }
    
    out << "TxID,Sender,Receiver,Amount,Timestamp\n";
    const auto& txs = baseinas.gautiTransakcijas();
    for (const auto& tx : txs) {
        auto time_t = std::chrono::system_clock::to_time_t(tx->gautiTimestamp());
        out << tx->gautiTransactionId() << ","
            << tx->gautiSender() << ","
            << tx->gautiReceiver() << ","
            << std::fixed << std::setprecision(2) << tx->gautiAmount() << ","
            << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S") << "\n";
    }
    out.close();
    std::cout << "Transakcijos issaugotos i " << failas << "\n";
}

void issaugotiBlockchain(const Blockchain& blockchain, const std::string& failas) {
    std::ofstream out(failas);
    if (!out.is_open()) {
        std::cout << "Klaida: Nepavyko atidaryti failo " << failas << "\n";
        return;
    }
    
    out << "BlockNum,Hash,PrevHash,MerkleRoot,Timestamp,Nonce,Difficulty,TxCount,TotalAmount\n";
    
    for (size_t i = 0; i < blockchain.gautiIlgi(); ++i) {
        const Blokas* blokas = blockchain.gautiBlokaPagalIndeksa(i);
        if (!blokas) continue;
        
        auto time_t = std::chrono::system_clock::to_time_t(blokas->gautiTimestamp());
        out << blokas->gautiBlokoNumeri() << ","
            << blokas->gautiBlokoHash() << ","
            << blokas->gautiPrevHash() << ","
            << blokas->gautiMerkleRoot() << ","
            << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S") << ","
            << blokas->gautiNonce() << ","
            << blokas->gautiDifficulty() << ","
            << blokas->gautiTransakcijuKieki() << ","
            << std::fixed << std::setprecision(2) << blokas->gautiBendraTransakcijuSuma() << "\n";
    }
    out.close();
    std::cout << "Blockchain issaugotas i " << failas << "\n";
}

void sukurtiAplankaJeiguNeegzistuoja(const std::string& path) {
    _mkdir(path.c_str());
}

int main() {
    sukurtiAplankaJeiguNeegzistuoja("output");
    
    std::vector<std::unique_ptr<Vartotojas>> vartotojai;
    TransakcijuBaseinas baseinas;
    std::unique_ptr<Blockchain> blockchain;
    
    int pasirinkimas;
    
    while (true) {
        std::cout << "\n=== BLOCKCHAIN SISTEMA ===\n";
        std::cout << "1. Generuoti 1000 vartotoju\n";
        std::cout << "2. Generuoti 10000 transakciju\n";
        std::cout << "3. Sukurti nauja bloka\n";
        std::cout << "4. Spausdinti blockchain\n";
        std::cout << "5. Spausdinti statistika\n";
        std::cout << "6. Issaugoti duomenis i failus\n";
        std::cout << "0. Iseiti\n";
        std::cout << "Pasirinkimas: ";
        std::cin >> pasirinkimas;
        
        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            std::cout << "\nNetinkamas pasirinkimas!\n";
            continue;
        }
        
        switch (pasirinkimas) {
            case 1: {
                Laikas timer("Vartotoju generavimas");
                timer.pradeti();
                
                std::random_device rd;
                std::mt19937 gen(rd());
                std::uniform_int_distribution<> vardas_dis(0, VARDAI.size() - 1);
                std::uniform_real_distribution<> balansas_dis(100.0, 1000000.0);
                
                vartotojai.clear();
                std::cout << "\nGeneruojami 1000 vartotoju...\n";
                
                for (int i = 0; i < 1000; ++i) {
                    std::string vardas = VARDAI[vardas_dis(gen)] + std::to_string(i + 1);
                    double balansas = balansas_dis(gen);
                    vartotojai.push_back(std::make_unique<Vartotojas>(vardas, balansas));
                    
                    if ((i + 1) % 100 == 0) {
                        std::cout << "Sugeneruota: " << (i + 1) << "/1000\n";
                    }
                }
                
                timer.baigti();
                std::cout << "Sekmingai sugeneruoti 1000 vartotoju!\n";
                break;
            }
            case 2: {
                if (vartotojai.size() < 2) {
                    std::cout << "\nKlaida: Pirmiausia sugeneruokite vartotojus!\n";
                    break;
                }
                
                Laikas timer("Transakciju generavimas");
                timer.pradeti();
                
                baseinas.isvalyti();
                baseinas.generuotiTransakcijas(vartotojai, 10000);
                
                timer.baigti();
                baseinas.spausdintiStatistika();
                break;
            }
            case 3: {
                if (baseinas.gautiKieki() < 100) {
                    std::cout << "\nKlaida: Nepakanka transakciju (reikia 100)!\n";
                    break;
                }
                
                if (!blockchain) {
                    blockchain = std::make_unique<Blockchain>(3); // Sunkumas = 3
                }
                
                Laikas timer("Bloko kasimas");
                timer.pradeti();
                
                const auto& visos_tx = baseinas.gautiTransakcijas();
                std::vector<std::shared_ptr<Transakcija>> pasirinktos;
                
                std::random_device rd;
                std::mt19937 gen(rd());
                std::vector<size_t> indeksai;
                for (size_t i = 0; i < visos_tx.size(); ++i) {
                    indeksai.push_back(i);
                }
                std::shuffle(indeksai.begin(), indeksai.end(), gen);
                
                for (int i = 0; i < 100; ++i) {
                    pasirinktos.push_back(
                        std::make_shared<Transakcija>(*visos_tx[indeksai[i]])
                    );
                }
                
                const Blokas* paskutinis = blockchain->gautiPaskutiniBloka();
                std::string prev_hash = paskutinis ? paskutinis->gautiBlokoHash() : 
                    "0000000000000000000000000000000000000000000000000000000000000000";
                
                // Select random miner from users
                std::uniform_int_distribution<> miner_dis(0, vartotojai.size() - 1);
                std::string miner_address = vartotojai[miner_dis(gen)]->gautiPublicKey();
                
                auto naujas_blokas = std::make_unique<Blokas>(
                    blockchain->gautiIlgi(),
                    prev_hash,
                    pasirinktos,
                    miner_address,
                    3  // Difficulty
                );
                
                timer.baigti();
                
                naujas_blokas->spausdintiInfo();
                blockchain->pridetiBloka(std::move(naujas_blokas));
                break;
            }
            case 4: {
                if (!blockchain || blockchain->gautiIlgi() == 0) {
                    std::cout << "\nBlockchain tuščias!\n";
                    break;
                }
                blockchain->spausdintiGrandine();
                break;
            }
            case 5: {
                if (!blockchain || blockchain->gautiIlgi() == 0) {
                    std::cout << "\nBlockchain tuščias!\n";
                    break;
                }
                blockchain->spausdintiStatistika();
                break;
            }
            case 6: {
                Laikas timer("Saving data");
                timer.pradeti();
                
                if (!vartotojai.empty()) {
                    issaugotiVartotojus(vartotojai, "output/vartotojai.csv");
                }
                if (baseinas.gautiKieki() > 0) {
                    issaugotiTransakcijas(baseinas, "output/transakcijos.csv");
                }
                if (blockchain && blockchain->gautiIlgi() > 0) {
                    issaugotiBlockchain(*blockchain, "output/blockchain.csv");
                }
                
                timer.baigti();
                std::cout << "\nIssaugota 'output' albume!\n";
                break;
            }
            case 0: {
                std::cout << "\nViso gero!\n";
                return 0;
            }
            default: {
                std::cout << "\nNetinkama įvestis!\n";
            }
        }
    }
    
    return 0;
}