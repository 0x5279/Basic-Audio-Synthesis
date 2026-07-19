#include "wave.h"

twave::twave(const cwave& base_wave) {
    this->base_wave = base_wave;
}

twave::twave(const cwave& base_wave, float start_time) {
    this->base_wave = base_wave;
    this->start_time = start_time;
}

twave::twave(const cwave& base_wave, float start_time, float end_time) {
    this->base_wave = base_wave;
    this->end_time = end_time;
    this->start_time = start_time;
}

void twave::setDuration(float duration) {
    end_time = start_time + duration;
}

float twave::getDuration() const {
    return end_time - start_time;
}

float twave::sample(float t) const {
    if (t < start_time || t > end_time) {
        return 0.0f;
    }

    return base_wave.sample(t);
}
