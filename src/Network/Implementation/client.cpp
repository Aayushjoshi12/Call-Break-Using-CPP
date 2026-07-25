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

json Client::handleMessage(const std::string &msg)
{
    json j = json::parse(msg);
    return j;
}