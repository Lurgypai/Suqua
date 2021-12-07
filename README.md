# Suqua
A 2D networked game library.

## Content
This is a set of game libraries for 2D (or 3D if you want to manually write the OpenGL) games, with a focus on organizational tools, and handling netcode. The dream is to be able to (almost) seamlessly develop networked games that have little lag, with no concern for managing the synchronization of data manually.

## Testing on Windows
The latest release containes Box3, and modified source to allow the display of the networking capabilities. Simply download Box3.zip from the releases page and run it. (Windows may warn you that its unsafe, simply proceed to running.)

## Testing on Linux (Ubuntu)
**Note: I haven't yet gotten this to run (the machines I have don't support the minimum OpenGL required). The minimum required version is 4.3. You can check with the following command.**
```
glxinfo | grep "OpenGL version"
```
### Begin by installing the dependencies.
```
sudo apt install git cmake g++ libsdl2-dev
```
Following this, clone the repo and enter the demo branch.
```
git clone https://github.com/Lurgypai/Suqua.git
git switch NetworkingDemo
```
Enter the directory and run the script to generate the build files
```
cd Suqua
./out.sh
```
Enter the build directory for the target (Box3) and compile
```
cd out/Tests/Box3
make
```
Asssuming the compilation is successful, simply run the generated output!
```
./Box3
```

# THIS IS A VERY WORK IN PROGRESS
Feel free to contact me on twitter if you want to talk about it ([@Lurgypai](https://twitter.com/Lurgypai))