cmake_minimum_required(VERSION 4.3)
project(VocaloidFromScratch)

set(CMAKE_CXX_STANDARD 20)

find_package(PkgConfig REQUIRED)
pkg_check_modules(PORTAUDIO REQUIRED portaudio-2.0)

add_executable(VocaloidFromScratch main.cpp
        ./wav/wave.cpp
        ./wav/wave.h
        ./wav/cwave.cpp
        ./wav/twave.cpp
        ./wav/waveformat.cpp
        ./audio/audio.h
        ./wav/rt_waveformat.cpp
        ./audio/audio.cpp)

target_include_directories(
        VocaloidFromScratch
        PRIVATE
        ${PORTAUDIO_INCLUDE_DIRS}
)

target_link_libraries(
        VocaloidFromScratch
        ${PORTAUDIO_LIBRARIES}
)