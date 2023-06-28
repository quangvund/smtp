CXX := g++
CXX_FLAGS := -std=c++11 -g

BIN := bin
SRC := 
INCLUDE := 

LIBRARIES :=
EXECUTABLE := main


all: $(BIN)/$(EXECUTABLE)

run: clean all
	clear
	./$(BIN)/$(EXECUTABLE)

$(BIN)/$(EXECUTABLE): $(SRC)/*.cpp
	$(CXX) $(CXX_FLAGS) -I$(INCLUDE) $^ -o $@ $(LIBRARIES)

clean:
	-rm $(BIN)/*