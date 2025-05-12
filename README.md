# JaySim
C++ Golf Sim built using RayLib  
***WARNING*** - This is software is pre-pre-pre-alpha. You should expect it to break and be buggy.  
That is all.  
## Build Instructions
### Linux
#### Install Dependencies
##### GCC
- Follow the appropriate installation instructions for your distro.
- Currently using gcc version 15.0.1-0
##### Make
- Follow the appropriate installation instructions for your distro.
- Currently using GNU Make 4.4.1
##### Raylib 
- Follow the instructions on the Raylib GitHub page: https://github.com/raysan5/raylib
- Make sure to install the built files so that they are findable by the pre-processor/linker using `make install` or other variation.
#### Clone Repository
- Clone repository into a local folder
- `git clone https://github.com/jhauck2/JaySim.git`
- `cd JaySim`
#### Download json Library
- Create a folder in the src directory for the nlohmann json library
- `cd src`
- `mkdir nlohmann`
- Go to https://github.com/nlohmann/json and download the json.hpp file from the single_include/nlohmann folder
- Copy/move this file into the src/nlohmann folder
#### Build
- Navigate to the root directory. `cd ..` (or `cd ../..` if currently in the nlohmann folder).
- Create the build directory: `mkdir build`
- Run `make` to compile the program
- All *.o files and the executable should be located in the build directory.
#### Run
- `make run` or `./build/jSim` to run the program
- Note: JaySim uses port 49152 for connection with a LM. If you need to change this port for any reason you can change the value in the file `TCPSocket.hpp` and rebuild with `make` or `make run`
  
### Windows  
**This is a work in progress. Does not currently build**
#### Install Dependencies
##### MinGW64 Devkit
- Navigate to https://github.com/skeeto/w64devkit/releases and download the w64devkit-x86-2.2.0.7z.exe
- Note: Windows Defender and/or your browser may flag this as a virus. Override this and run the exe
- This is a self extracting executable. Exctract the files to your C: drive
- Navigate to C:/w64devkit and run w64devkit.exe. You should see a w64devkit terminal
##### RayLib
- Download the raylib zipped source files from https://ghithub.com/raysan5/raylib
- Extract the files to your C: drive
- In your w64devkit terminal naviage to C:/raylib-master/src
- Run `make`
#### Clone Repository
- Clone repository into a local folder
- `git clone https://github.com/jhauck2/JaySim.git`
- `cd JaySim`
#### Download json Library
- Create a folder in the src directory for the nlohmann json library
- `cd src`
- `mkdir nlohmann`
- Go to https://github.com/nlohmann/json and download the json.hpp file from the single_include/nlohmann folder
- Copy/move this file into the src/nlohmann folder
#### Build
- Navigate to the root directory. `cd ..` (or `cd ../..` if currently in the nlohmann folder).
- Create the build directory: `mkdir build`
- Run `make` to compile the program
- All *.o files and the executable should be located in the build directory.
#### Run
- `make run` or `./build/jSim` to run the program
- Note: JaySim uses port 49152 for connection with a LM. If you need to change this port for any reason you can change the value in the file `TCPSocket.hpp` and rebuild with `make` or `make run`

