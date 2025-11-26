CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra -I/usr/local/include
LDFLAGS = -L/usr/local/lib
LIBS = -lbitcoin-system -lboost_system -lboost_thread -lpthread -lsecp256k1 -lssl -lcrypto

SOURCES = main.cpp vartotojas.cpp transakcija.cpp blokasirgrandine.cpp hashas.cpp
OBJECTS = $(SOURCES:.cpp=.o)
TARGET = blockchain

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(LDFLAGS) -o $@ $^ $(LIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)

rebuild: clean all

.PHONY: all clean rebuild
