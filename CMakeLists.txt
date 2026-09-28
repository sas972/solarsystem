cmake_minimum_required(VERSION 3.20)
project(SolarSystemSimulator VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# SFML 3 megkeresése (Graphics, Window és System komponensekre van szükség)
find_package(SFML 3 COMPONENTS Graphics Window System REQUIRED)

add_executable(SolarSystemSimulator main.cpp)

# Linkelés az SFML 3 könyvtáraival
target_link_libraries(SolarSystemSimulator PRIVATE SFML::Graphics SFML::Window SFML::System)
