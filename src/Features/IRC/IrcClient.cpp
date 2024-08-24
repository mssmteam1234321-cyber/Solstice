//
// Created by vastrakai on 8/24/2024.
//

#include "IrcClient.hpp"

#include <codecvt>
#include <utility>
#include <SDK/Minecraft/ClientInstance.hpp>


IrcClient::IrcClient()
{
    gFeatureManager->mDispatcher->listen<PacketOutEvent, &IrcClient::onPacketOutEvent>(this);
    gFeatureManager->mDispatcher->listen<BaseTickEvent, &IrcClient::onBaseTickEvent>(this);
}

IrcClient::~IrcClient()
{
    gFeatureManager->mDispatcher->deafen<PacketOutEvent, &IrcClient::onPacketOutEvent>(this);
    gFeatureManager->mDispatcher->deafen<BaseTickEvent, &IrcClient::onBaseTickEvent>(this);
}

std::string IrcClient::getHwid()
{
    return Solstice::sHWID;
}

void IrcClient::sendIdentifySelf()
{
    if (!isConnected())
    {
        spdlog::error("[irc] Cannot send packet, not connected to server");
        return;
    }

    auto player = ClientInstance::get()->getLocalPlayer();
    if (!player)
    {
        spdlog::error("[irc] Cannot send identify packet, player is null");
        return;
    }


    IrcIdentifySelfPacket packet;
    packet.hwid = getHwid();
    packet.xuid = "0"; // Temporary, until i impl getXuid
    packet.playerName = player->getLocalName();
    sendPacket(&packet);
    mIdentifyNeeded = false;
}

void IrcClient::queryName()
{
    if (!isConnected())
    {
        spdlog::error("[irc] Cannot send packet, not connected to server");
        return;
    }
    IrcQueryNamePacket packet;
    sendPacket(&packet);
}

bool IrcClient::isConnected()
{
    return mConnectionState == ConnectionState::Connected && mSocket != nullptr;
}

bool IrcClient::connectToServer()
{
    mLastPing = NOW;
    // If we are connecting, return false
    if (mConnectionState == ConnectionState::Connecting)
    {
        spdlog::error("[irc] Cannot connect to server, already connecting");
        return false;
    }
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        spdlog::error("[irc] WSAStartup failed with error: {}", WSAGetLastError());
        return false;
    }

    // Resolve the server address into a numeric address with dns
    // This is done by calling getaddrinfo() with the host and port
    std::string host = mServer;
    std::string port = std::to_string(mPort);
    addrinfo* result = nullptr;
    addrinfo hints;
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    if (getaddrinfo(host.c_str(), port.c_str(), &hints, &result) != 0) {
        spdlog::error("[irc] getaddrinfo failed with error: {}", WSAGetLastError());
        return false;
    }

    host = std::string(inet_ntoa(reinterpret_cast<sockaddr_in*>(result->ai_addr)->sin_addr));

    // use da winrt socket api
    Sockets::MessageWebSocket socket;
    mSocket = socket;
    mSocket.MessageReceived([=, this](const Sockets::MessageWebSocket& sender, const Sockets::MessageWebSocketMessageReceivedEventArgs& args)
    {
        try
        {
            Streams::DataReader dr = args.GetDataReader();
                std::wstring wmessage{ dr.ReadString(dr.UnconsumedBufferLength()) };
                std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
                std::string message = converter.to_bytes(wmessage);

            onMessageReceived(message.c_str());
        }
        catch (winrt::hresult_error const& ex)
        {
            spdlog::error("[irc] Error: {}", winrt::to_string(ex.message()));

            disconnect();
        } catch (const std::exception& ex)
        {
            spdlog::error("[irc] Error: {}", ex.what());
        } catch (...)
        {
            spdlog::error("[irc] Unknown error");
        }
    });
    mSocket.Closed([&](Sockets::IWebSocket sender, Sockets::WebSocketClosedEventArgs args) {
        spdlog::info("[irc] Disconnected from server");
        //ChatUtils::displayClientMessageRaw("§7[§dirc§7] §cConnection closed.");
        disconnect();
    });
    Streams::DataWriter writer = Streams::DataWriter(mSocket.OutputStream());
    mWriter = writer;
    mConnectionState = ConnectionState::Connecting;

    try
    {
        mSocket.ConnectAsync(winrt::Windows::Foundation::Uri(winrt::to_hstring("ws://" + host + ":" + port))).Completed([=](auto&&, auto&&)
        {
            mConnectionState = ConnectionState::Connected;
            spdlog::info("[irc] Connected to server");
            std::string username = getPreferredUsername();
            changeUsername(username);
            sendIdentifySelf();
        });
    } catch (winrt::hresult_error const& ex)
    {
        spdlog::error("[irc] Error: {}", winrt::to_string(ex.message()));
        disconnect();
        return false;
    } catch (const std::exception& ex)
    {
        spdlog::error("[irc] Error: {}", ex.what());
        disconnect();
        return false;
    } catch (...)
    {
        spdlog::error("[irc] Unknown error");
        disconnect();
        return false;
    }


    spdlog::info("[irc] Connected to server");

    // Start receiving messages in a separate thread
    //mReceiveThread = std::thread(&IrcClient::receiveMessages, this);


    return true;
}

