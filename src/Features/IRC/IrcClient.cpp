//
// Created by vastrakai on 8/24/2024.
//

#include "IrcClient.hpp"

#include <utility>


IrcClient::IrcClient()
{
    gFeatureManager->mDispatcher->listen<PacketOutEvent, &IrcClient::onPacketOutEvent>(this);
}

IrcClient::~IrcClient()
{
    gFeatureManager->mDispatcher->deafen<PacketOutEvent, &IrcClient::onPacketOutEvent>(this);
}

void IrcClient::queryName()
{
    IrcQueryNamePacket packet;
    sendPacket(&packet);
}

bool IrcClient::isConnected()
{
    return mSocket != INVALID_SOCKET;
}

bool IrcClient::connectToServer()
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        spdlog::error("[irc] WSAStartup failed with error: {}", WSAGetLastError());
        return false;
    }

    mSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (mSocket == INVALID_SOCKET) {
        spdlog::error("[irc] Failed to create socket with error: {}", WSAGetLastError());
        WSACleanup();
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
        closesocket(mSocket);
        WSACleanup();
        return false;
    }

    host = std::string(inet_ntoa(((sockaddr_in*)result->ai_addr)->sin_addr));

    auto fam = AF_INET; // IPv4
    sockaddr_in clientService;
    clientService.sin_family = fam;
    inet_pton(fam, host.c_str(), &clientService.sin_addr);
    clientService.sin_port = htons(mPort);

    if (connect(mSocket, (SOCKADDR*)&clientService, sizeof(clientService)) == SOCKET_ERROR) {
        spdlog::error("[irc] Failed to connect to server with error: {}", WSAGetLastError());
        closesocket(mSocket);
        WSACleanup();
        return false;
    }

    spdlog::info("[irc] Connected to server");

    // Start receiving messages in a separate thread
    mReceiveThread = std::thread(&IrcClient::receiveMessages, this);

    // Get the USERNAME environment variable
    char* username = nullptr;
    size_t len;
    if (_dupenv_s(&username, &len, "USERNAME") == 0 && username != nullptr) {
        spdlog::info("[irc] Username: {}", username);
        free(username);
    }

    if (username == nullptr) {
        // Send a blank join packet
        changeUsername("");
        return true;
    }

    // Send a join packet
    changeUsername(std::string(username));
    return true;
}

void IrcClient::disconnect()
{
    closesocket(mSocket);
    WSACleanup();

    mSocket = INVALID_SOCKET;
    mReceiveThread.join();

    ChatUtils::displayClientMessageRaw("§7[§dirc§7] §cDisconnected from IRC.");
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
            ChatUtils::displayClientMessageRaw("§7[§dirc§7] §cYou aren't connected to IRC!");
            return;
        }
        message = message.substr(1);
        sendMessage(message);
        event.cancel();
    }
}

void IrcClient::sendPacket(const IrcPacket* packet)
{
    const nlohmann::json j = packet->serialize();
    const std::string serializedString = j.dump();
    send(mSocket, serializedString.c_str(), serializedString.size(), 0);
}

void IrcClient::receiveMessages()
{
    int bytesReceived;
    while ((bytesReceived = recv(mSocket, mBuffer, sizeof(mBuffer) - 1, 0)) > 0) {
        mBuffer[bytesReceived] = '\0'; // Null-terminate the received data
        try
        {
            onMessageReceived(mBuffer);
        } catch (const nlohmann::json::exception& e) {
            spdlog::error("[irc] Failed to parse message: {}", e.what());
        } catch (const std::exception& e) {
            spdlog::error("[irc] Failed to parse message: {}", e.what());
        } catch (...) {
            spdlog::error("[irc] Failed to parse message: unknown error");
        }
    }

    if (bytesReceived == 0) {
        spdlog::info("[irc] Server closed connection");
        ChatUtils::displayClientMessageRaw("§7[§dirc§7] §cDisconnected from IRC.");
    } else if (bytesReceived < 0) {
        spdlog::error("[irc] recv failed with error: {}", WSAGetLastError());
    }
}

void IrcClient::sendMessage(std::string& message)
{
    // Trim the message
    message = StringUtils::trim(message);
    IrcMessagePacket packet;
    packet.message = message;
    nlohmann::json j = packet.serialize();
    std::string serializedString = j.dump();
    send(mSocket, serializedString.c_str(), serializedString.size(), 0);
}

void IrcClient::onMessageReceived(const char* message)
{
    spdlog::info("[irc] Received message: {}", message);

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
            spdlog::info("[irc] Parsed message: {}", packet.message);
            ChatUtils::displayClientMessageRaw(packet.message);
            break;
    }
    case IrcPacketType::QueryName: {
            IrcQueryNamePacket packet = IrcQueryNamePacket();
            packet.deserialize(j);
            spdlog::info("[irc] Parsed query name: {}", packet.user);
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
    sendPacket(&packet);
}

void IrcManager::init()
{
    mClient = std::make_unique<IrcClient>();
    if (!mClient->connectToServer())
    {
        mClient.reset();
        ChatUtils::displayClientMessageRaw("§7[§dirc§7] §cFailed to connect to IRC server.");
    } else
    {
        ChatUtils::displayClientMessageRaw("§7[§dirc§7] §aConnected to IRC.");
    }
}

void IrcManager::deinit()
{
    if (mClient)
        mClient->disconnect();
    mClient.reset();
}

void IrcManager::requestListUsers()
{
    if (mClient) mClient->listUsers();
    else ChatUtils::displayClientMessageRaw("§7[§dirc§7] §cYou aren't connected to IRC!");
}

void IrcManager::requestChangeUsername(std::string username)
{
    if (mClient) mClient->changeUsername(std::move(username));
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
