build/jSim: src/main.cpp build/ball.o
	g++ -Wall -I ./src src/main.cpp -L /usr/local/lib64 -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -o build/jSim build/ball.o

build/ball.o: src/ball.hpp src/ball.cpp
	g++ -Wall -L /usr/local/lib64 -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -c src/ball.cpp -I ./src -o build/ball.o

run: build/jSim
	./build/jSim

clean:
	rm -I build/*
