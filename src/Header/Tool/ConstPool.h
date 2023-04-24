#ifndef _CONST_POOL
#define _CONST_POOL

#include <string>
#include <istream>
#include <fstream>

#include "../Json/json.h"
#include "../Tool/Singleton.h"

class ConstPool
{
public:
    ConstPool();

    ~ConstPool();

    bool ReadNet();

    bool ReadRoom();

    bool ReadServer();

    bool ReadINI();

public:
    bool IS_CHASE_FRAME;
    bool DEBUG;
    bool PRESSURE_TEST;

    int HEAD_SIZE;
    int HEAD_SIZE_S;
    int TMP_BUFFER_SIZE;
    int MAX_BUFFER_SIZE;

    int EPOLL_WAIT_TIME;
    int MAX_CLIENTS;

    int ROOM_SIZE;
    int DEFAULT_ROOM_COUNT;
    int ROOM_POOL_SIZE;

    int ONE_FRAME_MS;
    int CHECK_ALL_CLIENT_CONN;
    int TRY_CONNECT_SERVER_TIME;
    int MAX_HEARTBEAT_DIFF;
    int SEND_CENTERSERVER_TIME;

    int GATE_SERVER_PORT_1;
    int GATE_SERVER_PORT_2;
    int GATE_SERVER_PORT_3;
    int GATE_SERVER_PORT_4;
    int GATE_SERVER_PORT_5;
    int GATE_SERVER_PORT_6;

    std::string GATE_SERVER_IP_1;
    std::string GATE_SERVER_IP_2;
    std::string GATE_SERVER_IP_3;
    std::string GATE_SERVER_IP_4;
    std::string GATE_SERVER_IP_5;
    std::string GATE_SERVER_IP_6;

    std::string GATE_SERVER_NAME_1;
    std::string GATE_SERVER_NAME_2;
    std::string GATE_SERVER_NAME_3;
    std::string GATE_SERVER_NAME_4;
    std::string GATE_SERVER_NAME_5;
    std::string GATE_SERVER_NAME_6;

    int LOGIC_SERVER_PORT_1;
    int LOGIC_SERVER_PORT_2;
    int LOGIC_SERVER_PORT_3;
    int LOGIC_SERVER_PORT_4;
    int LOGIC_SERVER_PORT_5;
    int LOGIC_SERVER_PORT_6;

    std::string LOGIC_SERVER_IP_1;
    std::string LOGIC_SERVER_IP_2;
    std::string LOGIC_SERVER_IP_3;
    std::string LOGIC_SERVER_IP_4;
    std::string LOGIC_SERVER_IP_5;
    std::string LOGIC_SERVER_IP_6;

    int CENTER_SERVER_PORT;
    std::string CENTER_SERVER_IP;

    int DATABASE_SERVER_PORT;
    std::string DATABASE_SERVER_IP;

    std::string LOCAL_IP;

    int SERVER_FREE_COUNT;
    int SERVER_COMMON_COUNT;
    int SERVER_BUSY_COUNT;
};

#define JSON (*Singleton<ConstPool>::get())

#endif