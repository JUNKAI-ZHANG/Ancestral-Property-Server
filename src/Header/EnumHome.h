#ifndef _PROFILE_H
#define _PROFILE_H

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

    JoinRoom = 5,                  // 用户申请加入房间的请求和回应
    LeaveRoom = 6,                 // 用户申请离开房间的请求和回应
    CreateRoom = 7,                // 用户申请创建房间的请求和回应
    GetRoomList = 8,               // 用户申请得到房间列表的请求和回应
    ChangeRole = 9,   // 用户的状态改变请求，包括角色信息等
    JoinGame = 10,                 // 有用户加入游戏的通知
    QuitGame = 11,                 // 有用户退出游戏的通知
    NotifyRoomInfo = 12, // 用户加入房间/退出游戏会收到这个

    StartGame = 50, // 用户申请开始游戏，大家从房间进入游戏
    EndGame = 51,   // 用户(房主)申请结束游戏，大家一起返回Room

    Frame = 70,       // 服务器下发的帧信息
    UserOperate = 71, // 客户端发送的操作信息
    ChaseFrame = 72,  // 用户申请追帧的信息
    GameReplay = 73,  // 服务器的累计帧数据
    Reconnect = 74,   // 重连消息
    UserHeart = 75,   // 客户端心跳包

    /* server之间通信协议,绝对不会发往客户端 */
    ServerInfo = 100,       // 服务器之间维持连接的消息
    UserInfo = 101,         // 服务器之间发送用户状态的信息
    ServerConnChange = 102, // 服务器之间连接数量变化协议

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

    ServerBase_FixedTimeFire = 5,
};

#endif