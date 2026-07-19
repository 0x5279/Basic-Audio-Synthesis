#include <algorithm>
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

void waveformat::setSampleRate(int sample_rate) {
    this->sample_rate = sample_rate;
}

float waveformat::getLen() {
    return this->len;
}

void waveformat::compile() {
    this->len = 0.0f;

    for (const auto& wave : this->waves) {
        if (wave.end_time > len) {
            len = wave.end_time;
        }
    }

    int total_cycles = static_cast<int>(static_cast<float>(this->sample_rate) * len);
    samples.reserve(total_cycles);
    samples.clear();
    samples.assign(total_cycles, 0);

    for (int i = 0; i < total_cycles; i++) {
        float value = 0.0f;

        float t = static_cast<float>(i) / this->sample_rate;

        for (const auto& wave : this->waves) {
            value += wave.sample(t);
        }

        value = std::clamp(value, 0.0f, 1.0f);

        samples[i] = static_cast<uint16_t>(value * 65535);

    }
}

void waveformat::writeWav(std::string path) {
    std::fstream wav(path, std::fstream::out | std::fstream::binary);

    wav.write("RIFF",4);

    auto wav_length = static_cast<uint32_t>((samples.size() * 2) + 36);
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

    uint32_t byte_rate = 88200;
    wav.write(reinterpret_cast<const char*>(&byte_rate), sizeof(byte_rate));

    uint16_t block_align = num_channels * bit_depth/8;
    wav.write(reinterpret_cast<const char*>(&block_align), sizeof(block_align));

    uint16_t bits_per_sample = 16;
    wav.write(reinterpret_cast<const char*>(&bits_per_sample), sizeof(bits_per_sample));

    wav.write("data",4);
    wav_length = static_cast<uint32_t>(samples.size() * 2);
    wav.write(reinterpret_cast<const char*>(&wav_length), sizeof(wav_length));

    wav.write(reinterpret_cast<const char*>(samples.data()), samples.size() * sizeof(samples[0]));
}
