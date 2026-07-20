#include "wave.h"
#include <cmath>
#define pi 3.14159265358979323846

wave::wave(const Wave &base) {
    this->data = base;
}


float wave::sample(float t) const {

    float phase = std::fmod(t*data.pitch + data.phase,1.0f);
    phase += static_cast<float>(phase < 0.0f);

    const auto on_lhs = static_cast<float>(phase < 0.5); //useful for removing unnecessary branches

    switch (data.type) { //return f(phase) * amplitude for the given waveform.
        case SAW:
            return data.amplitude * (phase * 2 - 1);
        case TRIANGLE:
            return data.amplitude * ((on_lhs * phase) + ((1.0f - on_lhs) * (1.0f - phase))) * 4.0f - 1;
        case SQUARE:
            return data.amplitude * (on_lhs * 2 - 1);
        case SINE:
            return data.amplitude * static_cast<float>(std::sin(2*pi*phase));
            //not as performant, use sparingly.
    }
    return 0.0f;
}