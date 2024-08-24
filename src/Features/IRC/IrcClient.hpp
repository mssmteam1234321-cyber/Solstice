#pragma once
//
// Created by vastrakai on 8/24/2024.
//
#include <winsock2.h>
#include <ws2tcpip.h>
#include <Features/Events/BaseTickEvent.hpp>
#include <Features/Events/PacketOutEvent.hpp>
#include <SDK/Minecraft/Network/Packets/TextPacket.hpp>

enum class IrcPacketType {
    Join,
    Leave,
    Message,
    QueryName,
    ListUsers
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

    IrcJoinPacket() : IrcPacket(IrcPacketType::Join) {}

    nlohmann::json serialize() const override {
        nlohmann::json j;
        j["type"] = "Join";
        j["user"] = user;
        return j;
    }

    void deserialize(const nlohmann::json& j) override {
        user = j.at("user").get<std::string>();
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

class IrcClient {
public:
    constexpr static const char* mServer = "irc.solstice.works";
    constexpr static int mPort = 27335;

    SOCKET mSocket;
    char mBuffer[1024];

    std::thread mReceiveThread;

    std::string mCurrentUsername = "";

    IrcClient();
    ~IrcClient();

    void queryName();
    bool isConnected();
    bool connectToServer();
    void disconnect();
    void onPacketOutEvent(PacketOutEvent& event);
    void sendPacket(const IrcPacket* packet);
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

    static void init();
    static void deinit();
    static void requestListUsers();
    static void requestChangeUsername(std::string username);
    static void sendMessage(std::string& message);
    static bool isConnected();
};