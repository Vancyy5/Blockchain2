#include "vartotojas.h"
#include "transakcija.h"
#include "blokasirgrandine.h"
#include "lib.h"
#include "laikas.h"

#ifdef _WIN32
#include <direct.h>
#define MKDIR(path) _mkdir(path)
#else
#include <sys/stat.h>
#define MKDIR(path) mkdir(path, 0755)
#endif

const std::vector<std::string> VARDAI = {
    "Vanesa", "Jokubas", "Gabija", "Vakare", "Patricija",
    "Margarita", "Tomas", "Mantas", "Lukas", "Matas",
    "Paulius", "Andrius", "Vytautas", "Rokas", "Laura",
    "Marija", "Ona", "Rasa", "Egle", "Indre"
};

// PATAISYTA: Funkcija perkelta i priekį
void sukurtiAplankaJeiguNeegzistuoja(const std::string& path) {
    MKDIR(path.c_str());
}

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

// NAUJA: Issaugoti atskirai kiekviena bloka
void issaugotiBlokaDetales(const Blokas* blokas, const std::string& aplankas) {
    if (!blokas) return;
    
    std::stringstream filename;
    filename << aplankas << "/blokas_" 
             << std::setfill('0') << std::setw(4) << blokas->gautiBlokoNumeri() 
             << ".csv";
    
    std::ofstream out(filename.str());
    if (!out.is_open()) {
        std::cout << "Klaida: Nepavyko sukurti failo " << filename.str() << "\n";
        return;
    }
    
    out << "TxID,Sender,Receiver,Amount,Timestamp\n";
    
    const auto& txs = blokas->gautiTransakcijas();
    for (const auto& tx : txs) {
        auto time_t = std::chrono::system_clock::to_time_t(tx->gautiTimestamp());
        out << tx->gautiTransactionId() << ","
            << tx->gautiSender() << ","
            << tx->gautiReceiver() << ","
            << std::fixed << std::setprecision(2) << tx->gautiAmount() << ","
            << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S") << "\n";
    }
    
    out.close();
}

void issaugotiVisusBlokus(const Blockchain& blockchain, const std::string& aplankas) {
    sukurtiAplankaJeiguNeegzistuoja(aplankas);
    
    std::cout << "\nIssaugojami atskiri blokai...\n";
    for (size_t i = 0; i < blockchain.gautiIlgi(); ++i) {
        const Blokas* blokas = blockchain.gautiBlokaPagalIndeksa(i);
        if (blokas) {
            issaugotiBlokaDetales(blokas, aplankas);
        }
    }
    std::cout << "Issaugota " << blockchain.gautiIlgi() << " bloku i '" << aplankas << "'\n";
}

bool sukurtiVienaBloka(Blockchain& blockchain, 
                       TransakcijuBaseinas& baseinas,
                       std::vector<std::unique_ptr<Vartotojas>>& vartotojai,
                       int tx_per_block = 100) {
    
    if (baseinas.gautiKieki() < static_cast<size_t>(tx_per_block)) {
        std::cout << "\nNepakanka transakciju! (Reikia: " << tx_per_block 
                  << ", Yra: " << baseinas.gautiKieki() << ")\n";
        return false;
    }
    
    std::cout << "\n=== KURIAMAS NAUJAS BLOKAS ===\n";
    
    Laikas timer("Bloko kasimas ir patvirtinimas");
    
    // 1. Pasirinkti transakcijas
    const auto& visos_tx = baseinas.gautiTransakcijas();
    std::vector<std::shared_ptr<Transakcija>> pasirinktos;
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::vector<size_t> indeksai;
    for (size_t i = 0; i < std::min(visos_tx.size(), static_cast<size_t>(tx_per_block)); ++i) {
        indeksai.push_back(i);
    }
    
    for (size_t i : indeksai) {
        pasirinktos.push_back(
            std::make_shared<Transakcija>(*visos_tx[i])
        );
    }
    
    // 2. Parinkti kaseja
    std::uniform_int_distribution<> miner_dis(0, vartotojai.size() - 1);
    std::string miner_address = vartotojai[miner_dis(gen)]->gautiPublicKey();
    
    // 3. Gauti paskutinio bloko hash
    const Blokas* paskutinis = blockchain.gautiPaskutiniBloka();
    std::string prev_hash = paskutinis ? paskutinis->gautiBlokoHash() : 
        "0000000000000000000000000000000000000000000000000000000000000000";
    
    // 4. Sukurti ir iskasti bloka
    auto naujas_blokas = std::make_unique<Blokas>(
        blockchain.gautiIlgi(),
        prev_hash,
        pasirinktos,
        miner_address,
        blockchain.gautiDifficulty()
    );
    
    // 5. Vykdyti transakcijas (atnaujinti balansus)
    std::cout << "\nAtnaujinami vartotoju balansai...\n";
    const auto& bloko_tx = naujas_blokas->gautiTransakcijas();
    int sekmingos = 0;
    for (auto& tx : bloko_tx) {
        if (baseinas.vykdytiTransakcija(*tx, vartotojai)) {
            sekmingos++;
        }
    }
    std::cout << "Sekmingai atnaujinta: " << sekmingos << " transakciju\n";
    
    // 6. Prideti bloka i grandine
    naujas_blokas->spausdintiInfo();
    blockchain.pridetiBloka(std::move(naujas_blokas));
    
    // 7. Pasalinti itrauktas transakcijas is baseino
    std::cout << "Salinamos itrauktos transakcijos is baseino...\n";
    baseinas.pasalintiTransakcijas(pasirinktos);
    std::cout << "Liko transakciju baseine: " << baseinas.gautiKieki() << "\n";
    
    return true;
}

