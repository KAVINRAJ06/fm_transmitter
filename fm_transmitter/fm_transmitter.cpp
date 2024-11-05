#include "transmitter.hpp"
#include <iostream>
#include <csignal>
#include <unistd.h>

std::mutex mtx;
bool enable = true;
Transmitter *transmitter = nullptr;

void sigIntHandler(int sigNum)
{
    if (transmitter) {
        std::cout << "Stopping..." << std::endl;
        transmitter->Stop();
        enable = false;
    }
}

int main(int argc, char** argv)
{
    float frequency = 100.f, bandwidth = 200.f;
    uint16_t dmaChannel = 0;
    bool showUsage = true, loop = false;
    int opt, filesOffset;

    while ((opt = getopt(argc, argv, "rf:d:b:v")) != -1) {
        switch (opt) {
            case 'r':
                loop = true;
                break;
            case 'f':
                frequency = std::stof(optarg);
                break;
            case 'd':
                dmaChannel = std::stoi(optarg);
                break;
            case 'b':
                bandwidth = std::stof(optarg);
                break;
            case 'v':
                std::cout << EXECUTABLE << " version: " << VERSION << std::endl;
                return 0;
        }
    }
    if (optind < argc) {
        filesOffset = optind;
        showUsage = false;
    }
    if (showUsage) {
        std::cout << "Usage: " << EXECUTABLE << " [-f <frequency>] [-b <bandwidth>] [-d <dma_channel>] [-r] <file>" << std::endl;
        return 0;
    }

    int result = EXIT_SUCCESS;

    std::signal(SIGINT, sigIntHandler);
    std::signal(SIGTERM, sigIntHandler);

    try {
        transmitter = new Transmitter();
        std::cout << "Broadcasting at " << frequency << " MHz with "
            << bandwidth << " kHz bandwidth" << std::endl;
        do {
            std::string filename = argv[optind++];
            if ((optind == argc) && loop) {
                optind = filesOffset;
            }
            WaveReader reader(filename != "-" ? filename : std::string(), enable, mtx);
            WaveHeader header = reader.GetHeader();
            std::cout << "Playing: " << reader.GetFilename() << ", "
                << header.sampleRate << " Hz, "
                << header.bitsPerSample << " bits, "
                << ((header.channels > 0x01) ? "stereo" : "mono") << std::endl;
            transmitter->Transmit(reader, frequency, bandwidth, dmaChannel, optind < argc);
        } while (enable && (optind < argc));
    } catch (std::exception &catched) {
        std::cout << "Error: " << catched.what() << std::endl;
        result = EXIT_FAILURE;
    }
    if (transmitter) {
        auto temp = transmitter;
        transmitter = nullptr;
        delete temp;
    }

    return result;
}
