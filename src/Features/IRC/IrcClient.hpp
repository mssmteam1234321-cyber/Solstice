#pragma once
//
// Created by vastrakai on 8/24/2024.
//
#include <winsock2.h>
#include <ws2tcpip.h>
#include <Features/Events/BaseTickEvent.hpp>
#include <Features/Events/PacketInEvent.hpp>
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
    DiscordMessage,
    ConnectedUsers
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

    [[nodiscard]] nlohmann::json serialize() const override {
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

    [[nodiscard]] nlohmann::json serialize() const override {
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

    [[nodiscard]] nlohmann::json serialize() const override {
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

    [[nodiscard]] nlohmann::json serialize() const override {
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

    [[nodiscard]] nlohmann::json serialize() const override {
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

    [[nodiscard]] nlohmann::json serialize() const override {
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

// Derived class for a "ConnectedUsers" packet.
// This packet contains a dictionary of all connected users and their respective player names, if available.

/*
public struct ConnectedIrcUser
{
    public string Username;
    public string Xuid;
    public string PlayerName;

    public ConnectedIrcUser(string username, string xuid, string playerName)
    {
        Username = username;
        Xuid = xuid;
        PlayerName = playerName;
    }

    public JObject Serialize()
    {
        JObject j = new();
        j["username"] = Username;
        j["xuid"] = Xuid;
        j["playerName"] = PlayerName;
        return j;
    }

    public void Deserialize(JObject j)
    {
        Username = j["username"]?.ToString() ?? "";
        Xuid = j["xuid"]?.ToString() ?? "";
        PlayerName = j["playerName"]?.ToString() ?? "";
    }
}
public class IrcConnectedUsersPacket : IrcPacket
{
    public ConnectedIrcUser[] Users { get; set; }

    public IrcConnectedUsersPacket()
    {
        Type = IrcPacketType.ConnectedUsers;
    }

    public override JObject Serialize()
    {
        JObject j = base.Serialize();
        JArray users = new();
        foreach (ConnectedIrcUser user in Users)
        {
            users.Add(user.Serialize());
        }
        j["users"] = users;
        return j;
    }

    public override void Deserialize(JObject j)
    {
        base.Deserialize(j);
        JArray users = j["users"] as JArray;
        Users = new ConnectedIrcUser[users.Count];
        for (int i = 0; i < users.Count; i++)
        {
            ConnectedIrcUser user = new();
            user.Deserialize(users[i] as JObject);
            Users[i] = user;
        }
    }
}*/

struct ConnectedIrcUser {
    std::string username;
    std::string xuid;
    std::string playerName;

    ConnectedIrcUser(std::string username, std::string xuid, std::string playerName) : username(username), xuid(xuid), playerName(playerName) {}

    nlohmann::json serialize() const {
        nlohmann::json j;
        j["username"] = username;
        j["xuid"] = xuid;
        j["playerName"] = playerName;
        return j;
    }

    void deserialize(const nlohmann::json& j) {
        username = j.at("username").get<std::string>();
        xuid = j.at("xuid").get<std::string>();
        playerName = j.at("playerName").get<std::string>();
    }
};

class IrcConnectedUsersPacket : public IrcPacket {
public:
    std::vector<ConnectedIrcUser> users;

    IrcConnectedUsersPacket() : IrcPacket(IrcPacketType::ConnectedUsers) {}

    [[nodiscard]] nlohmann::json serialize() const override {
        nlohmann::json j;
        j["type"] = "ConnectedUsers";
        nlohmann::json userArray = nlohmann::json::array();
        for (const auto& user : users) {
            userArray.push_back(user.serialize());
        }
        j["users"] = userArray;
        return j;
    }

    void deserialize(const nlohmann::json& j) override {
        for (const auto& user : j.at("users")) {
            ConnectedIrcUser connectedUser("", "", "");
            connectedUser.deserialize(user);
            users.push_back(connectedUser);
        }
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

    // this is because the mConnectedUsers map WILL be accessed from multiple threads
    std::mutex mConnectedUsersMutex;
    std::vector<ConnectedIrcUser> mConnectedUsers;

    bool mShowNamesInChat = false;

    // copied because we need to access it from multiple threads
    std::vector<ConnectedIrcUser> getConnectedUsers();
    void setConnectedUsers(const std::vector<ConnectedIrcUser>& users);

    IrcClient();
    ~IrcClient();

    std::string getHwid();

    // Minecraft events
    void onBaseTickEvent(BaseTickEvent& event);
    void onPacketInEvent(PacketInEvent& event);
    void onPacketOutEvent(PacketOutEvent& event);

    // IRC functions
    void sendIdentifySelf();
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

    static bool setShowNamesInChat(bool showNamesInChat);
    static void init();
    static void deinit();
    static void disconnectCallback();
    static void requestListUsers();
    static void requestChangeUsername(std::string username);
    static void sendMessage(std::string& message);
    static bool isConnected();
};