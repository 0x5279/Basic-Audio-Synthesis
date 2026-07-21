#include <algorithm>
#include <utility>

#include "wave.h"

rt_waveformat::rt_waveformat(std::vector<twave> waves, double sample_rate, float duration) {
    this->sample_rate = sample_rate;
    this->waves = std::move(waves);
    this->current_time = 0.0;
    this->duration = duration;
    updateDuration();
}

void rt_waveformat::updateDuration() {
    for (const auto& wave : this->waves) {
        this->duration = std::max(this->duration, static_cast<double>(wave.end_time));
    }
}

void rt_waveformat::addWave(const twave& wave) {
    this->waves.push_back(wave);
    updateDuration();
}

void rt_waveformat::removeWave(const twave& wave) {
    for (int i = 0; i < this->waves.size(); i++) {
        if (this->waves[i] == wave) {
            this->waves.erase(this->waves.begin() + i);
            i -= (i > 0);
        }
    }
    updateDuration();
}

void rt_waveformat::setSampleRate(double sample_rate) {
    this->sample_rate = sample_rate;
}

uint32_t rt_waveformat::getSampleRate() const{
    return this->sample_rate;
}

float rt_waveformat::nextSample(float volume) {
    float out = 0.0;

    for (auto wave : this->waves) {
        out += wave.sample(this->current_time, this->duration);
    }

    this->current_time += 1.0f / this->sample_rate;

    return std::clamp(out * volume, -1.0f, 1.0f);
}

bool rt_waveformat::fillBuffer(float *buf, size_t size) {

    for (int i = 0; i < size; i++) {
        float sample = nextSample();

        buf[i*2] = sample;
        buf[i*2+1] = sample;
    }

    return current_time >= this->duration;
}
