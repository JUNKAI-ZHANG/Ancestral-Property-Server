//
// Created by haowendeng on 2023/2/26.
//

#ifndef MESSAGE_TYPES_H
#define MESSAGE_TYPES_H

//实体类型
#define ENTITY_UNKNOWN (-1)
#define ENTITY_CLIENT 0x00
#define ENTITY_DB_SERVER 0x01
#define ENTITY_PROXY 0x02
#define ENTITY_GAME_SERVER 0x03
#define ENTITY_GATE 0x04

//消息类型
#define MSG_ERR (-1)
#define MSG_REQUEST_LOGIN 0x01 // 请求登录
#define MSG_RESPONSE_LOGIN 0x02 // 响应登录
#define MSG_NOTIFY_OPERATE 0x03 // 通知玩家操作
#define MSG_NOTIFY_START_GAME 0x04 // 通知开始游戏
#define MSG_NOTIFY_STOP_GAME 0x05 // 通知结束游戏
#define MSG_REQUEST_GAME_REPLAY 0x06 // 客户端请求回放数据
#define MSG_RESPONSE_GAME_REPLAY 0x07 // 响应回放数据给客户端
#define MSG_NOTIFY_RECONNECT_GAMEDATA 0x08 // 断线重连发送所有帧
#define MSG_REQUEST_LOGIN_S2S 0x09
#define MSG_RESPONSE_LOGIN_S2S 0x0A
#define MSG_HEARTBEAT 0x0B

#endif //MESSAGE_TYPES_H
