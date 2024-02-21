
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
        ASIOBufferInfo _bufferInfos[2 + 2];
        ASIOChannelInfo _channelInfos[2 + 2];
        std::atomic<std::chrono::microseconds> _output_latency;

        std::vector<double> _buffer;
        
        void create_asio_buffers();
        void init();
        void init_driver();
        void start();
        void stop();
        void createAsioBuffers();
        void audio_callback_engine(const std::chrono::microseconds hostTime, const std::size_t numSamples);
        ASIOCallbacks _asio_callbacks;
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
}