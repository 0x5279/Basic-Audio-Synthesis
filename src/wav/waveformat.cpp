#include <algorithm>
#include <cmath>
#include <fstream>
#include <utility>
#include <iostream>

#include "wave.h"

waveformat::waveformat(std::vector<twave> waves) {
    this->waves = std::move(waves);
}

waveformat::waveformat(std::vector<twave> waves, int bit_depth, int sample_rate) {
    this->waves = std::move(waves);
    this->bit_depth = bit_depth;
    this->sample_rate = sample_rate;
}

waveformat::waveformat(std::vector<twave> waves, int sample_rate) {
    this->waves = std::move(waves);
    this->sample_rate = sample_rate;
}

void waveformat::addWave(twave wave) {
    this->waves.push_back(std::move(wave));
}

void waveformat::removeWave(const twave& wave) {
    for (int i = 0; i < this->waves.size(); i++) {
        if (this->waves[i] == wave) {
            this->waves.erase(this->waves.begin() + i);
            i -= (i > 0);
        }
    }
}

void waveformat::setBitDepth(int bit_depth) {
    this->bit_depth = bit_depth;
}

float waveformat::getLen() const {
    return this->len;
}

void waveformat::setSampleRate(int sample_rate) {
    this->sample_rate = sample_rate;
}

void waveformat::compile(float volume, float length) {

    this->len = length;
    uint32_t true_sample_size = std::ceil(static_cast<float>(bit_depth)/8);

    for (const auto& wave : this->waves) {  //if the initial length wasn´t longer than the end time of any of the waves, we snap the length to the wave that ends the latest.
        if (wave.end_time > len) { this->len = wave.end_time; }
    }

    int total_cycles = static_cast<int>(static_cast<float>(this->sample_rate) * len);  //get the correct length for the samples vector, then reserve and assign that much space.
    samples.reserve(total_cycles * true_sample_size);
    samples.clear();
    samples.assign(total_cycles * true_sample_size, 0);

    for (int i = 0; i < total_cycles; i++) { //run once per cycle (roughly 44.1khz on CD sample rate)

        float value = 0.0f;
        float t = static_cast<float>(i) / this->sample_rate;  //the time the sample is supposed to be at.

        for (const auto& wave : this->waves) { //add up the samples
            value += wave.sample(t, len);
        }

        value = std::clamp(value * volume, -1.0f, 1.0f); //clamp them to valid readouts.

        int64_t value_i; //cast value to int for quantization

        if (true_sample_size == 1) {   //different behavior for 8 bits compared to anything else, 8 bit is represented as uint, the rest are signed int.

            value_i = static_cast<uint8_t>( value * 127.0f + 128.0f );

        } else {

            uint64_t max_value = (1LL << (true_sample_size * 8 - 1)) - 1;
            value_i = static_cast<int64_t>(value * max_value);

        }
        for (int j = true_sample_size - 1; j >= 0; j--) {
            char value_c = static_cast<char>((value_i >> (8 * j)) & 0xff);
            samples[i * true_sample_size + j] = value_c;
        }
    }
}

void waveformat::writeWav(std::string path) {
    std::fstream wav(path, std::fstream::out | std::fstream::binary);

    auto true_sample_size = static_cast<uint32_t>(std::ceil(static_cast<float>(this->bit_depth)/8) * 8);

    wav.write("RIFF",4);

    auto wav_length = static_cast<uint32_t>(samples.size() + 36);
    wav.write(reinterpret_cast<const char*>(&wav_length), sizeof(wav_length));

    wav.write("WAVEfmt ",8);
    uint32_t sample_size = 16;
    wav.write(reinterpret_cast<const char*>(&sample_size), sizeof(sample_size));

    uint16_t audio_format = 1;
    wav.write(reinterpret_cast<const char*>(&audio_format), sizeof(audio_format));

    uint16_t num_channels = 1;
    wav.write(reinterpret_cast<const char*>(&num_channels), sizeof(num_channels));

    uint32_t rate = this->sample_rate;
    wav.write(reinterpret_cast<const char*>(&rate), sizeof(rate));

    uint32_t byte_rate = rate * true_sample_size/8;
    wav.write(reinterpret_cast<const char*>(&byte_rate), sizeof(byte_rate));

    uint16_t block_align = num_channels * true_sample_size/8;
    wav.write(reinterpret_cast<const char*>(&block_align), sizeof(block_align));

    uint16_t bits_per_sample = true_sample_size;
    wav.write(reinterpret_cast<const char*>(&bits_per_sample), sizeof(bits_per_sample));

    wav.write("data",4);
    wav_length = static_cast<uint32_t>(samples.size());
    wav.write(reinterpret_cast<const char*>(&wav_length), sizeof(wav_length));

    wav.write(samples.data(), wav_length);
}