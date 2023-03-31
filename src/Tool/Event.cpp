#ifndef _EVENT_H
#define _EVENT_H

#include <vector>
#include <iostream>
#include <functional>

// 事件类
class Event
{
public:
    typedef std::function<void(void *)> Callback; // 回调函数类型

    Event() : m_name(""), m_callback(nullptr), m_data(nullptr) {}
    Event(const std::string &name, Callback callback, void *data = nullptr)
        : m_name(name), m_callback(callback), m_data(data) {}

    // 获取事件名
    std::string getName() const { return m_name; }

    // 设置回调函数
    void setCallback(Callback callback) { m_callback = callback; }

    // 设置事件数据
    void setData(void *data) { m_data = data; }

    // 获取事件数据
    void *getData() const { return m_data; }

    // 触发事件
    void fire() { m_callback(m_data); }

private:
    std::string m_name;  // 事件名
    Callback m_callback; // 回调函数
    void *m_data;        // 事件数据
};

// 事件管理器类
class EventManager
{
public:
    // 获取事件管理器实例
    static EventManager &getInstance()
    {
        static EventManager instance;
        return instance;
    }

    // 注册事件
    void registerEvent(const std::string &eventName, Event::Callback callback, void *data = nullptr)
    {
        m_events.emplace_back(eventName, callback, data);
    }

    // 注销事件
    void unregisterEvent(const std::string &eventName)
    {
        for (auto it = m_events.begin(); it != m_events.end(); ++it)
        {
            if (it->getName() == eventName)
            {
                m_events.erase(it);
                break;
            }
        }
    }

    // 触发事件
    void fireEvent(const std::string &eventName, void *data = nullptr)
    {
        for (auto &event : m_events)
        {
            if (event.getName() == eventName)
            {
                event.setData(data);
                event.fire();
            }
        }
    }

    // 禁止复制和赋值
    EventManager(const EventManager &) = delete;
    EventManager &operator=(const EventManager &) = delete;

private:
    EventManager() {} // 构造函数设为私有，防止外部创建实例

    std::vector<Event> m_events; // 事件列表
};

#endif

/* An example */

// // 处理事件的回调函数
// void onEvent1(void *data)
// {
//     if (data != nullptr)
//     {
//         int *numPtr = static_cast<int *>(data);
//         std::cout << "Event 1 fired with data: " << *numPtr << std::endl;
//     }
// }

// void onEvent2(void *data)
// {
//     if (data != nullptr)
//     {
//         std::string *strPtr = static_cast<std::string *>(data);
//         std::cout << "Event 2 fired with data: " << *strPtr << std::endl;
//     }
// }

// int main()
// {
//     // 获取事件管理器实例
//     EventManager &eventManager = EventManager::getInstance();

//     // 注册事件
//     eventManager.registerEvent("Event 1", onEvent1);
//     eventManager.registerEvent("Event 2", onEvent2);

//     // 触发事件
//     int num = 42;
//     std::string str = "hello";
//     eventManager.fireEvent("Event 1", &num);
//     eventManager.fireEvent("Event 2", &str);

//     // 注销事件
//     eventManager.unregisterEvent("Event 1");

//     return 0;
// }
