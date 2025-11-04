# Supaprastinta Blokų Grandinė #v0.1

## Turinys
- [Apie projektą](#apie-projektą)
- [Naudojimosi instrukcija](#naudojimosi-instrukcija)
- [Pagrindinės klasės](#pagrindinės-klasės)
- [UTXO modelio realizavimas](#utxo-modelio-realizavimas)
- [Proof-of-Work mechanizmas](#proof-of-work-mechanizmas)
- [Merkle Tree realizacija (v0.2)](#merkle-tree-realizacija-v02)
- [Kandidatinių blokų kasimas (v0.2)](#kandidatinių-blokų-kasimas-v02)
- [Dirbtinio intelekto pagalba](#dirbtinio-intelekto-pagalba)

## Apie projektą

Šis projektas realizuoja supaprastintą blokų grandinę (blockchain), imituojančią jos veikimą realiomis sąlygomis. 

**v0.1:**
1. Centralizuota blokų grandinė  
2. Modifikuota maišos funkcija (generuoja hash su 000... pradžia)  
3. Vartotojų generavimas (~1000)  
4. Transakcijų generavimas (~10000)  
5. Proof-of-Work kasimo procesas  
6. UTXO modelio realizavimas  
7. Detalus konsolės išvedimas su vizualizacijomis  
8. OOP principų taikymas (enkapsuliacija, RAII, konstruktoriai)

**v0.2 papildymai:**
1. Merkle Tree realizacija su tikru Merkle Root Hash
2. Transakcijų verifikacija:
   - Balanso tikrinimas (siuntėjas negali siųsti daugiau, nei turi)
   - Transaction ID tikrinimas (maišos reikšmės teisingumas)
   - UTXO egzistavimo tikrinimas
3. Kandidatinių blokų kasimas:
   - Generuojami 5 kandidatiniai blokai (~100 transakcijų kiekviename)
   - Kasami ribotas laikas (~5 sekundės)
   - Parenkamas geriausias (su mažiausiu nonce)
4. Praplėstas meniu


# Naudojimosi instrukcija

### Iš GitHub

```bash
# Klonuoti repozitoriją
git clone https://github.com/[vartotojas]/blockchain-v01.git
cd blockchain-v01

# ARBA parsisiųsti ZIP
Eiti į GitHub → Code → Download ZIP
Išarchyvuoti ir atidaryti terminalą tame aplanke
```

### Reikalingi įrankiai

**Linux (Ubuntu/Debian):**
```bash
sudo apt update
sudo apt install build-essential g++ make
```

**MacOS:**
```bash
# Įdiegti Xcode Command Line Tools
xcode-select --install
```

**Windows:**
- **MinGW-w64**: https://www.mingw-w64.org/downloads/
- **MSYS2** (rekomenduojama): https://www.msys2.org/
  ```bash
  # Po MSYS2 įdiegimo
  pacman -S mingw-w64-x86_64-gcc make
  ```

## Kompiliavimas ir paleidimas

### Naudojant Makefile 

**Linux / MacOS / Git Bash (Windows):**

```bash
# Kompiliuoti
make

# Paleisti
make run

# Išvalyti sukompiliuotus failus
make clean

# Kompiliuoti ir iš karto paleisti
make && make run
```

**Makefile komandos:**
- `make` arba `make all` - Sukompiliuoja programą
- `make run` - Paleidžia programą
- `make clean` - Ištrina .o failus ir executable
- `make rebuild` - Išvalo ir perkompiliuoja iš naujo

### Meniu sistema

![Meniu](<nuotraukos/Screenshot 2025-11-04 225315.png>)
![Meniu](<nuotraukos/Screenshot 2025-11-04 225844.png>)
![Meniu](<nuotraukos/Screenshot 2025-11-04 231009.png>)
![Meniu](<nuotraukos/Screenshot 2025-11-04 230744.png>)

### Išvesties failai

![Output failai](<nuotraukos/Screenshot 2025-10-28 115701 copy.png>)

![Output failai](<nuotraukos/Screenshot 2025-10-28 120145.png>)

## Pagrindinės klasės

### 1. `Vartotojas` (vartotojas.h/.cpp)

Reprezentuoja blockchain vartotoją su UTXO modeliu.

**Pagrindiniai laukai:**
```cpp
- std::string vardas           // Vartotojo vardas
- std::string public_key       // Viešasis raktas (0x... formatu)
- std::vector<UTXO> utxo_rinkinys  // Nepanaudotų išėjimų rinkinys
```

**Pagrindinės funkcijos:**
- `Vartotojas(vardas, pradinis_balansas)` - Konstruktorius, generuoja UTXO
- `gautiBalansa()` - Skaičiuoja balansą iš visų UTXO
- `pridetiUTXO(utxo)` - Prideda naują nepanaudotą išėjimą
- `panaudotiUTXO(txid, vout)` - Panaudoja konkretų UTXO

**UTXO struktūra:**
```cpp
struct UTXO {
    std::string txid;      // Transakcijos ID
    unsigned int vout;     // Išėjimo indeksas
    double suma;           // Suma
};
```

![Vartotojas](<nuotraukos/Screenshot 2025-11-04 225345.png>)

![Vartotojas](<nuotraukos/Screenshot 2025-11-04 225352.png>)

### 2. `Transakcija` (transakcija.h/.cpp)

Reprezentuoja vieną pervedimą tarp vartotojų.

**Pagrindiniai laukai:**
```cpp
- std::string transaction_id    // Hash iš visų kitų laukų
- std::string sender_key        // Siuntėjo viešasis raktas
- std::string receiver_key      // Gavėjo viešasis raktas
- double amount                 // Pervedama suma
- std::chrono::system_clock::time_point timestamp
```

**Pagrindinės funkcijos:**
- `Transakcija(sender, receiver, amount)` - Sukuria naują transakciją
- `arValid()` - **v0.2: Patikrina transakcijos validumą**
- `arValidusHash()` - **v0.2: Tikrina hash teisingumą**
- `generuotiHash()` - Generuoja unikalų transakcijos ID

**v0.2 Validacijos patikros:**
```cpp
bool Transakcija::arValid() const {
    // 1. Pagrindinių laukų tikrinimas
    if (transaction_id.empty() || receiver_key.empty() || amount <= 0)
        return false;
    
    // 2. Coinbase specialus atvejis
    if (sender_key == "COINBASE")
        return true;
    
    // 3. Siuntėjas ≠ Gavėjas
    if (sender_key.empty() || sender_key == receiver_key)
        return false;
    
    // 4. Hash teisingumo tikrinimas
    if (!arValidusHash())
        return false;
    
    return true;
}
```

![Transakcija](<nuotraukos/Screenshot 2025-11-04 225409.png>)

![Transakcija](<nuotraukos/Screenshot 2025-11-04 225425.png>)

### 3. `TransakcijuBaseinas` (transakcija.h/.cpp)

Valdo laukiančių transakcijų sąrašą (mempool).

**Pagrindinės funkcijos:**
- `generuotiTransakcijas(vartotojai, kiekis)` - Generuoja transakcijas
- `vykdytiTransakcija(tx, vartotojai)` - **v0.2: Su balanso tikrinimu**
- `pasalintiTransakcijas(txs)` - Pašalina patvirtintas transakcijas
- `spausdintiStatistika()` - Rodo baseino statistiką

**v0.2 Balanso tikrinimas:**
```cpp
bool TransakcijuBaseinas::vykdytiTransakcija(...) {
    // KRITINIS BALANSO TIKRINIMAS
    double sender_balansas = sender->gautiBalansa();
    if (sender_balansas < amount) {
        return false;  // Nepakankamas balansas
    }
    
    // Renkame UTXO kol surinksime reikiamą sumą
    // ...
    
    if (surinkta_suma < amount) {
        return false;  // Nepakanka UTXO
    }
    
    // Vykdome transakciją
    // ...
}
```
![Transakcijubaseinas](<nuotraukos/Screenshot 2025-10-28 120132.png>)

### 4. `BlokoAntraste` (blokasirgrandine.h/.cpp)

Bloko antraštės struktūra.

```cpp
struct BlokoAntraste {
    std::string prev_block_hash;           // Ankstesnio bloko hash
    std::chrono::system_clock::time_point timestamp;
    uint32_t version;                      // Versija
    std::string merkle_root;               // Merkle Tree šaknis
    uint64_t nonce;                        // Proof-of-Work nonce
    uint32_t difficulty_target;            // Sunkumas (kiek nulių)
};
```
### 5. `MerkleNode` (blokasirgrandine.h/.cpp)

**v0.2: Merkle Tree mazgo struktūra**

```cpp
struct MerkleNode {
    std::string hash;
    std::shared_ptr<MerkleNode> kairys;
    std::shared_ptr<MerkleNode> desine;
    
    MerkleNode(const std::string& h) : hash(h), kairys(nullptr), desine(nullptr) {}
};
```


### 6. `Blokas` (blokasirgrandine.h/.cpp)

Vienas blokų grandinės blokas.

**Pagrindiniai laukai:**
```cpp
- BlokoAntraste antraste
- std::vector<std::shared_ptr<Transakcija>> transakcijos
- std::string bloko_hash
- uint32_t bloko_numeris
- std::shared_ptr<MerkleNode> merkle_tree  // v0.2
```

**Pagrindinės funkcijos:**
- `Blokas(numeris, prev_hash, txs, miner_address, difficulty)` - Konstruktorius
- `sukurtiMerkleTree(hashes)` - **v0.2: Kuria tikrą Merkle Tree**
- `skaiciuotiMerkleRoot()` - **v0.2: Skaičiuoja Merkle Root**
- `skaiciuotiBlokoHash()` - Hash'uoja antraštę
- `atliktiProofOfWork()` - Kasa bloką (randa tinkamą nonce)
- `arValidusHash(hash)` - Tikrina ar hash prasideda nuliais
- `arValid()` - **v0.2: Pilnas bloko validavimas su Merkle Root**

**v0.2 Bloko validacija:**
```cpp
bool Blokas::arValid() const {
    // 1. Hash formato tikrinimas
    if (!arValidusHash(bloko_hash))
        return false;
    
    // 2. Hash perskaičiavimas ir palyginimas
    if (skaiciuotiBlokoHash() != bloko_hash)
        return false;
    
    // 3. Merkle Root tikrinimas
    if (skaiciuotiMerkleRoot() != antraste.merkle_root)
        return false;
    
    // 4. Kiekvienos transakcijos validacija
    for (const auto& tx : transakcijos) {
        if (!tx->arValid())
            return false;
    }
    
    return true;
}
```

![Blokas](<nuotraukos/Screenshot 2025-11-04 225457.png>)

### 7. `Blockchain` (blokasirgrandine.h/.cpp)

Visa blokų grandinė.

**Pagrindinės funkcijos:**
- `Blockchain(difficulty)` - Sukuria genesis bloką
- `pridetiBloka(blokas)` - Prideda naują bloką su validacija
- `arGrandineValidi()` - Patikrina visos grandinės validumą
- `spausdintiGrandine()` - Vizualizuoja visus blokus
- `spausdintiStatistika()` - Rodo statistiką

![Blokas](<nuotraukos/Screenshot 2025-11-04 225819.png>)

![Blokas](<nuotraukos/Screenshot 2025-11-04 225807.png>)

## UTXO modelio realizavimas

### Kaip veikia?

#### 1. Pradinis balansas
```cpp
void Vartotojas::generuotiPradinisUTXO(double balansas) {
    // Padalijame pradinį balansą į 1-5 UTXO
    int kiek_utxo = random(1, 5);
    
    // Kiekvienam UTXO sukuriame atsitiktinį txid ir sumą
    for (int i = 0; i < kiek_utxo; ++i) {
        std::string txid = generuotiAtsitiktiniHash();
        double suma = dalisBalanso();
        utxo_rinkinys.emplace_back(txid, i, suma);
    }
}
```

#### 2. Transakcijos vykdymas (v0.2: su balanso tikrinimu)

```cpp
bool TransakcijuBaseinas::vykdytiTransakcija(
    Transakcija& tx,
    std::vector<std::unique_ptr<Vartotojas>>& vartotojai) {
    
    // 1. Surandame siuntėją ir gavėją
    Vartotojas* sender = suraskVartotoja(tx.gautiSender());
    Vartotojas* receiver = suraskVartotoja(tx.gautiReceiver());
    
    // 2. v0.2: BALANSO TIKRINIMAS
    double amount = tx.gautiAmount();
    double sender_balansas = sender->gautiBalansa();
    
    if (sender_balansas < amount) {
        return false;  // Nepakankamas balansas!
    }
    
    // 3. Renkame UTXO kol surinksime reikiamą sumą
    std::vector<size_t> naudojami_utxo;
    double surinkta_suma = 0.0;
    
    for (size_t i = 0; i < sender->gautiUTXO().size(); ++i) {
        surinkta_suma += utxo_list[i].suma;
        naudojami_utxo.push_back(i);
        if (surinkta_suma >= amount) break;
    }
    
    // 4. Panaudojame (pašaliname) UTXO iš siuntėjo
    for (auto& idx : naudojami_utxo) {
        sender->panaudotiUTXO(utxo.txid, utxo.vout);
    }
    
    // 5. Sukuriame naujus UTXO:
    //    - Gavėjui: pilna suma
    receiver->pridetiUTXO(UTXO(tx.gautiTransactionId(), 0, amount));
    
    //    - Siuntėjui: grąža (jei yra)
    double graza = surinkta_suma - amount;
    if (graza > 0.01) {
        sender->pridetiUTXO(UTXO(tx.gautiTransactionId(), 1, graza));
    }
    
    return true;
}
```

#### 3. Coinbase transakcijos

Kiekviename bloke pirmoji transakcija yra coinbase - kasėjo atlygis:

```cpp
// Coinbase transakcija (bloko atmokėjimas)
double block_reward = 50.0;
auto coinbase = std::make_shared<Transakcija>(
    "COINBASE",           // Nėra siuntėjo
    miner_address,        // Gavėjas - kasėjas
    block_reward,
    timestamp
);
```

## Proof-of-Work mechanizmas

### Maišos funkcija

Modifikuota `hashas()` funkcija (iš 1-os užduoties):

```cpp
void hashas(const string &ivestis, string &isvestis) {
    // 1. Konvertuojame lietuviškus simbolius
    string konvertuotasIvestis = convertLithuanianText(ivestis);
    
    // 2. Skaičiuojame seed iš viso input
    uint32_t seed = 5381; // DJB2 hash
    for (size_t i = 0; i < konvertuotasIvestis.size(); i++) {
        seed = ((seed << 5) + seed) + 
               static_cast<unsigned char>(konvertuotasIvestis[i]);
    }
    
    // 3. Generuojame 64 hex simbolius (256 bitai)
    mt19937 rng(seed);
    uniform_int_distribution<> hex_dist(0, 15);
    
    for (int i = 0; i < 64; i++) {
        int val = hex_dist(rng);
        isvestis += hexToString(val);
    }
}
```

### Kasimo procesas

```cpp
bool Blokas::atliktiProofOfWork() {
    const uint64_t MAX_NONCE = 10000000;
    
    for (uint64_t n = 0; n < MAX_NONCE; ++n) {
        antraste.nonce = n;
        std::string hash = skaiciuotiBlokoHash();
        
        // Tikriname ar hash prasideda nuliais
        if (arValidusHash(hash)) {
            bloko_hash = hash;
            return true;
        }
    }
    
    return false;
}

bool Blokas::arValidusHash(const std::string& hash) const {
    // Tikrina ar pirmi N simboliai yra '0'
    for (size_t i = 0; i < antraste.difficulty_target; ++i) {
        if (hash[i] != '0') return false;
    }
    return true;
}
```

**Hash'uojami elementai:**
1. `prev_block_hash`
2. `timestamp`
3. `version`
4. `merkle_root` (v0.2: tikras Merkle Root)
5. `nonce`
6. `difficulty_target`

## Merkle Tree realizacija (v0.2)

### Kas tai?

Merkle Tree yra dvejetainis medis, kuriame:
- Lapai yra transakcijų hash'ai
- Kiekvienas vidinis mazgas yra dviejų vaikų hash'ų kombinacija
- Šaknis (root) yra vienas hash, reprezentuojantis visas transakcijas

### Realizacija

```cpp
std::shared_ptr<MerkleNode> Blokas::sukurtiMerkleTree(
    const std::vector<std::string>& hashes) const {
    
    if (hashes.empty()) {
        return std::make_shared<MerkleNode>("000...000");
    }
    
    // 1. Sukuriame lapus
    std::vector<std::shared_ptr<MerkleNode>> lygis;
    for (const auto& hash : hashes) {
        lygis.push_back(std::make_shared<MerkleNode>(hash));
    }
    
    // 2. Statome medį iš apačios į viršų
    while (lygis.size() > 1) {
        std::vector<std::shared_ptr<MerkleNode>> naujas_lygis;
        
        for (size_t i = 0; i < lygis.size(); i += 2) {
            auto kairys = lygis[i];
            auto desine = (i + 1 < lygis.size()) ? lygis[i + 1] : lygis[i];
            
            // Sujungiame du hash ir sukuriame naują
            std::string kombinuotas = kairys->hash + desine->hash;
            std::string naujas_hash;
            hashas(kombinuotas, naujas_hash);
            
            auto tevinis = std::make_shared<MerkleNode>(naujas_hash);
            tevinis->kairys = kairys;
            tevinis->desine = desine;
            
            naujas_lygis.push_back(tevinis);
        }
        
        lygis = std::move(naujas_lygis);
    }
    
    return lygis[0];  // Grąžiname šaknį
}
```

### Merkle Root skaičiavimas

```cpp
std::string Blokas::skaiciuotiMerkleRoot() const {
    if (transakcijos.empty()) {
        return "000...000";
    }
    
    // 1. Surenkame visų transakcijų hash'us
    std::vector<std::string> hashes;
    for (const auto& tx : transakcijos) {
        hashes.push_back(tx->gautiTransactionId());
    }
    
    // 2. Sukuriame Merkle Tree
    auto root = sukurtiMerkleTree(hashes);
    
    // 3. Grąžiname šaknies hash
    return root->hash;
}
```

### Vizualizacija

Programa gali atspausdinti Merkle Tree struktūrą:

![Transakcijubaseinas](<nuotraukos/Screenshot 2025-11-04 234337.png>)

## Kandidatinių blokų kasimas (v0.2)

### Koncepcija

Decentralizuotame blockchain:
1. **Keli kasėjai** vienu metu bando kasti tą patį bloką
2. **Pirmas suradęs** tinkamą nonce laimi
3. **Realybėje** kiekvienas kasėjas naudoja kitokius duomenis (pvz., skirtingi coinbase gavėjai)

### Mūsų realizacija

```cpp
bool sukurtiBlokaSuKandidatais(
    Blockchain& blockchain,
    TransakcijuBaseinas& baseinas,
    std::vector<std::unique_ptr<Vartotojas>>& vartotojai,
    int tx_per_block = 100) {
    
    // 1. Paruošti bendrus duomenis
    const Blokas* paskutinis = blockchain.gautiPaskutiniBloka();
    std::string prev_hash = paskutinis->gautiBlokoHash();
    
    // 2. Pasirinkti transakcijas
    std::vector<std::shared_ptr<Transakcija>> pasirinktos;
    // ... (100 transakcijų)
    
    // 3. Sukurti 5 kandidatus su skirtingais kasėjais
    std::vector<KandidatinisBokas> kandidatai(5);
    
    for (int i = 0; i < 5; ++i) {
        // Kiekvienas kandidatas turi skirtingą kasėją
        std::string miner_address = atsitiktinis_kasejas();
        
        // Bandome kasti 5 sekundes
        bool pavyko = false;
        kandidatai[i].blokas = kastiKandidatiniBloka(
            numeris, prev_hash, pasirinktos, 
            miner_address, difficulty,
            5.0,  // 5 sekundės
            pavyko
        );
        
        kandidatai[i].iskastas = pavyko;
    }
    
    // 4. Parinkti geriausią (mažiausias nonce)
    int geriausias_idx = -1;
    uint64_t maziausias_nonce = UINT64_MAX;
    
    for (int i = 0; i < 5; ++i) {
        if (kandidatai[i].iskastas) {
            uint64_t nonce = kandidatai[i].blokas->gautiNonce();
            if (nonce < maziausias_nonce) {
                maziausias_nonce = nonce;
                geriausias_idx = i;
            }
        }
    }
    
    // 5. Pridėti geriausią bloką į grandinę
    if (geriausias_idx != -1) {
        blockchain.pridetiBloka(
            std::move(kandidatai[geriausias_idx].blokas)
        );
        return true;
    }
    
    return false;
}
```

### Konsolės išvestis

```
========================================================
        KANDIDATINIU BLOKU KASIMO PROCESAS (v0.2)      
========================================================
Kandidatu skaicius: 5
Transakciju per bloka: 100
Kasimo laikas vienam: ~5 sekundes
========================================================

--- KANDIDATAS #1 ---
Kasejas: Tomas42
Pradedamas kasimo procesas (Sunkumas: 3, Max Nonce: 10000000)...
✓ Blokas iskastas! Nonce: 123456 (per 2.345 s)
Hash: 00012abc...

--- KANDIDATAS #2 ---
Kasejas: Laura89
Pradedamas kasimo procesas (Sunkumas: 3, Max Nonce: 10000000)...
✓ Blokas iskastas! Nonce: 456789 (per 4.567 s)
Hash: 00034def...

...

========================================================
              KANDIDATU PALYGINIMAS                     
========================================================
Kandidatas #1: Nonce = 123456  ← GERIAUSIAS
```

## Dirbtinio intelekto pagalba


- OOP gerųjų praktikų patarimai
- Pagalba su UTXO realizavimu
- Išvedimo į konsolę formavimas
- Pagalba su Merkle tree įgyvendinimu
- Pagalba su kodo klaidom
- Kodo peržiūra ir patarimai
- README.md generavimas