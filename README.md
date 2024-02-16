﻿# PCOAbleton
This projects depends on the following submodules: Glew, GLFW and ImGui

## Building
Follow the following steps to clone this repository in vscode.

### If you're using VSCode
make sure you have the <code>C/C++ Extension Pack</code> by Microsoft installed.
```
git clone --recursive https://github.com/NertNiels/PCOAbleton.git
cd PCOAbleton
code .
```
CMake should build automatically.

### If you're not using VSCode
```
git clone --recursive https://github.com/NertNiels/PCOAbleton.git
cd PCOAbleton
mkdir build
cd build
cmake ..
cmake --build .
```

## Executing
From inside the main directory run:
### Windows
```
.\build\Debug\pco_ableton.exe
```
