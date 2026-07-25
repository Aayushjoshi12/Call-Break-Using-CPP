#include "../headerfiles/client.h"
#include <iostream>
bool Client::init(const char *addr, const int port)
{
    if (enet_initialize() != 0)
    {
        std::cerr << "[ERROR] Failed to initialize ENet.\n";
        return false;
    }
    client = enet_host_create(nullptr, 1, 2, 0, 0);
    if (!client)
    {
        std::cerr << "[ERROR] Failed to create ENet client host.\n";
        enet_deinitialize();
        return false;
    }
    ENetAddress address;
    enet_address_set_host(&address, addr);
    address.port = port;
    peer = enet_host_connect(client, &address, 2, 0);
    if (!peer)
    {
        std::cerr << "[ERROR] No available peers for connection.\n";
        enet_host_destroy(client);
        enet_deinitialize();
        return false;
    }
    return true;
}
void Client::poll()
{
    ENetEvent event;
    while (enet_host_service(client, &event, 0) > 0)
    {
        switch (event.type)
        {
        case ENET_EVENT_TYPE_CONNECT:
            std::cout << "[INFO] Connected to server!\n";
            connected = true;
            break;
        case ENET_EVENT_TYPE_RECEIVE:
        {
            json msg = json::parse((char *)event.packet->data,
                                   (char *)event.packet->data + event.packet->dataLength);
            incomingMessages.push(msg);
            enet_packet_destroy(event.packet);
            break;
        }
        case ENET_EVENT_TYPE_DISCONNECT:
            std::cout << "[INFO] Disconnected from server.\n";
            connected = false;
            break;
        default:
            break;
        }
    }
}
json Client::handleMessage(const std::string &msg)
{
    json j = json::parse(msg);
    return j;
}

// NEW: clients had no way to talk back to the server (needed to submit
// bids / play cards). Mirrors Server::sendTo.
void Client::send(const json &j)
{
    if (!peer)
        return;
    std::string msg = j.dump();
    ENetPacket *packet = enet_packet_create(msg.c_str(), msg.size(), ENET_PACKET_FLAG_RELIABLE);
    enet_peer_send(peer, 0, packet);
}