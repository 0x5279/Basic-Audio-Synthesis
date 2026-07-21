#include <iostream>
#include <thread>

#include "wav/wave.h"
#include "audio/audio.h"

int main()
{
    wave base({wave::SINE,220,0.2,0});
    wave layer1({wave::SINE,2,0.5,0});
    wave layer2({wave::SINE,4,0.25,0});
    wave layer3({wave::SINE,8,0.125,0});

    std::vector<wave> harmonics = {layer1, layer2, layer3};

    std::vector<twave::modNode> adsr = {
        {0,0,false}, //start
        {1,0.05,false}, //attack
        {0.8,0.1,false}, //decay
        {0.8,-0.2,false}, //sustain
        {0,-0.001,false}, //release
    };

    cwave cwave1(base, harmonics);
    twave twave1(cwave1,0,5);
    twave1.mods = adsr;

    base.data.pitch = 220 * 1.25;

    cwave cwave2(base, harmonics);
    twave twave2(cwave2,1,4);
    twave2.mods = adsr;

    base.data.pitch = 220 * 1.5;

    cwave cwave3(base, harmonics);
    twave twave3(cwave3,2,3);
    twave3.mods = adsr;

    rt_waveformat track1({twave1,twave2,twave3},48000,6);

    audioPlayer audio(track1);
    audio.Init(4);
    audio.Start();

    std::this_thread::sleep_for(std::chrono::seconds(6));
}