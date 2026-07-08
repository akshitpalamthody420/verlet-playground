# Verlet Playground

An interactive 2D physics sandbox built in C++ with SFML.

The project simulates particles using Verlet integration and connects them with distance constraints to create ropes, cloth, and soft-body structures. It is based on a particle-and-stick physics system, expanded into a more complete interactive simulation.

## Features

* Verlet-based particle motion
* Distance constraints between particles
* Pinned and unpinned particles
* Rope generation
* Cloth grid generation
* Soft-body blob generation
* Gravity toggle
* Wind toggle
* Constraint tearing
* Mouse-based dragging
* Mouse-based cutting
* Real-time interaction using SFML

## Controls

| Input              | Action                               |
| ------------------ | ------------------------------------ |
| Left mouse         | Drag nearest particle                |
| Right mouse        | Pin or unpin nearest particle        |
| Middle mouse       | Cut constraints near the cursor      |
| Space              | Spawn a pinned rope at the cursor    |
| C                  | Spawn a cloth sheet near the cursor  |
| B                  | Spawn a soft-body blob at the cursor |
| W                  | Toggle wind                          |
| T                  | Toggle tearing                       |
| G                  | Toggle gravity                       |
| R                  | Reset demo scene                     |
| Backspace / Delete | Clear the world                      |
| Escape             | Quit                                 |

## Requirements

* C++17 or newer
* CMake
* SFML 2.5 or newer

## Build and Run

From the project root:

```bash
mkdir build
cd build
cmake ..
cmake --build .
./VerletPlayground
```

## macOS Setup with Homebrew

Install CMake and SFML 2:

```bash
brew install cmake sfml@2
```

Then build with the SFML path passed to CMake:

```bash
mkdir build
cd build
cmake -DCMAKE_PREFIX_PATH="$(brew --prefix sfml@2)" ..
cmake --build .
./VerletPlayground
```

## Windows Notes

Use CMake to generate a Visual Studio project or another supported build system.

Make sure the SFML DLL files are available next to the generated executable or included in your system PATH.
