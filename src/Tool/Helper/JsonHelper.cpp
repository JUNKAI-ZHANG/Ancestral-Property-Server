#ifndef _JSON_HELPER
#define _JSON_HELPER

#include <bits/stdc++.h>

#include "../../Header/Json/json.h"
#include "../../Header/Tool/ConstPool.h"

const std::string Path = "./ini/";

void ReadJson(std::string fileName)
{
    Json::Reader reader;
    Json::Value root;

    std::ifstream in(Path + fileName, std::ios::binary);

    if (!in.is_open())
    {
        std::cout << "Error opening" << std::endl;
        return;
    }

    if (reader.parse(in, root))
    {
        const Json::Value CenterServer = root["CENTER_SERVER"];
        std::vector<std::string> keys = CenterServer.getMemberNames();
        for (int i = 0; i < CenterServer.size(); i++) 
        {
            std::cout << keys[i] << ":" << CenterServer[keys[i]] << std::endl;
        }
    }

    in.close();
}

int main()
{
    ConstPool constPool;
}

#endif