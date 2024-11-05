#pragma once

#include "wave_reader.hpp"
#include <condition_variable>
#include <thread>

class ClockOutput;

class Transmitter
{
    public:
        Transmitter();
        virtual ~Transmitter();
        Transmitter(const Transmitter &) = delete;
        Transmitter(Transmitter &&) = delete;
        Transmitter &operator=(const Transmitter &) = delete;
        void Transmit(WaveReader &reader, float frequency, float bandwidth, unsigned dmaChannel, bool preserveCarrier);
        void Stop();
    private:
        void TxViaCpu(WaveReader &reader, unsigned sampleRate, unsigned bufferSize, unsigned clockDivisor, unsigned divisorRange);
        void TxViaDma(WaveReader &reader, unsigned sampleRate, unsigned bufferSize, unsigned clockDivisor, unsigned divisorRange, unsigned dmaChannel);
        void CpuTxThread(unsigned sampleRate, unsigned clockDivisor, unsigned divisorRange, unsigned *sampleOffset, std::vector<Sample> *samples, bool *stop);

        std::condition_variable cv;
        std::thread txThread;
        ClockOutput *output;
        std::mutex mtx;
        bool enable;
};
