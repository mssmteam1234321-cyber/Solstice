//
// Created by vastrakai on 7/5/2024.
//

#include "AutoQueue.hpp"

#include <Features/FeatureManager.hpp>
#include <Features/Events/BaseTickEvent.hpp>
#include <Features/Events/PacketInEvent.hpp>
#include <SDK/Minecraft/ClientInstance.hpp>
#include <Utils/MiscUtils/ColorUtils.hpp>
#include <Utils/MiscUtils/CommandUtils.hpp>
#include <SDK/Minecraft/Network/Packets/Packet.hpp>
#include <SDK/Minecraft/Network/Packets/TextPacket.hpp>
#include <Utils/StringUtils.hpp>
#include <Utils/GameUtils/ChatUtils.hpp>
#include <Utils/MiscUtils/NotifyUtils.hpp>

void AutoQueue::queueForGame()
{
    mQueuedCommands[NOW] = "/q " + mLastGame;
}


void AutoQueue::onEnable()
{
    gFeatureManager->mDispatcher->listen<PacketInEvent, &AutoQueue::onPacketInEvent>(this);
    gFeatureManager->mDispatcher->listen<BaseTickEvent, &AutoQueue::onBaseTickEvent>(this);
}

void AutoQueue::onDisable()
{
    gFeatureManager->mDispatcher->deafen<PacketInEvent, &AutoQueue::onPacketInEvent>(this);
    gFeatureManager->mDispatcher->deafen<BaseTickEvent, &AutoQueue::onBaseTickEvent>(this);
}

void AutoQueue::onBaseTickEvent(BaseTickEvent& event)
{
    uint64_t now = NOW;

    if(now - mLastQueueTime > mQueueDelay.mValue * 1000 && mQueueForGame) {
        mQueueForGame = false;
        queueForGame();
        if(mShowInChat.mValue) ChatUtils::displayClientMessage("§bQueued for §6" + mLastGame + "§b!");
    }

    for (auto const& [_, val] : mQueuedCommands)
    {
        if (now - mLastCommandExecuted < 1000) continue;

        CommandUtils::executeCommand(val);
        mLastCommandExecuted = now;

        mQueuedCommands.erase(_);
        break;
    }
}

void AutoQueue::onPacketInEvent(PacketInEvent& event)
{
    auto player = ClientInstance::get()->getLocalPlayer();
    if (!player) return;

    if (event.mPacket->getId() == PacketID::ChangeDimension)
    {
        static int64_t lastCommandOutput = 0;
        if (!mQueueForGame && abs(NOW - lastCommandOutput) > 2000)
        {
            mQueuedCommands[NOW] = "/connection";
            lastCommandOutput = NOW;
        }

        return;
    }

    if (event.mPacket->getId() == PacketID::Text) {
        auto tp = event.getPacket<TextPacket>();
        if (tp->mMessage == "§c§l» §r§c§lGame OVER!" && mQueueOnGameEnd) {
            mQueueForGame = true;
            mLastQueueTime = NOW;
            NotifyUtils::Notify("Queuing for " + mLastGame + "!", 1.f + (mQueueDelay.mValue), Notification::Type::Info);
            return;
        }

        static std::vector<std::string> ignored = {
            "You are connected to public IP",
            "You are connected to internal IP",
            "You are connected to proxy"
        };

        if (StringUtils::containsAnyIgnoreCase(tp->mMessage, ignored)) {
            event.mCancelled = true;
            return;
        }

        if (StringUtils::containsIgnoreCase(tp->mMessage, "You are connected to server "))
        {
            // You are connected to server name HUB-SKY9
            std::string game = tp->mMessage.substr(tp->mMessage.find("server name ") + 12, tp->mMessage.length() - 1);
            if (std::ranges::count_if(game, ::isdigit) > 3)
            {
                event.mCancelled = true;
                return;
            }
            std::erase_if(game, ::isdigit);
            game = StringUtils::toLower(game);

            if (!StringUtils::equalsIgnoreCase(mLastGame, game) && mShowInChat) {
                ChatUtils::displayClientMessage("§bConnected to §6" + game + "§b.");
            }
            mLastGame = game;
            event.mCancelled = true;
            return;
        }
    }
}