# Kompiliatorius ir parametrai
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2

# Objektiniai failai
OBJS = main.o vartotojas.o transakcija.o blokas.o hashas.o

# Vykdomasis failas
TARGET = blockchain

# Pagrindinis taikinys
all: $(TARGET)

# Kompiliavimas
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

# Objektinių failų kompiliavimas
main.o: main.cpp vartotojas.h transakcija.h blokas.h lib.h
	$(CXX) $(CXXFLAGS) -c main.cpp

vartotojas.o: vartotojas.cpp vartotojas.h lib.h
	$(CXX) $(CXXFLAGS) -c vartotojas.cpp

transakcija.o: transakcija.cpp transakcija.h vartotojas.h hashas.h lib.h
	$(CXX) $(CXXFLAGS) -c transakcija.cpp

blokas.o: blokas.cpp blokas.h transakcija.h hashas.h lib.h
	$(CXX) $(CXXFLAGS) -c blokas.cpp

hashas.o: hashas.cpp hashas.h lib.h
	$(CXX) $(CXXFLAGS) -c hashas.cpp

# Valymas
clean:
	rm -f $(OBJS) $(TARGET)

# Paleidimas
run: $(TARGET)
	./$(TARGET)

# Rebuild
rebuild: clean all

.PHONY: all clean run rebuild