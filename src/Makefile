CC := g++
flags := -std=c++11 -Wall -Wextra

parser := cnc
includes := util/* lex/*

all: build

build: $(parser).cpp
	$(CC) $< $(includes) $(flags) -o $(parser)

test: test.cn $(parser)
	./$(parser) $< $@.cpp -o $@
	$(CC) $@.cpp -o $@
	./$@

helpTest1: test.cn $(parser)
	gdb ./$(parser) $< $@.cpp -o $@

clean:
	rm -rf $(parser)