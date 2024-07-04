//
// Created by vastrakai on 7/4/2024.
//

#include "SoundUtils.hpp"

#include <Utils/Resource.hpp>
#include <Utils/Resources.hpp>
#include <iostream>
#include <xaudio2.h>
#include "Audio.hpp"

#include "spdlog/spdlog.h"


void SoundUtils::playSoundFromEmbeddedResource(std::string resourceName, float volume)
{
    static Audio audioManager = Audio();
    if (!ResourceLoader::Resources.contains(resourceName))
    {
        spdlog::error("Resource {} not found", resourceName);
        return;
    }
    Resource& soundResource = ResourceLoader::Resources[resourceName];
    audioManager.Play(soundResource, volume, false);
}

// use the reference code to play sound from embedded resource

