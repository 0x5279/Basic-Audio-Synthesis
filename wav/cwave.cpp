#include "wave.h"

#include <utility>
#include <iostream>

//declarations

cwave::cwave(const wave &base) {
    this->base_wave = base;
}

cwave::cwave(const wave& base, std::vector<wave> children) {
    this->base_wave = base;
    this->child_waves = std::move(children);
}

void cwave::addChild(wave child) {
    this->child_waves.push_back(std::move(child));
}

void cwave::removeChild(const wave& child) {   //iterate through all child waves, removing all that are equal to the target child
    for (int i = 0; i < this->child_waves.size(); i++) {
        if (this->child_waves[i] == child) {
            this->child_waves.erase(this->child_waves.begin() + i);
            i -= (i > 0);
        }
    }
}

void cwave::clearChildren() {
    this->child_waves.clear();
}

void cwave::changeBaseWave(const wave& base) {
    this->base_wave = base;
}

//I'm genuinely going to have an aneurysm if you can't understand these.

cwave cwave::operator*(float const &other) const {
    cwave copy(*this);
    copy *= other;
    return copy;
}

cwave cwave::operator+(cwave &other) const {
    cwave copy(*this);
    copy += other;
    return copy;
}

cwave cwave::operator-(cwave &other) const {
    cwave copy(*this);
    copy -= other;
    return copy;
}

cwave cwave::operator-() const {
    cwave copy(*this);
    copy *= -1;
    return copy;
}

void cwave::operator*=(float other) {   //multiply all the pitches together when multiplying by a float.
    this->base_wave.data.pitch *= other;

    for (auto & child_wave : this->child_waves) {
        child_wave.data.pitch *= other;
    }
}

void cwave::operator+=(const cwave &other) {    //add the latter cwave to the former's child waves
    float modifier = this->base_wave.data.pitch / other.base_wave.data.pitch;
    cwave copy = other * modifier;
    this->child_waves.push_back(copy.base_wave);
    this->child_waves.insert(this->child_waves.end(), copy.child_waves.begin(), copy.child_waves.end());
}

void cwave::operator-=(const cwave &other) {    //same as previous, but shifted 1/2 of a phase
    float modifier = this->base_wave.data.pitch / other.base_wave.data.pitch;
    cwave copy = other * -modifier;
    this->child_waves.push_back(copy.base_wave);
    this->child_waves.insert(this->child_waves.end(), copy.child_waves.begin(), copy.child_waves.end());
}

float cwave::sample(float t) const {

    auto &base = this->base_wave; //reference for making code easier to read
    float master_sample = base.sample(t); //start off with the sample from the base wave


    for (const auto& child_wave : this->child_waves) { //iterate over all child waves, making sure to shift them from relative to absolute
        wave absolute = child_wave;
        absolute.data.amplitude *= base.data.amplitude;
        absolute.data.pitch *= base.data.pitch;
        master_sample += absolute.sample(t);  //once in absolute perspective, sum over base sample
    }

    return master_sample;
}
