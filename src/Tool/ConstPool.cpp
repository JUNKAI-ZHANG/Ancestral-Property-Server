#ifndef _CONST_POOL_CPP
#define _CONST_POOL_CPP

#include "../Header/Tool/ConstPool.h"

ConstPool::ConstPool()
{
    if (!ReadINI()) std::cout << "Read ini.json Fail..." << std::endl;
    if (!ReadNet()) std::cout << "Read Net.json Fail..." << std::endl;
    if (!ReadRoom()) std::cout << "Read Room.json Fail..." << std::endl;
    if (!ReadServer())std::cout << "Read Server.json Fail..." << std::endl;
}

ConstPool::~ConstPool()
{

}

bool ConstPool::ReadINI()
{
    Json::Reader reader;
    Json::Value root;

    std::ifstream in("./ini/ini.json", std::ios::binary);

    if (!in.is_open())
    {
        return false;
    }

    if (reader.parse(in, root))
    {
        IS_CHASE_FRAME = root["Support_Chase_Frame"].asBool();
        DEBUG = root["Debug_Out"].asBool();
        PRESSURE_TEST = root["Pressure_Test"].asBool();

        EPOLL_WAIT_TIME = root["Epoll_Wait_Time"].asInt();
        MAX_CLIENTS = root["Epoll_Listen_Number"].asInt();
        ONE_FRAME_MS = root["One_Frame_Ms"].asInt();
        CHECK_ALL_CLIENT_CONN = root["Check_All_Client_DropOut_Time"].asInt();
        TRY_CONNECT_SERVER_TIME = root["Try_Connect_Server_Time"].asInt();
        MAX_HEARTBEAT_DIFF = root["Check_HeartBeat_Time"].asInt();
        SEND_CENTERSERVER_TIME = root["Try_Send_CenterServer_Time"].asInt();

        LOCAL_IP = root["LOCAL_IP"].asString();

        SERVER_FREE_COUNT = root["Server_Free_Count"].asInt();
        SERVER_COMMON_COUNT = root["Server_Common_Count"].asInt();
        SERVER_BUSY_COUNT = root["Server_Busy_Count"].asInt();
    }

    in.close();
    return true;
}

bool ConstPool::ReadNet()
{
    Json::Reader reader;
    Json::Value root;

    std::ifstream in("./ini/Net.json", std::ios::binary);

    if (!in.is_open())
    {
        return false;
    }

    if (reader.parse(in, root))
    {
        HEAD_SIZE = root["Head_Size"].asInt();
        HEAD_SIZE_S = root["Head_Size_S"].asInt();
        TMP_BUFFER_SIZE = root["Tmp_Buffer_Size"].asInt();
        MAX_BUFFER_SIZE = root["Max_Buffer_Size"].asInt();
    }

    in.close();
    return true;
}

bool ConstPool::ReadRoom()
{
    Json::Reader reader;
    Json::Value root;

    std::ifstream in("./ini/Room.json", std::ios::binary);

    if (!in.is_open())
    {
        return false;
    }

    if (reader.parse(in, root))
    {
        ROOM_SIZE = root["Room_Size"].asInt();
        DEFAULT_ROOM_COUNT = root["Default_Room_Count"].asInt();
        ROOM_POOL_SIZE = root["Room_Pool_Size"].asInt();
    }

    in.close();
    return true;
}

