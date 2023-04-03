#include "../Header/Message.h"
#include "../Header/MessageUtils.h"

MessageHead::MessageHead(const uint8_t *buffer, int len)
{
    if (len < HEAD_SIZE)
    {
        m_packageType = BODYTYPE::ErrorPackage;
        return;
    }

    // 0~3位存大小
    // m_packageSize = ntohl(*((uint32_t *)buffer));
    m_packageSize = ByteToInt(buffer);
    // 4~7位存类型
    // m_packageType = static_cast<BODYTYPE>(ntohl(*((uint32_t *)(buffer + 4))));
    m_packageType = ByteToInt(buffer + 4);
    // 8~11位存token
    // m_token = ntohl(*((uint32_t *)(buffer + 8)));
    m_token = ByteToUint(buffer + 8);
    return;
}

bool MessageHead::SerailizeToArray(uint8_t *buffer, int len) const
{
    if (len < HEAD_SIZE)
    {
        return false;
    }

    /*
        // 转大端序
        uint32_t size = htonl(m_packageSize);
        uint32_t type = htonl(static_cast<uint32_t>(m_packageType));
        uint32_t token = htonl(static_cast<uint32_t>(m_token));

        uint8_t *tmp = (uint8_t *)(&size);
        for (int i = 0; i < 4; i++)
        {
            buffer[i] = tmp[i];
        }

        tmp = (uint8_t *)(&type);
        for (int i = 0; i < 4; i++)
        {
            buffer[i + 4] = tmp[i];
        }

        // 最后4Byte不处理，前端不需要解析token.(算了吧，都写了这么多了，也不差这一点了，hh)
        tmp = (uint8_t *)(&token);
        for (int i = 0; i < 4; i++)
        {
            buffer[i + 8] = tmp[i];
        }
    */
    uint8_t *tmp = IntToByte(m_packageSize);
    for (int i = 0; i < 4; i++)
    {
        buffer[i] = tmp[i];
    }

    tmp = IntToByte(m_packageType);
    for (int i = 0; i < 4; i++)
    {
        buffer[i + 4] = tmp[i];
    }

    // 最后4Byte不处理，前端不需要解析token.(算了吧，都写了这么多了，也不差这一点了，hh)
    tmp = UintToByte(m_token);
    for (int i = 0; i < 4; i++)
    {
        buffer[i + 8] = tmp[i];
    }
    delete tmp;
    return true;
}

// ******************** Body **********************

int MessageBody::length() const
{
    if (message == nullptr)
    {
        return 0;
    }
    return (int)message->ByteSizeLong();
}

bool MessageBody::ParseFromArray(const uint8_t *data, int len) const
{
    if (message == nullptr)
    {
        return true;
    }
    return message->ParseFromArray(data, len);
}

bool MessageBody::SerializeToArray(uint8_t *buffer, int len) const
{
    if (message == nullptr)
    {
        return true;
    }
    return message->SerializeToArray(buffer, len);
}

MessageBody::~MessageBody()
{
    delete message;
}

// ******************** Message **********************

Message::Message()
{
    // head = new MessageHead;
    // body = new MessageBody;
}

int Message::length() const
{
    if (body == nullptr)
    {
        return HEAD_SIZE;
    }
    return HEAD_SIZE + body->length();
}

bool Message::SerializeToArray(uint8_t *buffer, int len) const
{
    if (len < length())
    {
        return false;
    }
    if (!head->SerailizeToArray(buffer, len))
    {
        return false;
    }
    if (body == nullptr)
    {
        return true; // 允许发送没有消息体的信息
    }
    return body->SerializeToArray(buffer + HEAD_SIZE, len - HEAD_SIZE);
}

Message::~Message()
{
    delete head;
    delete body;
}