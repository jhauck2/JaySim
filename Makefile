deps := src/main.cpp src/ball.hpp src/button.hpp src/jayShader.hpp src/shotParser.hpp src/TCPSocket.hpp build/ball.o src/dynamics.hpp build/dynamics.o build/button.o build/shotParser.o build/TCPSocket.o
OUTS := build/ball.o build/dynamics.o build/button.o build/shotParser.o build/TCPSocket.o
WARN := -Wall -Wzero-as-null-pointer-constant

PLATFORM             ?= PLATFORM_LINUX

ifeq ($(PLATFORM), PLATFORM_LINUX)
INC := -I ./src
LDFLAGS := -L /usr/local/lib64
LDLIBS := -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
endif

ifeq ($(PLATFORM), PLATFORM_WINDOWS)
INC := -I ./src -I C:/raylib-master/src
LDFLAGS := -L C:/raylib-master/src 
LDLIBS := -lraylib -lgdi32 -lwinmm -lws2_32
endif

build/jSim.exe: $(deps)
	g++ -D$(PLATFORM) $(WARN) -g $(INC) $(LDFLAGS) src/main.cpp -o build/jSim.exe $(OUTS) $(LDLIBS)

build/ball.o: src/ball.hpp src/ball.cpp
	g++ -D$(PLATFORM) $(WARN) -g -c src/ball.cpp $(INC) -o build/ball.o

build/dynamics.o: src/dynamics.hpp src/dynamics.cpp src/ball.hpp
	g++ -D$(PLATFORM) $(WARN) -g  -c src/dynamics.cpp $(INC) -o build/dynamics.o

build/button.o: src/button.hpp src/button.cpp
	g++ -D$(PLATFORM) $(WARN) -g  -c src/button.cpp $(INC) -o build/button.o

build/shotParser.o: src/shotParser.hpp src/shotParser.cpp
	g++ -D$(PLATFORM) $(WARN) -g -c src/shotParser.cpp $(INC) -o build/shotParser.o
	
build/TCPSocket.o: src/TCPSocket.hpp src/TCPSocket.cpp build/shotParser.o
	g++ -D$(PLATFORM) $(WARN) -g -c src/TCPSocket.cpp $(INC) -o build/TCPSocket.o


run: build/jSim
	./build/jSim

clean:
	rm -I build/*
