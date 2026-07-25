#include <enet/enet.h>
using namespace std;
#include <iostream>
#include "../headerfiles/server.h"

bool Server::init(int port, int maxPlayers)
{
    if (enet_initialize() != 0)
    {
        std::cerr << "[ERROR] Failed to initialize ENet.\n";
        return false;
    }
    ENetAddress address;
    address.host = ENET_HOST_ANY;
    address.port = port;
    host = enet_host_create(&address, maxPlayers, 2, 0, 0);
    if (!host)
    {
        std::cerr << "[ERROR] Failed to create ENet server host.\n";
        enet_deinitialize();
        return false;
    }
    return true;
}

void Server::poll(Player *players[4])
{
    ENetEvent event;

    while (enet_host_service(host, &event, 0) > 0)
    {
        switch (event.type)
        {
        case ENET_EVENT_TYPE_CONNECT:
        {
            if (players_connected >= 4)
            {
                std::cout << "[INFO] Rejecting connection — lobby full.\n";
                json fullMsg = {{"method", "lobbyFull"}};
                std::string msg = fullMsg.dump();
                ENetPacket *packet = enet_packet_create(msg.c_str(), msg.size(), ENET_PACKET_FLAG_RELIABLE);
                enet_peer_send(event.peer, 0, packet);

                // Sentinel: mark this peer as "not a real player" so that
                // when its DISCONNECT event fires later, we don't corrupt
                // player_id ordering or players_connected count.
                event.peer->data = (void *)(uintptr_t)0;

                enet_peer_disconnect_later(event.peer, 0); // sends packet first, then disconnects
                break;
            }

            event.peer->data = (void *)(uintptr_t)player_id++;
            players_connected++;
            std::cout << "[INFO] Client connected with ID: " << (int)(intptr_t)event.peer->data << std::endl;
            peers[players_connected - 1] = event.peer;
            players[players_connected - 1] = new Player(true, true); // isHuman=true, isnetworked=true
            players[players_connected - 1]->player_id = (int)(intptr_t)event.peer->data;

            broadcast({{"method", "updateLobby"},
                       {"params", {{"playersConnected", players_connected}}}});
            break;
        }
        case ENET_EVENT_TYPE_DISCONNECT:
        {
            int disconnected_id = (int)(intptr_t)event.peer->data;

            if (disconnected_id == 0)
            {
                // This peer was rejected (lobby full) and never actually
                // admitted as a player — ignore, don't touch any state.
                std::cout << "[INFO] Rejected peer cleaned up (no state change).\n";
                break;
            }

            std::cout << "[INFO] Client disconnected with ID: " << disconnected_id << std::endl;

            // FIX: find which slot this peer actually occupies in peers[]/
            // players[] and compact both arrays. The old code decremented
            // player_id fields but never touched peers[]/players[], so
            // server_main.cpp's server.peers[i] / players[i] lookups went
            // stale (and the Player was leaked) after any disconnect.
            int slot = -1;
            for (int i = 0; i < players_connected; i++)
            {
                if (peers[i] == event.peer)
                {
                    slot = i;
                    break;
                }
            }

            if (slot != -1)
            {
                delete players[slot];
                for (int i = slot; i < players_connected - 1; i++)
                {
                    peers[i] = peers[i + 1];
                    players[i] = players[i + 1];
                }
                peers[players_connected - 1] = nullptr;
                players[players_connected - 1] = nullptr;
            }

            players_connected--;
            break;
        }
        case ENET_EVENT_TYPE_RECEIVE:
        {
            int senderId = (int)(intptr_t)event.peer->data;
            std::cout << "[INFO] Message received from client with ID: " << senderId << std::endl;
            json msg = json::parse((char *)event.packet->data,
                                    (char *)event.packet->data + event.packet->dataLength);
            incomingMessages.push({senderId, msg});
            enet_packet_destroy(event.packet);
            break;
        }
        default:
            break;
        }
    }
}

void Server::sendTo(ENetPeer *peer, const json &j)
{
    std::string msg = j.dump();
    ENetPacket *packet = enet_packet_create(msg.c_str(), msg.size(), ENET_PACKET_FLAG_RELIABLE);
    enet_peer_send(peer, 0, packet);
}

void Server::broadcast(const json &j)
{
    std::string msg = j.dump();
    ENetPacket *packet = enet_packet_create(msg.c_str(), msg.size(), ENET_PACKET_FLAG_RELIABLE);
    enet_host_broadcast(host, 0, packet);
}

void Server::shutdown()
{
    if (host)
    {
        enet_host_destroy(host);
        host = nullptr;
    }
    enet_deinitialize();
}