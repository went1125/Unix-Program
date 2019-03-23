CXX = g++
CFLAGS = -Wall
OBJ = hw1_lib.o hw1_main.o
DEPS = hw1_lib.h

all: hw1

%.o : %.cpp $(DEPS)
	$(CXX) $(CFLAGS) -c -o $@ $<

hw1: $(OBJ)
	g++ $(CFLAGS) -o $@ $^

clean:
	@rm -rf *.o
