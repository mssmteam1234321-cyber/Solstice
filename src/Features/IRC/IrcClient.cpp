//
// Created by vastrakai on 8/24/2024.
//

#include "IrcClient.hpp"

#include <codecvt>
#include <regex>
#include <utility>
#include <SDK/Minecraft/ClientInstance.hpp>


std::vector<ConnectedIrcUser> IrcClient::getConnectedUsers()
{
    std::lock_guard<std::mutex> lock(mConnectedUsersMutex);
    return mConnectedUsers;
}

void IrcClient::setConnectedUsers(const std::vector<ConnectedIrcUser>& users)
{
    std::lock_guard<std::mutex> lock(mConnectedUsersMutex);
    mConnectedUsers = users;
}

IrcClient::IrcClient()
{
    gFeatureManager->mDispatcher->listen<PacketOutEvent, &IrcClient::onPacketOutEvent>(this);
    gFeatureManager->mDispatcher->listen<BaseTickEvent, &IrcClient::onBaseTickEvent>(this);
    gFeatureManager->mDispatcher->listen<PacketInEvent, &IrcClient::onPacketInEvent>(this);
}

IrcClient::~IrcClient()
{
    gFeatureManager->mDispatcher->deafen<PacketOutEvent, &IrcClient::onPacketOutEvent>(this);
    gFeatureManager->mDispatcher->deafen<BaseTickEvent, &IrcClient::onBaseTickEvent>(this);
    gFeatureManager->mDispatcher->deafen<PacketInEvent, &IrcClient::onPacketInEvent>(this);
}

bool IrcClient::isConnected() const
{
    return mConnectionState == ConnectionState::Connected;
}

void IrcClient::sendOpAuto(const ChatOp& op)
{
    if (mEncrypted) {
        spdlog::info("[irc] sending as encrypted");
        // Encrypt the message
        auto encryptedOp = EncryptedOp(op.serialize().dump(4), mClientKey);
        sendData(encryptedOp.serialize());
        return;
    }

    sendData(op.serialize().dump(4));
}

ChatOp IrcClient::parseOpAuto(std::string data)
{
    if (mEncrypted) {
        auto encryptedOp = EncryptedOp(data);
        encryptedOp.decrypt(mServerKey);
        return ChatOp::deserializeStr(encryptedOp.Encrypted);
    }

    spdlog::info("[irc] Parsing message: {}", data);
    return ChatOp::deserializeStr(data);
}

void IrcClient::sendData(std::string data)
{
    if (!isConnected())
    {
        spdlog::error("[irc] Cannot send data, not connected to server");
        return;
    }

    try
    {
        data = StringUtils::encode(data);

        std::lock_guard<std::mutex> guard(mMutex);
        mWriter.WriteString(winrt::to_hstring(data));
        mWriter.StoreAsync();
        mWriter.FlushAsync();
    } catch (winrt::hresult_error const& ex)
    {
        spdlog::error("[irc] Error: {}", winrt::to_string(ex.message()));
    } catch (const std::exception& ex)
    {
        spdlog::error("[irc] Error: {}", ex.what());
    } catch (...)
    {
        spdlog::error("[irc] Unknown error");
    }
}

std::string IrcClient::getHwid()
{
    return Solstice::sHWID;
}

void IrcClient::genClientKey()
{
    mClientKey = StringUtils::sha256(getHwid());
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

    try
    {
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
                message = StringUtils::decode(message);
                message = StringUtils::trim(message);
                // continue if this isnt valid json
                if (message.empty() || message[0] != '{' || message[message.size() - 1] != '}')
                {
                    spdlog::error("[irc] Invalid message: {}", message);
                    return;
                }

                auto op = parseOpAuto(message);
                spdlog::info("[irc] Received message: {}", magic_enum::enum_name(op.opCode));

                if (op.opCode == OpCode::KeyOut)
                {
                    mServerKey = op.data;
                    spdlog::info("[irc] Server key received: {}", mServerKey);
                    genClientKey();
                    spdlog::info("[irc] Client key generated: {}", mClientKey);
                    auto op = ChatOp(OpCode::KeyIn, mClientKey, true);
                    spdlog::info("[irc] Sending client key");
                    sendOpAuto(op);
                    spdlog::info("[irc] Client key sent, encryption enabled");
                    mEncrypted = true; // from now on, we will encrypt messages
                }

                if (op.opCode == OpCode::Ping)
                {
                    auto op = ChatOp(OpCode::Ping, std::to_string(NOW), true);
                    sendOpAuto(op);
                    mLastPing = NOW;
                }

                if (op.opCode == OpCode::Work)
                {
                    int result = WorkingVM::SolveProofTask(op.data);
                    auto op = ChatOp(OpCode::CompleteWork, std::to_string(result), true);
                    sendOpAuto(op);
                }

                if (op.opCode == OpCode::AuthFinish)
                {
                    spdlog::info("[irc] Authenticated with server");
                    onConnected();
                }


            }
            catch (winrt::hresult_error const& ex)
            {
                spdlog::error("[irc] Error: {}", winrt::to_string(ex.message()));

                disconnect();
            } catch (const std::exception& ex)
            {
                spdlog::error("[irc] StdError: {}", ex.what());
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

        mSocket.ConnectAsync(winrt::Windows::Foundation::Uri(winrt::to_hstring("ws://" + host + ":" + port))).Completed([=](auto&&, auto&&)
        {
            mConnectionState = ConnectionState::Connected;
            spdlog::info("[irc] Connected to server");
            ChatUtils::displayClientMessageRaw("§7[§dirc§7] §aConnected to IRC.");

        });
    } catch (winrt::hresult_error const& ex)
    {
        spdlog::error("[irc] Error: {} [{}]", winrt::to_string(ex.message()), ex.code());
        return false;
    } catch (const std::exception& ex)
    {
        spdlog::error("[irc] Error: {}", ex.what());
        return false;
    } catch (...)
    {
        spdlog::error("[irc] Unknown error");
        return false;
    }

    spdlog::info("[irc] Connected to server");

    // Start receiving messages in a separate thread
    //mReceiveThread = std::thread(&IrcClient::receiveMessages, this);


    return true;
}

