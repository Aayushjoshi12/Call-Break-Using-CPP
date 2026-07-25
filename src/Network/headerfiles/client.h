#pragma once

#include <enet/enet.h>
#include <nlohmann/json.hpp>
#include <string>

using json = nlohmann::json;

class Client
{
public:
    bool init(const char *addr, const int port);
    json handleMessage(const std::string &msg);

private:
    ENetHost *client = nullptr;
    ENetPeer *peer = nullptr;
};