#ifndef _PROFILE_H
#define _PROFILE_H

#ifdef _PROFILE_H

#define HEAD_SIZE               12

#define EPOLL_WAIT_TIME         11              // epoll_wait 阻塞时间
#define MAX_CLIENTS             64              // epoll 最大连接数量
#define TMP_BUFFER_SIZE         1024            // 自定义临时缓冲区大小
#define MAX_BUFFER_SIZE         1045876         // 自定义最大缓冲区大小

#define DEFAULT_ROOM_COUNT      32
#define ROOM_POOL_SIZE          32

#define ONE_FRAME_MS            33              // 一帧的毫秒数

#define CHECK_ALL_CLIENT_CONN   500
#define TRY_CONNECT_SERVER_TIME 1000            // 服务器失去与某服务器连接，申请CenterServer重新分配的时间
#define MAX_HEARTBEAT_DIFF      4000
#define SEND_CENTERSERVER_TIME  5000            // 成功连接到CenterServer后，每秒向CenterServer发包的时间

#define GATE_SERVER_PORT_1      10801           // 目前接收所有客户端连接的端口，后续将CenterServer部署到此端口
#define GATE_SERVER_PORT_2      10802
#define GATE_SERVER_PORT_3      10803
#define GATE_SERVER_PORT_4      10804
#define GATE_SERVER_PORT_5      10805
#define GATE_SERVER_PORT_6      10806

#define LOGIC_SERVER_PORT_1     10811
#define LOGIC_SERVER_PORT_2     10812
#define LOGIC_SERVER_PORT_3     10813
#define LOGIC_SERVER_PORT_4     10814
#define LOGIC_SERVER_PORT_5     10815
#define LOGIC_SERVER_PORT_6     10816

#define CENTER_SERVER_PORT      10808

#define DATABASE_SERVER_PORT    10809

#define LOCAL_IP                "127.0.0.1"     // 本地IP

#define SERVER_FREE_COUNT       2               // 服务器空闲的人数上限
#define SERVER_COMMON_COUNT     4               // 服务器中等繁忙的人数上限
#define SERVER_BUSY_COUNT       50              // 服务器繁忙的人数上限

#else

#define HEAD_SIZE               12

#define EPOLL_WAIT_TIME         11              // epoll_wait 阻塞时间
#define MAX_CLIENTS             64              // epoll 最大连接数量
#define TMP_BUFFER_SIZE         1024            // 自定义临时缓冲区大小
#define MAX_BUFFER_SIZE         1045876         // 自定义最大缓冲区大小

#define DEFAULT_ROOM_COUNT      32
#define ROOM_POOL_SIZE          32

#define ONE_FRAME_MS            33              // 一帧的毫秒数

#define CHECK_ALL_CLIENT_CONN   500
#define TRY_CONNECT_SERVER_TIME 1000            // 服务器失去与某服务器连接，申请CenterServer重新分配的时间
#define MAX_HEARTBEAT_DIFF      4000
#define SEND_CENTERSERVER_TIME  5000            // 成功连接到CenterServer后，每秒向CenterServer发包的时间

#define GATE_SERVER_PORT_1      10808           // 目前接收所有客户端连接的端口，后续将CenterServer部署到此端口
#define GATE_SERVER_PORT_2      18080
#define LOGIC_SERVER_PORT_1     10809
#define LOGIC_SERVER_PORT_2     18090
#define DATABASE_SERVER_PORT    10811
#define CENTER_SERVER_PORT      8088
#define LOCAL_IP                "127.0.0.1"     // 本地IP

#define SERVER_FREE_COUNT       2               // 服务器空闲的人数上限
#define SERVER_COMMON_COUNT     4               // 服务器中等繁忙的人数上限
#define SERVER_BUSY_COUNT       50              // 服务器繁忙的人数上限

#endif
/*
 * @brief 服务器类型
 */
enum SERVER_TYPE
{
    NONE = 0,
    CENTER = 1,
    GATE = 2,
    LOGIC = 3,
    DATABASE = 4,
    MATCH = 5,
};

/*
 * @brief 服务器繁忙程度
 */
enum SERVER_FREE_LEVEL
{
    FREE = 0,
    COMMON = 1,
    BUSY = 2,
    DOWN = 3 //宕机
};

/*
 * @brief 包体类型
 */
enum BODYTYPE
{
    ErrorPackage = -1, // 错误的包体类型

    LoginRequest = 0,   // 用户的登录请求
    LoginResponse = 1,  // 用户的登录回应
    RegistRequest = 2,  // 用户的注册请求
    RegistResponse = 3, // 用户的注册回应
    LoginOut = 4,       // 用户的登出请求

    JoinRoom = 5,    // 用户申请加入房间的请求和回应
    LeaveRoom = 6,   // 用户申请离开房间的请求和回应
    CreateRoom = 7,  // 用户申请创建房间的请求和回应
    GetRoomList = 8, // 用户申请得到房间列表的请求和回应

    StartGame = 9, // 用户申请开始游戏，大家从房间进入游戏
    EndGame = 10,  // 用户(房主)申请结束游戏，大家一起返回Room

    EnterGame = 11, // 有用户加入游戏的通知
    QuitGame = 12,  // 有用户退出游戏的通知

    Frame = 13,       // 服务器下发的帧信息
    UserOperate = 14, // 客户端发送的操作信息
    ChaseFrame = 15,  // 用户申请追帧的信息
    GameReplay = 16,  // 服务器的累计帧数据
    Reconnect = 17,   // 重连消息
    UserHeart = 18,   // 客户端心跳包

    /* server之间通信协议,绝对不会发往客户端 */
    ServerInfo = 100, // 服务器之间维持连接的消息
    UserInfo = 101,   // 服务器之间发送用户状态的信息
    ServerConnChange = 102,   // 服务器之间连接数量变化协议

    /* 大区协议 */
    RegionInfo = 200,            // 大区信息
    GetRegionInfoRequest = 201,  // 获取大区信息列表请求
    GetRegionInfoResponse = 202, // 获取大区信息列表响应
    JoinRegionRequest = 203,     // 加入大区结果
    JoinRegionResponse = 204,    // 加入大区响应的结果

    /* 客户端token出问题 */
    ErrorToken = 400, // 用户的Token过期通知
};

/*
 * @brief 回调函数类型
 */
enum CallbackType
{
    LogicServer_Update = 1,

    FuncServer_TryToConnectAvailabeServer = 2,
    FuncServer_SendSelfInfoToCenter = 3,

    GateServer_CheckAllClientConn = 4,
};

#endif
