#pragma once
#include <enet/enet.h>
#include <nlohmann/json.hpp>
#include "../../Backend/headerfiles/GameManager.h"
#include <vector>
#include <string>

using json = nlohmann::json;

class Server
{
public:
    int player_id = 1;
    int players_connected = 0;
    bool init(int port, int maxPlayers); // Initializes the server on the specified port with a maximum number of players
    void poll(Player *players[4]);       // Polls for incoming events and handles them
    void shutdown();
    ENetPeer *peers[4] = {nullptr};
    void sendTo(ENetPeer *peer, const json &j);

private:
    ENetHost *host = nullptr;
    void broadcast(const json &j);
};