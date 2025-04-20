build/main: src/main.cpp
	g++ src/main.cpp -o build/main

run: build/main
	./build/main

clean:
	rm -I build/*
