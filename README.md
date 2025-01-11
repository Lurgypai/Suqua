# Suqua
A 2D networked game library.

## Content
This is a set of game libraries for 2D (or 3D if you want to manually write the OpenGL) games, with a focus on organizational tools, and handling netcode. The dream is to be able to (almost) seamlessly develop networked games that have little lag, with no concern for managing the synchronization of data manually.

## On Windows
For developing on Windows, you should be able to clone the repo and then open the directory within Visual Studio. VS should recognize it as a CMake project, letting you simply compile and run it. Feel free to reach out with issues running on windows.

## On Linux (Ubuntu)
**The minimum required OpenGL version is 4.3. You can check with the following command.**
```
glxinfo | grep "OpenGL version"
```
### Begin by installing the dependencies.
```
sudo apt install git cmake g++ libsdl2-dev
```
Following this, clone the repo. The default branch (master) contains the demos.
```
git clone https://github.com/Lurgypai/Suqua.git
```
Enter the directory and run the script to generate the build files
```
cd Suqua
./out.sh
```
Enter the build directory for the target (Demo/Platformer) and compile
```
cd out/Demo/Platformer
make
```
Asssuming the compilation is successful, simply run the generated output!
```
./Platformer
```

# THIS IS A VERY WORK IN PROGRESS
Feel free to contact me on twitter if you want to talk about it ([@Lurgypai](https://twitter.com/Lurgypai))
