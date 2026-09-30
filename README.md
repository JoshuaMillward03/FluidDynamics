# 2D Particle Fluid Simulation

A real-time, particle-based 2D fluid simulation written in C++ using the [Raylib](https://www.raylib.com/) library. This project implements a custom physics solver inspired by Smoothed Particle Hydrodynamics (SPH) to simulate realistic fluid behaviors through density, pressure, and viscosity calculations.

## Features

* **Custom Physics Solver:** Calculates density-based pressure and velocity-based viscosity between individual nodes using specialized smoothing kernels.
* **Interactive Fluid Mechanics:** 
  * **Left Click:** Attract particles toward the mouse cursor.
  * **Right Click:** Repel particles away from the mouse cursor.
* **Dynamic Boundaries:** Implements both hard boundary collisions and repulsive wall forces to contain the fluid naturally without clipping.
* **Stable Integration:** Utilizes delta-time integration and acceleration clamping to ensure smooth particle velocities regardless of frame rate.

## Prerequisites

To build and run this project, you need:
1. A C++ compiler (GCC, Clang, or MSVC) supporting C++17 or later.
2. [Raylib](https://github.com/raysan5/raylib) installed on your system.
   * **macOS:** `brew install raylib`
   * **Linux (Ubuntu/Debian):** `sudo apt install libraylib-dev`
   * **Windows:** Install the Raylib environment to `C:/raylib`

## Compiling & Running

This project uses a cross-platform `Makefile` that automatically detects your operating system (Windows, macOS, or Linux) and applies the necessary compiler and linker flags.

1. Clone the repository and navigate to the project folder:
```bash
git clone [https://github.com/JoshuaMillward03/FluidDynamics.git](https://github.com/JoshuaMillward03/FluidDynamics.git)
cd FluidDynamics
```
Compile the project:
```bash
make
```
Run the executable:

macOS / Linux: ```./fluidsim ```

Windows: ``` .\fluidsim.exe ```
