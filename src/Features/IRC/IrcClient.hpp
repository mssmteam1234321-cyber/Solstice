#pragma once
//
// Created by vastrakai on 8/24/2024.
//
#include <winsock2.h>
#include <ws2tcpip.h>
#include <Features/Events/BaseTickEvent.hpp>
#include <Features/Events/PacketOutEvent.hpp>
#include <SDK/Minecraft/Network/Packets/TextPacket.hpp>
#include <winrt/base.h>
#include <winrt/windows.foundation.h>
#include <winrt/windows.networking.sockets.h>
#include <winrt/Windows.Data.Xml.Dom.h>
#include <winrt/Windows.UI.Notifications.h>
#include <winrt/windows.storage.streams.h>

enum class IrcPacketType {
    Join,
    Leave,
    Message,
    QueryName,
    ListUsers,
    Ping,
    IdentifySelf,
};

// Base class for all IRC packets
class IrcPacket {
public:
    IrcPacketType mType;

    IrcPacket(IrcPacketType type) : mType(type) {}
    virtual ~IrcPacket() = default;

    virtual nlohmann::json serialize() const = 0;
    virtual void deserialize(const nlohmann::json& j) = 0;
};

// Derived class for a "Message" packet
class IrcMessagePacket : public IrcPacket {
public:
    std::string message;

    IrcMessagePacket() : IrcPacket(IrcPacketType::Message) {}

    nlohmann::json serialize() const override {
        nlohmann::json j;
        j["type"] = "Message";
        j["message"] = message;
        return j;
    }

    void deserialize(const nlohmann::json& j) override {
        message = j.at("message").get<std::string>();
    }
};

class IrcJoinPacket : public IrcPacket {
public:
    std::string user = "";
    std::string client = "";

    IrcJoinPacket() : IrcPacket(IrcPacketType::Join) {}

    nlohmann::json serialize() const override {
        nlohmann::json j;
        j["type"] = "Join";
        j["user"] = user;
        j["client"] = client;
        return j;
    }

    void deserialize(const nlohmann::json& j) override {
        user = j.at("user").get<std::string>();
        client = j.at("client").get<std::string>();
    }
};

class IrcLeavePacket : public IrcPacket {
public:
    IrcLeavePacket() : IrcPacket(IrcPacketType::Leave) {}

    nlohmann::json serialize() const override {
        nlohmann::json j;
        j["type"] = "Leave";
        return j;
    }

    void deserialize(const nlohmann::json& j) override {
        // Nothing to deserialize, this packet simply disconnects the user
    }
};

class IrcQueryNamePacket : public IrcPacket {
public:
    std::string user = "";

    IrcQueryNamePacket() : IrcPacket(IrcPacketType::QueryName) {}

    nlohmann::json serialize() const override {
        nlohmann::json j;
        j["type"] = "QueryName";
        j["user"] = user;
        return j;
    }

    void deserialize(const nlohmann::json& j) override {
        user = j.at("user").get<std::string>();
    }
};

class IrcListUsersPacket : public IrcPacket {
public:
    IrcListUsersPacket() : IrcPacket(IrcPacketType::ListUsers) {}

    nlohmann::json serialize() const override {
        nlohmann::json j;
        j["type"] = "ListUsers";
        return j;
    }

    void deserialize(const nlohmann::json& j) override {
        // Nothing to deserialize, this packet simply lists all users
    }
};

// #define NOW std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count()
class IrcPingPacket : public IrcPacket {
public:
    uint64_t timestamp;

    IrcPingPacket() : IrcPacket(IrcPacketType::Ping) {}

    nlohmann::json serialize() const override {
        nlohmann::json j;
        j["type"] = "Ping";
        j["timestamp"] = timestamp;
        return j;
    }

    void deserialize(const nlohmann::json& j) override {
        timestamp = j.at("timestamp").get<uint64_t>();
    }

    uint64_t getPingTime() {
        return NOW - timestamp;
    }
};

class IrcIdentifySelfPacket : public IrcPacket {
public:
    std::string xuid = "";
    std::string playerName = "";
    std::string hwid = "";

    IrcIdentifySelfPacket() : IrcPacket(IrcPacketType::Message) {}

    nlohmann::json serialize() const override {
        nlohmann::json j;
        j["type"] = "IdentifySelf";
        j["xuid"] = xuid;
        j["playerName"] = playerName;
        j["hwid"] = hwid;
        return j;
    }

    void deserialize(const nlohmann::json& j) override {
        playerName = j.at("playerName").get<std::string>();
        xuid = j.at("xuid").get<std::string>();
        hwid = j.at("hwid").get<std::string>();
    }
};

namespace Sockets = winrt::Windows::Networking::Sockets;
namespace Streams = winrt::Windows::Storage::Streams;

enum class ConnectionState {
    Disconnected,
    Connecting,
    Connected,
};

class IrcClient {
public:
    constexpr static const char* mServer = "irc.solstice.works";
    constexpr static int mPort = 27335;

    Sockets::MessageWebSocket mSocket = nullptr;
    Streams::DataWriter mWriter = nullptr;
    char mBuffer[1024];

    std::thread mReceiveThread;
    std::string mCurrentUsername = "";
    std::vector<std::string> mQueuedMessages;
    uint64_t mLastPing = 0;
    std::mutex mMutex;
    ConnectionState mConnectionState = ConnectionState::Disconnected;
    bool mIdentifyNeeded = true;

    IrcClient();
    ~IrcClient();

    std::string getHwid();

    void sendIdentifySelf();
    void onPacketOutEvent(PacketOutEvent& event);
    void onBaseTickEvent(BaseTickEvent& event);
    void displayMsg(std::string message);
    void queryName();
    bool isConnected();
    bool connectToServer();
    void disconnect();
    void sendPacket(const IrcPacket* packet);
    std::string getPreferredUsername();
    void receiveMessages();
    void sendMessage(std::string& message);
    void onMessageReceived(const char* message);
    void listUsers();
    void changeUsername(std::string username);
};

class IrcManager
{
public:
    static inline std::unique_ptr<IrcClient> mClient = nullptr;
    static inline uint64_t mLastConnectAttempt = 0;

    static void init();
    static void deinit();
    static void disconnectCallback();
    static void requestListUsers();
    static void requestChangeUsername(std::string username);
    static void sendMessage(std::string& message);
    static bool isConnected();
};