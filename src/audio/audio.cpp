#include "audio.h"

#include <utility>



audioPlayer::audioPlayer(rt_waveformat track) {
    this->current_track = std::move(track);
}

audioPlayer::~audioPlayer()
{
    if (is_playing) {
        Pa_StopStream(stream);
    }

    if (stream != nullptr)
    {
        Pa_CloseStream(stream);
    }

    if (init)
    {
        Pa_Terminate();
    }
}

int audioPlayer::callback(
    const void* input,
    void* output,
    unsigned long frames,
    const PaStreamCallbackTimeInfo* time,
    PaStreamCallbackFlags flags,
    void* userData
)
{

    auto* player =
        static_cast<audioPlayer*>(userData);

    auto* out = static_cast<float*>(output);

    player->is_playing = player->is_playing && !player->current_track.fillBuffer(out, frames);

    if (player->is_playing)
        return paContinue;

    return paComplete;
}

bool audioPlayer::Init(int device) {
    Pa_Initialize();
    init = true;

    auto defaultDevice = Pa_GetDefaultOutputDevice(); //grab the default device

    if (device != paNoDevice) { //if device is given, use that
        this->output.device = device;
    }

    if (this->output.device == paNoDevice) { //if not already initialized in some other way
        if (defaultDevice != paNoDevice) { //try to use default. If that fails, ask the user to choose an output device.
            this->output.device = defaultDevice;
        }else {
            if (!selectCurrentOutputDevice()) {return false;} //return if it didn't finish.
        }
    }

    output.channelCount = 2;
    output.sampleFormat = paFloat32;
    output.suggestedLatency =
        Pa_GetDeviceInfo(output.device)->defaultLowOutputLatency;
    output.hostApiSpecificStreamInfo = nullptr;

    const PaDeviceInfo* info = Pa_GetDeviceInfo(output.device);

    double sampleRate = info->defaultSampleRate;

    this->current_track.setSampleRate(sampleRate);

    std::cout << "Using " << info->name << "\n";
    std::cout << "Current sample rate: " << sampleRate << "\n";
    std::cout << "Current latency: "
              << info->defaultLowOutputLatency
              << "\n";

    PaError err = Pa_OpenStream(
        &stream,
        nullptr,
        &output,
        sampleRate,
        256,
        paNoFlag,
        callback,
        this
    );

    if (err != paNoError)
    {
        std::cout << Pa_GetErrorText(err) << "\n";
        return false;
    }

    return true;
}

void audioPlayer::Start() {
    if (!is_playing) {
        is_playing = true;
        Pa_StartStream(stream);
    }
}

void audioPlayer::Stop() {
    if (is_playing) {
        is_playing = false;
        Pa_StopStream(stream);
    }
}

bool audioPlayer::selectCurrentOutputDevice() {
#ifdef _WIN32
    std::system("cls");
#else
    std::system("clear");
#endif

    std::cout << "Type the number next to the output device you would like to use:\n";

    std::vector<Device> devices = getOutputDevices();
    for (size_t i = 0; i < devices.size(); i++) {

        std::cout << "#" << i << ": " << devices[i].name << "\n";

    }
    int num = -1;

    while (num == -1) {

        std::string num_str;
        std::cin >> num_str;

        try {
            num = std::stoi(num_str);
            if (num < 0 || num >= devices.size()) {
                num = -1;
            }
        }catch (std::invalid_argument& e) {
            std::cout << "Invalid number: " << num_str << "\n";
        }
    }

    std::cout << "Output device selected: " << devices[num].name << "\n";
    this->output.device = devices[num].index;

    auto info = Pa_GetDeviceInfo(output.device);

    if (is_playing) {
        Pa_StopStream(stream);
    }
    Pa_CloseStream(stream);

    PaError err = Pa_OpenStream(
    &stream,
    nullptr,
    &output,
    info->defaultSampleRate,
    256,
    paNoFlag,
    callback,
    this
    );

    if (err != paNoError)
    {
        std::cout << Pa_GetErrorText(err) << "\n";
        return false;
    }

    this->current_track.setSampleRate(info->defaultSampleRate);

    return true;
}

std::vector<audioPlayer::Device> audioPlayer::getOutputDevices() const {
    const int count = Pa_GetDeviceCount();
    std::vector<Device> devices;

    if (!init) {
        std::cout << "audioPlayer hasn't been Initialized yet!\n";
        return devices;
    }

    if (count < 0)
    {
        std::cout << "PortAudio error: "
                  << Pa_GetErrorText(count)
                  << "\n";
        return devices;
    }

    for (int i = 0; i < count; i++) {
        const PaDeviceInfo* info = Pa_GetDeviceInfo(i);
        if (info->maxOutputChannels > 0) {
            devices.emplace_back(Device(info->name,i));
        }
    }

    return devices;
}
