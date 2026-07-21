#include <algorithm>
#include <iostream>
#include <ostream>

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

float twave::getAbsoluteTime(modNode mod, float len) {
    if (mod.time < 0.0f) {
        return len - mod.time;
    }
    return mod.time;
}

float twave::sample(float t, float len) const {
    if (t < start_time || t > end_time) {
        return 0.0f;
    }

    float sample = base_wave.sample(t);

    if (mods.empty()) {return sample;}

    for (size_t i = 0; i < mods.size(); ++i)
    {
        float rightTime = getAbsoluteTime(mods[i], len);

        if (rightTime >= t)
        {
            if (i == 0)
                return mods[0].value * sample;

            float leftTime = getAbsoluteTime(mods[i - 1], len);

            float dt = rightTime - leftTime;
            if (dt == 0.0f) {
                return mods[i].value * sample;
            }

            float alpha =
                (t - leftTime) /
                (rightTime - leftTime);
            alpha = std::clamp(alpha, 0.0f, 1.0f);

            float gain =
                mods[i - 1].value * (1.0f - alpha) +
                mods[i].value * alpha;

            return gain * sample;
        }
    }

    return mods.back().value * sample;
}