void kastiVisusBlokus(Blockchain& blockchain, 
                      TransakcijuBaseinas& baseinas,
                      std::vector<std::unique_ptr<Vartotojas>>& vartotojai,
                      int tx_per_block = 100) {
    
    if (baseinas.gautiKieki() < static_cast<size_t>(tx_per_block)) {
        std::cout << "\nNepakanka transakciju blokui!\n";
        return;
    }
    
    std::cout << "\n=== PRADEDAMAS AUTOMATINIS BLOKU KASIMAS ===\n";
    std::cout << "Transakciju baseine: " << baseinas.gautiKieki() << "\n";
    std::cout << "Transakciju per bloka: " << tx_per_block << "\n";
    
    int numatytas_bloku_sk = baseinas.gautiKieki() / tx_per_block;
    std::cout << "Numatoma sukurti ~" << numatytas_bloku_sk << " bloku\n\n";
    
    Laikas bendras_laikas("Visu bloku kasimas");
    
    int sukurta_bloku = 0;
    
    while (baseinas.gautiKieki() >= static_cast<size_t>(tx_per_block)) {
        sukurta_bloku++;
        std::cout << "\n========== BLOKAS #" << sukurta_bloku << " ==========\n";
        
        if (!sukurtiVienaBloka(blockchain, baseinas, vartotojai, tx_per_block)) {
            break;
        }
        
        std::cout << "\n";
    }
    
    std::cout << "\n=== KASIMO SUVESTINE ===\n";
    std::cout << "Sukurta bloku: " << sukurta_bloku << "\n";
    std::cout << "Liko transakciju baseine: " << baseinas.gautiKieki() << "\n";
    std::cout << "Bloku grandineje: " << blockchain.gautiIlgi() << "\n";
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
        std::cout << "3. Sukurti nauja bloka (1 blokas)\n";
        std::cout << "4. Kasti blokus kol neliks transakciju (AUTOMATINIS)\n";
        std::cout << "5. Spausdinti blockchain\n";
        std::cout << "6. Spausdinti statistika\n";
        std::cout << "7. Spausdinti transakciju baseino statistika\n";
        std::cout << "8. Issaugoti duomenis i failus\n";
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
                
                std::random_device rd;
                std::mt19937 gen(rd());
                std::uniform_int_distribution<> vardas_dis(0, VARDAI.size() - 1);
                std::uniform_real_distribution<> balansas_dis(100.0, 1000000.0);
                
                vartotojai.clear();
                
                std::cout << "\n========================================================\n";
                std::cout << "          VARTOTOJU GENERAVIMO PROCESAS                 \n";
                std::cout << "========================================================\n";
                std::cout << "Tikslas: Sugeneruoti 1000 vartotoju\n";
                std::cout << "Balansu ruozas: 100.00 - 1,000,000.00 vnt.\n";
                std::cout << "========================================================\n\n";
                
                double bendra_suma = 0.0;
                
                for (int i = 0; i < 1000; ++i) {
                    std::string vardas = VARDAI[vardas_dis(gen)] + std::to_string(i);
                    double balansas = balansas_dis(gen);
                    
                    auto vartotojas = std::make_unique<Vartotojas>(vardas, balansas);
                    bendra_suma += balansas;
                    
                    if (i < 5) {
                        std::cout << "Vartotojas #" << (i + 1) << ":\n";
                        std::cout << "  Vardas: " << vardas << "\n";
                        std::cout << "  Balansas: " << std::fixed << std::setprecision(2) 
                                  << balansas << " vnt.\n";
                        std::cout << "  Public Key: " << vartotojas->gautiPublicKey().substr(0, 20) << "...\n\n";
                    }
                    
                    vartotojai.push_back(std::move(vartotojas));
                    
                    if ((i + 1) % 200 == 0) {
                        std::cout << "Pazanga: " << (i + 1) << " / 1000 vartotoju\n";
                    }
                }
                
                std::cout << "\n========================================================\n";
                std::cout << "              GENERAVIMO REZULTATAI                     \n";
                std::cout << "========================================================\n";
                std::cout << "Sugeneruota vartotoju: " << vartotojai.size() << "\n";
                std::cout << "Bendra suma: " << std::fixed << std::setprecision(2) 
                          << bendra_suma << " vnt.\n";
                std::cout << "Vidutinis balansas: " << (bendra_suma / vartotojai.size()) << " vnt.\n";
                std::cout << "========================================================\n\n";
                
                break;
            }
            case 2: {
                if (vartotojai.size() < 2) {
                    std::cout << "\nKlaida: Pirmiausia sugeneruokite vartotojus!\n";
                    break;
                }
                
                Laikas timer("Transakciju generavimas");
                
                baseinas.isvalyti();
                baseinas.generuotiTransakcijas(vartotojai, 10000);
                
                baseinas.spausdintiStatistika();
                break;
            }
            case 3: {
                if (!blockchain) {
                    blockchain = std::make_unique<Blockchain>(3);
                }
                
                if (vartotojai.empty()) {
                    std::cout << "\nKlaida: Pirmiausia sugeneruokite vartotojus!\n";
                    break;
                }
                
                sukurtiVienaBloka(*blockchain, baseinas, vartotojai, 100);
                break;
            }
            case 4: {
                if (!blockchain) {
                    blockchain = std::make_unique<Blockchain>(3);
                }
                
                if (vartotojai.empty()) {
                    std::cout << "\nKlaida: Pirmiausia sugeneruokite vartotojus!\n";
                    break;
                }
                
                kastiVisusBlokus(*blockchain, baseinas, vartotojai, 100);
                break;
            }
            case 5: {
                if (!blockchain || blockchain->gautiIlgi() == 0) {
                    std::cout << "\nBlockchain tuscias!\n";
                    break;
                }
                blockchain->spausdintiGrandine();
                break;
            }
            case 6: {
                if (!blockchain || blockchain->gautiIlgi() == 0) {
                    std::cout << "\nBlockchain tuscias!\n";
                    break;
                }
                blockchain->spausdintiStatistika();
                break;
            }
            case 7: {
                baseinas.spausdintiStatistika();
                break;
            }
            case 8: {
                Laikas timer("Duomenu issaugojimas");
                
                if (!vartotojai.empty()) {
                    issaugotiVartotojus(vartotojai, "output/vartotojai.csv");
                }
                if (baseinas.gautiKieki() > 0) {
                    issaugotiTransakcijas(baseinas, "output/transakcijos.csv");
                }
                if (blockchain && blockchain->gautiIlgi() > 0) {
                    issaugotiBlockchain(*blockchain, "output/blockchain.csv");
                    // NAUJA: Issaugoti atskirai kiekviena bloka
                    issaugotiVisusBlokus(*blockchain, "output/blokai");
                }
                
                std::cout << "\nVisi duomenys issaugoti 'output' aplanke!\n";
                break;
            }
            case 0: {
                std::cout << "\nViso gero!\n";
                return 0;
            }
            default: {
                std::cout << "\nNetinkama ivestis!\n";
            }
        }
    }
    
    return 0;
}