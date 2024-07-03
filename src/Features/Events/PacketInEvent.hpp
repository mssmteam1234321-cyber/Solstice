//
// Created by vastrakai on 7/2/2024.
//

#include "Event.hpp"
#include <memory>

class PacketInEvent : public CancellableEvent {
public:
    explicit PacketInEvent(std::shared_ptr<class Packet> packet, void* networkIdentifier, void* netEventCallback) : mPacket(packet), mNetworkIdentifier(networkIdentifier), mNetEventCallback(netEventCallback) {}

    std::shared_ptr<class Packet> mPacket;
    void* mNetworkIdentifier;
    void* mNetEventCallback;
};