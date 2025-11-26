#ifndef HASHAS_H      
#define HASHAS_H

#include "lib.h" 

map<wchar_t, uint16_t> getLithuanianCharMap();
string convertLithuanianText(const string &input);

void hashas(const string &ivestis, string &isvestis); 
uint32_t safeStringToUint32(const string& str, const string& seedui);


#endif // HASHAS_H