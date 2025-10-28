# Supaprastinta Blokų Grandinė #v0.1

## Turinys
- [Apie projektą](#apie-projektą)
- [Naudojimosi instrukcija](#naudojimosi-instrukcija)
- [Pagrindinės klasės](#pagrindinės-klasės)
- [UTXO modelio realizavimas](#utxo-modelio-realizavimas)
- [Proof-of-Work mechanizmas](#proof-of-work-mechanizmas)

## Apie projektą

Šis projektas realizuoja supaprastintą blokų grandinę (blockchain), imituojančią jos veikimą realiomis sąlygomis. 

1. Centralizuota blokų grandinė  
2. Modifikuota maišos funkcija (generuoja hash su 000... pradžia)  
3. Vartotojų generavimas (~1000)  
4. Transakcijų generavimas (~10000)  
5. Proof-of-Work kasimo procesas  
6. UTXO modelio realizavimas  
7. Detalus konsolės išvedimas su vizualizacijomis  
8. OOP principų taikymas (enkapsuliacija, RAII, konstruktoriai)

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

![Meniu](<nuotraukos/Screenshot 2025-10-28 115813.png>)

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

![Vartotojas](<nuotraukos/Screenshot 2025-10-28 115840.png>)

![Vartotojas](<nuotraukos/Screenshot 2025-10-28 115853.png>)

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
- `arValid()` - Patikrina transakcijos validumą
- `generuotiHash()` - Generuoja unikalų transakcijos ID

![Transakcija](<nuotraukos/Screenshot 2025-10-28 115906.png>)

![Transakcija](<nuotraukos/Screenshot 2025-10-28 115921.png>)

### 3. `TransakcijuBaseinas` (transakcija.h/.cpp)

Valdo laukiančių transakcijų sąrašą (mempool).

**Pagrindinės funkcijos:**
- `generuotiTransakcijas(vartotojai, kiekis)` - Generuoja transakcijas
- `vykdytiTransakcija(tx, vartotojai)` - Vykdo transakciją (UTXO atnaujinimas)
- `pasalintiTransakcijas(txs)` - Pašalina patvirtintas transakcijas
- `spausdintiStatistika()` - Rodo baseino statistiką

![Transakcijubaseinas](<nuotraukos/Screenshot 2025-10-28 120132.png>)

### 4. `BlokoAntraste` (blokasirgrandine.h/.cpp)

Bloko antraštės struktūra.

```cpp
struct BlokoAntraste {
    std::string prev_block_hash;           // Ankstesnio bloko hash
    std::chrono::system_clock::time_point timestamp;
    uint32_t version;                      // Versija
    std::string merkle_root;               // Visų TX hash (supaprastinta)
    uint64_t nonce;                        // Proof-of-Work nonce
    uint32_t difficulty_target;            // Sunkumas (kiek nulių)
};
```

### 5. `Blokas` (blokasirgrandine.h/.cpp)

Vienas blokų grandinės blokas.

**Pagrindiniai laukai:**
```cpp
- BlokoAntraste antraste
- std::vector<std::shared_ptr<Transakcija>> transakcijos
- std::string bloko_hash
- uint32_t bloko_numeris
```

**Pagrindinės funkcijos:**
- `Blokas(numeris, prev_hash, txs, miner_address, difficulty)` - Konstruktorius
- `skaiciuotiMerkleRoot()` - Skaičiuoja hash iš visų transakcijų
- `skaiciuotiBlokoHash()` - Hash'uoja antraštę
- `atliktiProofOfWork()` - Kasa bloką (randa tinkamą nonce)
- `arValidusHash(hash)` - Tikrina ar hash prasideda nuliais
- `arValid()` - Pilnas bloko validavimas

![Blokas](<nuotraukos/Screenshot 2025-10-28 120010.png>)

### 6. `Blockchain` (blokasirgrandine.h/.cpp)

Visa blokų grandinė.

**Pagrindinės funkcijos:**
- `Blockchain(difficulty)` - Sukuria genesis bloką
- `pridetiBloka(blokas)` - Prideda naują bloką su validacija
- `arGrandineValidi()` - Patikrina visos grandinės validumą
- `spausdintiGrandine()` - Vizualizuoja visus blokus
- `spausdintiStatistika()` - Rodo statistiką

![Blokas](<nuotraukos/Screenshot 2025-10-28 120027.png>)

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

#### 2. Transakcijos vykdymas

```cpp
bool TransakcijuBaseinas::vykdytiTransakcija(
    Transakcija& tx,
    std::vector<std::unique_ptr<Vartotojas>>& vartotojai) {
    
    // 1. Surandame siuntėją ir gavėją
    Vartotojas* sender = suraskVartotoja(tx.gautiSender());
    Vartotojas* receiver = suraskVartotoja(tx.gautiReceiver());
    
    // 2. Renkame UTXO kol surinksime reikiamą sumą
    double amount = tx.gautiAmount();
    std::vector<size_t> naudojami_utxo;
    double surinkta_suma = 0.0;
    
    for (size_t i = 0; i < sender->gautiUTXO().size(); ++i) {
        surinkta_suma += utxo_list[i].suma;
        naudojami_utxo.push_back(i);
        if (surinkta_suma >= amount) break;
    }
    
    // 3. Panaudojame (pašaliname) UTXO iš siuntėjo
    for (auto& idx : naudojami_utxo) {
        sender->panaudotiUTXO(utxo.txid, utxo.vout);
    }
    
    // 4. Sukuriame naujus UTXO:
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

Kiekviename bloke pirmoji transakcija yra  - kasėjo atlygis:

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
4. `merkle_root`
5. `nonce`
6. `difficulty_target`




