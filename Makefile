# PortSim Makefile (MinGW / MSYS2 recommended on Windows)

CXX      = g++
CXXFLAGS = -std=c++17 -O2 -Wall
LDFLAGS  = -lfreeglut -lopengl32 -lglu32 -lwinmm

TARGET   = PortSim.exe
SRC      = src/main.cpp

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(TARGET) *.o

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run
