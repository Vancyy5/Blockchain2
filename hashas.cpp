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

    // Skaiciuojame seed is VISO input stringo
    uint32_t seed = 5381; // DJB2 hash pradine reiksme
    
    for (size_t i = 0; i < konvertuotasIvestis.size(); i++) {
        seed = ((seed << 5) + seed) + static_cast<unsigned char>(konvertuotasIvestis[i]);
    }

    mt19937 rng(seed);

    // Generuojame 64 hex simbolius (256 bitai)
    uniform_int_distribution<> hex_dist(0, 15);
    
    for (int i = 0; i < 64; i++) {
        int val = hex_dist(rng);
        stringstream ss;
        ss << hex << val;
        isvestis += ss.str();
    }
}

uint32_t safeStringToUint32(const string& str, const string& seedui) 
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