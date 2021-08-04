# Spectre2D project

Spectre2D project is a 2D game library. Its goal is to provide features for controlling as
many fields of game development as possible. Spectre2D is created alongside project FactAstra.

The name comes from the basic ideas of what the library should be:
- It should provide a full spectrum of fields to work in
- In each field it should provide a full spectrum of features
- For each category it should provide a full spectrum of control - both high-level and low-level options
- The name sounds cool and so should the library be

## Installation

To install the library, you'll need to build it from source with CMake.

1. Copy the repository
```git
git clone "https://github.com/DragonGamesStudios/Spectre2D.git"
```

2. Build the repository with CMake
```cmake
cmake -B ./build -G <your generator> .
```
3. Compile using your chosen build system

## Issues
Issues can be used for the following:
1. Reporting bugs
2. Requesting features

Bugs and features directly required by FactAstra project will always have priority over feature requests, but the requests will surely be taken care of.

## Using the library
1. In your project, add path to the library's include directory to additional include directories.

2. Link the built Spectre2D.lib to your project. Since it's a static library, you'll also be required to link its dependencies:
* [GLEW](http://glew.sourceforge.net/)
* [GLFW](https://www.glfw.org/)

Note, that glm is also one of dependencies of the library, but since it's header-only, there is no need to link it. Its features are, however accessible, by including most Spectre2D headers (e. g. Math.hpp).