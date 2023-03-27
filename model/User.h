//
// Created by haowendeng on 2023/3/2.
//

#ifndef USER_H
#define USER_H

#include "QueryResult.h"

class MySQLUser : public QueryResult {
public:
    void decode(MYSQL_ROW row, size_t *lengths, int num_fields) override {
        m_id = std::stoi(row[0]);
        m_username = row[1];
        m_password = row[2];
    }

DEFINE_GETSET_ELEMENT(int, id)
DEFINE_GETSET_ELEMENT(std::string, username)
DEFINE_GETSET_ELEMENT(std::string, password)
};


#endif //USER_H
