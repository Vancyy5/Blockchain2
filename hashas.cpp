#include "hashas.h"

map<wchar_t, uint16_t> getLithuanianCharMap() 
{
    map<wchar_t, uint16_t> charMap;

   
    charMap[L'ą'] = 0xC485;  
    charMap[L'č'] = 0xC48D;  
    charMap[L'ę'] = 0xC499;  
    charMap[L'ė'] = 0xC497;  
    charMap[L'į'] = 0xC4AF;  
    charMap[L'š'] = 0xC5A1;  
    charMap[L'ų'] = 0xC5B3;  
    charMap[L'ū'] = 0xC5AB;  
    charMap[L'ž'] = 0xC5BE;  

    
    charMap[L'Ą'] = 0xC484;  
    charMap[L'Č'] = 0xC48C;  
    charMap[L'Ę'] = 0xC498;  
    charMap[L'Ė'] = 0xC496;  
    charMap[L'Į'] = 0xC4AE;  
    charMap[L'Š'] = 0xC5A0;  
    charMap[L'Ų'] = 0xC5B2;  
    charMap[L'Ū'] = 0xC5AA;  
    charMap[L'Ž'] = 0xC5BD;  

    return charMap;
}

string convertLithuanianText(const string &input) {
    static auto charMap = getLithuanianCharMap();
    wstring_convert<codecvt_utf8<wchar_t>> conv;
    wstring wide = conv.from_bytes(input);

    string result;

    for (wchar_t wc : wide) {
        auto it = charMap.find(wc);
        if (it != charMap.end()) {
            uint16_t code = it->second;
            result.push_back(static_cast<char>((code >> 8) & 0xFF));
            result.push_back(static_cast<char>(code & 0xFF));
        } else {
            string utf8char = conv.to_bytes(wc);
            result += utf8char;
        }
    }

    return result;
}


void hashas(const string &ivestis, string &isvestis) 
{
    isvestis.clear();

    string konvertuotasIvestis = convertLithuanianText(ivestis);

    string seedString;
    string seedui;
    if (!konvertuotasIvestis.empty()) {
        // kas 1000 simbolių ASCII suma
        for (size_t i = 0; i < konvertuotasIvestis.size(); i += 1000) {
            int suma = 0;
            for (size_t j = i; j < i + 10 && j < konvertuotasIvestis.size(); j++) {
                suma += static_cast<unsigned char>(konvertuotasIvestis[j]);
            }
            seedString += to_string(suma);
        }

        // kas 20 simbolių '1' bitų kiekis
        for (size_t i = 0; i < konvertuotasIvestis.size(); i += 20) {
            int ones = 0;
            for (size_t j = i; j < i + 20 && j < konvertuotasIvestis.size(); j++) {
                bitset<8> bits(static_cast<unsigned char>(konvertuotasIvestis[j]));
                ones += bits.count();
            }
            seedString += to_string(ones);
            seedui+= to_string(ones);
        }
    } else {
        seedString = "0";
    }

    // Įvesties binary kodas
    string binaryInput;
    for (unsigned char c : konvertuotasIvestis) {
        binaryInput += bitset<8>(c).to_string();
    }

    // jeigu tuščias, sukuriame bent 1 baitą (10000000)
    if (binaryInput.empty()) {
        binaryInput = "10000000";
    }

    // prailginam iki 256 bitų
    string originalBinary = binaryInput;
    while (binaryInput.size() < 256) {
        string toAdd = originalBinary;
        // XOR su jau esančiais bitais
        for (size_t i = 0; i < toAdd.size() && binaryInput.size() < 256; i++) {
            char newBit = (binaryInput[i % binaryInput.size()] == toAdd[i]) ? '0' : '1';
            binaryInput += newBit;
        }
    }
    if (binaryInput.size() > 256) {
        binaryInput = binaryInput.substr(0, 256);
    }

    uint32_t mySeed = safeStringToUint32(seedString, seedui);
    mt19937 rng(mySeed);

    // Maišymas
    string mixedBinary;
    for (size_t i = 0; i < binaryInput.size(); i += 32) {
        uint32_t randVal = rng();
        for (size_t j = 0; j < 32 && i + j < binaryInput.size(); j++) {
            int bit = binaryInput[i + j] - '0';
            int rbit = (randVal >> j) & 1;
            bit ^= rbit;
            mixedBinary.push_back(bit ? '1' : '0');
        }
    }

    // konversija į HEX
    for (size_t i = 0; i + 4 <= mixedBinary.size(); i += 4) {
        string nibble = mixedBinary.substr(i, 4);
        int value = stoi(nibble, nullptr, 2);
        stringstream ss;
        ss << hex << value;
        isvestis += ss.str();
    }
}
uint32_t safeStringToUint32(const string& str,const string& seedui) 
{
    string truncated = str;
    if (truncated.length() > 9) {

        truncated = truncated.substr(0, 9);
    }
    
     uint32_t seed = 0;
    for (unsigned char c : seedui) 
    {
        seed = seed * 31 + c; 
    }
    uint32_t hash = seed;

    
    for (unsigned char c : truncated) 
    {
        hash = hash * seed + c; 
    }

    return hash;
    
}