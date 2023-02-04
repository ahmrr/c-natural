CC := g++
flags := -std=c++11 -Wall -Wextra

parser := cnc
includes := util/* lex/*

all: $(parser)

$(parser): $(parser).cpp
	$(CC) $< $(includes) $(flags) -o $(parser)

test: tests/test.cn $(parser)
	./$(parser) $< tests/$@.cpp -o tests/$@
	$(CC) tests/$@.cpp -o tests/$@
	./tests/$@

test2: tests/test2.cn $(parser)
	./$(parser) $< tests/$@.cpp -o tests/$@
	$(CC) tests/$@.cpp -o tests/$@
	./tests/$@

helpTest1: tests/test.cn $(parser)
	gdb ./$(parser) $< tests/$@.cpp -o tests/$@

clean:
	rm -rf $(parser)

everything: clean all test