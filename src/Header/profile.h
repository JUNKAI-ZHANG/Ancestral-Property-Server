#ifndef _PROFILE_H

#define _PROFILE_H

#define HEAD_SIZE       12
#define MAX_CLIENTS     128
#define TMP_BUFFER_SIZE 1024
#define MAX_BUFFER_SIZE 131072

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
    ErrorPackage = -1,

    LoginRequest = 0,
    LoginResponse,
    RegistRequest,
    RegistResponse,

    LoginOut,

    // JoinRoomRequest,
    // LeaveRoomRequest,
    // CreateRoomRequest,
    // GetRoomListRequest,

    // JoinRoomResponse,
    // LeaveRoomResponse,
    // CreateRoomResponse,
    // GetRoomListResponse,

    JoinRoom,
    LeaveRoom,
    CreateRoom,
    GetRoomList,

    StartGame,
    CloseGame,

    Frame,
    GameState,
    HashString,

    /* server之间通信协议,绝对不会发往客户端 */
    ServerInfo = 100,
    UserInfo,

    /* 客户端token出问题 */
    ErrorToken = 400,
};

#endif