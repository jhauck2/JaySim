deps := src/main.cpp src/ball.hpp src/button.hpp src/jayShader.hpp src/shotParser.hpp src/TCPSocket.hpp build/ball.o src/dynamics.hpp build/dynamics.o build/button.o build/shotParser.o build/TCPSocket.o
INC := -I ./src
INC_W := -I ./src -I C:/raylib-master/src
LINK := -L /usr/local/lib64 -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
LDFLAGS_W := -L C:/raylib-master/src 
LDLIBS_W := -lraylib -lgdi32 -lwinmm -lws2_32
OUTS := build/ball.o build/dynamics.o build/button.o build/shotParser.o build/TCPSocket.o
WARN := -Wall -Wzero-as-null-pointer-constant

PLATFORM             ?= PLATFORM_LINUX

ifeq ($(PLATFORM), PLATFORM_LINUX)
build/jSim: $(deps)
	g++ -DPLATFORM_LINUX $(WARN) -g $(INC) src/main.cpp $(LINK) -o build/jSim $(OUTS)

build/ball.o: src/ball.hpp src/ball.cpp
	g++ $(WARN) -g $(LINK) -c src/ball.cpp $(INC) -o build/ball.o

build/dynamics.o: src/dynamics.hpp src/dynamics.cpp src/ball.hpp
	g++ $(WARN) -g $(LINK) -c src/dynamics.cpp $(INC) -o build/dynamics.o

build/button.o: src/button.hpp src/button.cpp
	g++ $(WARN) -g $(LINK) -c src/button.cpp $(INC) -o build/button.o

build/shotParser.o: src/shotParser.hpp src/shotParser.cpp
	g++ $(WARN) -g $(LINK) -c src/shotParser.cpp $(INC) -o build/shotParser.o
	
build/TCPSocket.o: src/TCPSocket.hpp src/TCPSocket.cpp build/shotParser.o
	g++ $(WARN) -g $(LINK) -c src/TCPSocket.cpp $(INC) -o build/TCPSocket.o
endif

ifeq ($(PLATFORM), PLATFORM_WINDOWS)
build/jSim.exe: $(deps)
	g++ -DPLATFORM_WINDOWS $(WARN) -g $(INC_W) $(LDFLAGS_W) src/main.cpp -o build/jSim.exe $(OUTS) $(LDLIBS_W)

build/ball.o: src/ball.hpp src/ball.cpp
	g++ -DPLATFORM_WINDOWS $(WARN) -g -c src/ball.cpp $(INC_W) -o build/ball.o

build/dynamics.o: src/dynamics.hpp src/dynamics.cpp src/ball.hpp
	g++ -DPLATFORM_WINDOWS $(WARN) -g  -c src/dynamics.cpp $(INC_W) -o build/dynamics.o

build/button.o: src/button.hpp src/button.cpp
	g++ -DPLATFORM_WINDOWS $(WARN) -g  -c src/button.cpp $(INC_W) -o build/button.o

build/shotParser.o: src/shotParser.hpp src/shotParser.cpp
	g++ -DPLATFORM_WINDOWS $(WARN) -g -c src/shotParser.cpp $(INC_W) -o build/shotParser.o
	
build/TCPSocket.o: src/TCPSocket.hpp src/TCPSocket.cpp build/shotParser.o
	g++ -DPLATFORM_WINDOWS $(WARN) -g -c src/TCPSocket.cpp $(INC_W) -o build/TCPSocket.o
endif


run: build/jSim
	./build/jSim

clean:
	rm -I build/*
