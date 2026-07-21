#include <iostream>
#include <thread>

#include "wav/wave.h"
#include "audio/audio.h"
#include "filesystem"

int main()
{

    float notes[12] = {
        220.0f,
        233.1f,
        246.9f,
        261.6f,
        277.2f,
        293.7f,
        311.1f,
        329.6f,
        349.2f,
        370.0f,
        392.0f,
        415.4f
    };

    wave base({wave::SAW,220,0.2,0});
    wave layer1({wave::SAW,2,0.5,0});
    wave layer2({wave::SAW,4,0.25,0});
    wave layer3({wave::SAW,8,0.125,0});

    std::vector<wave> harmonics = {layer1, layer2, layer3};

    std::vector<twave::modNode> adsr = {
        {0,0,false}, //start
        {1,0.05,false}, //attack
        {0.8,0.1,false}, //decay
        {0.8,-0.2,false}, //sustain
        {0,-0.001,false}, //release
    };

    std::vector<twave> twaves;

    for (int i = 0; i < 12; i++) {
        base.data.pitch = notes[i];
        cwave cwave1(base, harmonics);
        twave twave1(cwave1,static_cast<float>(i)/2.0f,(1 + static_cast<float>(i))/2);
        twave1.mods = adsr;

        twaves.push_back(twave1);
    }

    rt_waveformat track1(twaves,48000,6);
    waveformat saveable_track(twaves,16,48000);

    audioPlayer audio(track1);
    audio.Init();
    audio.selectCurrentOutputDevice();
    audio.Start();

    auto dir_to_project = std::filesystem::current_path().parent_path().string();

    saveable_track.compile();
    saveable_track.writeWav(dir_to_project + "/test_wav.wav");

    std::cout << ".wav file created at " << dir_to_project << std::endl;

    std::this_thread::sleep_for(std::chrono::seconds(10));
}