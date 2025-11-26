#ifndef LIB_H      
#define LIB_H

// Windows specific (tik jei reikia) - PIRMIAU NEI kiti headeriai
#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif

// Standartine I/O
#include <iostream>
#include <fstream>
#include <sstream>

// Kontaineriai
#include <string>
#include <vector>
#include <map>
#include <set>

// Atmintis
#include <memory>

// Algoritmai
#include <algorithm>
#include <utility>
#include <functional>

// Matematika
#include <random>
#include <cmath>
#include <cstdint>

// Laikas
#include <chrono>
#include <ctime>

// Formatavimas
#include <iomanip>
#include <bitset>

// Lokalizacija
#include <locale>
#include <codecvt>

using namespace std;

#endif // LIB_H