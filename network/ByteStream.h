//
// Created by haowendeng on 2023/2/28.
//

#ifndef BYTESTREAM_H
#define BYTESTREAM_H

#include <arpa/inet.h>


template<typename T>
T ntoh(T n) {
    if (sizeof(T) == 1) return n;
    if (sizeof(T) == 2) return ntohs(n);
    else if (sizeof(T) == 4) return ntohl(n);
    else {
        std::cerr << "ntoh can not convert, sizeof(T) is " << sizeof(T) << std::endl;
        return 0;
    }
}

template<typename T>
T hton(T n) {
    if (sizeof(T) == 1) return n;
    if (sizeof(T) == 2) return htons(n);
    else if (sizeof(T) == 4) return htonl(n);
    else {
        std::cerr << "hton can not convert, sizeof(T) is " << sizeof(T) << std::endl;
        return 0;
    }
}

class InputByteStream {
public:
    InputByteStream(const char *data, int len) {
        buffer = data;
        offset = 0;
        this->len = len;
        ok = true;
    }

    template<typename T>
    InputByteStream &operator>>(T &val) {
        if (!ok || offset + sizeof(T) > len) {
            ok = false;
            return *this;
        }
        val = ntoh(*(T *) (buffer + offset));
        offset += sizeof(T);
        return *this;
    }

    int length() const { return len - offset; }

    bool is_available() const { return ok; }

private:
    const char *buffer;
    int offset;
    int len;
    bool ok;
};

class OutputByteStream {
public:
    OutputByteStream(char *buf, int len) {
        buffer = buf;
        max_len = len;
        offset = 0;
        ok = true;
    }

    template<typename T>
    OutputByteStream &operator<<(const T &val) {
        if (!ok || offset + sizeof(T) > max_len) {
            ok = false;
        }
        *(T *) (buffer + offset) = hton(val);
        offset += sizeof(T);
        return *this;
    }

    int length() const { return offset; }

    bool is_available() const { return ok; }

private:
    bool ok;
    char *buffer;
    int offset;
    int max_len;
};


#endif //BYTESTREAM_H
