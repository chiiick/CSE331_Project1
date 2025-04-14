# Makefile for Sorting Algorithm Evaluation
CXX = g++
CXXFLAGS = -std=c++17 -O2 -Wall

SRC = main.cpp
HEADERS = \
	input_generator.h \
	test.h\
	advanced_sorts.h\
	basic_sorts.h 

TARGET = AlgorithmEvaluation

all: $(TARGET)

$(TARGET): $(SRC) $(HEADERS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)
