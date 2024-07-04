#pragma once
//
// Created by vastrakai on 7/3/2024.
//

#include <Features/Modules/Module.hpp>


class DeviceSpoof : public ModuleBase<DeviceSpoof>
{
public:
    DeviceSpoof() : ModuleBase("DeviceSpoof", "Spoofs your device id, useful for bypassing bans", ModuleCategory::Misc, 0, true)
    {
        mNames = {
            {Lowercase, "devicespoof"},
            {LowercaseSpaced, "device spoof"},
            {Normal, "DeviceSpoof"},
            {NormalSpaced, "Device Spoof"}
        };
    }

    void onEnable() override;
    void onDisable() override;
    void onConnectionRequestEvent(class ConnectionRequestEvent& event);
};