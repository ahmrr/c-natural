#Vars
CC := g++
flags := -std=c++11 -Wall -Wextra

parser := cnc
includes := util/* lex/*
tests := tests/set tests/operators tests/print tests/switch tests/struct tests/array tests/function

# Default make the parse
all: $(parser)

# Make the parser
$(parser): $(parser).cpp
	$(CC) $< -o $(parser) $(includes) $(flags)

# Make the tests
$(tests): $(parser)
	./$(parser) $@.cn $@.cpp
	$(CC) $@.cpp -o $@
	./$@

# Run all tests
test: clean $(tests)

# Clean all exe's 
clean:
	rm -rf $(parser)
	rm -rf $(tests)
	rm -rf tests/*.cpp

# Ahmer's beautiful command
each: clean all test