#pragma once
#include <string>
#include <Utils/MemUtils.hpp>
//
// Created by vastrakai on 6/24/2024.
//


class ClientInstance {
public:
    //CLASS_FIELD(class MinecraftGame*, mcGame, 0xC8);

    static ClientInstance* get();

    class MinecraftSim* getMinecraftSim();
    class LevelRenderer* getLevelRenderer();
    class PacketSender* getPacketSender();
    class GuiData* getGuiData();

    class MinecraftGame* getMinecraftGame();
    class Actor* getLocalPlayer();
    class BlockSource* getBlockSource();
    class Options* getOptions();
    std::string getScreenName();
    void setDisableInput(bool disable);
    void getMouseGrabbed();
    void grabMouse();
    void releaseMouse();
    void playUi(const std::string& soundName, float volume, float pitch);
};