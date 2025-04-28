# JaySim
C++ Golf Sim built using RayLib
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
- Follow the instructions on the Raylib GitHub page.
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
- Run `make` to compile the program
- All *.o files and the executable should be located in the build directory.
#### Run
- `make run` or `./build/jSim` to run the program
