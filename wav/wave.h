#pragma once

#ifndef VOCALOIDFROMSCRATCH_WAV_H
#define VOCALOIDFROMSCRATCH_WAV_H
#include <cstdint>
#include <string>
#include <vector>

class wave {
    public:
    enum WaveType { //pre-defined wave 'shapes'
        SAW,
        TRIANGLE,
        SQUARE,
        SINE,
    };

    struct Wave {
        WaveType type; //the 'shape' of a wave

        float pitch; //As a base wave or standalone wave, pitch is measured in hz.
        //For child waves, the pitch is a multiplier added onto the base wave.
        //This allows for greater flexibility while keeping the code itself clean.

        float amplitude; // decimal anywhere between 0 and 1, synonymous with volume.

        float phase; //distance ahead the given wave is, measured in wavelengths.
        //Any value should work, but using decimals between 0 and 1 is recommended.
        bool operator==(const Wave & wave) const = default;
    };

    Wave data{};

    //definitions found in wave.cpp

    wave() = default;
    wave(const Wave &base);

    [[nodiscard]] float sample(float t) const;

    bool operator==(std::vector<wave>::const_reference wave) const = default;
};


class cwave { // cwave here stands for compound wave, the result of combining simpler waves.

    wave base_wave{}; //The 'parent' wave that all the children waves are attached to.

    std::vector<wave> child_waves; //Every parameter in the children waves(besides type) is relative to the base wave.

    public:

    //definitions found in cwave.cpp

    cwave() = default;
    cwave(const wave& base, std::vector<wave> children);
    cwave(const wave &base);

    void addChild(wave child);
    void removeChild(const wave& child);
    void changeBaseWave(const wave& base);
    void clearChildren();
    [[nodiscard]] float sample(float t) const;

    cwave& operator=(cwave const &other) = default;
    cwave operator*(float const &other) const;
    void operator*=(float other);
    cwave operator+(cwave& other) const;
    cwave operator-(cwave& other) const;
    void operator+=(const cwave& other);
    void operator-=(const cwave& other);
    cwave operator-() const;

    bool operator==(const cwave & cwave) const = default;
};

class twave { //cwave wrapper with defined temporal positions
public:

    struct modNode {
        float value;
        float time;
        bool isAbsolute;

        bool operator==(const modNode & mod) const = default;
    };

    std::vector<modNode> mods;
    cwave base_wave{};
    float start_time = 0;
    float end_time = 0;

    //definitions in twave.cpp

    twave() = default;
    twave(const cwave& base_wave, float start_time, float end_time);
    twave(const cwave& base_wave, float start_time);
    twave(const cwave& base_wave);

    void setDuration(float duration);
    [[nodiscard]] float getDuration() const;
    [[nodiscard]] static float getAbsoluteTime(modNode mod, float len);
    [[nodiscard]] float sample(float t, float len) const;

    bool operator==(std::vector<twave>::const_reference wave) const = default;
};

class waveformat { //the final 'compilation' step in creating a wave. This is where it gets turned into a .wav file.

    std::vector<twave> waves; //the waves to be compiled
    std::vector<char> samples; //where the final samples will be kept until they are passed into the file.
    float len;

    int bit_depth = 16; //determines the # of bits allowed for
    int sample_rate = 44100; //how fast the audio will be sampled
    //the rest of the details in the fmt section will be filled out with 'default' formatting.

public:

    //definitions in waveformat.cpp

    waveformat() = default;
    waveformat(std::vector<twave> waves, int bit_depth, int sample_rate);
    waveformat(std::vector<twave> waves, int sample_rate);
    waveformat(std::vector<twave> waves);

    void addWave(twave wave);
    void removeWave(const twave& wave);
    void setBitDepth(int bit_depth);
    void setSampleRate(int sample_rate);
    [[nodiscard]] float getLen() const;
    void compile(float length, float volume = 0.2);
    void writeWav(std::string path);

};


class rt_waveformat {
    std::vector<twave> waves;
    double sample_rate;
    double current_time;    //time currently being read out
    double duration;

    void updateDuration();

public:

    rt_waveformat() = default;
    rt_waveformat(std::vector<twave> waves, double sample_rate, float duration);


    void addWave(const twave& wave);
    void removeWave(const twave& wave);
    void setSampleRate(double sample_rate);
    [[nodiscard]] uint32_t getSampleRate() const;

    float nextSample(float volume = 0.2);
    bool fillBuffer(float* buf, size_t size); //returns current_time >= duration
};

#endif //VOCALOIDFROMSCRATCH_WAV_H