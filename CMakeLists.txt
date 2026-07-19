cmake_minimum_required(VERSION 4.3)
project(VocaloidFromScratch)

set(CMAKE_CXX_STANDARD 20)

add_executable(VocaloidFromScratch main.cpp
        wav/wave.cpp
        wav/wave.h
        wav/cwave.cpp
        wav/twave.cpp
        wav/waveformat.cpp)