void IrcClient::disconnect()
{
    if (!isConnected())
    {
        spdlog::error("[irc] Cannot disconnect, not connected to server");
        return;
    }

    mSocket.Close();

    mSocket = Sockets::MessageWebSocket();
    mWriter = Streams::DataWriter();

    spdlog::info("[irc] Disconnected from server");
    if (mConnectionState != ConnectionState::Disconnected)
        ChatUtils::displayClientMessageRaw("§7[§dirc§7] §cDisconnected from IRC.");
    mConnectionState = ConnectionState::Disconnected;
    IrcManager::mLastConnectAttempt = NOW;
}

void IrcClient::onPacketOutEvent(PacketOutEvent& event)
{
    if (event.mPacket->getId() != PacketID::Text)
        return;
    auto packet = event.getPacket<TextPacket>();
    std::string message = packet->mMessage;

    if (message.starts_with("#"))
    {
        if (!isConnected())
        {
            displayMsg("§7[§dirc§7] §cYou aren't connected to IRC!");
            return;
        }
        message = message.substr(1);
        sendMessage(message);
        event.cancel();
    }
}

void IrcClient::onBaseTickEvent(BaseTickEvent& event)
{
    auto player = event.mActor; // when this event is called, this will never be null

    if (mLastPing != 0 && NOW - mLastPing > 15000 && isConnected())
    {
        spdlog::info("[irc] Ping timeout, disconnecting");
        ChatUtils::displayClientMessageRaw("§7[§dirc§7] §cTimed out.");
        disconnect();
        return;
    }

    static std::string lastPlayerName = "";
    if (player->getLocalName() != lastPlayerName)
    {
        lastPlayerName = player->getLocalName();
        sendIdentifySelf();
    }
    static std::string lastXuid = "";
    if (player->getXuid() != lastXuid)
    {
        lastXuid = player->getXuid();
        sendIdentifySelf();
    }

    if (mIdentifyNeeded && isConnected())
    {
        sendIdentifySelf();
        mIdentifyNeeded = false;
    }

    static uint64_t lastPingSent = 0;
    if (NOW - lastPingSent > 3000 && isConnected())
    {
        if (!isConnected())
        {
            spdlog::error("[irc] Cannot send packet, not connected to server");
            return;
        }
        IrcPingPacket packet;
        packet.timestamp = NOW;
        sendPacket(&packet);
        lastPingSent = NOW;
    }

    if (mQueuedMessages.empty()) return;
    std::string constructedMessage;
    for (const auto& message : mQueuedMessages)
    {
        constructedMessage += message + "\n";
    }
    // Clear the queued messages
    mQueuedMessages.clear();
    constructedMessage.pop_back(); // Remove the trailing newline
    // Displays all the queued messages at once to avoid crashing
    ChatUtils::displayClientMessageRaw(constructedMessage);
}

void IrcClient::displayMsg(std::string message)
{
    mQueuedMessages.push_back(message);
}

void IrcClient::sendPacket(const IrcPacket* packet)
{
    if (!isConnected())
    {
        spdlog::error("[irc] Cannot send packet, not connected to server");
        return;
    }
    const nlohmann::json j = packet->serialize();
    const std::string serializedString = j.dump();
    std::lock_guard<std::mutex> guard(mMutex);
    mWriter.WriteString(winrt::to_hstring(serializedString));
    mWriter.StoreAsync();
    mWriter.FlushAsync();
}

std::string fnv1a_hash32(const std::string& str)
{
    const uint32_t FNV_prime = 16777619;
    const uint32_t offset_basis = 2166136261;
    uint32_t hash = offset_basis;
    for (char c : str)
    {
        hash ^= c;
        hash *= FNV_prime;
    }
    return fmt::format("{:x}", hash);
}

std::string IrcClient::getPreferredUsername()
{
    if (!Solstice::Prefs->mIrcName.empty())
        return Solstice::Prefs->mIrcName;

    char* username = nullptr;
    size_t len;
    if (_dupenv_s(&username, &len, "USERNAME") == 0 && username != nullptr) {
        std::string usernameStr(username);
        free(username);
        usernameStr = StringUtils::trim(usernameStr);
        usernameStr = fnv1a_hash32(usernameStr);
        return usernameStr;
    }
    return "";
}


