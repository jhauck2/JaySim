deps := src/main.cpp src/ball.hpp src/button.hpp src/jayShader.hpp src/shotParser.hpp build/ball.o src/dynamics.hpp build/dynamics.o build/button.o build/shotParser.o
INC := -I ./src
LINK := -L /usr/local/lib64 -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
OUTS := build/ball.o build/dynamics.o build/button.o build/shotParser.o

build/jSim: $(deps)
	g++ -Wall -g $(INC) src/main.cpp $(LINK) -o build/jSim $(OUTS)

build/ball.o: src/ball.hpp src/ball.cpp
	g++ -Wall -g $(LINK) -c src/ball.cpp $(INC) -o build/ball.o

build/dynamics.o: src/dynamics.hpp src/dynamics.cpp src/ball.hpp
	g++ -Wall -g $(LINK) -c src/dynamics.cpp $(INC) -o build/dynamics.o

build/button.o: src/button.hpp src/button.cpp
	g++ -Wall -g $(LINK) -c src/button.cpp $(INC) -o build/button.o

build/shotParser.o: src/shotParser.hpp src/shotParser.cpp
	g++ -Wall -g $(LINK) -c src/shotParser.cpp $(INC) -o build/shotParser.o
	

run: build/jSim
	./build/jSim

clean:
	rm -I build/*
