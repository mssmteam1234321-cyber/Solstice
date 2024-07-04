//
// Created by vastrakai on 7/3/2024.
//

#include "DeviceSpoof.hpp"

#include <Features/FeatureManager.hpp>
#include <Features/Events/ConnectionRequestEvent.hpp>
#include <Utils/StringUtils.hpp>

void DeviceSpoof::onEnable()
{
    gFeatureManager->mDispatcher->listen<ConnectionRequestEvent, &DeviceSpoof::onConnectionRequestEvent>(this);
}

void DeviceSpoof::onDisable()
{
    gFeatureManager->mDispatcher->deafen<ConnectionRequestEvent, &DeviceSpoof::onConnectionRequestEvent>(this);
}

void DeviceSpoof::onConnectionRequestEvent(ConnectionRequestEvent& event)
{
    // Randomize the client random id (type: int64)
    event.mClientRandomId = std::rand();
    // Randomize the device id (type: string, also uuidv4)
    std::string deviceId = StringUtils::generateUUID();
    *event.mDeviceId = deviceId;
    *event.mSkinId = "Custom" + deviceId;
    *event.mSelfSignedId = StringUtils::generateUUID();
}
