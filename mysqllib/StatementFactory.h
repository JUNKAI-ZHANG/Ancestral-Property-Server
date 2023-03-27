//
// Created by haowendeng on 2023/3/2.
//

#ifndef STATEMENTFACTORY_H
#define STATEMENTFACTORY_H

// sql语句工厂类
class StatementFactory {
public:
    explicit StatementFactory(MYSQL *conn) {
        connection = conn;
        data = "";
    }

    // 将c字符串转义为sql字符串, 并防止sql注入
    StatementFactory &escape(const char *str, size_t len) {
        char buf[len * 2];
        mysql_real_escape_string(connection, buf, str, len);
        data += '\'';
        data += buf;
        data += '\'';
        return *this;
    }

    StatementFactory &escape(const std::string &str) {
        return escape(str.c_str(), str.length());
    }

    void clear() {
        data.clear();
    }

    std::string end() {
        std::string ret = data;
        clear();
        return std::move(ret);
    }

    // 将str添加至语句末尾并自动在前后加上空格, 防止粘连
    StatementFactory &append(const std::string &str) {
        data += ' ' + str + ' ';
        return *this;
    }

    StatementFactory &append(const char &c) {
        std::string temp;
        temp += c;
        return append(temp);
    }

private:
    std::string data;
    MYSQL *connection;
};


#endif //STATEMENTFACTORY_H
