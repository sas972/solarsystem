# Solar System Simulator

A precise 2D Solar System simulator written in C++20 using SFML 3.

## Features

- **Accurate Physics:** Utilizes 4th-order Runge-Kutta (RK4) integration for stable and precise calculation of planetary orbits.
- **N-Body Gravity:** Calculates gravitational forces between all bodies in the system based on Newton's law of universal gravitation.
- **Visuals:** Renders planetary bodies and their orbital trails using SFML 3.
- **Modern C++:** Written in C++20, taking advantage of modern features.
- **Cross-Platform Build:** Uses CMake for easy configuration on macOS (with native `.app` bundle support), Linux, and Windows.

## Requirements

- C++20 compatible compiler (e.g., GCC, Clang, MSVC)
- CMake 3.20 or higher
- [SFML 3](https://www.sfml-dev.org/) (Graphics, Window, System components)

## Building the Project

1. **Clone the repository:**
   ```bash
   git clone git@github.com:sas972/solarsystem.git
   cd solarsystem
   ```

2. **Create a build directory and configure with CMake:**
   ```bash
   mkdir build
   cd build
   cmake ..
   ```

3. **Build the executable:**
   ```bash
   make
   ```
   *(Note: On macOS, this will generate a `SolarSystemSimulator.app` bundle natively).*

4. **Run the simulator:**
   ```bash
   ./SolarSystemSimulator
   # Or on macOS, you can also launch the generated app bundle:
   # open SolarSystemSimulator.app
   ```

## Simulation Details

- The simulation is currently pre-configured with the Sun, Mercury, Venus, Earth, and Mars.
- It uses a realistic scaling system and accurate initial parameters (Mass, Distance, Velocity) mapped to astronomical units.
- Simply run the application to watch the planetary orbits evolve over time!
