#pragma once
#include <nlohmann/json.hpp>
using json = nlohmann::json;
#include <enet/enet.h>
#include <iostream>
#include <string>
#include <vector>
#include <queue>

class Client {
    public:
    void send(const json &j);
     ENetHost* client;
     ENetPeer* peer;
     bool connected;
     int playerID;
     std::vector<int> hand;
     bool init(const char* addr, const int port);
     void poll();
     json Datapacket;
     std::queue<json> incomingMessages;   // <-- new
     void disconnect();
     private:
      json handleMessage(const std::string& msg);
};