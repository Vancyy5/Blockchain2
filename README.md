# 3-oji (papildoma) užduotis: Bitcoin transakcijų ir blokų analizė su Libbitcoin ir python-bitcoinlib

## 1 užduotis: Merkle medžio implementacija su Libbitcoin

### Aplinka ir problemos
Mano operacinė sistema - Windows.

Iškilo problemų su Windows kompiliavimu:
- Senesnės v3.5.x versijos galėjo būti kompiliuojamos su MinGW, bet reikėjo rankinio konfigūravimo
- Nauja libbitcoin v3.8 nebuvo skirta tiesioginiam Windows kompiliavimui
- Kilo kompiliavimo problemos su Boost biblioteka
- Senos Boost versijos ir libbitcoin v3.5.0 parašytos C++11, bet naujesni Boost (1.81–1.82) reikalauja C++14

Išbandžiau Boost 1.82, 1.81, version3, bet nepavyko.

### Sprendimas
Galiausiai pasirinkau version4 su C++20 standartu ir perašiau kodą naudojant naują API:

Pagrindiniai pakeitimai:
- `bc::hash_digest` → `bc::system::hash_digest`
- `bc::hash_list` → `bc::system::hashes`
- `bc::null_hash` → `bc::system::null_hash`
- `bc::data_chunk` → `bc::system::data_chunk`
- `bc::bitcoin_hash()` → `bc::system::bitcoin_hash()`
- `bc::encode_base16()` → `bc::system::encode_base16()`

Kompiliavimo komanda:
```bash
g++ -std=c++20 -I/usr/local/include -L/usr/local/lib -o merkle merkle.cpp -lbitcoin-system -lboost_system -lboost_thread -lpthread -lsecp256k1 -lssl -lcrypto
```

![nuotrauka](<nuotraukos/Screenshot 2025-11-26 230536.png>)

### Testavimas su tikrais duomenimis
`merklechange.cpp` - parašytas kodas naudoti transakcijų hash'us iš bloko 170:

![nuotrauka](<nuotraukos/Screenshot 2025-11-26 233558.png>)

![nuotrauka](<nuotraukos/Screenshot 2025-11-26 231358.png>)

Šaltinis: https://www.blockchain.com/explorer/blocks/btc/170

### Integracija į blockchain projektą
Viena iš vietų, kur integravau `create_merkle()` funkciją:

```cpp
// LIBBITCOIN create_merkle funkcija
bc::system::hash_digest create_merkle(bc::system::hashes& merkle)
{
    if (merkle.empty())
        return bc::system::null_hash;
    else if (merkle.size() == 1)
        return merkle[0];
    
    while (merkle.size() > 1)
    {
        if (merkle.size() % 2 != 0)
            merkle.push_back(merkle.back());
        
        assert(merkle.size() % 2 == 0);
        
        bc::system::hashes new_merkle;
        
        for (auto it = merkle.begin(); it != merkle.end(); it += 2)
        {
            bc::system::data_chunk concat_data(bc::system::hash_size * 2);
            
            std::copy(it->begin(), it->end(), concat_data.begin());
            std::copy((it + 1)->begin(), (it + 1)->end(), concat_data.begin() + bc::system::hash_size);
            
            // ← BITCOIN DOUBLE SHA-256
            auto temp_hash = bc::system::sha256::hash(concat_data);
            bc::system::hash_digest new_root = bc::system::sha256::hash(temp_hash);
            
            new_merkle.push_back(new_root);
        }
        
        merkle = new_merkle;
    }
    
    return merkle[0];
}
```

![nuotrauka](<nuotraukos/Screenshot 2025-11-26 232842.png>)

### Kodo analizė (merkle.cpp)

Pagrindinė funkcija `create_merkle()` implementuoja Merkle medžio šaknies skaičiavimą:

1. **Baziniai atvejai:**
   - Jei hash'ų sąrašas tuščias → grąžina `null_hash`
   - Jei vienas elementas → grąžina tą hash'ą

2. **Pagrindinė logika:**
   - Jei hash'ų skaičius nelyginis → dubliuojamas paskutinis hash'as
   - Hash'ai jungiami po du (konkatenacija)
   - Kiekvienai porai atliekamas dvigubas SHA-256 (`bitcoin_hash`)
   - Procesas kartojamas, kol lieka vienas hash'as (šaknis)

