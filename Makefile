CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Werror -pedantic -Iinclude
LDFLAGS = -pthread

TARGET = main.out

SRC = main.cpp src/Scan.cpp src/Output.cpp
OBJ = $(SRC:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(OBJ) $(LDFLAGS) -o $(TARGET)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

rebuild: clean all