#include <algorithm>
#include <iostream>
#include <chrono>
#include "./wav/wave.h"

/* THIS IS A TEMPORARY SETUP FOR BENCHMARKING PURPOSES. */

int main() {
    wave my_wave({wave::SINE,375,0.1,0,});
    wave layer1({wave::SINE,2,0.3,0,});
    wave layer2({wave::SINE,4,0.15,0,});
    wave layer3({wave::SINE,8,0.075,0,});

    cwave my_cwave(my_wave, {layer1,layer2,layer3});

    twave my_twave(my_cwave,0,5.0);

    waveformat my_wavef({my_twave},16,44100);

    const auto start = std::chrono::high_resolution_clock::now();

    my_wavef.compile();
    my_wavef.writeWav("/home/megag/Desktop/new-wav.wav");

    const auto end = std::chrono::high_resolution_clock::now();
    const auto dur = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

    int count = my_wavef.getLen() * 44100;

    std::cout << dur.count() << "us to run batch of " << count << "samples.\n";
    float rate = static_cast<float>(count) / static_cast<float>(dur.count());
    std::cout << "runs at " << rate * 1000 << " samples/ms.\n";
    std::cout << "can process CD samples " << ((rate * 1000000) / 44100) << "x faster than real-time.\n";

}