#pragma once

#ifndef VOCALOIDFROMSCRATCH_AUDIO_H
#define VOCALOIDFROMSCRATCH_AUDIO_H
#include <portaudio.h>
#include <iostream>

#include "../wav/wave.h"

class audioPlayer {


    static int callback(
    const void* input,
    void* output,
    unsigned long frames,
    const PaStreamCallbackTimeInfo* time,
    PaStreamCallbackFlags flags,
    void* userData
    );

    PaStream* stream = nullptr;
    PaStreamParameters output;
    bool init = false;
    rt_waveformat current_track;
    bool is_playing = false;

    public:

    struct Device {
        std::string name;
        int index;
    };

    audioPlayer() = default;
    audioPlayer(rt_waveformat track);

    ~audioPlayer();

    bool Init(int device = paNoDevice);
    void Start();
    void Stop();

    void setTrack(rt_waveformat track);
    void setTrackSampleRate(double sample_rate);
    bool selectCurrentOutputDevice();
    [[nodiscard]] std::vector<Device> getOutputDevices() const;
    int getCurrentOutputDevice();

};

#endif //VOCALOIDFROMSCRATCH_AUDIO_H
