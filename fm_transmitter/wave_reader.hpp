#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <mutex>

#define WAVE_FORMAT_PCM 0x0001

struct WaveHeader
{
    uint8_t chunkID[4];
    uint32_t chunkSize;
    uint8_t format[4];
    uint8_t subchunk1ID[4];
    uint32_t subchunk1Size;
    uint16_t audioFormat;
    uint16_t channels;
    uint32_t sampleRate;
    uint32_t byteRate;
    uint16_t blockAlign;
    uint16_t bitsPerSample;
    uint8_t subchunk2ID[4];
    uint32_t subchunk2Size;
};

class Sample
{
public:
    Sample(uint8_t *data, unsigned channels, unsigned bitsPerChannel);
    float GetMonoValue() const;
protected:
    float value;
};

class WaveReader
{
    public:
        WaveReader(const std::string &filename, bool &enable, std::mutex &mtx);
        virtual ~WaveReader();
        WaveReader(const WaveReader &) = delete;
        WaveReader(WaveReader &&) = delete;
        WaveReader &operator=(const WaveReader &) = delete;
        std::string GetFilename() const;
        const WaveHeader &GetHeader() const;
        std::vector<Sample> GetSamples(unsigned quantity, bool &enable, std::mutex &mtx);
        bool SetSampleOffset(unsigned offset);
    private:
        std::vector<uint8_t> ReadData(unsigned bytesToRead, bool headerBytes, bool &enable, std::mutex &mtx);

        std::string filename;
        WaveHeader header;
        unsigned dataOffset, headerOffset, currentDataOffset;
        int fileDescriptor;
};
