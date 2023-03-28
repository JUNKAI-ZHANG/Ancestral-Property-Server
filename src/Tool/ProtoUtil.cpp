#ifndef _PROTOUTIL_H
#define _PROTOUTIL_H

#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include "../Protobuf/NetworkData.pb.h"
#include "../Header/profile.h"

#define GENERTATE_METHOD_BY_BODYTYPE(type, typename)                               \
    static type ParseBodyTo##typename(const uint8_t *array, int length, bool &ret) \
    {                                                                              \
        type data;                                                                 \
        ret = true;                                                                \
        if (!data.ParseFromArray(array, length))                                   \
        {                                                                          \
            ret = false;                                                           \
            std::cerr << "Failed to ParseBody" << std::endl;                       \
        }                                                                          \
        return data;                                                               \
    }

enum BODYTYPE
{
    LoginData = 0,
    LoginResponse,
    CharacterData,
    LoginOut,
    Frame,
    GameState,
    HashString,
    UserMoney,

    /* server之间通信协议 */
    ServerInfo = 100

};

struct Header
{
    /* size = header + body */
    uint32_t package_size;

    BODYTYPE type;

    uint32_t token;

    Header()
    {
        package_size = 0;
    }
};

class ProtoUtil
{
public:
    /*
     * @brief array size must longer than header size(8)
     */
    static Header ParseHeaderFromArray(uint8_t *array)
    {
        Header header;
        // 前4位存大小
        header.package_size = ntohl(*((uint32_t *)array));
        // 后4位存类型
        header.type = static_cast<BODYTYPE>(ntohl(*((uint32_t *)(array + 4))));

        header.token = ntohl(*((uint32_t *)(array + 8)));

        return header;
    }

    static bool CheckHeaderIsValid(Header header)
    {
        if (header.package_size <= HEAD_SIZE)
        {
            return false;
        }

        // 规定:单包大小不会超过1024
        if (header.package_size > 1024)
        {
            return false;
        }

        if (header.type < BODYTYPE::LoginData)
        {
            return false;
        }

        if (header.type > BODYTYPE::ServerInfo)
        {
            return false;
        }

        return true;
    }

    static void SerializeHeaderToArray(uint8_t *res, Header h)
    {
        // 转大端序
        uint32_t size = htonl(h.package_size);
        uint32_t type = htonl(static_cast<uint32_t>(h.type));

        uint8_t *tmp = (uint8_t *)(&size);
        for (int i = 0; i < 4; i++)
        {
            res[i] = tmp[i];
        }

        tmp = (uint8_t *)(&type);
        for (int i = 0; i < 4; i++)
        {
            res[i + 4] = tmp[i];
        }

        // 最后4Byte不处理，前端不需要解析token
    }

    /*
     * @brief 需要手动释放!!
     */
    static uint8_t *GetBodyFromArray(uint8_t *array, int offset, uint32_t body_size)
    {
        if (body_size == 0)
            return nullptr;

        uint8_t *body = new uint8_t[body_size];

        for (int i = 0; i < body_size; i++)
        {
            body[i] = array[offset + i];
        }

        return body;
    }

    // 反序列化body
    GENERTATE_METHOD_BY_BODYTYPE(Net::LoginData, LoginData);
    GENERTATE_METHOD_BY_BODYTYPE(Net::LoginResponse, LoginResponse);
    GENERTATE_METHOD_BY_BODYTYPE(Net::CharacterData, CharacterData);
    GENERTATE_METHOD_BY_BODYTYPE(Net::LoginOut, LoginOut);
    GENERTATE_METHOD_BY_BODYTYPE(Net::Frame, Frame);
    GENERTATE_METHOD_BY_BODYTYPE(Net::GameState, GameState);
    GENERTATE_METHOD_BY_BODYTYPE(Net::HashString, HashString);
    GENERTATE_METHOD_BY_BODYTYPE(Net::UserMoney, UserMoney);
    GENERTATE_METHOD_BY_BODYTYPE(Net::ServerInfo, ServerInfo);

    // 序列化body

    // 务必手动释放buffer!!!
    static uint8_t *SerializeLoginResponseToArray(bool ret,
                                                  std::string msg,
                                                  int &length,
                                                  std::string userid,
                                                  uint32_t token,
                                                  Net::LoginResponse_Operation opt)
    {
        Net::LoginResponse data;
        data.set_result(ret);
        data.set_msg(msg);
        data.set_userid(userid);
        data.set_token(token);
        data.set_opt(opt);
        length = data.ByteSizeLong();

        uint8_t *array = new uint8_t[length];

        if (data.SerializeToArray(array, length))
        {
            return array;
        }

        std::cerr << "Failed to Serialize" << std::endl;
        return nullptr;
    }

    // 务必手动释放buffer!!!
    static uint8_t *SerializeServerInfoToArray(std::string ip,
                                               int port,
                                               SERVER_TYPE type,
                                               Net::ServerInfo_Operation opt,
                                               SERVER_FREE_LEVEL level,
                                               int &length)
    {
        Net::ServerInfo info;
        info.set_ip(ip);
        info.set_port(port);
        info.set_server_type(type);
        info.set_opt(opt);
        info.set_server_free_level(level);

        length = info.ByteSizeLong();

        uint8_t *array = new uint8_t[length];

        if (info.SerializeToArray(array, length))
        {
            return array;
        }

        std::cerr << "Failed to Serialize" << std::endl;
        return nullptr;
    }

    // 务必手动释放buffer!!!
    static uint8_t *SerializeUserMoneyToArray(std::string username,
                                              int money,
                                              Net::UserMoney_Operation opt,
                                              int &length)
    {
        Net::UserMoney info;
        info.set_userid(username);
        info.set_money(money);
        info.set_opt(opt);

        length = info.ByteSizeLong();

        uint8_t *array = new uint8_t[length];

        if (info.SerializeToArray(array, length))
        {
            return array;
        }

        std::cerr << "Failed to Serialize" << std::endl;
        return nullptr;
    }
};

#endif