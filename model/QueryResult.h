//
// Created by haowendeng on 2023/3/2.
//

#ifndef QUERYRESULT_H
#define QUERYRESULT_H

#include <mysql/mysql.h>
#include <type_traits>

#define DEFINE_GETSET_ELEMENT(type, name) public: \
type name() const { return isNull() ? type() : m_##name; }              \
void set_##name(const type &ano) { m_##name = ano; }\
private:                                         \
type m_##name;

// 定义查询结果基类
class QueryResult {
public:
    QueryResult() { m_null = false; }

    virtual void decode(MYSQL_ROW row, size_t *lengths, int num_fields) = 0;

    void setNull() { m_null = true; }

    bool isNull() const { return m_null; }

    template<class T>
    static T createNullObject() {
        static_assert(std::is_base_of<QueryResult, T>::value, "type T should inherit from QueryResult");
        T result;
        result.m_null = true;
        return result;
    }

private:
    bool m_null;
};


#endif //QUERYRESULT_H
