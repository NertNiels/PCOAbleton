#include "audio_handler.h"
#undef max

using namespace audio_handler;

audio_engine::audio_engine() {
    init();
    start();
    engine = this;
}

audio_engine::~audio_engine() {
    stop();
    ASIODisposeBuffers();
    ASIOExit();
    if(asioDrivers != nullptr) asioDrivers->removeCurrentDriver();
}

void audio_engine::start() {
    ASIOError result = ASIOStart();
    if(result != ASE_OK) asio_fatalError(result, "ASIOStart");
}

void audio_engine::stop() {
    ASIOError result = ASIOStop();
    if(result != ASE_OK) asio_fatalError(result, "ASIOStop");
}

void audio_engine::init() {
    if(!loadAsioDriver("ASIO4ALL v2")) {
        std::cerr << "Failed to open ASIO driver " << "ASIO4ALL v2" << std::endl;
        throw 1;
    }

    ASIOError result = ASIOInit(&_driver_info);
    if(result != ASE_OK) asio_fatalError(result, "ASIOInit");

    std::cout << "ASIOInit Succeeded" << std::endl;
    std::cout << " - name: " << _driver_info.name << std::endl;
    std::cout << " - asio version: " << _driver_info.asioVersion << std::endl;
    std::cout << " - driver version: " << _driver_info.driverVersion << std::endl;
    
    init_driver();

    ASIOCallbacks *callbacks = &(_asio_callbacks);
    callbacks->bufferSwitch = buffer_switch;
    callbacks->bufferSwitchTimeInfo = buffer_switch_time_info;
    createAsioBuffers();
}

void audio_engine::init_driver() {
    ASIOError result = ASIOGetChannels(&_inputChannels, &_outputChannels);
    if(result != ASE_OK) asio_fatalError(result, "ASIOGetChannels");
    std::cout << " - input channels: " << _inputChannels << std::endl;
    std::cout << " - output channels: " << _outputChannels << std::endl;
    
    long minSize, maxSize, granularity;
    result = ASIOGetBufferSize(&minSize, &maxSize, &_preferredSize, &granularity);
    if(result != ASE_OK) asio_fatalError(result, "ASIOGetBufferSize");
    std::cout << " - minimum buffer size: " << minSize << std::endl;
    std::cout << " - maximum buffer size: " << maxSize << std::endl;
    std::cout << " - preferred buffer size: " << _preferredSize << std::endl;
    std::cout << " - granularity: " << granularity << std::endl;
    
    result = ASIOGetSampleRate(&_sampleRate);
    if(result != ASE_OK) asio_fatalError(result, "ASIOGetSampleRate");
    std::cout << " - sample rate: " << _sampleRate << "hz" << std::endl;

    _outputReady = ASIOOutputReady()== ASE_OK;
    std::cout << " - output ready: " << (_outputReady ? "enabled" : "disabled") << std::endl;
}

void audio_engine::audio_callback(ASIOTime *timeInfo, long index) {
    auto hostTime = std::chrono::microseconds(0);
    const auto bufferBeginAtOutput = hostTime + _output_latency.load();
    
    ASIOBufferInfo *bufferInfos = _bufferInfos;
    const long numSamples = _preferredSize;
    const long numChannels = _numBuffers;
    const double maxAmp = std::numeric_limits<int>::max();

    audio_callback_engine(bufferBeginAtOutput, numSamples);

    for (long i = 0; i < numSamples; ++i)
    {
        for (long j = 0; j < numChannels; ++j)
        {
        int* buffer = static_cast<int*>(bufferInfos[j].buffers[index]);
        buffer[i] = static_cast<int>(_buffer[i] * maxAmp);
        }
    }
    if(_outputReady) ASIOOutputReady();
}

void audio_engine::audio_callback_engine(const std::chrono::microseconds hostTime, const std::size_t numSamples) {
    std::fill(_buffer.begin(), _buffer.end(), 0);
    // renderMetronomeIntoBuffer(); TODO: Make function
}

void audio_engine::createAsioBuffers() {
    ASIOBufferInfo *bufferInfo = _bufferInfos;
    _numBuffers = 0;

    int numInputBuffers = _inputChannels;
    for (long i = 0; i < numInputBuffers; ++i, ++bufferInfo)
    {
        bufferInfo->isInput = ASIOTrue;
        bufferInfo->channelNum = i;
        bufferInfo->buffers[0] = bufferInfo->buffers[1] = nullptr;
    }

    int numOutputBuffers = _outputChannels;
    for (long i = 0; i < numOutputBuffers; i++, bufferInfo++)
    {
        bufferInfo->isInput = ASIOFalse;
        bufferInfo->channelNum = i;
        bufferInfo->buffers[0] = bufferInfo->buffers[1] = nullptr;
    }

    _numBuffers = numInputBuffers + numOutputBuffers;
    ASIOError result = ASIOCreateBuffers(_bufferInfos, _numBuffers, _preferredSize, &(_asio_callbacks));
    if(result != ASE_OK) asio_fatalError(result, "ASIOCreateBuffers");

    for(long i = 0; i < _numBuffers; i++) {
        _channelInfos[i].channel = _bufferInfos[i].channelNum;
        _channelInfos[i].isInput = _bufferInfos[i].isInput;
        
        result = ASIOGetChannelInfo(&_channelInfos[i]);
        if(result != ASE_OK) asio_fatalError(result, "ASIOGetChannelInfo");

        std::cout << "Channel:" << std::endl;
        std::cout << " - channel: " << i << std::endl;
        std::cout << " - type: " << (_bufferInfos[i].isInput ? "input" : "output") << std::endl;
        std::cout << " - sample type: " << _channelInfos[i].type << std::endl;

        if(_channelInfos[i].type != ASIOSTInt32LSB) asio_fatalError(ASE_OK, "Unsupported sample type");
    }

    long inputLatency, outputLatency;
    result = ASIOGetLatencies(&inputLatency, &outputLatency);
    if(result != ASE_OK) asio_fatalError(result, "ASIOGetLatencies");
    std::cout << "Latencies:" << std::endl;
    std::cout << " - input: " << inputLatency << "usec" << std::endl;
    std::cout << " - output: " << outputLatency << "usec" << std::endl;

    const double bufferSize = _preferredSize / _sampleRate;
    auto outputLatencyMicros =
        std::chrono::microseconds(llround(outputLatency / _sampleRate));
    outputLatencyMicros += std::chrono::microseconds(llround(1.0e6 * bufferSize));

    _output_latency.store(outputLatencyMicros);
    std::cout << " - total:" << outputLatencyMicros.count() << "usec" << std::endl;
}