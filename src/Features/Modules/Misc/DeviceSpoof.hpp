#pragma once
//
// Created by vastrakai on 7/3/2024.
//

#include <Features/Modules/Module.hpp>


class DeviceSpoof : public ModuleBase<DeviceSpoof>
{
public:
    BoolSetting mSpoofMboard = BoolSetting("Spoof Mboard", "Whether or not to spoof your motherboard", true);

    DeviceSpoof() : ModuleBase("DeviceSpoof", "Spoofs your device id, useful for bypassing bans", ModuleCategory::Misc, 0, true)
    {
        addSetting(&mSpoofMboard);

        mNames = {
            {Lowercase, "devicespoof"},
            {LowercaseSpaced, "device spoof"},
            {Normal, "DeviceSpoof"},
            {NormalSpaced, "Device Spoof"}
        };
    }

    static inline byte originalData[7];
    static inline byte patch[] = {0x48, 0xBA, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 };
    static inline void* patchPtr = nullptr;
    static inline std::string DeviceModel;

    void inject();
    void eject();
    void spoofMboard();

    void onInit() override;
    void onEnable() override;
    void onDisable() override;
    void onConnectionRequestEvent(class ConnectionRequestEvent& event);
};