bool ConstPool::ReadServer()
{
    Json::Reader reader;
    Json::Value root;

    std::ifstream in("./ini/Server.json", std::ios::binary);

    if (!in.is_open())
    {
        return false;
    }

    if (reader.parse(in, root))
    {
        GATE_SERVER_NAME_1 =  root["GATE_SERVER"]["ID_1"]["NAME"].asString();
        GATE_SERVER_NAME_2 =  root["GATE_SERVER"]["ID_2"]["NAME"].asString();
        GATE_SERVER_NAME_3 =  root["GATE_SERVER"]["ID_3"]["NAME"].asString();
        GATE_SERVER_NAME_4 =  root["GATE_SERVER"]["ID_4"]["NAME"].asString();
        GATE_SERVER_NAME_5 =  root["GATE_SERVER"]["ID_5"]["NAME"].asString();
        GATE_SERVER_NAME_6 =  root["GATE_SERVER"]["ID_6"]["NAME"].asString();
        GATE_SERVER_NAME_7 =  root["GATE_SERVER"]["ID_7"]["NAME"].asString();
        GATE_SERVER_NAME_8 =  root["GATE_SERVER"]["ID_8"]["NAME"].asString();
        GATE_SERVER_IP_1 =    root["GATE_SERVER"]["ID_1"]["IP"].asString();
        GATE_SERVER_IP_2 =    root["GATE_SERVER"]["ID_2"]["IP"].asString();
        GATE_SERVER_IP_3 =    root["GATE_SERVER"]["ID_3"]["IP"].asString();
        GATE_SERVER_IP_4 =    root["GATE_SERVER"]["ID_4"]["IP"].asString();
        GATE_SERVER_IP_5 =    root["GATE_SERVER"]["ID_5"]["IP"].asString();
        GATE_SERVER_IP_6 =    root["GATE_SERVER"]["ID_6"]["IP"].asString();
        GATE_SERVER_IP_7 =    root["GATE_SERVER"]["ID_7"]["IP"].asString();
        GATE_SERVER_IP_8 =    root["GATE_SERVER"]["ID_8"]["IP"].asString();
        GATE_SERVER_PORT_1 =  root["GATE_SERVER"]["ID_1"]["PORT"].asInt();
        GATE_SERVER_PORT_2 =  root["GATE_SERVER"]["ID_2"]["PORT"].asInt();
        GATE_SERVER_PORT_3 =  root["GATE_SERVER"]["ID_3"]["PORT"].asInt();
        GATE_SERVER_PORT_4 =  root["GATE_SERVER"]["ID_4"]["PORT"].asInt();
        GATE_SERVER_PORT_5 =  root["GATE_SERVER"]["ID_5"]["PORT"].asInt();
        GATE_SERVER_PORT_6 =  root["GATE_SERVER"]["ID_6"]["PORT"].asInt();
        GATE_SERVER_PORT_7 =  root["GATE_SERVER"]["ID_7"]["PORT"].asInt();
        GATE_SERVER_PORT_8 =  root["GATE_SERVER"]["ID_8"]["PORT"].asInt();

        LOGIC_SERVER_NAME_1 =  root["LOGIC_SERVER"]["ID_1"]["NAME"].asString();
        LOGIC_SERVER_NAME_2 =  root["LOGIC_SERVER"]["ID_2"]["NAME"].asString();
        LOGIC_SERVER_NAME_3 =  root["LOGIC_SERVER"]["ID_3"]["NAME"].asString();
        LOGIC_SERVER_NAME_4 =  root["LOGIC_SERVER"]["ID_4"]["NAME"].asString();
        LOGIC_SERVER_NAME_5 =  root["LOGIC_SERVER"]["ID_5"]["NAME"].asString();
        LOGIC_SERVER_NAME_6 =  root["LOGIC_SERVER"]["ID_6"]["NAME"].asString();
        LOGIC_SERVER_NAME_7 =  root["LOGIC_SERVER"]["ID_7"]["NAME"].asString();
        LOGIC_SERVER_NAME_8 =  root["LOGIC_SERVER"]["ID_8"]["NAME"].asString();
        LOGIC_SERVER_IP_1 =  root["LOGIC_SERVER"]["ID_1"]["IP"].asString();
        LOGIC_SERVER_IP_2 =  root["LOGIC_SERVER"]["ID_2"]["IP"].asString();
        LOGIC_SERVER_IP_3 =  root["LOGIC_SERVER"]["ID_3"]["IP"].asString();
        LOGIC_SERVER_IP_4 =  root["LOGIC_SERVER"]["ID_4"]["IP"].asString();
        LOGIC_SERVER_IP_5 =  root["LOGIC_SERVER"]["ID_5"]["IP"].asString();
        LOGIC_SERVER_IP_6 =  root["LOGIC_SERVER"]["ID_6"]["IP"].asString();
        LOGIC_SERVER_IP_7 =  root["LOGIC_SERVER"]["ID_7"]["IP"].asString();
        LOGIC_SERVER_IP_8 =  root["LOGIC_SERVER"]["ID_8"]["IP"].asString();
        LOGIC_SERVER_PORT_1 =  root["LOGIC_SERVER"]["ID_1"]["PORT"].asInt();
        LOGIC_SERVER_PORT_2 =  root["LOGIC_SERVER"]["ID_2"]["PORT"].asInt();
        LOGIC_SERVER_PORT_3 =  root["LOGIC_SERVER"]["ID_3"]["PORT"].asInt();
        LOGIC_SERVER_PORT_4 =  root["LOGIC_SERVER"]["ID_4"]["PORT"].asInt();
        LOGIC_SERVER_PORT_5 =  root["LOGIC_SERVER"]["ID_5"]["PORT"].asInt();
        LOGIC_SERVER_PORT_6 =  root["LOGIC_SERVER"]["ID_6"]["PORT"].asInt();
        LOGIC_SERVER_PORT_7 =  root["LOGIC_SERVER"]["ID_7"]["PORT"].asInt();
        LOGIC_SERVER_PORT_8 =  root["LOGIC_SERVER"]["ID_8"]["PORT"].asInt();

        DATABASE_SERVER_NAME = root["DB_SERVER"]["NAME"].asString();
        DATABASE_SERVER_IP = root["DB_SERVER"]["IP"].asString();
        DATABASE_SERVER_PORT = root["DB_SERVER"]["PORT"].asInt();

        CENTER_SERVER_NAME = root["CENTER_SERVER"]["NAME"].asString();
        CENTER_SERVER_IP = root["CENTER_SERVER"]["IP"].asString();
        CENTER_SERVER_PORT = root["CENTER_SERVER"]["PORT"].asInt();
    }

    in.close();
    return true;
}

#endif