#pragma once
#include <nlohmann/json.hpp>
using json = nlohmann::json;
#include <enet/enet.h>
#include <iostream>
#include <string>
class Client{
    public:
     ENetHost*client;
     ENetPeer*peer;
     bool connected;
     int playerID;
     std::vector<int>hand;
     bool init(const char*addr,const int port);
     void poll();
     std::queue<json> incomingMessages;   
     json Datapacket;
     void disconnect();
     private:
      json handleMessage(const std::string& msg);

};