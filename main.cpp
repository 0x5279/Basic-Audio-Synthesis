#include <algorithm>
#include <iostream>
#include <chrono>
#include "./wav/wave.h"

/* THIS IS A TEMPORARY SETUP FOR BENCHMARKING PURPOSES. */

int main() {
    wave my_wave({wave::SAW,220,0.1,0,});
    wave layer1({wave::SAW,1,0.3,0,});
    wave layer2({wave::SAW,1,0.15,0,});
    wave layer3({wave::SAW,1,0.075,0,});

    std::vector<twave::modNode> adsr {
        {0,0,false}, //start
        {1,0.02,false}, //attack
        {0.8,0.1,false}, //decay
        {0.7,-0.2,false}, //sustain
        {0,0.001,false} //release
    };

    std::vector<wave> harmonics {
        layer1,
        layer2,
        layer3,
    };

    cwave my_cwave(my_wave, harmonics);
    twave my_twave(my_cwave,0,5.0);
    my_twave.mods = adsr;

    my_wave.data.pitch = 220 * 1.25;
    cwave my_cwave2(my_wave, harmonics);
    twave my_twave2(my_cwave2,1.0,4.0);
    my_twave2.mods = adsr;

    my_wave.data.pitch = 220 * 1.5;
    cwave my_cwave3(my_wave, harmonics);
    twave my_twave3(my_cwave3,2.0,3.0);
    my_twave3.mods = adsr;

    waveformat my_wavef({my_twave,my_twave2,my_twave3},16,44100);

    const auto start = std::chrono::high_resolution_clock::now();

    my_wavef.compile(6,1.0);
    my_wavef.writeWav("/home/megag/Desktop/new-wav.wav");

    const auto end = std::chrono::high_resolution_clock::now();
    const auto dur = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

    int count = my_wavef.getLen() * 44100;

    std::cout << dur.count() << "us to run batch of " << count << "samples.\n";
    float rate = static_cast<float>(count) / static_cast<float>(dur.count());
    std::cout << "runs at " << rate * 1000 << " samples/ms.\n";
    std::cout << "can process CD samples " << ((rate * 1000000) / 44100) << "x faster than real-time.\n";

}