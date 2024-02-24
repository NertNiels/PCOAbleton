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

    std::vector<double> _buffer;
    
    void create_asio_buffers();
    void init();
    void init_driver();
    void start();
    void stop();
    void createAsioBuffers();
    void audio_callback_engine(const std::chrono::microseconds hostTime, double currentSample, const std::size_t numSamples);
    void set_buffer_size(size_t size);
    void set_sample_rate(double samplerate);
    std::chrono::microseconds calculateTimeAtSamplePosition(ASIOTime *time);
    ASIOCallbacks _asio_callbacks;

    std::chrono::microseconds lastHostTime;
    void test_BEEP(const std::chrono::microseconds beginHostTime, double& currentSample, const std::size_t numSamples) {
        // Metronome frequencies
        static const double highTone = 1567.98;
        static const double lowTone = 1000;

        // auto beginHostTime2 = beginHostTime;
        // std::cout << std::this_thread::get_id << ": " << beginHostTime.count() << std::endl;

        const auto microsPerSample = 1e6 / _sampleRate;
        std::chrono::microseconds hostTime;
        for(size_t i = 0; i < numSamples; i++) {
            double amplitude = 0;
            hostTime = beginHostTime + std::chrono::microseconds(llround(static_cast<double>(i) * microsPerSample));
            // const auto lastSampleHostTime = hostTime - std::chrono::microseconds(llround(microsPerSample));
            // const auto seconds = std::chrono::duration_cast<std::chrono::duration<double>>(hostTime);
            // const auto seconds = std::chrono::duration_cast<std::chrono::duration<double>>(hostTime);
            // amplitude = cos(4.*M_PI*(currentSample/512));
            amplitude = cos(2.*M_PI*hostTime.count()*0.001);
            // std::cout << hostTime.count() << std::endl;
            _buffer[i] = amplitude;
            currentSample++;
        }

        // std::cout << std::chrono::duration_cast<std::chrono::microseconds>
        //                         (std::chrono::system_clock::now().time_since_epoch()).count() << std::endl;
        
        lastHostTime = hostTime;
    }

    long current_time_micros() {
        static const auto nano_start_time = std::chrono::high_resolution_clock::now();
        return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now()-nano_start_time).count();
    }

    std::pair<long, long> lineairRegression() {
        auto begin = _sampleTimePoints.begin();
        auto end = _sampleTimePoints.end();

        using NumberType = double;

        NumberType sumX = 0.0;
        NumberType sumXX = 0.0;
        NumberType sumXY = 0.0;
        NumberType sumY = 0.0;
        for (auto i = begin; i != end; ++i)
        {
            sumX += i->first;
            sumXX += i->first * i->first;
            sumXY += i->first * i->second;
            sumY += i->second;
        }

        const NumberType numPoints = static_cast<NumberType>(distance(begin, end));
        const NumberType denominator = numPoints * sumXX - sumX * sumX;
        const NumberType slope = denominator == NumberType{0}
                                    ? NumberType{0}
                                    : (numPoints * sumXY - sumX * sumY) / denominator;
        const NumberType intercept = (sumY - slope * sumX) / numPoints;

        return std::make_pair(slope, intercept);
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