void IrcClient::onConnected()
{
    spdlog::info("[irc] Connected to server");
    mConnectionState = ConnectionState::Connected;
    mLastPing = NOW;
}

void IrcClient::disconnect()
{
    try
    {
        mEncrypted = false;
        mClientKey = "";
        mServerKey = "";

        if (!isConnected())
        {
            spdlog::error("[irc] Cannot disconnect, not connected to server");
            return;
        }

        if (mConnectionState != ConnectionState::Disconnected)
            ChatUtils::displayClientMessageRaw("§7[§dirc§7] §cDisconnected from IRC.");

        mSocket.Close();
        mConnectionState = ConnectionState::Disconnected;
        mSocket = Sockets::MessageWebSocket();
        mWriter = Streams::DataWriter();

        spdlog::info("[irc] Disconnected from server");
        IrcManager::mLastConnectAttempt = NOW;
    } catch (winrt::hresult_error const& ex)
    {
        spdlog::error("[irc] Error: {}", winrt::to_string(ex.message()));
    } catch (const std::exception& ex)
    {
        spdlog::error("[irc] Error: {}", ex.what());
    } catch (...)
    {
        spdlog::error("[irc] Unknown error");
    }
    mConnectedUsers.clear();

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
        sendPlayerIdentity();
    }
    static std::string lastXuid = "";
    if (player->getXuid() != lastXuid)
    {
        lastXuid = player->getXuid();
        sendPlayerIdentity();
    }

    if (mIdentifyNeeded && isConnected())
    {
        sendPlayerIdentity();
        mIdentifyNeeded = false;
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

void IrcClient::onPacketInEvent(PacketInEvent& event)
{
    if (event.mPacket->getId() != PacketID::Text || !mShowNamesInChat) return;

    auto packet = event.getPacket<TextPacket>();
    std::string message = packet->mMessage;

    auto users = getConnectedUsers();

    // If the message doesn't contain any playerNames, return
    if (std::ranges::none_of(users, [&message](const ConnectedIrcUser& user) { return message.find(user.playerName) != std::string::npos; }))
        return;

    for (const auto& user : users) {
        std::regex regex(user.playerName);
        message = std::regex_replace(message, regex, user.username + " (" + user.playerName + ")");
    }

    // Update the packet's message
    packet->mMessage = message;
}

void IrcClient::sendPlayerIdentity()
{
    if (!isConnected()) return;

    auto player = ClientInstance::get()->getLocalPlayer();
    if (!player) return;

    // TODO: Implement this
}

void IrcClient::displayMsg(std::string message)
{
    mQueuedMessages.push_back(message);
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

bool IrcManager::setShowNamesInChat(bool showNamesInChat)
{
    if (!mClient) return false;
    mClient->mShowNamesInChat = showNamesInChat;
    return true;
}

void IrcManager::init()
{
    if (!mClient) mClient = std::make_unique<IrcClient>();

    if (!mClient->connectToServer())
    {
        ChatUtils::displayClientMessageRaw("§7[§dirc§7] §cFailed to connect to IRC server.");
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
    /*if (mClient) mClient->listUsers();
    else*/ ChatUtils::displayClientMessageRaw("§7[§dirc§7] §cYou aren't connected to IRC!");
}

void IrcManager::requestChangeUsername(std::string username)
{
    if (mClient)
    {
        // mClient->changeUsername(username);
        Solstice::Prefs->mIrcName = username;
        PreferenceManager::save(Solstice::Prefs);
        spdlog::info("[irc] Changed username to {}", Solstice::Prefs->mIrcName);
    }
    else ChatUtils::displayClientMessageRaw("§7[§dirc§7] §cYou aren't connected to IRC!");
}

void IrcManager::sendMessage(std::string& message)
{
    /*if (mClient) mClient->sendMessage(message);
    else*/ ChatUtils::displayClientMessageRaw("§7[§dirc§7] §cYou aren't connected to IRC!");
}

bool IrcManager::isConnected()
{
    return mClient && mClient->isConnected();
}