/*void IrcClient::receiveMessages()
{
    Streams::DataReader reader(
    reader.UnicodeEncoding(Streams::UnicodeEncoding::Utf8);
    reader.ByteOrder(Streams::ByteOrder::LittleEndian);
    while (true)
    {
        uint32_t bytesRead = reader.LoadAsync(1024).get();
        if (bytesRead == 0)
        {
            spdlog::info("[irc] Disconnected from server");
            disconnect();
            return;
        }
        std::string message = winrt::to_string(reader.ReadString(reader.UnconsumedBufferLength()));
        onMessageReceived(message.c_str());
    }
}*/


void IrcClient::sendMessage(std::string& message)
{
    if (!isConnected())
    {
        spdlog::error("[irc] Cannot send message, not connected to server");
        return;
    }
    // Trim the message
    message = StringUtils::trim(message);
    IrcMessagePacket packet;
    packet.message = message;
    nlohmann::json j = packet.serialize();
    std::string serializedString = j.dump();
    std::lock_guard<std::mutex> guard(mMutex);
    mWriter.WriteString(winrt::to_hstring(serializedString));
    mWriter.StoreAsync();
    mWriter.FlushAsync();
}

void IrcClient::onMessageReceived(const char* message)
{
    nlohmann::json j = nlohmann::json::parse(message);
    if (!j.contains("type")) {
        spdlog::error("[irc] Received message without type");
        return;
    }

    std::string type = j.at("type").get<std::string>();
    // Convert the type string to an enum so we can switch on it
    IrcPacketType packetType;

    if (type == "Join") packetType = IrcPacketType::Join;
    else if (type == "Leave") packetType = IrcPacketType::Leave;
    else if (type == "Message") packetType = IrcPacketType::Message;
    else if (type == "QueryName") packetType = IrcPacketType::QueryName;
    else if (type == "ListUsers") packetType = IrcPacketType::ListUsers;
    else if (type == "Ping") packetType = IrcPacketType::Ping;
    else
    {
        spdlog::error("[irc] Received message with unknown type: {}", type);
        return;
    }

    switch (packetType)
    {
    case IrcPacketType::Message: {
            IrcMessagePacket packet = IrcMessagePacket();
            packet.deserialize(j);
            displayMsg(packet.message);
            break;
    }
    case IrcPacketType::QueryName: {
            IrcQueryNamePacket packet = IrcQueryNamePacket();
            packet.deserialize(j);
            spdlog::info("[irc] Parsed query name: {}", packet.user);
            break;
    }
    case IrcPacketType::Ping: {
            mLastPing = NOW;
            // this gets really annoying to see in the console lol
            //spdlog::info("[irc] Received ping response from server");
            break;
    }
    default: {
            spdlog::error("[irc] Received message with unknown type: {}", type);
            break;
    }
    }
}

void IrcClient::listUsers()
{
    if (!isConnected())
    {
        spdlog::error("[irc] Cannot send packet, not connected to server");
        return;
    }
    IrcListUsersPacket packet;
    sendPacket(&packet);
}

void IrcClient::changeUsername(std::string username)
{
    if (username.empty())
    {
        ChatUtils::displayClientMessageRaw("§7[§dirc§7] §cUsername cannot be empty!");
        return;
    }
    if (username.size() > 16)
    {
        ChatUtils::displayClientMessageRaw("§7[§dirc§7] §cUsername cannot be longer than 16 characters!");
        return;
    }

    IrcJoinPacket packet;
    packet.user = username;
    packet.client = "§asolstice§r";
    sendPacket(&packet);
}

void IrcManager::init()
{
    if (!mClient) mClient = std::make_unique<IrcClient>();

    if (!mClient->connectToServer())
    {
        ChatUtils::displayClientMessageRaw("§7[§dirc§7] §cFailed to connect to IRC server.");
    } else
    {
        ChatUtils::displayClientMessageRaw("§7[§dirc§7] §aConnected to IRC.");
    }

    mLastConnectAttempt = NOW;
}

void IrcManager::deinit()
{
    if (mClient) mClient->disconnect();
}

void IrcManager::disconnectCallback()
{
    spdlog::info("[irc] Client dallocated.");
}

void IrcManager::requestListUsers()
{
    if (mClient) mClient->listUsers();
    else ChatUtils::displayClientMessageRaw("§7[§dirc§7] §cYou aren't connected to IRC!");
}

void IrcManager::requestChangeUsername(std::string username)
{
    if (mClient)
    {
        mClient->changeUsername(username);
        Solstice::Prefs->mIrcName = username;
        PreferenceManager::save(Solstice::Prefs);
        spdlog::info("[irc] Changed username to {}", Solstice::Prefs->mIrcName);
    }
    else ChatUtils::displayClientMessageRaw("§7[§dirc§7] §cYou aren't connected to IRC!");
}

void IrcManager::sendMessage(std::string& message)
{
    if (mClient) mClient->sendMessage(message);
    else ChatUtils::displayClientMessageRaw("§7[§dirc§7] §cYou aren't connected to IRC!");
}

bool IrcManager::isConnected()
{
    return mClient && mClient->isConnected();
}
