#Vars
CC := g++
flags := -std=c++11 -Wall -Wextra

parser := cnc
includes := util/* lex/*
# tests/functions tests/libfun
# tests := tests/set tests/operators tests/print tests/switch tests/comment
tests := tests/fundamentals

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
test: $(tests)

# Clean all exe's 
clean:
	rm -rf $(parser)
	rm -rf $(tests)
	rm -rf tests/*.cpp

# Ahmer's beautiful command
each: clean all test