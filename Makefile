build/jSim: src/main.cpp src/ball.hpp src/button.hpp src/jayShader.hpp build/ball.o src/dynamics.hpp build/dynamics.o build/button.o
	g++ -Wall -g -I ./src src/main.cpp -L /usr/local/lib64 -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -o build/jSim build/ball.o build/dynamics.o build/button.o

build/ball.o: src/ball.hpp src/ball.cpp
	g++ -Wall -g -L /usr/local/lib64 -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -c src/ball.cpp -I ./src -o build/ball.o

build/dynamics.o: src/dynamics.hpp src/dynamics.cpp src/ball.hpp
	g++ -Wall -g -L /usr/local/lib64 -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -c src/dynamics.cpp -I ./src -o build/dynamics.o

build/button.o: src/button.hpp src/button.cpp
	g++ -Wall -g -L /usr/local/lib64 -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -c src/button.cpp -I ./src -o build/button.o
	

run: build/jSim
	./build/jSim

clean:
	rm -I build/*
