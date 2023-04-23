#include "../Header/Message/Message.h"
#include "../Header/Message/MessageUtils.h"

MessageHead::MessageHead(const uint8_t *buffer, int len)
{
    if (len < HEAD_SIZE)
    {
        m_packageType = BODYTYPE::ErrorPackage;
        return;
    }

    // 0~3位存大小
    m_packageSize = ByteToInt(buffer);
    // 4~7位存类型
    m_packageType = ByteToInt(buffer + 4);
    // 8~11位存userid
    m_userid = ByteToUint(buffer + 8);
    return;
}

MessageHead::MessageHead(int packageSize, int packageType, int userid)
{
    m_packageSize = packageSize;
    m_packageType = packageType;
    m_userid = userid;
}

bool MessageHead::SerailizeToArray(uint8_t *buffer, int len) const
{
    if (len < HEAD_SIZE)
    {
        return false;
    }

    uint8_t *tmp = IntToByte(m_packageSize);
    for (int i = 0; i < 4; i++) buffer[i] = tmp[i];
    delete tmp;

    tmp = IntToByte(m_packageType);
    for (int i = 0; i < 4; i++) buffer[i + 4] = tmp[i];
    delete tmp;

    tmp = UintToByte(m_userid);
    for (int i = 0; i < 4; i++) buffer[i + 8] = tmp[i];
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