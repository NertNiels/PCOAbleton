#define _USE_MATH_DEFINES
#include <math.h>

#include <asiosys.h>
#include <asio.h>
#include <asiodrivers.h>
#include <iostream>
#include <atomic>
#include <chrono>
#include <vector>
#include <mutex>
#include <xutility>
#include <limits>
#include <ctime>
#include <windows.h>

#pragma comment( lib, "winmm.lib")

// Not declared in ASIO SDK's header files
extern AsioDrivers* asioDrivers;
bool loadAsioDriver(char* name);



namespace audio_handler {

static void asio_fatalError(const ASIOError result, const std::string& function)
{
    std::cerr << "Call to ASIO function " << function << " failed";
    if (result != ASE_OK)
    {
        std::cerr << " (ASIO error code " << result << ")";
    }
    std::cerr << std::endl;
    throw 1;
}

static double asioSamplesToDouble(const ASIOSamples& samples)
{
    return samples.lo + samples.hi * std::pow(2, 32);
}


class audio_engine {

public:
    audio_engine();
    ~audio_engine();
    void audio_callback(ASIOTime *timeInfo, long index);

    void (*callback_metronome) (const double& timeAtStart, const double& sampleAtStart, const size_t& numSamples, std::vector<double>* buffer, std::chrono::microseconds& latency);

private:
    ASIODriverInfo _driver_info;
    long _inputChannels;
    long _outputChannels;
    long _preferredSize;
    ASIOSampleRate _sampleRate;
    bool _outputReady;
    long _numBuffers;
    ASIOBufferInfo* _bufferInfos;
    ASIOChannelInfo* _channelInfos;
    std::atomic<std::chrono::microseconds> _output_latency;
    std::vector<std::pair<long, long>> _sampleTimePoints;
    double _microsPerSample;

    std::vector<double> _buffer;
    
    void create_asio_buffers();
    void init();
    void init_driver();
    void start();
    void stop();
    void createAsioBuffers();
    void audio_callback_engine(const double& hostTime, double& currentSample, const std::size_t& numSamples);
    void set_buffer_size(size_t size);
    void set_sample_rate(double samplerate);
    std::chrono::microseconds calculateTimeAtSamplePosition(ASIOTime *time);
    ASIOCallbacks _asio_callbacks;

    double current_time_on_sample(double& currentSample) {
        return currentSample * _microsPerSample;
    }
};

static audio_engine *engine;

static ASIOTime* buffer_switch_time_info(ASIOTime* timeInfo, long index, ASIOBool)
{
    if (engine) engine->audio_callback(timeInfo, index);
    return nullptr;
}

static void buffer_switch(long index, ASIOBool processNow) {
    ASIOTime timeInfo{};
    ASIOError result = ASIOGetSamplePosition(
        &timeInfo.timeInfo.samplePosition, &timeInfo.timeInfo.systemTime);
    if (result != ASE_OK) std::cerr << "ASIOGetSamplePosition failed with ASIO error: " << result << std::endl;
    else timeInfo.timeInfo.flags = kSystemTimeValid | kSamplePositionValid;
    buffer_switch_time_info(&timeInfo, index, processNow);
}

static void sample_rate_did_change(ASIOSampleRate sRate) {
    std::cout << "ASIO called method: sample_rate_did_change: " << sRate << "hz" << std::endl;
}

static long asio_message(long selector, long value, void* message, double* opt) {
    std::cout << "ASIO called method: asio_message: " << message << "hz" << std::endl;
    return 0;
}
}