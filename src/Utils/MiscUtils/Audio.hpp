#pragma once
#include <SDL2/SDL.h>
#include <string>
#include <span>
#include <iostream>
#include "../Resource.hpp"
#include <thread>

#include "spdlog/spdlog.h"

class Audio {
public:
    Audio() {
        if (SDL_Init(SDL_INIT_AUDIO) < 0) {
            std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        }
    }

    ~Audio() {
        SDL_Quit();
    }

    // variables
    struct AudioData {
        Uint8* pos;
        Uint32 length;
        bool isPlaying;
        SDL_AudioDeviceID deviceId;
    };

    // functions
    double howManySeconds(const Resource& resource) {
        SDL_AudioSpec spec;
        uint32_t audioLen;
        uint8_t *audioBuf;
        double seconds = 0.0;

        if(SDL_LoadWAV_RW(SDL_RWFromConstMem(resource.data(), resource.size()), 1, &spec, &audioBuf, &audioLen) != nullptr) {
            SDL_FreeWAV(audioBuf);
            uint32_t sampleSize = SDL_AUDIO_BITSIZE(spec.format) / 8;
            uint32_t sampleCount = audioLen / sampleSize;
            uint32_t sampleLen = 0;
            if(spec.channels) {
                sampleLen = sampleCount / spec.channels;
            } else {
                sampleLen = sampleCount;
            }
            seconds = (double)sampleLen / (double)spec.freq;
        } else {
            spdlog::error("Failed to load audio: {}", SDL_GetError());
        }

        return seconds;
    }

    int Play(const Resource& resource, float volume, bool ShouldLoop) {
        SDL_AudioSpec wavSpec;
        Uint8* wavStart;
        Uint32 wavLength;

        if (SDL_LoadWAV_RW(SDL_RWFromConstMem(resource.data(), resource.size()), 1, &wavSpec, &wavStart, &wavLength) == nullptr) {
            spdlog::error("Failed to load audio: {}", SDL_GetError());
            return -1;
        }

        // validate wavLength
        if (wavLength == 0) {
            spdlog::error("Failed to load audio: wavLength is 0");
            return -1;
        }

        SDL_AudioDeviceID deviceId = SDL_OpenAudioDevice(nullptr, 0, &wavSpec, nullptr, 0);
        if (deviceId == 0) {
            spdlog::error("Failed to open audio: {}", SDL_GetError());
            return -1;
        }

        AudioData audioData = {wavStart, wavLength, true, deviceId};
        SDL_PauseAudioDevice(deviceId, 0);

        uint64_t duration = howManySeconds(resource) * 1000;


        if (ShouldLoop) {
            std::thread([audioData, wavSpec, volume]() {
                while (audioData.isPlaying) {
                    SDL_QueueAudio(audioData.deviceId, audioData.pos, audioData.length);
                    SDL_PauseAudioDevice(audioData.deviceId, 0);
                    SDL_Delay(wavSpec.samples / wavSpec.freq * 1000);
                }
            }).detach();
        } else {
            SDL_QueueAudio(deviceId, wavStart, wavLength);
            SDL_PauseAudioDevice(deviceId, 0);

            std::thread([deviceId, duration]() {
                SDL_Delay(duration);
                SDL_CloseAudioDevice(deviceId);
            }).detach();
        }

        return 0;
    }
};
