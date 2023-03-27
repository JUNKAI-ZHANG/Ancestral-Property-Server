//
// Created by haowendeng on 2023/2/24.
//

#ifndef TCPCONNECTION_H
#define TCPCONNECTION_H

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <cerrno>
#include <arpa/inet.h>
#include <cstring>
#include <unistd.h>

const int buffer_size = 32768;


class TcpConnection {
public:
    TcpConnection() {
        init();
    }

    ~TcpConnection() {
        close();
    }

    void init();

    int flush();

    //暂时没做
    bool check_hearbeat() { return true; }

    //返回第一个完整包的包长，在这之前请先调用flush接收缓冲区数据
    int available_length();

    //返回单个包的字节流，不包含包长信息，如果给定的len存不下则什么也不做返回0
    //返回值表示实际长度
    int read(char *data, int len);

    void skip_package();

    //写入字节流，包长自动附加，会自动调用一次send
    int write(char *data, int len);

    //应该在检查到可写时应该调用send来将缓冲区的数据发送出去，但是时间来不及了，epollout大部分都是一直返回，所以先不监听了，以后优化
    int send();

    int connect_to(const char *ip, int port);

    int accept_from(int server_fd);

    void close();

    int get_type() const { return type; };

    bool is_alive() const { return alive; }

private:
    friend class TcpManager;

    int get_fd() const { return sockfd; }

    //包长信息占用字节数
    int length_size = 2;

    int sockfd;
    time_t last_heartbeat;
    int type; //0 -> not alive, 1 -> accepted, 2 -> connected
    bool alive;
    char read_buf[buffer_size], send_buf[buffer_size];
    int read_data_len, send_data_len;
    sockaddr_in addr;
};


#endif //TEST_TCPCONNECTION_H
