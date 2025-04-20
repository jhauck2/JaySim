build/jSim: src/main.cpp
	g++ src/main.cpp -L /usr/local/lib64 -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -o build/jSim

run: build/jSim
	./build/jSim

clean:
	rm -I build/*