3. **Bitcoin specifika:**
   - Naudojamas dvigubas SHA-256: `SHA256(SHA256(data))`
   - Hash'ai konkatenuojami po 32 baitus (hash_size)
   - Nelyginiam skaičiui - paskutinis hash dubliuojamas

**main() funkcijoje:**
- Naudojami tikri transakcijų hash'ai iš bloko #100,000
- Tikrinama, ar apskaičiuota šaknis sutampa su tikrąja

---

## 2 užduotis: Pilno Bitcoin mazgo (Bitcoin Core) įdiegimas

Kadangi neturėjau pakankamai vietos savo kompiuteryje, siunčiausi 140GB Bitcoin Core mazgą.

### Įdiegimo žingsniai (Windows):

1. Atsisiųsti iš [GitHub](https://github.com/bitcoin/bitcoin) naujausiame release'e siūlomą versiją `bitcoin-28.3-win64.zip`
2. Atidaryti zip failą
3. Paleisti `bitcoin-qt.exe` (pvz., `C:\bitcoin-30.0\bin\bitcoin-qt.exe`)

### Naudojimas per Command Prompt:

Tie, kurie žino mano prieigos informaciją, gali prisijungti:
```bash
bitcoin-cli -rpcconnect=... -rpcuser=... -rpcpassword=... getblockchaininfo
```

Pavyzdys:
```bash
C:\bitcoin-30.0\bin>.\bitcoin-qt.exe
```

### Tinklo informacija

```bash
C:\bitcoin-30.0\bin>.\bitcoin-cli.exe getnetworkinfo
{
  "version": 300000,
  "subversion": "/Satoshi:30.0.0/",
  "protocolversion": 70016,
  "localservices": "0000000000000c08",
  "localservicesnames": [
    "WITNESS",
    "NETWORK_LIMITED",
    "P2P_V2"
  ],
  "localrelay": true,
  "timeoffset": 0,
  "networkactive": true,
  "connections": 3,
  "connections_in": 0,
  "connections_out": 3,
  "networks": [
    {
      "name": "ipv4",
      "limited": false,
      "reachable": true,
      "proxy": "",
      "proxy_randomize_credentials": false
    },
    {
      "name": "ipv6",
      "limited": false,
      "reachable": true,
      "proxy": "",
      "proxy_randomize_credentials": false
    },
    {
      "name": "onion",
      "limited": true,
      "reachable": false,
      "proxy": "",
      "proxy_randomize_credentials": false
    },
    {
      "name": "i2p",
      "limited": true,
      "reachable": false,
      "proxy": "",
      "proxy_randomize_credentials": false
    },
    {
      "name": "cjdns",
      "limited": true,
      "reachable": false,
      "proxy": "",
      "proxy_randomize_credentials": false
    }
  ],
  "relayfee": 0.00000100,
  "incrementalfee": 0.00000100,
  "localaddresses": [
    {
      "address": "2a00:1eb8:c0ef:a39d:9be:6261:cfaf:5aab",
      "port": 8333,
      "score": 1
    },
    {
      "address": "2a00:1eb8:c0ef:a39d:f99c:c4bc:f7e8:492c",
      "port": 8333,
      "score": 1
    }
  ],
  "warnings": []
}
```

Transakcijų sąrašas:
```bash
C:\bitcoin-30.0\bin>.\bitcoin-cli.exe listtransactions
[]
```

---

## 3 užduotis: Bitcoin tinklo analizė su python-bitcoinlib

Naudojau VU Bitcoin mazgą.

### Kaip python-bitcoinlib bendrauja su Bitcoin mazgu?

`python-bitcoinlib` biblioteka naudoja **RPC (Remote Procedure Call)** protokolą bendrauti su Bitcoin Core mazgu.

**Veikimo principas:**

1. **Ryšio užmezgimas:** `RawProxy()` sukuria ryšį su lokaliu Bitcoin Core mazgu
2. **Konfigūracija:** `bitcoin.conf` faile reikia nustatyti:
   - `rpcuser` ir `rpcpassword` (autentifikacijai)
   - `rpcport` (paprastai 8332 mainnet)
   - `server=1` (įjungti RPC serverį)

3. **RPC metodai** per `RawProxy()`:
   - `getblockchaininfo()` - bendra blockchain informacija
   - `getrawtransaction(txid)` - transakcijos duomenys
   - `decoderawtransaction(raw_tx)` - dekoduoja hex → JSON
   - `getblock(blockhash)` - bloko informacija
   - `getblockhash(height)` - bloko hash pagal aukštį

**Failai:**
- `python-bitcoinlib/putty.txt` - visas pokalbis išsaugotas
- `python-bitcoinlib/` - visi naudoti failai

### Programa 1: Bitcoin transakcijos mokesčio skaičiavimas

**Failas:** `btc_fee_calculator.py`

Testuota su 2019-09-06 viena vertingiausių transakcijų.

**Output:**
```
user35@aleksandr-OptiPlex-790:~/bitcoin_task3$ python3 btc_fee_calculator.py
Bitcoin transakcijos mokesčio analizė
==================================================
Gaunama transakcija: 4410c8d14ff9f87ceeed1d65cb58e7c7b2422b2d7529afc675208ce2ce09ed7d

=== OUTPUTS ===
  Output 0: 94504.03465148 BTC

Bendra output'ų suma: 94504.03465148 BTC

=== INPUTS ===
  Input iš 8cace7bd82a5e01a...:127 = 0.00000666 BTC
  Input iš ae760f937e8e8462...:0 = 0.00000666 BTC
  [... 91 input'ų daugiau ...]
  Input iš 4b0cea363df8214e...:0 = 0.00010197 BTC

Bendra input'ų suma: 94504.10000000 BTC

==================================================
TRANSAKCIJOS MOKESTIS: 0.06534852 BTC
Transakcijos dydis: 13611 vbytes
Mokestis už bytą: 480.12 satoshi/vbyte
==================================================

Mokestis USD (2019-09-06 kursu): $686.16
Mokestis satoshi: 6,534,852 sat
```

### Programa 2: Bloko hash'o validatorius

**Failas:** `btc_block_validator.py`

**Output:**
```
user35@aleksandr-OptiPlex-790:~/bitcoin_task3$ python3 btc_block_validator.py
Bitcoin bloko hash'o validatorius
======================================================================

======================================================================
Testuojamas blokas: 0
======================================================================
Bloko #0 hash: 000000000019d6689c085ae165831e934ff763ae46a2a6c172b3f1b60a8ce26f

======================================================================
BLOKO HEADER INFORMACIJA:
======================================================================
Versija:           1 (0x00000001)
Ankstesnis blokas: 0000000000000000000000000000000000000000000000000000000000000000
Merkle Root:       4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b
Laikas:            1231006505 (1231006505)
Bits (Difficulty): 1d00ffff
Nonce:             2083236893

Bloko hash (tikrasis): 000000000019d6689c085ae165831e934ff763ae46a2a6c172b3f1b60a8ce26f

Suformuotas header (hex): 0100000000000000000000000000000000000000000000000000000000000000000000003ba3edfd7a7b12b27ac72c3e67768f617fc81bc3888a51323a9fb8aa4b1e5e4a29ab5f49ffff001d1dac2b7c
Header'io ilgis: 80 baitai

======================================================================
VALIDACIJOS REZULTATAS:
======================================================================
Apskaičiuotas hash: 000000000019d6689c085ae165831e934ff763ae46a2a6c172b3f1b60a8ce26f
Tikrasis hash:      000000000019d6689c085ae165831e934ff763ae46a2a6c172b3f1b60a8ce26f

✓ HASH'AS TEISINGAS! Blokas validus.

======================================================================
PAPILDOMA INFORMACIJA:
======================================================================
Leading zeros: 10
Hash (decimal): 10628944869218562084050143519444549580389464591454674019345556079
Difficulty: 1
Transakcijų skaičius: 1
```

---

## Išvados

Šioje užduotyje sėkmingai:
- Implementavau Merkle medžio šaknies skaičiavimą naudojant Libbitcoin biblioteką
- Įdiegiau ir sukonfigūravau Bitcoin Core mazgą Windows aplinkoje
- Patikrinau transakcijos mokesčio skaičiavimo tikslumą
- Validavau bloko hash'o teisingumą pagal header informaciją